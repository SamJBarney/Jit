#pragma once

#ifndef __cplusplus
#include <stdbool.h>
#include <stdint.h>
#else
#include <cstdint>
#endif

typedef uint32_t module_id_t;
#define MODULE_INVALID_ID UINT32_MAX

#define ERR_MISSING_FRONTEND 1
#define ERR_COMPILATION_ERR  2
#define ERR_FILE_MISSING     3

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int code;
	char * message;
} jit_error_t;

#ifdef __cplusplus
}
#endif
