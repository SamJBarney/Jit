cmake_minimum_required(VERSION 3.5.2)
set (CMAKE_CXX_STANDARD 11)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(
	src
	${LLVM_INCLUDE_DIRS}
)

add_definitions(${LLVM_DEFINITIONS})
# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs all mcjit)

set(SRC
	src/FrontendRepository.cpp
	src/interface.cpp
	src/Jit.cpp
	src/JITMetadata.cpp
	src/JITModule.cpp
	src/ModuleRepository.cpp
)

add_library(jit SHARED "${SRC}")


target_link_libraries(jit ${llvm_libs})
# target_link_libraries(jit m z curses ${LIBS})

file(MAKE_DIRECTORY test_files)
file(COPY tests/test_files/main DESTINATION test_files)



enable_testing(true)  # Enables unit-testing.

# add_executable(howuse tests/HowToUseJIT.cpp)
# target_link_libraries(howuse ${llvm_libs})

add_executable(tests tests/JITMetadata.cpp tests/ModuleRepository.cpp tests/interface.cpp)
# add_executable(tests tests/JITMetadata.cpp)
target_link_libraries(tests jit)

add_test(NAME Tests COMMAND tests)
