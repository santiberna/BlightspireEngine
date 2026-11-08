import argparse
import os
import subprocess
import sys
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("--slangc", required=True)
    parser.add_argument("--dep-file", required=True)
    parser.add_argument("--shader-output", required=True)
    parser.add_argument("--shader-module", required=True)
    parser.add_argument("--shader", required=True)
    parser.add_argument("--shader-dir", required=True)
    parser.add_argument("--shader-module-dir", required=True)
    parser.add_argument("--debug-level", required=True, type=int)
    parser.add_argument("--optimization-level", required=True, type=int)

    args = parser.parse_args()

    # Read dependencies
    dep_content = Path(args.dep_file).read_text().strip()

    # Split dependencies and remove the first two entries:
    # 1. Shader module path
    # 2. Source Slang file
    dependencies = dep_content.split()
    dependencies = dependencies[2:]

    module_dependencies = []

    for dependency in dependencies:
        # Undo escaping performed in the dependency file
        dependency = dependency.replace("\\\\", "\\")
        dependency = dependency.replace("\\:", ":")

        # Compute path relative to SHADER_DIR
        shader_rel = os.path.relpath(dependency, args.shader_dir)

        module_dependencies.append(
            str(Path(args.shader_module_dir) / f"{shader_rel}-module")
        )

    # Configure compiler flags
    assert args.debug_level >= 0 and args.debug_level <= 3
    assert args.optimization_level >= 0 and args.optimization_level <= 3
    debug_cmd = "-g" + str(args.debug_level)
    opt_cmd = "-O" + str(args.optimization_level)

    # Build compiler command
    command = [
        args.slangc,
        "-I", args.shader_dir,
        *module_dependencies,
        args.shader_module,
        "-target", "spirv",
        "-o", args.shader_output,
        "-reflection-json", args.shader_output + ".json",
        "-warnings-disable", "39001", # Disable overlapping parameters warning, this is intended for our bindless set up.
        "-preserve-params",           # TODO: Should be replaced when we remove SpirvReflect and move to use the reflection-json output.
        "-emit-spirv-directly",
        debug_cmd,
        opt_cmd,
    ]

    # Execute compiler
    print("Executing:", subprocess.list2cmdline(command))

    result = subprocess.run(command)

    if result.returncode != 0:
        print("Shader compilation failed", file=sys.stderr)
        sys.exit(result.returncode)


if __name__ == "__main__":
    main()
