import os
from pathlib import Path

STUB='''/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

'''

if __name__ == '__main__':

    paths = ['include', 'src']

    for path in paths:
        print('path: ', path)
        files = list(Path(path).rglob("*.hpp")) + list(Path(path).rglob("*.cpp"))

        for f in files:
            print('file: ', f.name)
            content = f.read_text()
            if content.split('\n')[0] != STUB.split('\n')[0]:
                print('adding header...')
                content = STUB + content
                f.write_text(content);
