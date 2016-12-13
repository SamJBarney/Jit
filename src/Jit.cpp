#include "Jit.h"

#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/Module.h>

#include "FrontendRepository.h"
#include "ModuleRepository.h"

namespace jit
{
  Jit::Jit()
  {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    m_Context = new llvm::LLVMContext();

    auto module = llvm::make_unique<llvm::Module>("_DUMMY_", *m_Context);
    std::string err;
    llvm::TargetOptions Opts;
    std::unique_ptr<llvm::RTDyldMemoryManager> MemMgr(new llvm::SectionMemoryManager());


    llvm::EngineBuilder factory(std::move(module));

    factory.setErrorStr(&err);

    factory.setEngineKind(llvm::EngineKind::JIT);

    factory.setTargetOptions(Opts);

    factory.setMCJITMemoryManager(std::move(MemMgr));

    m_Engine = factory.create();
    m_FrontendRepo = new FrontendRepository(this);
    m_ModuleRepo = new ModuleRepository(this);
  }




  Jit::~Jit()
  {
    delete m_Engine;
    delete m_Context;
    delete m_FrontendRepo;
    delete m_ModuleRepo;
  }




  llvm::ExecutionEngine* Jit::getEngine()
  {
    return m_Engine;
  }




  llvm::LLVMContext* Jit::getContext()
  {
    return m_Context;
  }




  void Jit::addModuleSearchPath(const std::string & a_Path)
  {
    for (auto & path : m_ModuleSearchPaths)
    {
      if (path != a_Path);
      else
      {
        return;
      }
    }
    m_ModuleSearchPaths.push_back(a_Path);
  }




  void Jit::addFrontendSearchPath(const std::string & a_Path)
  {
    for (auto & path : m_FrontendSearchPaths)
    {
      if (path != a_Path);
      else
      {
        return;
      }
    }
    m_FrontendSearchPaths.push_back(a_Path);
  }




  void Jit::addLibrarySearchPath(const std::string & a_Path)
  {
    for (auto & path : m_LibrarySearchPaths)
    {
      if (path != a_Path);
      else
      {
        return;
      }
    }
    m_LibrarySearchPaths.push_back(a_Path);
  }




  void Jit::removeModuleSearchPath(const std::string & a_Path)
  {
    for (auto it = m_ModuleSearchPaths.cbegin(); it != m_ModuleSearchPaths.cend(); ++it)
    {
      if (*it != a_Path);
      else
      {
        m_ModuleSearchPaths.erase(it);
        return;
      }
    }
  }




  void Jit::removeFrontendSearchPath(const std::string & a_Path)
  {
    for (auto it = m_FrontendSearchPaths.cbegin(); it != m_FrontendSearchPaths.cend(); ++it)
    {
      if (*it != a_Path);
      else
      {
        m_FrontendSearchPaths.erase(it);
        return;
      }
    }
  }




  void Jit::removeLibrarySearchPath(const std::string & a_Path)
  {
    for (auto it = m_LibrarySearchPaths.cbegin(); it != m_LibrarySearchPaths.cend(); ++it)
    {
      if (*it != a_Path);
      else
      {
        m_LibrarySearchPaths.erase(it);
        return;
      }
    }
  }




  Frontend * Jit::getOrLoad(const char * a_Name)
  {
    return m_FrontendRepo->getOrLoad(a_Name);
  }




  ModuleRepository * Jit::getModuleRepository()
  {
    return m_ModuleRepo;
  }
}
