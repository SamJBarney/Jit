#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif


void jit_init();

void jit_deinit();

void jit_addModuleSearchPaths(const char ** paths);

void jit_addFrontendSearchPaths(const char ** paths);

void jit_addLibrarySearchPaths(const char ** paths);

void jit_removeModuleSearchPaths(const char ** paths);

void jit_removeFrontendSearchPaths(const char ** paths);

void jit_removeLibrarySearchPaths(const char ** paths);

module_id_t jit_compile(const char * compiler, const char * module_name, const char ** files, jit_error_t * error);

module_id_t jit_loadModule(const char * module_path, jit_error_t * error);

module_id_t jit_getModule(const char * module_name, jit_error_t * error);

bool jit_unloadModule(module_id_t id, jit_error_t * error);

void * jit_getModuleFunction(module_id_t id, const char * name);

void * jit_getFunction(const char * name);




#ifdef __cplusplus
}
#endif
