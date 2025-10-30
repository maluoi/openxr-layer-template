#!/usr/bin/env python3
"""
Simple OpenXR XML parser to extract function signatures

This parser reads the OpenXR registry XML and extracts function prototypes,
which can then be used for code generation.
"""

import xml.etree.ElementTree as ET
from dataclasses import dataclass
from typing import List, Optional

@dataclass
class Parameter:
    """Represents a function parameter"""
    type: str
    name: str
    full_decl: str  # Full declaration including const, pointers, etc.

@dataclass
class Command:
    """Represents an OpenXR command (function)"""
    name: str
    return_type: str
    params: List[Parameter]

    def get_param_list(self) -> str:
        """Get the full parameter list as a string"""
        if not self.params:
            return "void"
        return ", ".join(p.full_decl for p in self.params)

    def get_arg_list(self) -> str:
        """Get just the parameter names for forwarding calls"""
        return ", ".join(p.name for p in self.params)

class OpenXRParser:
    """Parser for OpenXR registry XML"""

    def __init__(self, xml_path: str):
        self.tree = ET.parse(xml_path)
        self.root = self.tree.getroot()
        self.commands = {}
        self._parse_commands()

    def _parse_commands(self):
        """Parse all command definitions from the registry"""
        # Note: There can be multiple <commands> nodes in the XML
        commands_nodes = self.root.findall('commands')
        if not commands_nodes:
            return

        for commands_node in commands_nodes:
            for command in commands_node.findall('command'):
                proto = command.find('proto')
                if proto is None:
                    continue

                # Get return type and function name
                return_type = self._get_type_from_proto(proto)
                name_elem = proto.find('name')
                if name_elem is None:
                    continue
                name = name_elem.text

                # Parse parameters
                params = []
                for param in command.findall('param'):
                    param_info = self._parse_param(param)
                    if param_info:
                        params.append(param_info)

                self.commands[name] = Command(
                    name=name,
                    return_type=return_type,
                    params=params
                )

    def _get_type_from_proto(self, proto) -> str:
        """Extract the return type from a proto element"""
        type_elem = proto.find('type')
        if type_elem is None:
            return "void"

        # Reconstruct the type with const and pointer qualifiers
        parts = []
        if proto.text:
            parts.append(proto.text.strip())
        parts.append(type_elem.text)
        if type_elem.tail:
            parts.append(type_elem.tail.strip())

        return " ".join(p for p in parts if p)

    def _parse_param(self, param) -> Optional[Parameter]:
        """Parse a parameter element"""
        name_elem = param.find('name')
        if name_elem is None:
            return None

        name = name_elem.text

        # Reconstruct the full declaration
        parts = []
        if param.text:
            parts.append(param.text.strip())

        type_elem = param.find('type')
        if type_elem is not None:
            parts.append(type_elem.text)
            if type_elem.tail:
                parts.append(type_elem.tail.strip())

        parts.append(name)

        if name_elem.tail:
            parts.append(name_elem.tail.strip())

        full_decl = " ".join(p for p in parts if p)

        # Clean up extra spaces
        full_decl = " ".join(full_decl.split())

        # Get just the type part (everything before the parameter name)
        type_str = full_decl.rsplit(name, 1)[0].strip()

        return Parameter(
            type=type_str,
            name=name,
            full_decl=full_decl
        )

    def get_command(self, name: str) -> Optional[Command]:
        """Get a command by name"""
        return self.commands.get(name)

    def list_commands(self) -> List[str]:
        """List all available command names"""
        return sorted(self.commands.keys())

def find_openxr_xml() -> Optional[str]:
    """Try to find the OpenXR registry XML file"""
    import os
    from pathlib import Path

    # Try CMake build directory first
    build_dir = Path(__file__).parent.parent / "build"
    if build_dir.exists():
        # Look for FetchContent downloaded SDK
        for xml_path in build_dir.rglob("xr.xml"):
            if "openxr-src" in str(xml_path):
                return str(xml_path)

    # Try common locations
    common_paths = [
        # Relative to this script
        Path(__file__).parent.parent / "external" / "OpenXR-SDK" / "specification" / "registry" / "xr.xml",
        Path(__file__).parent.parent / "external" / "OpenXR-SDK-Source" / "specification" / "registry" / "xr.xml",
        # System locations
        Path("/usr/share/openxr/registry/xr.xml"),
        Path("/usr/local/share/openxr/registry/xr.xml"),
    ]

    for path in common_paths:
        if path.exists():
            return str(path)

    return None

if __name__ == "__main__":
    # Test the parser
    xml_path = find_openxr_xml()
    if not xml_path:
        print("Could not find xr.xml - build the project first or set up OpenXR SDK")
        import sys
        sys.exit(1)

    print(f"Parsing: {xml_path}")
    parser = OpenXRParser(xml_path)

    # Test with a few functions
    test_functions = ["xrCreateSession", "xrGetSystem", "xrEnumerateSwapchainFormats"]

    for func_name in test_functions:
        cmd = parser.get_command(func_name)
        if cmd:
            print(f"\n{cmd.return_type} {cmd.name}({cmd.get_param_list()})")
        else:
            print(f"\n{func_name}: Not found")
