#pragma once


#include <llvm/IR/Module.h>

#include "JITMetadata.h"

namespace jit
{
  class JITModule
  {
    llvm::Module * m_Raw;
    JITMetadata * m_Data;
    bool m_Generated;

  public:
    JITModule(llvm::Module *);
    ~JITModule();
    llvm::Module * getRaw();
    JITMetadata * getMetadata();
    bool isGenerated();
    void setGenerated();
  };
}
