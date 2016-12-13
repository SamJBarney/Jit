#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include "../src/Jit.h"
#include "../src/JITMetadata.h"
using namespace jit;

extern llvm::LLVMContext context;

TEST_CASE("I can add an empty JITMetadata to an llvm::Module", "[JITMetadata]")
{
	Jit::Jit jit;
	auto module_name = llvm::StringRef("test");
	llvm::Module module(module_name, *(jit.getContext()));
	JITMetadata jitmetadata;

	REQUIRE(module.getNamedMetadata(JITMetadata::METADATA_NAME) == nullptr);
	// Test
	JITMetadata::set(&module, &jitmetadata);

	// Validate
	auto metadata = module.getNamedMetadata(JITMetadata::METADATA_NAME);
	REQUIRE(metadata != nullptr);
	REQUIRE(metadata->getNumOperands() == 1);

}

TEST_CASE("I can get JITMetadata from an llvm::Module", "[JITMetadata]")
{
	// Setup
		Jit::Jit jit;
	llvm::Module module(llvm::StringRef("test"), *(jit.getContext()));
	{
		JITMetadata metadata;
		metadata.setLanguage("cish");
		metadata.addRequiredModule("some.module.to.depend.upon");
		metadata.addRequiredModule("another.module.to.depend.upon");
		JITMetadata::set(&module, &metadata);
	}

	// Test
	auto metadata = JITMetadata::get(&module);

	REQUIRE(metadata->getLanguage().str() == "cish");
	auto & required_modules = metadata->getRequiredModules();
	REQUIRE(required_modules.size() == 2);
	REQUIRE(required_modules[0].str() == "some.module.to.depend.upon");
	REQUIRE(required_modules[1].str() == "another.module.to.depend.upon");
	REQUIRE(metadata->getStruct("asdf") == nullptr);
}
