#include "catch.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include "../src/Jit.h"
#include "../src/ModuleRepository.h"
using namespace jit;


TEST_CASE("I can get a Module back from the module repository by id", "[ModuleRepository]")
{
  // Setup
  Jit::Jit jit;
  llvm::StringRef name("test");
	llvm::Module * _module = new llvm::Module(name, *(jit.getContext()));
  auto repo = jit.getModuleRepository();
  JITModule* module = new JITModule(_module);

  // Test
  REQUIRE(repo->add(module) != MODULE_INVALID_ID);

  // Validate
  auto result = repo->get(0);
  REQUIRE(result == module);

  // Cleanup
  repo->unload(0);
}

TEST_CASE("I get a valid module id back when searching by name", "[ModuleRepository]")
{
  // Setup
  Jit::Jit jit;
  llvm::StringRef name("test");
	llvm::Module * _module = new llvm::Module(name, *(jit.getContext()));
  auto repo = jit.getModuleRepository();
  JITModule* module = new JITModule(_module);

  // Test
  REQUIRE(repo->add(module) != MODULE_INVALID_ID);

  // Validate
  auto id = repo->get(name);
  REQUIRE(id != MODULE_INVALID_ID);
  REQUIRE(id == 0);

  // Cleanup
  repo->unload(0);
}
