#pragma once

#include <vector>
#include <string>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include "Frontend.h"

namespace jit
{
  class FrontendRepository;
  class ModuleRepository;
  class Jit
  {
    std::vector<std::string> m_ModuleSearchPaths;
    std::vector<std::string> m_FrontendSearchPaths;
    std::vector<std::string> m_LibrarySearchPaths;
    FrontendRepository* m_FrontendRepo;
    ModuleRepository* m_ModuleRepo;

    llvm::ExecutionEngine* m_Engine;
    llvm::LLVMContext* m_Context;
  public:

    typedef std::vector<std::string>::const_iterator const_iterator;

    Jit();
    ~Jit();

    llvm::ExecutionEngine* getEngine();
    llvm::LLVMContext* getContext();

    void addModuleSearchPath(const std::string & a_Path);
    void addFrontendSearchPath(const std::string & a_Path);
    void addLibrarySearchPath(const std::string & a_Path);
    void removeModuleSearchPath(const std::string & a_Path);
    void removeFrontendSearchPath(const std::string & a_Path);
    void removeLibrarySearchPath(const std::string & a_Path);

    Frontend * getOrLoad(const char * a_Name);

    ModuleRepository * getModuleRepository();


    const_iterator getModulePathBegin()
    {
      return m_ModuleSearchPaths.cbegin();
    }

    const_iterator getModulePathEnd()
    {
      return m_ModuleSearchPaths.cend();
    }

    const_iterator getFrontendPathBegin()
    {
      return m_FrontendSearchPaths.cbegin();
    }

    const_iterator getFrontendPathEnd()
    {
      return m_FrontendSearchPaths.cend();
    }

    const_iterator getLibraryPathBegin()
    {
      return m_LibrarySearchPaths.cbegin();
    }

    const_iterator getLibraryPathEnd()
    {
      return m_LibrarySearchPaths.cend();
    }
  };
}
