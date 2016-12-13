#include "ModuleRepository.h"

#include <iterator>

#include "JITModule.h"
#include "Jit.h"



namespace jit
{
  ModuleRepository::ModuleRepository(Jit* a_Jit): m_Jit(a_Jit){}




  bool ModuleRepository::isUnloadable(JITModule * a_Module)
  {
    return false;
  }




  JITModule * ModuleRepository::get(module_id_t a_ID)
  {
    if(a_ID < m_Modules.size())
    {
      return m_Modules[a_ID];
    }
    return nullptr;
  }




  module_id_t ModuleRepository::get(std::string & a_Name)
  {
    llvm::StringRef ref(a_Name);
    return get(ref);
  }




  module_id_t ModuleRepository::get(llvm::StringRef & a_Name)
  {
    for(auto it = m_Modules.begin(); it != m_Modules.end(); ++it)
    {
      if ((*it)->getRaw()->getName() != a_Name){}
      else
      {
        return std::distance(m_Modules.begin(),it);
      }
    }
    return MODULE_INVALID_ID;
  }



 #include <stdio.h>
  module_id_t ModuleRepository::add(JITModule * a_Module)
  {
    auto name = a_Module->getRaw()->getModuleIdentifier();
    JITModule ** location = nullptr;
    module_id_t id = MODULE_INVALID_ID;
    // Search to see if the module is already a part of the repo
    // and also for a blank spot that we can put it in
    for(auto it = m_Modules.begin(); it != m_Modules.end(); ++it)
    {
      if (*it != nullptr)
      {
        if ((*it)->getRaw()->getModuleIdentifier() != name){}
        else
        {
          return MODULE_INVALID_ID;
        }
      }
      else if(location != nullptr) {}
      else
      {
        location = &(*it);
        id = std::distance(m_Modules.begin(), it);
      }
    }

    // If we didn't find a place to put it in, just append
    if (location == nullptr)
    {
      m_Modules.push_back(a_Module);
      id = m_Modules.size() - 1;
    }
    else
    {
      *location = a_Module;
    }

    return id;
  }




  bool ModuleRepository::unload(module_id_t a_ID)
  {
    if (a_ID < m_Modules.size())
    {
      auto module = m_Modules[a_ID];
      if (module == nullptr || !module->isGenerated())
      {
        if (module != nullptr)
        {
          auto engine = m_Jit->getEngine();
          if (engine != nullptr)
          {
            engine->removeModule(module->getRaw());
          }
          delete module;
          m_Modules[a_ID] = nullptr;
        }
        return true;
      }
      else
      {
        // TODO: If the module isn't depended on by any other module, then we can safely
        // remove it from the jit engine
      }
    }
    return false;
  }




  bool ModuleRepository::unload(std::string & a_Name)
  {
    llvm::StringRef temp(a_Name);
    return unload(a_Name);
  }




  bool ModuleRepository::unload(llvm::StringRef & a_Name)
  {
    for(auto it = m_Modules.begin(); it != m_Modules.end(); ++it)
    {
      if(*it != nullptr)
      {
        if((*it)->getRaw()->getName() != a_Name){}
        else
        {
          return unload(std::distance(m_Modules.begin(), it));
        }
      }
    }
    return false;
  }




  size_t ModuleRepository::count()
  {
    return m_Modules.size();
  }
}
