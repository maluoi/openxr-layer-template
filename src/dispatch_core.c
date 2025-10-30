// Dispatch implementation - handles routing OpenXR calls

#include "layer_config.h"
#include "dispatch.h"
#include "layer.h"
#include <string.h>
#include <stdio.h>

// Forward declarations for manually implemented interceptors
static XRAPI_ATTR XrResult XRAPI_CALL layer_xrDestroyInstance(XrInstance instance);

// Auto-generated forward declarations
#include "generated/dispatch_gen.h"

// Negotiation with the loader
XRAPI_ATTR XrResult XRAPI_CALL xrNegotiateLoaderApiLayerInterface(const XrNegotiateLoaderInfo* loaderInfo, const char* layerName, XrNegotiateApiLayerRequest* apiLayerRequest) {
	if (!loaderInfo || !apiLayerRequest) {
		return XR_ERROR_INITIALIZATION_FAILED;
	}

	// Validate structures
	if (loaderInfo->structType    != XR_LOADER_INTERFACE_STRUCT_LOADER_INFO ||
		loaderInfo->structVersion != XR_LOADER_INFO_STRUCT_VERSION          ||
		loaderInfo->structSize    != sizeof(XrNegotiateLoaderInfo)) {
		return XR_ERROR_INITIALIZATION_FAILED;
	}

	if (apiLayerRequest->structType    != XR_LOADER_INTERFACE_STRUCT_API_LAYER_REQUEST ||
		apiLayerRequest->structVersion != XR_API_LAYER_INFO_STRUCT_VERSION             ||
		apiLayerRequest->structSize    != sizeof(XrNegotiateApiLayerRequest)) {
		return XR_ERROR_INITIALIZATION_FAILED;
	}

	// Check version compatibility
	if (loaderInfo->minInterfaceVersion > XR_CURRENT_LOADER_API_LAYER_VERSION ||
		loaderInfo->maxInterfaceVersion < XR_CURRENT_LOADER_API_LAYER_VERSION ||
		loaderInfo->minApiVersion       > XR_CURRENT_API_VERSION              ||
		loaderInfo->maxApiVersion       < XR_CURRENT_API_VERSION) {
		return XR_ERROR_INITIALIZATION_FAILED;
	}

	// Verify layer name if provided
	if (layerName && strcmp(layerName, LAYER_NAME) != 0) {
		fprintf(stderr, "Layer name mismatch: expected %s, got %s\n", LAYER_NAME, layerName);
		return XR_ERROR_INITIALIZATION_FAILED;
	}

	// Set up our layer
	apiLayerRequest->layerInterfaceVersion  = XR_CURRENT_LOADER_API_LAYER_VERSION;
	apiLayerRequest->layerApiVersion        = XR_CURRENT_API_VERSION;
	apiLayerRequest->getInstanceProcAddr    = xrGetInstanceProcAddr;
	apiLayerRequest->createApiLayerInstance = xrCreateApiLayerInstance;

	printf("%s: Layer initialized\n", LAYER_NAME);

	return XR_SUCCESS;
}

// Create the layer instance
XRAPI_ATTR XrResult XRAPI_CALL xrCreateApiLayerInstance(const XrInstanceCreateInfo* instanceCreateInfo, const XrApiLayerCreateInfo* apiLayerInfo, XrInstance* instance) {
	if (!instanceCreateInfo || !apiLayerInfo || !instance) {
		return XR_ERROR_INITIALIZATION_FAILED;
	}

	// Validate API layer info
	if (!apiLayerInfo->nextInfo ||
		apiLayerInfo->nextInfo->structType    != XR_LOADER_INTERFACE_STRUCT_API_LAYER_NEXT_INFO ||
		apiLayerInfo->nextInfo->structVersion != XR_API_LAYER_NEXT_INFO_STRUCT_VERSION          ||
		strcmp(apiLayerInfo->nextInfo->layerName, LAYER_NAME) != 0                              ||
		!apiLayerInfo->nextInfo->nextGetInstanceProcAddr                                        ||
		!apiLayerInfo->nextInfo->nextCreateApiLayerInstance) {
		return XR_ERROR_INITIALIZATION_FAILED;
	}

	// Create instance through the chain
	XrApiLayerCreateInfo chainApiLayerInfo = *apiLayerInfo;
	chainApiLayerInfo.nextInfo = apiLayerInfo->nextInfo->next;

	XrResult result = apiLayerInfo->nextInfo->nextCreateApiLayerInstance(instanceCreateInfo, &chainApiLayerInfo, instance);

	if (XR_SUCCEEDED(result)) {
		// Initialize our layer context
		if (layer_context_init(*instance, apiLayerInfo->nextInfo->nextGetInstanceProcAddr) != 0) {
			// Cleanup on failure
			PFN_xrDestroyInstance destroyInstance = NULL;
			apiLayerInfo->nextInfo->nextGetInstanceProcAddr(*instance, "xrDestroyInstance", (PFN_xrVoidFunction*)&destroyInstance);
			if (destroyInstance) {
				destroyInstance(*instance);
			}
			return XR_ERROR_INITIALIZATION_FAILED;
		}

		printf("%s: Instance created for application: %s\n", LAYER_NAME, instanceCreateInfo->applicationInfo.applicationName);
	}

	return result;
}

// Dispatch function calls
XRAPI_ATTR XrResult XRAPI_CALL xrGetInstanceProcAddr(XrInstance instance, const char* name, PFN_xrVoidFunction* function) {
	if (!name || !function) {
		return XR_ERROR_VALIDATION_FAILURE;
	}

	if (strcmp(name, "xrGetInstanceProcAddr") == 0) {
		*function = (PFN_xrVoidFunction)xrGetInstanceProcAddr;
		return XR_SUCCESS;
	}
	if (strcmp(name, "xrDestroyInstance") == 0) {
		*function = (PFN_xrVoidFunction)layer_xrDestroyInstance;
		return XR_SUCCESS;
	}

	// Auto-generated dispatch cases
	#include "generated/dispatch_gen_cases.inc"

	// Pass through to next layer for all other functions
	return g_context.next_get_instance_proc_addr(instance, name, function);
}

// Manually implemented interceptors

static XRAPI_ATTR XrResult XRAPI_CALL layer_xrDestroyInstance(XrInstance instance) {
	printf("%s: Destroying instance\n", LAYER_NAME);

	XrResult result = g_context.xrDestroyInstance(instance);

	if (XR_SUCCEEDED(result)) {
		layer_context_cleanup();
	}

	return result;
}
