#include "interface.h"

#include <cstring>
#include <string>
#include <fstream>
#include <iostream>

#include <unistd.h>

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>

#include "FrontendRepository.h"
#include "Jit.h"
#include "ModuleRepository.h"

using namespace jit;

Jit* instance = nullptr;

extern "C" {
  void jit_init()
  {
    if (instance == nullptr)
    {
      instance = new Jit();
    }
  }




  void jit_deinit()
  {
    if (instance != nullptr)
    {
      delete instance;
      instance = nullptr;
    }
  }




  void jit_addModuleSearchPaths(const char ** paths)
  {
    while (*paths != nullptr)
    {
      instance->addModuleSearchPath(*(paths++));
    }
  }




  void jit_addFrontendSearchPaths(const char ** paths)
  {
    while (*paths != nullptr)
    {
      instance->addFrontendSearchPath(*(paths++));
    }
  }




  void jit_addLibrarySearchPaths(const char ** paths)
  {
    while (*paths != nullptr)
    {
      instance->addLibrarySearchPath(*(paths++));
    }
  }




  void jit_removeModuleSearchPaths(const char ** paths)
  {
    while (*paths != nullptr)
    {
      instance->removeModuleSearchPath(*(paths++));
    }
  }




  void jit_removeFrontendSearchPaths(const char ** paths)
  {
    while (*paths != nullptr)
    {
      instance->removeFrontendSearchPath(*(paths++));
    }
  }




  void jit_removeLibrarySearchPaths(const char ** paths)
  {
    while (*paths != nullptr)
    {
      instance->removeLibrarySearchPath(*(paths++));
    }
  }




  module_id_t jit_compile(const char * compiler, const char * module_name, const char ** files, jit_error_t * error)
  {
    error->message = nullptr;
    // Get the requested frontend
    auto frontend = instance->getOrLoad(compiler);
    if (frontend)
    {
      auto module = new llvm::Module(llvm::StringRef(module_name), *(instance->getContext()));
      auto jitmodule = new JITModule(module);
      auto metadata = jitmodule->getMetadata();

      // Try to compile all of the files provided
      while (files != nullptr)
      {
        std::ifstream file;
        file.open(*files);
        if (file.is_open())
        {
          if (frontend->compile(module, metadata, file))
          {
            ++files;
          }
          else
          {
            error->code = ERR_COMPILATION_ERR;
            delete jitmodule;
            return MODULE_INVALID_ID;
          }
        }
        else
        {
          error->code = ERR_FILE_MISSING;
          delete jitmodule;
          return MODULE_INVALID_ID;
        }
      }

      // Write the metadata to the module
      JITMetadata::set(module, metadata);

      // Write the module to disk
      {
        std::error_code EC;
        llvm::raw_fd_ostream OS(module_name, EC, llvm::sys::fs::F_None);
        WriteBitcodeToFile(module, OS);
        OS.flush();
      }

      // Load module into repo, ready to be loaded into vm or used during compilation
      return instance->getModuleRepository()->add(jitmodule);
    }
    else
    {
      error->code = ERR_MISSING_FRONTEND;
    }
    return MODULE_INVALID_ID;
  }



  module_id_t jit_loadModule(const char * module_name, jit_error_t * error)
  {
    char cwd[512];
    getcwd(cwd, sizeof(cwd));
    for (auto path = instance->getModulePathBegin(); path != instance->getModulePathEnd(); ++path)
    {
      llvm::StringRef name(module_name);
      chdir(path->c_str());
      auto buf_or = llvm::MemoryBuffer::getFile(name);
      chdir(cwd);
      // Try to load the module
      if (!buf_or)
      {
        auto err = buf_or.getError();
        error->code = err.value();
        error->message = new char[err.message().size()];
        strcpy(error->message, err.message().c_str());
        continue;
      }
      auto module_or = llvm::parseBitcodeFile(buf_or.get()->getMemBufferRef(), *(instance->getContext()));

      // Check if the file was a valid module
      if (!module_or)
      {
          auto err = module_or.getError();
          error->code = err.value();
          error->message = new char[err.message().size()];
          strcpy(error->message, err.message().c_str());
          continue;
      }
      // Create JITModule
      auto module = module_or.get().release();
      module->setModuleIdentifier(name);
      auto jitmodule = new JITModule(module);

      auto metadata = jitmodule->getMetadata();
      auto & required_modules = metadata->getRequiredModules();
      bool successfuly_loaded = true;
      jit_error_t err;
      // Try to load dependencies
      for (auto & required : required_modules)
      {
        if (jit_loadModule(required.str().c_str(), &err) != MODULE_INVALID_ID){}
        else
        {
          successfuly_loaded = false;
          break;
        }
      }
      // return id on successful load
      // Cleanup on fail and return MODULE_INVALID_ID
      if (successfuly_loaded)
      {
        if (instance == nullptr)
        {
          instance = new Jit();
        }
        auto engine = instance->getEngine();
        engine->addModule(std::unique_ptr<llvm::Module>(jitmodule->getRaw()));
        engine->finalizeObject();
        return instance->getModuleRepository()->add(jitmodule);
      }
      else
      {
        delete jitmodule;
        *error = err;
        // return MODULE_INVALID_ID;
      }
    }
    return MODULE_INVALID_ID;
  }




  module_id_t jit_getModule(const char * module_name, jit_error_t * error)
  {
    llvm::StringRef temp(module_name);
    return instance->getModuleRepository()->get(temp);
  }




  bool jit_unloadModule(module_id_t id, jit_error_t * error)
  {
    return instance->getModuleRepository()->unload(id);
  }




  void * jit_getModuleFunction(module_id_t id, const char * name)
  {
    auto engine = instance->getEngine();
    if (engine != nullptr)
    {
      auto jitmodule = instance->getModuleRepository()->get(id);
      if (jitmodule)
      {
        auto qualified_name = jitmodule->getRaw()->getModuleIdentifier();
        qualified_name += '.';
        qualified_name += name;
        auto function = engine->FindFunctionNamed(qualified_name.c_str());
        if (function)
        {
          return engine->getPointerToFunction(function);
        }
      }
    }
    return nullptr;
  }




  void * jit_getFunction(const char * name)
  {
    auto engine = instance->getEngine();
    if (engine != nullptr)
    {
      auto function = engine->FindFunctionNamed(name);
      if (function)
      {
        return engine->getPointerToFunction(function);
      }
    }
    return nullptr;
  }
}
