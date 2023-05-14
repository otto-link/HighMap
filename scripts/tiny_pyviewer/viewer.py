#!/usr/bin/python3
import numpy as np
import os
import sys

import tools

if __name__ == "__main__":
    # 1st argument is the filename
    fname = os.path.abspath(sys.argv[1])
    print('fname: {}'.format(fname))

    # load heightmap
    w = np.fromfile(fname, dtype=np.float32)
    n = int(np.sqrt(len(w)))
    shape = (n, n)
    w = w.reshape(shape)
    print('shape: {}'.format(shape))

    # rescale
    w = 0.2 * (w - w.min()) / (w.max() - w.min())
    
    mesh = tools.hmap_to_trimesh(w)
    tools.show(mesh)
