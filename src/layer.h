// Minimal OpenXR API Layer Template
//
// This is a simplified C implementation of an OpenXR API layer.
// Based on the OpenXR-Layer-Template by Matthieu Bucchianeri.

#ifndef LAYER_H
#define LAYER_H

#include "layer_config.h"
#include <stdbool.h>
#include <stdint.h>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct layer_context_t {
    XrInstance                instance;
    PFN_xrGetInstanceProcAddr next_get_instance_proc_addr;
    // Auto-generated function pointers
    #include "generated/layer_gen.h"
} layer_context_t;
extern layer_context_t g_context;

int32_t  layer_context_init     (XrInstance instance, PFN_xrGetInstanceProcAddr next_proc_addr);
void     layer_context_cleanup  (void);
XrResult layer_resolve_functions(layer_context_t* ctx);

#ifdef __cplusplus
}
#endif

#endif // LAYER_H
