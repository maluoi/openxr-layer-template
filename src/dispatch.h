// Dispatch functions for the OpenXR API layer

#ifndef DISPATCH_H
#define DISPATCH_H

#include <openxr/openxr.h>
#include <openxr/openxr_loader_negotiation.h>

#if defined(_WIN32)
	#define LAYER_EXPORT __declspec(dllexport)
#else
	#define LAYER_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Main entry point called by the OpenXR loader
LAYER_EXPORT XRAPI_ATTR XrResult XRAPI_CALL xrNegotiateLoaderApiLayerInterface(
	const XrNegotiateLoaderInfo* loaderInfo,
	const char*                  layerName,
	XrNegotiateApiLayerRequest*  apiLayerRequest);

// Layer instance creation
LAYER_EXPORT XRAPI_ATTR XrResult XRAPI_CALL xrCreateApiLayerInstance(
	const XrInstanceCreateInfo*  instanceCreateInfo,
	const XrApiLayerCreateInfo*  apiLayerInfo,
	XrInstance*                  instance);

// Function pointer dispatch
LAYER_EXPORT XRAPI_ATTR XrResult XRAPI_CALL xrGetInstanceProcAddr(
	XrInstance                   instance,
	const char*                  name,
	PFN_xrVoidFunction*          function);

#ifdef __cplusplus
}
#endif

#endif // DISPATCH_H
