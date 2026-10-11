#!/usr/bin/env python3
"""Compile the OpenCL program assembled at startup, without requiring a GPU."""
import argparse
from pathlib import Path
import re
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--clang', default='clang', help='Clang executable')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1] / 'HighMap/src/gpu_opencl'
    includes = re.findall(r'#include "(kernels/[^"\n]+)"',
                          (root / 'gpu_opencl.cpp').read_text())
    if not includes:
        parser.error('No startup OpenCL sources found')
    parts = []
    for include in includes:
        text = (root / include).read_text().strip()
        if not (text.startswith('R""(') and text.endswith(')""')):
            parser.error(f'Unexpected OpenCL string wrapper: {include}')
        parts.append(f'#line 1 "{include}"\n' + text[4:-3])
    with tempfile.TemporaryDirectory() as directory:
        program = Path(directory) / 'startup.cl'
        program.write_text('\n'.join(parts))
        subprocess.run([args.clang, '-x', 'cl', '-cl-std=CL1.2',
                        '-Wno-unused-command-line-argument', '-fsyntax-only',
                        str(program)], check=True)
    print(f'Compiled all {len(includes)} startup OpenCL source fragments')


if __name__ == '__main__':
    main()
