#include "layer_config.h"
#include "layer.h"
#include "dispatch.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

layer_context_t g_context = {0};

int32_t layer_context_init(XrInstance instance, PFN_xrGetInstanceProcAddr next_proc_addr) {
	g_context = (layer_context_t){
		.instance                    = instance,
		.next_get_instance_proc_addr = next_proc_addr,
	};

	// Resolve function pointers we need
	if (XR_FAILED(layer_resolve_functions(&g_context))) {
		fprintf(stderr, "%s: Failed to resolve functions\n", LAYER_NAME);
		return -1;
	}

	// Resolve auto-generated functions
	extern XrResult resolve_generated_functions(layer_context_t* ctx);
	if (XR_FAILED(resolve_generated_functions(&g_context))) {
		fprintf(stderr, "%s: Failed to resolve generated functions\n", LAYER_NAME);
		return -1;
	}

	printf("%s: Layer context initialized\n", LAYER_NAME);

	return 0;
}

void layer_context_cleanup(void) {
	printf("%s: Cleaning up layer context\n", LAYER_NAME);
	g_context = (layer_context_t){};
}

XrResult layer_resolve_functions(layer_context_t* ctx) {
	if (!ctx || !ctx->next_get_instance_proc_addr) {
		return XR_ERROR_INITIALIZATION_FAILED;
	}

	// Add custom manual function resolutions here if needed
	// (Most functions are auto-resolved in resolve_generated_functions)

	return XR_SUCCESS;
}
