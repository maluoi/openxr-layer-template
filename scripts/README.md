# Code Generation Scripts

These scripts help you set up your OpenXR layer by generating boilerplate code! NOTE that this will **_replace_** everything in the `/src/generated` folder, and `/src/dispatch.c`, so if you've already been coding, make sure you've got your code backed up!

## Quick Start

### 1. Configure What Functions to Intercept

Edit `layer_config.py`:

```python
# Functions your layer will intercept
override_functions = [
    "xrGetSystem",
    "xrCreateSession",
    "xrBeginFrame",  # Add your functions here
]

# Functions your layer needs from the runtime
requested_functions = [
    "xrGetInstanceProperties",
    "xrGetSystemProperties",
    "xrWaitFrame",  # Add functions you need to call
]
```

### 2. Generate Code Snippets

```bash
# From project root
python3 scripts/generate_layer.py # Generate files
```

### 3. Profit

Now just fill out your functions in `/src/dispatch.c`!