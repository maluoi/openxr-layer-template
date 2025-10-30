#!/usr/bin/env python3
"""
Layer Configuration

Edit this file to specify which OpenXR functions your layer will intercept
and which functions from the runtime you need to call.
"""

# Functions that your layer will intercept and potentially modify behavior
# These will get wrapper functions generated in dispatch.c
override_functions = [
    "xrGetSystem",
    "xrCreateSession",
]

# Functions that your layer needs to call from the next layer/runtime
# These will get function pointer declarations and resolution code generated
requested_functions = [
    "xrGetInstanceProperties",
    "xrGetSystemProperties",
]

# OpenXR extensions that your layer uses
# Format: list of extension name strings
# Example: ["XR_KHR_vulkan_enable", "XR_KHR_composition_layer_depth"]
extensions = []

# Note: The following functions are automatically handled and should NOT be listed:
# - xrCreateInstance (implicitly overridden)
# - xrDestroyInstance (implicitly overridden)
# - xrGetInstanceProcAddr (implicitly overridden)
# - xrEnumerateInstanceExtensionProperties (implicitly overridden)
