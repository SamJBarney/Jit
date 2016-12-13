#include "JITModule.h"

#include "JITMetadata.h"


namespace jit
{
  JITModule::JITModule(llvm::Module * a_Raw)
  {
    m_Raw = a_Raw;
    m_Data = nullptr;
    m_Generated = false; 
  }




  JITModule::~JITModule()
  {
    delete m_Raw;
    if (m_Data)
    {
      delete m_Data;
    }
  }




  llvm::Module * JITModule::getRaw()
  {
    return m_Raw;
  }




  JITMetadata * JITModule::getMetadata()
  {
    if (m_Data == nullptr)
    {
      m_Data = JITMetadata::get(m_Raw);
    }
    return m_Data;
  }




  bool JITModule::isGenerated()
  {
    return m_Generated;
  }




  void JITModule::setGenerated()
  {
    m_Generated = true;
  }
}
