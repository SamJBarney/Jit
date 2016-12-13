#pragma once

#include <map>
#include <string>

#include <llvm/IR/Module.h>

#include "JITModule.h"
#include "types.h"



namespace jit
{
  class Jit;
  class ModuleRepository
  {
    std::vector<JITModule *> m_Modules;
    Jit * m_Jit;

    bool isUnloadable(JITModule*);

  public:
    ModuleRepository(Jit* a_Jit);
    module_id_t add(JITModule *);
    JITModule * get(module_id_t);
    module_id_t get(std::string &);
    module_id_t get(llvm::StringRef &);
    bool unload(module_id_t);
    bool unload(std::string &);
    bool unload(llvm::StringRef &);
    size_t count();
  };
}
