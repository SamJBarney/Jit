#pragma once

#include <vector>
#include <map>

#include <llvm/IR/Module.h>

namespace jit
{
  class JITMetadata
  {
    llvm::StringRef m_Language;
    std::vector<llvm::StringRef> m_RequiredModules;
    std::map<llvm::StringRef, std::vector<llvm::StringRef> > m_StructMemberNames;
  public:
    static const char * METADATA_NAME;
    static const char * DEFAULT_LANGUAGE;
    static JITMetadata * get(const llvm::Module*);
    static void set(llvm::Module*, const JITMetadata*);

    JITMetadata();

    void addStruct(llvm::StringRef, std::vector<llvm::StringRef>&);
    std::vector<llvm::StringRef>* getStruct(llvm::StringRef);
    llvm::StringRef getLanguage();
    void setLanguage(llvm::StringRef);
    void addRequiredModule(llvm::StringRef);
    std::vector<llvm::StringRef> & getRequiredModules();
  };
}
