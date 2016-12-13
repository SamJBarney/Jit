#include "catch.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include "../src/interface.h"
#include "../src/ModuleRepository.h"
#include "../src/Jit.h"
using namespace jit;

#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Bitcode/ReaderWriter.h>

const char * mod_search_paths[] =
{
  "",
  "test_files/",
  nullptr
};

TEST_CASE("I can load a module from disk", "[interface]")
{
  // Setup
  jit_init();
  jit_addModuleSearchPaths(mod_search_paths);
  jit_error_t error;
  const char * path = "main";

  // Test
  auto id = jit_loadModule(path, &error);

  // Validate
  REQUIRE(id != MODULE_INVALID_ID);

  // Cleanup
  jit_unloadModule(id, &error);
  jit_deinit();
}

TEST_CASE("I can unload a module after loading it", "[interface]")
{
    // Setup
    jit_init();
    jit_addModuleSearchPaths(mod_search_paths);
    jit_error_t error;
    const char * path = "main";
    auto id = jit_loadModule(path, &error);

    // Test
    jit_error_t err;
    auto result = jit_unloadModule(id, &err);

    // Validate
    REQUIRE(result == true);
    jit_deinit();
}

TEST_CASE("I can run a function from a module loaded from disk", "[interface]")
{
  typedef int(*entry)(int, char**);
  // Setup
  jit_init();
  jit_addModuleSearchPaths(mod_search_paths);
  jit_error_t error;
  const char * path = "main";

  // Test
  auto id = jit_loadModule(path, &error);

  // Validate
  REQUIRE(id != MODULE_INVALID_ID);

  entry func = (entry)jit_getModuleFunction(id, "main");
  REQUIRE(func != nullptr);
  REQUIRE(func(1, nullptr) == 0);



  // Cleanup
  jit_unloadModule(id, &error);
  jit_deinit();
}
