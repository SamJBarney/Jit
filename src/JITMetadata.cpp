#include "JITMetadata.h"


namespace jit
{
  const char * JITMetadata::METADATA_NAME = "JITMetadata";
  const char * JITMetadata::DEFAULT_LANGUAGE = "IR";
  JITMetadata * JITMetadata::get(const llvm::Module* a_Module)
  {
    auto metadata = new JITMetadata();
  	auto named_metadata = a_Module->getNamedMetadata(METADATA_NAME);
    if (named_metadata != nullptr && named_metadata->getNumOperands() > 0)
    {
      auto container = named_metadata->getOperand(0);
      auto language = reinterpret_cast<llvm::MDString*>(container->getOperand(0).get());
      auto types = reinterpret_cast<llvm::MDTuple*>(container->getOperand(1).get());
      auto required = reinterpret_cast<llvm::MDTuple*>(container->getOperand(2).get());
      // Set language
      metadata->setLanguage(language->getString());
      // Insert struct member names
      for(int i = 0; i < types->getNumOperands(); ++i)
      {
        auto type = reinterpret_cast<llvm::MDTuple*>(types->getOperand(i).get());
        auto type_name = reinterpret_cast<llvm::MDString*>(type->getOperand(0).get());
        auto _member_names = reinterpret_cast<llvm::MDTuple*>(type->getOperand(1).get());
        std::vector<llvm::StringRef> member_names;
        for(int j = 0; j < _member_names->getNumOperands(); ++j)
        {
          auto member = reinterpret_cast<llvm::MDString*>(_member_names->getOperand(j).get());
          member_names.push_back(member->getString());
        }
        metadata->m_StructMemberNames[type_name->getString()] = member_names;
      }
      // Insert required module namespace
      for(int i = 0; i < required->getNumOperands(); ++i)
      {
        auto module = reinterpret_cast<llvm::MDString*>(required->getOperand(i).get());
        metadata->m_RequiredModules.push_back(module->getString());
      }
    }
    return metadata;
  }




  void JITMetadata::set(llvm::Module * a_Module, const JITMetadata * a_Metadata)
  {
    // std::vector<MDTuple>
    std::vector<llvm::Metadata*> _struct_types;
    std::vector<llvm::Metadata*> member_names;
    llvm::LLVMContext & context = a_Module->getContext();
    // Iterate over struct defs
    for(const auto& atype : a_Metadata->m_StructMemberNames)
    {
    	// Convert member names to metadata
    	for(const auto & name : atype.second)
    	{
    		member_names.push_back(llvm::MDString::get(context, name));
    	}
      // Create MDTuple from new ArrayRef(std::vector)
    	llvm::ArrayRef<llvm::Metadata*> _names(member_names);
    	llvm::MDTuple* names = llvm::MDTuple::get(context, _names);
      // Create MDTuple composed of struct name and previous MDTuple
    	llvm::Metadata* _struct_type[2] = {
    		reinterpret_cast<llvm::Metadata*>(llvm::MDString::get(context, atype.first)),
    		reinterpret_cast<llvm::Metadata*>(names)
    	};
    	llvm::ArrayRef<llvm::Metadata*> _type(_struct_type);
    	_struct_types.push_back(reinterpret_cast<llvm::Metadata*>(llvm::MDTuple::get(context, llvm::ArrayRef<llvm::Metadata*>(_type))));
      // Insert the tuple into the std::vector
    	member_names.clear();
    }
    // Create MDTuple from new ArrayRef(std::vector)
    llvm::ArrayRef<llvm::Metadata*> _types(_struct_types);
    llvm::MDTuple * struct_types = llvm::MDTuple::get(context, _types);
    // Create MDTuple for language, struct types, and required modules
    std::vector<llvm::Metadata*> _required;
    for(const auto& required : a_Metadata->m_RequiredModules)
    {
    	_required.push_back(reinterpret_cast<llvm::Metadata*>(llvm::MDString::get(context, required)));
    }
    llvm::ArrayRef<llvm::Metadata*> required(_required);
    llvm::Metadata* _metadata[3] = {
    	reinterpret_cast<llvm::Metadata*>(llvm::MDString::get(context, a_Metadata->m_Language)),
    	reinterpret_cast<llvm::Metadata*>(struct_types),
    	reinterpret_cast<llvm::Metadata*>(llvm::MDTuple::get(context,required))
    };
    llvm::ArrayRef<llvm::Metadata*> _meta(_metadata);
    llvm::MDTuple * metadata = llvm::MDTuple::get(context, _meta);
    // Insert new tuple into module
    auto named_metadata = a_Module->getOrInsertNamedMetadata(METADATA_NAME);
    if(named_metadata->getNumOperands() < 1)
    {
    	named_metadata->addOperand(metadata);
    }
    else
    {
    	named_metadata->setOperand(0, metadata);
    }
  }




  JITMetadata::JITMetadata() : m_Language(JITMetadata::DEFAULT_LANGUAGE){}




  void JITMetadata::addStruct(llvm::StringRef a_Name, std::vector<llvm::StringRef>& a_Members)
  {
    m_StructMemberNames[a_Name] = a_Members;
  }




  std::vector<llvm::StringRef> * JITMetadata::getStruct(llvm::StringRef a_Name)
  {
    auto result = m_StructMemberNames.find(a_Name);
    if (result != m_StructMemberNames.end())
    {
      return &(result->second);
    }

    return nullptr;
  }




  llvm::StringRef JITMetadata::getLanguage()
  {
    return m_Language;
  }




  void JITMetadata::setLanguage(llvm::StringRef a_Language)
  {
    m_Language = a_Language;
  }




  void JITMetadata::addRequiredModule(llvm::StringRef a_Required)
  {
    m_RequiredModules.push_back(a_Required);
  }




  std::vector<llvm::StringRef> & JITMetadata::getRequiredModules()
  {
    return m_RequiredModules;
  }
}
