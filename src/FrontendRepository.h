#pragma once

#include <map>
#include <string>
#include "Frontend.h"
#include "Jit.h"

namespace jit
{
  class FrontendRepository
  {
    std::map<std::string, Frontend *> s_Frontends;
    Jit * m_Jit;
  public:
    FrontendRepository(Jit* a_Jit): m_Jit(a_Jit){}
    
    Frontend * getOrLoad(const char * a_Name)
    {
      std::string temp(a_Name);
      return getOrLoad(temp);
    }

    Frontend * getOrLoad(std::string &);
  };
}
