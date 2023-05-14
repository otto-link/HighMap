import numpy as np
import pyrender
import trimesh

def hmap_to_trimesh(z, x=None, y=None, add_skirt=True):
    """
    Create a triangular mesh based on a given heightmap.

    Parameters
    ----------
    z : ndarray
        Input array, 2-dimensional.
    x : ndarray, optional
        'x' coordinate values, 1D-array or as a meshgrid ('ij' indexing).
        Default is none.
    y : ndarray, optional
        'x' coordinate values, 1D-array or as a meshgrid ('ij' indexing).
        Default is none.
    add_skirt : bool, optional
        Add a 'skirt' to the mesh. Default is True.

    Returns
    -------
    out : trimesh.Trimesh object
        The mesh.
    """

    nx, ny = z.shape

    if add_skirt:
        z0 = z.min() - 0.05 * z.ptp()
        z[0, :] = z0
        z[nx - 1, :] = z0
        z[:, 0] = z0
        z[:, ny - 1] = z0

    if x is None:
        x, y = np.meshgrid(np.linspace(0, nx / max(nx, ny), nx),
                           np.linspace(0, ny / max(nx, ny), ny),
                           indexing='ij')
    else:
        x, y = np.meshgrid(x, y, indexing='ij')

    xyz = np.vstack((x.flatten(), y.flatten(), z.flatten())).T

    nn = nx * ny
    idx = np.linspace(0, nn - 1, nn).astype(int)

    faces = []
    for j in range(nx - 1):
        i1 = j * ny
        faces += list(
            np.vstack((
                idx[i1:i1 + ny - 1],
                idx[i1 + ny + 1:i1 + 2 * ny],
                idx[i1 + 1:i1 + ny],
            )).T)
        faces += list(
            np.vstack((
                idx[i1:i1 + ny - 1],
                idx[i1 + ny:i1 + 2 * ny - 1],
                idx[i1 + ny + 1:i1 + 2 * ny],
            )).T)

    return trimesh.Trimesh(vertices=xyz, faces=faces)


def show(mesh, print_kcmds=True, offscreen=False, plot_image=True):
    """
    Open rendering window and render the given mesh(es).

    Parameters
    ----------
    mesh : trimesh.Trimesh, singleton or list
        Mesh(es) to render.
    print_kcmds : bool
        Print the viewer keyboard commands in the console. Default is True.
    offscreen : bool, optional
        Turn on or off the off screen rendering. Default is False.
    plot_image : bool, optional
        Plot offscreen rendering. Default is True.

    Returns
    -------
    None
    """

    if type(mesh) != list:
        mesh = [mesh]

    scene = pyrender.Scene(ambient_light=np.array([0.2, 0.2, 0.2, 1.0]),
                           bg_color=(15, 15, 15))

    centroid = np.zeros(3)
    for m in mesh:
        centroid += np.array(m.centroid)
        rmesh = pyrender.Mesh.from_trimesh(m, smooth=True)
        scene.add(rmesh)
    centroid /= len(mesh)

    if print_kcmds and not (offscreen):
        print_keyboard_cmds()

    if not (offscreen):
        viewer = pyrender.Viewer(scene,
                                 viewport_size=(800, 600),
                                 use_direct_lighting=True,
                                 cull_faces=False,
                                 shadows=False,
                                 show_world_axis=False,
                                 rotate_rate=np.pi / 20)

        cam_pose = viewer._camera_node.matrix

        return None

    else:
        os.environ['PYOPENGL_PLATFORM'] = 'egl'
        light = pyrender.SpotLight(color=np.ones(3),
                                   intensity=15.0,
                                   innerConeAngle=np.pi / 2.5,
                                   outerConeAngle=np.pi / 2)

        cam = pyrender.PerspectiveCamera(yfov=(np.pi / 3.0))

        scale = 0.9
        s2 = 1.0 / np.sqrt(2.0)
        cp = np.eye(4)
        cp[:3, :3] = np.array([[0.0, -s2, s2], [1.0, 0.0, 0.0], [0.0, s2, s2]])
        hfov = np.pi / 6.0
        dist = scale / (2.0 * np.tan(hfov))
        cp[:3, 3] = dist * np.array([1.0, 0.0, 1.0]) + centroid

        cam_pose = cp

        scene.add(cam, pose=cam_pose)

        # --- side light
        cp = np.eye(4)
        alpha = 45 / 180 * np.pi
        ca = np.cos(alpha)
        sa = np.sin(alpha)
        cp[:3, :3] = np.array([[0.0, sa, -ca], [-ca, 0.0, 0.0], [0.0, 1.0,
                                                                 sa]])
        # cp[:3, 3] = dist * np.array([-1.5, 0.0, 1.5]) + centroid
        cp[:3, 3] = dist * np.array([0.0, -1.5, 1.5]) + centroid
        cam_pose = cp

        scene.add(light, pose=cam_pose)

        r = pyrender.OffscreenRenderer(viewport_width=640 * 2,
                                       viewport_height=480 * 2)
        flags = pyrender.RenderFlags.SHADOWS_SPOT
        img, depth = r.render(scene, flags=flags)
        r.delete()

        if plot_image:
            plt.figure()
            plt.axis('off')
            plt.imshow(img)

        return img

def print_keyboard_cmds():
    """Print rendering GUI keyboard commands."""

    print('''
    a: Toggles rotational animation mode.
    c: Toggles backface culling.
    f: Toggles fullscreen mode.
    h: Toggles shadow rendering.
    i: Toggles axis display mode (no axes, world axis, mesh axes, all axes).
    l: Toggles lighting mode (scene lighting, Raymond lighting, or direct lighting).
    m: Toggles face normal visualization.
    n: Toggles vertex normal visualization.
    o: Toggles orthographic mode.
    q: Quits the viewer.
    r: Starts recording a GIF, and pressing again stops recording and opens a file dialog.
    s: Opens a file dialog to save the current view as an image.
    w: Toggles wireframe mode (scene default, flip wireframes, all wireframe, or all solid).
    z: Resets the camera to the initial view.
    ''')    
