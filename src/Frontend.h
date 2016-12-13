#pragma once

#include <fstream>
#include <string>

#include "JITMetadata.h"

#include <llvm/IR/Module.h>

namespace jit {
	class JITMetadata;

	class Frontend
	{
		Frontend(){}
	public:
		virtual bool compile(llvm::Module * a_Module, JITMetadata * a_Metadata, std::ifstream & a_Stream) = 0;
	};
}
