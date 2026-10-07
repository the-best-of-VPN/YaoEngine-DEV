"""Forged axe and ironbound timber shield for the Orc Warchief scene.

All geometry is generated locally; build_weapons returns every created object.
The character faces -Y.  The shaft passes through the right fist at
(-1.98, -.37, 2.94), and the shield centre is (2.05, -.72, 3.30).
"""
import bpy
import math
from mathutils import Vector


def build_weapons(collection, mats):
    objects = []

    def finish(obj, name, mat, smooth=False, bevel=0):
        obj.name = name
        for old in list(obj.users_collection):
            old.objects.unlink(obj)
        collection.objects.link(obj)
        if mat:
            obj.data.materials.append(mats[mat] if isinstance(mat, str) else mat)
        if obj.type == 'MESH':
            for polygon in obj.data.polygons:
                polygon.use_smooth = smooth
            if bevel:
                mod = obj.modifiers.new('Soft forged edges', 'BEVEL')
                mod.width = bevel
                mod.segments = 2
        objects.append(obj)
        return obj

    def mesh(name, verts, faces, mat, bevel=0, smooth=False):
        data = bpy.data.meshes.new(name + ' mesh')
        data.from_pydata(verts, [], faces)
        data.update()
        obj = bpy.data.objects.new(name, data)
        collection.objects.link(obj)
        return finish(obj, name, mat, smooth, bevel)

    def rod(name, a, b, radius, mat, radius2=None, vertices=12):
        a, b = Vector(a), Vector(b)
        delta = b - a
        bpy.ops.mesh.primitive_cone_add(vertices=vertices, radius1=radius,
                                        radius2=radius if radius2 is None else radius2,
                                        depth=delta.length, location=(a + b) * .5)
        obj = bpy.context.object
        obj.rotation_euler = delta.to_track_quat('Z', 'Y').to_euler()
        return finish(obj, name, mat, smooth=True, bevel=.006)

    def sphere(name, location, scale, mat, segments=12):
        bpy.ops.mesh.primitive_uv_sphere_add(segments=segments, ring_count=8,
                                           radius=1, location=location)
        obj = bpy.context.object
        obj.scale = scale
        return finish(obj, name, mat, True)

    def line(name, coords, radius, mat):
        data = bpy.data.curves.new(name + ' curve', 'CURVE')
        data.dimensions = '3D'
        data.resolution_u = 1
        data.bevel_depth = radius
        data.bevel_resolution = 2
        spline = data.splines.new('POLY')
        spline.points.add(len(coords) - 1)
        for pt, xyz in zip(spline.points, coords):
            pt.co = (*xyz, 1)
        obj = bpy.data.objects.new(name, data)
        collection.objects.link(obj)
        return finish(obj, name, mat)

    def slab(name, profile, front, back, mat, bevel=.008):
        """An extruded X/Z polygon with front and rear Y coordinates."""
        n = len(profile)
        verts = [(x, front, z) for x, z in profile]
        verts += [(x, back, z) for x, z in profile]
        faces = [tuple(range(n - 1, -1, -1)), tuple(range(n, 2 * n))]
        faces.extend((i, (i + 1) % n, (i + 1) % n + n, i + n)
                     for i in range(n))
        return mesh(name, verts, faces, mat, bevel)

    def ring(name, center, radius, thickness, mat):
        bpy.ops.mesh.primitive_torus_add(major_radius=radius, minor_radius=thickness,
                                         major_segments=48, minor_segments=8,
                                         location=center, rotation=(math.pi / 2, 0, 0))
        return finish(bpy.context.object, name, mat, True)

    # HEAVY SINGLE-EDGED AXE.  The outward blade leaves the torso unobstructed.
    ax, ay = -1.98, -.37
    rod('Axe | ashwood haft', (ax, ay, .89), (ax, ay, 5.09), .079, 'wood', .095, 16)
    rod('Axe | steel pommel', (ax, ay, .83), (ax, ay, 1.03), .116, 'darkmetal', .09)
    rod('Axe | pommel gold lip', (ax, ay, .89), (ax, ay, .92), .122, 'gold')
    rod('Axe | haft toe', (ax, ay, .81), (ax, ay, .88), .035, 'edge', .11)
    rod('Axe | handgrip leather', (ax, ay, 2.58), (ax, ay, 3.27), .105, 'leather')
    for z in (2.57, 3.27):
        rod('Axe | grip ferrule', (ax, ay, z - .024), (ax, ay, z + .024), .119, 'gold')
    helix = []
    for i in range(241):
        t = i / 240
        a = t * math.tau * 7
        helix.append((ax + .110 * math.cos(a), ay + .110 * math.sin(a), 2.61 + .63 * t))
    line('Axe | crimson wrapped grip', helix, .018, 'red')
    # Lower bindings repeat a restrained rhythm along the visible handle.
    for z in (1.12, 1.19, 1.26, 3.81, 3.90, 3.99):
        rod('Axe | leather haft binding', (ax, ay, z), (ax, ay, z + .047), .09, 'leather')
    line('Axe | wood grain one', [(ax - .036, ay - .074, z) for z in (1.34, 1.63, 1.98, 2.30, 2.54)], .004, 'leather')
    line('Axe | wood grain two', [(ax + .030, ay - .077, z) for z in (3.34, 3.50, 3.78)], .003, 'leather')
    # Collar gives the axe head a mechanically credible socket.
    rod('Axe | iron head socket', (ax, ay, 4.34), (ax, ay, 5.11), .145, 'darkmetal', .12, 12)
    for z in (4.36, 4.99, 5.095):
        rod('Axe | head socket bronze ring', (ax, ay, z - .025), (ax, ay, z + .025), .15, 'gold')
    core = [(-.08, .22), (-.35, .31), (-.67, .54), (-.96, .58),
            (-1.12, .39), (-1.19, .05), (-1.10, -.35), (-.86, -.63),
            (-.68, -.30), (-.33, -.13), (-.08, -.14)]
    profile = [(ax + x, 4.72 + z) for x, z in core]
    slab('Axe | broad forged blade', profile, ay - .115, ay + .115, 'steel', .023)
    # Wide sharpened cutting bevel, modelled on both sides rather than painted.
    outer = [(-.96, .58), (-1.12, .39), (-1.19, .05), (-1.10, -.35), (-.86, -.63)]
    inner = [(-.81, .40), (-.93, .26), (-.99, .035), (-.92, -.25), (-.78, -.43)]
    for front in (True, False):
        yf = ay - .122 if front else ay + .122
        ye = ay - .030 if front else ay + .030
        verts = [(ax + x, ye, 4.72 + z) for x, z in outer]
        verts += [(ax + x, yf, 4.72 + z) for x, z in inner]
        faces = [(i, i + 1, 6 + i, 5 + i) for i in range(4)]
        mesh('Axe | silver cutting bevel ' + ('front' if front else 'rear'), verts, faces, 'edge')
    # Decorative raised cheek plate echoes the wedge profile.
    plate = [(-.15, .135), (-.37, .18), (-.63, .33), (-.77, .31),
             (-.82, .07), (-.74, -.16), (-.59, -.13), (-.30, -.05), (-.15, -.045)]
    slab('Axe | inset iron cheek', [(ax + x, 4.72 + z) for x, z in plate], ay - .139, ay - .114, 'darkmetal', .009)
    for dx, dz in ((-.26, .075), (-.58, .15), (-.74, .05)):
        sphere('Axe | bronze cheek rivet', (ax + dx, ay - .155, 4.72 + dz), (.034, .018, .034), 'gold')
    # Compact back spike stays well outside the shoulder.
    rod('Axe | rear pick root', (ax + .05, ay, 4.81), (ax + .26, ay, 4.84), .115, 'darkmetal', .082, 8)
    rod('Axe | rear pick tip', (ax + .25, ay, 4.84), (ax + .39, ay, 4.89), .082, 'edge', .006, 8)
    line('Axe | copper rune', [(ax - .52, ay - .157, 4.99), (ax - .64, ay - .157, 4.83),
                              (ax - .54, ay - .157, 4.70)], .013, 'gold')
    # Small shallow edge scars maintain a hand-forged look.
    for n, (x, z) in enumerate(((-1.079, .29), (-1.134, -.025), (-1.025, -.34))):
        line('Axe | cutting scar %02d' % n,
             [(ax + x, ay - .058, 4.72 + z), (ax + x + .07, ay - .104, 4.72 + z + .042)], .004, 'darkmetal')

    # ROUND SHIELD. Convincing construction: six clipped timber boards,
    # backed iron hoop, broad bevelled rim, bronze rivets, and a forged boss.
    sx, sy, sz = 2.05, -.72, 3.30
    rod('Shield | inner iron backing', (sx, sy + .10, sz), (sx, sy + .07, sz), .624, 'darkmetal', vertices=48)
    radius = .592
    circle = [(radius * math.cos(i * math.tau / 96), radius * math.sin(i * math.tau / 96)) for i in range(96)]

    def clip(poly, x, keep_greater):
        out = []
        for a, b in zip(poly, poly[1:] + poly[:1]):
            ina = a[0] >= x if keep_greater else a[0] <= x
            inb = b[0] >= x if keep_greater else b[0] <= x
            if ina:
                out.append(a)
            if ina != inb:
                t = (x - a[0]) / (b[0] - a[0])
                out.append((x, a[1] + t * (b[1] - a[1])))
        return out

    for i in range(6):
        left = -radius + i * radius / 3 + .007
        right = -radius + (i + 1) * radius / 3 - .007
        poly = clip(clip(circle, left, True), right, False)
        slab('Shield | timber plank %02d' % (i + 1), [(sx + x, sz + z) for x, z in poly], sy - .10, sy + .08, 'wood', .007)
        # Varied parallel scratches are real shallow dark filaments.
        for k, offset in enumerate((.043, .099, .145)):
            local_x = left + offset
            if abs(local_x) < radius - .045:
                h = math.sqrt(max(0, (radius - .045) ** 2 - local_x ** 2))
                if h > .1:
                    line('Shield | timber grain %02d %02d' % (i, k),
                         [(sx + local_x, sy - .108, sz - h * .86),
                          (sx + local_x + .008, sy - .109, sz - h * .20),
                          (sx + local_x - .005, sy - .108, sz + h * .37),
                          (sx + local_x + .003, sy - .108, sz + h * .88)],
                         .0035 if k == 1 else .0025, 'leather')
    # Radial ring cross section, with flattened front and bevelled shoulders.
    section = [(.555, -.121), (.577, -.153), (.639, -.153), (.671, -.118),
               (.671, .075), (.639, .115), (.579, .115), (.555, .082)]
    verts, faces = [], []
    for r, y in section:
        verts.extend((sx + r * math.cos(j * math.tau / 64), sy + y,
                      sz + r * math.sin(j * math.tau / 64)) for j in range(64))
    for k in range(len(section)):
        for j in range(64):
            faces.append((k * 64 + j, k * 64 + (j + 1) % 64,
                          ((k + 1) % len(section)) * 64 + (j + 1) % 64,
                          ((k + 1) % len(section)) * 64 + j))
    mesh('Shield | broad forged rim', verts, faces, 'steel', .004, True)
    ring('Shield | burnished outer lip', (sx, sy - .132, sz), .649, .012, 'edge')
    ring('Shield | narrow bronze inner lip', (sx, sy - .132, sz), .568, .008, 'gold')
    for i in range(16):
        a = i * math.tau / 16
        sphere('Shield | rim rivet %02d' % i,
               (sx + .606 * math.cos(a), sy - .170, sz + .606 * math.sin(a)),
               (.024, .012, .024), 'gold')
    # Front red paint, as extremely thin irregular polygons, partly hidden by boss.
    for i, profile in enumerate((
            [(-.33, .36), (-.24, .38), (.18, -.37), (.10, -.42)],
            [(-.12, .44), (-.055, .44), (.34, -.26), (.28, -.33)])):
        slab('Shield | worn crimson slash %02d' % i,
             [(sx + x, sz + z) for x, z in profile], sy - .113, sy - .109, 'red', 0)
    rod('Shield | boss mounting plate', (sx, sy - .11, sz), (sx, sy - .145, sz), .235, 'darkmetal', vertices=32)
    ring('Shield | boss bronze collar', (sx, sy - .153, sz), .214, .012, 'gold')
    # Low poly dome, with concentric rings retaining a hammered steel silhouette.
    dome = [(0.202, -.15), (.173, -.22), (.113, -.282), (.055, -.306)]
    verts = []
    for r, y in dome:
        verts.extend((sx + r * math.cos(i * math.tau / 20), sy + y,
                      sz + r * math.sin(i * math.tau / 20)) for i in range(20))
    faces = []
    for k in range(3):
        faces.extend((k * 20 + i, k * 20 + (i + 1) % 20,
                      (k + 1) * 20 + (i + 1) % 20, (k + 1) * 20 + i) for i in range(20))
    faces.append(tuple(range(60, 80)))
    mesh('Shield | raised steel boss', verts, faces, 'steel', .004, True)
    rod('Shield | central spike', (sx, sy - .29, sz), (sx, sy - .49, sz), .072, 'edge', .001, 8)
    for i in range(6):
        a = i * math.tau / 6
        sphere('Shield | boss rivet %02d' % i,
               (sx + .191 * math.cos(a), sy - .171, sz + .191 * math.sin(a)),
               (.015, .011, .015), 'gold')
    # Back brace and leather hand loop remain editable for subsequent rigging.
    rod('Shield | rear wooden grip', (sx, sy + .20, sz - .27), (sx, sy + .20, sz + .27), .046, 'leather')
    for zz in (-.26, .26):
        rod('Shield | grip iron bracket', (sx, sy + .06, sz + zz), (sx, sy + .20, sz + zz), .049, 'darkmetal')
    return objects
