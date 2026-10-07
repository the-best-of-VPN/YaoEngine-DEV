"""Create an original stylized orc warrior in Blender. Run with Blender --background --python."""
import bpy
import math
import os
import sys
import json
from mathutils import Vector, Quaternion
from math import sin, cos, pi

ROOT = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, ROOT)
bpy.context.preferences.filepaths.save_version = 0
bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete(use_global=False)
for c in list(bpy.data.collections):
    if c.name != 'Collection':
        bpy.data.collections.remove(c)
default = bpy.data.collections.get('Collection')
if default:
    bpy.data.collections.remove(default)

def collection(name):
    c = bpy.data.collections.new(name)
    bpy.context.scene.collection.children.link(c)
    return c

BODY = collection('01 • Body sculpt')
FACE = collection('02 • Face, tusks & hair')
ARMOR = collection('03 • Forged armor & leather')
WEAPONS = collection('04 • Axe & shield')
STAGE = collection('05 • Display plinth')
STUDIO = collection('06 • Studio cameras & lighting')

def material(name, color, metal=0.0, rough=.5):
    m = bpy.data.materials.new(name)
    m.diffuse_color = (*color, 1)
    m.use_nodes = True
    p = m.node_tree.nodes.get('Principled BSDF')
    p.inputs['Base Color'].default_value = (*color, 1)
    p.inputs['Metallic'].default_value = metal
    p.inputs['Roughness'].default_value = rough
    return m

M = {
    'skin': material('Skin | moss jade', (.115,.28,.135), 0, .49),
    'skin_dark': material('Skin | eyelids & lip', (.054,.125,.059), 0, .56),
    'ear': material('Ear | warm olive', (.17,.235,.11), 0, .68),
    'scar': material('Scar | pale sage', (.29,.39,.21), 0, .6),
    'steel': material('Armor | blue forged steel', (.10,.145,.18), .78, .38),
    'edge': material('Edges | honed silver', (.48,.58,.62), .82, .28),
    'darkmetal': material('Metal | blackened iron', (.033,.049,.061), .72, .42),
    'gold': material('Trim | aged brass', (.48,.29,.095), .74, .34),
    'leather': material('Leather | oxblood brown', (.105,.040,.022), 0, .67),
    'red': material('Cloth | ember crimson', (.34,.023,.015), 0, .78),
    'bone': material('Ivory | warm bone', (.78,.66,.40), .02, .4),
    'wood': material('Wood | smoked oak', (.14,.061,.025), 0, .7),
    'hair': material('Hair | charcoal', (.016,.020,.021), 0, .53),
    'mouth': material('Mouth & pupils', (.008,.010,.005), 0, .6),
    'eye': material('Eyes | golden amber', (.95,.48,.045), .05, .27),
    'glint': material('Eye highlights', (.98,.88,.56), .05, .2),
    'stone': material('Plinth | basalt', (.059,.071,.073), .16, .75),
    'floor': material('Backdrop | midnight', (.022,.033,.041), .08, .8),
}
# Fine shader grain keeps surfaces tactile without requiring external images.
for key, scale, strength, distance in [('skin',38,.14,.028), ('leather',24,.21,.026), ('steel',19,.18,.018), ('stone',8,.33,.1), ('wood',7,.25,.035), ('red',95,.15,.008)]:
    nt = M[key].node_tree
    noise = nt.nodes.new('ShaderNodeTexNoise')
    noise.inputs['Scale'].default_value = scale
    noise.inputs['Detail'].default_value = 2
    bump = nt.nodes.new('ShaderNodeBump')
    bump.inputs['Strength'].default_value = strength
    bump.inputs['Distance'].default_value = distance
    nt.links.new(noise.outputs['Fac'], bump.inputs['Height'])
    nt.links.new(bump.outputs['Normal'], nt.nodes.get('Principled BSDF').inputs['Normal'])
M['skin'].node_tree.nodes.get('Principled BSDF').inputs['Subsurface Weight'].default_value = .06

def put(o, name, mat, col):
    o.name = name
    for c in list(o.users_collection):
        c.objects.unlink(o)
    col.objects.link(o)
    if mat:
        o.data.materials.append(M[mat] if isinstance(mat,str) else mat)
    return o

def apply_scale(o):
    bpy.context.view_layer.objects.active = o
    o.select_set(True)
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    o.select_set(False)

def smooth(o):
    for p in o.data.polygons:
        p.use_smooth = True
    return o

def uv(name, loc, scale, mat='skin', col=BODY, seg=32, rings=20, rot=None):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=seg, ring_count=rings, location=loc)
    o = put(bpy.context.object, name, mat, col)
    o.scale = scale
    if rot:
        o.rotation_euler = rot
    apply_scale(o)
    return smooth(o)

def mesh(name, verts, faces, mat, col=ARMOR, bevel=0):
    d = bpy.data.meshes.new(name)
    d.from_pydata(verts, [], faces)
    d.update()
    o = bpy.data.objects.new(name, d)
    col.objects.link(o)
    d.materials.append(M[mat] if isinstance(mat,str) else mat)
    if bevel:
        b = o.modifiers.new('Forged edge softness', 'BEVEL')
        b.width = bevel
        b.segments = 2
        n = o.modifiers.new('Weighted face normals', 'WEIGHTED_NORMAL')
    return o

def box(name, loc, size, mat, col=ARMOR, bevel=.04, rot=None):
    bpy.ops.mesh.primitive_cube_add(size=1, location=loc)
    o = put(bpy.context.object, name, mat, col)
    o.scale = size
    if rot:
        o.rotation_euler = rot
    apply_scale(o)
    if bevel:
        m = o.modifiers.new('Rounded crafted edges','BEVEL')
        m.width = bevel
        m.segments = 3
        o.modifiers.new('Weighted normals','WEIGHTED_NORMAL')
    return o

def segment(name, a, b, radii, mat='skin', col=BODY):
    a,b = Vector(a),Vector(b)
    o = uv(name, (a+b)*.5, (radii[0],radii[1],(b-a).length*.5+radii[2]), mat, col)
    o.rotation_mode = 'QUATERNION'
    o.rotation_quaternion = (b-a).to_track_quat('Z','Y')
    return o

def cone(name, a,b,r1,r2,mat='bone',col=ARMOR,vertices=12):
    a,b=Vector(a),Vector(b)
    bpy.ops.mesh.primitive_cone_add(vertices=vertices, radius1=r1, radius2=r2, depth=(b-a).length, location=(a+b)/2)
    o=put(bpy.context.object,name,mat,col)
    o.rotation_mode='QUATERNION'
    o.rotation_quaternion=(b-a).to_track_quat('Z','Y')
    bvl=o.modifiers.new('Edge bevel','BEVEL'); bvl.width=.012; bvl.segments=2
    o.modifiers.new('Weighted normals','WEIGHTED_NORMAL')
    return o

def tube(name, points, radii, mat, col=FACE, sides=12):
    points=[Vector(p) for p in points]
    vs=[]
    for i,p in enumerate(points):
        t=points[min(i+1,len(points)-1)]-points[max(i-1,0)]
        q=t.to_track_quat('Z','Y')
        for j in range(sides):
            vs.append(p+q@Vector((radii[i]*cos(j*2*pi/sides),radii[i]*sin(j*2*pi/sides),0)))
    fs=[]
    for i in range(len(points)-1):
        for j in range(sides):
            a=i*sides+j; b=i*sides+(j+1)%sides
            fs.append((a,b,b+sides,a+sides))
    fs += [tuple(range(sides-1,-1,-1)),tuple((len(points)-1)*sides+j for j in range(sides))]
    o=mesh(name,vs,fs,mat,col)
    return smooth(o)

def line(name, points, radius, mat, col=ARMOR, cyclic=False):
    c=bpy.data.curves.new(name,'CURVE'); c.dimensions='3D'; c.resolution_u=16
    c.bevel_depth=radius; c.bevel_resolution=3
    s=c.splines.new('POLY'); s.points.add(len(points)-1)
    for p,v in zip(s.points,points): p.co=(*v,1)
    s.use_cyclic_u=cyclic
    o=bpy.data.objects.new(name,c); col.objects.link(o); c.materials.append(M[mat])
    return o

def loft(name, rings, mat='skin', col=BODY, count=40):
    vs=[]
    for z,rx,ry,yc in rings:
        vs += [(rx*cos(t*2*pi/count),yc+ry*sin(t*2*pi/count),z) for t in range(count)]
    fs=[]
    for k in range(len(rings)-1):
        for j in range(count): fs.append((k*count+j,k*count+(j+1)%count,(k+1)*count+(j+1)%count,(k+1)*count+j))
    fs.extend([tuple(range(count-1,-1,-1)), tuple((len(rings)-1)*count+j for j in range(count))])
    return smooth(mesh(name,vs,fs,mat,col))

def union(objects,name,voxel=.035):
    bpy.ops.object.select_all(action='DESELECT')
    for o in objects: o.select_set(True)
    bpy.context.view_layer.objects.active=objects[0]
    # Apply primitive bevels before the unified sculpt pass.
    for o in objects:
        bpy.context.view_layer.objects.active=o
        for mod in list(o.modifiers): bpy.ops.object.modifier_apply(modifier=mod.name)
    bpy.context.view_layer.objects.active=objects[0]
    bpy.ops.object.join()
    o=bpy.context.object; o.name=name
    rem=o.modifiers.new('Unified sculpt volume','REMESH'); rem.mode='VOXEL'; rem.voxel_size=voxel; rem.use_smooth_shade=True
    bpy.ops.object.modifier_apply(modifier=rem.name)
    s=o.modifiers.new('Sculpt surface relax','SMOOTH'); s.factor=.65; s.iterations=4
    bpy.ops.object.modifier_apply(modifier=s.name)
    dec=o.modifiers.new('Clean sculpt density','DECIMATE'); dec.ratio=.48
    bpy.ops.object.modifier_apply(modifier=dec.name)
    smooth(o)
    bpy.ops.object.select_all(action='DESELECT')
    return o

print('Building body...',flush=True)
parts=[]
parts.append(loft('Torso volume',[(2.55,.57,.35,.06),(2.85,.70,.40,.04),(3.17,.61,.37,.01),(3.55,.66,.41,0),(3.92,.87,.45,0),(4.34,1.00,.46,.03),(4.62,.91,.38,.06),(4.82,.45,.29,.06)]))
parts.append(uv('Pelvis',(0,.07,2.64),(.69,.42,.39)))
parts.append(uv('Neck',(0,.10,4.96),(.40,.33,.48)))
for s in [-1,1]:
    parts.append(segment('Trapezius',(s*.21,.11,4.94),(s*1.12,.09,4.55),(.27,.30,.1)))
    parts.append(uv('Pectoral',(s*.48,-.355,4.35),(.54,.275,.33),rot=(0,-s*.09,0)))
    parts.append(uv('Latissimus',(s*.72,.23,4.05),(.30,.24,.53),rot=(0,s*.2,0)))
    parts.append(segment('External oblique',(s*.59,-.15,3.23),(s*.74,-.20,3.9),(.20,.24,.12)))
    for z,y,rx,rz in [(3.25,-.334,.23,.17),(3.56,-.371,.26,.20),(3.91,-.39,.28,.21)]:
        parts.append(uv('Abdominal muscle',(s*.235,y,z),(rx,.14,rz)))
    parts.append(uv('Deltoid',(s*1.08,.035,4.48),(.43,.41,.47)))
    parts.append(segment('Upper arm',(s*1.18,.02,4.45),(s*1.68,-.02,3.79),(.32,.33,.13)))
    parts.append(uv('Biceps',(s*1.50,-.15,4.05),(.29,.28,.40),rot=(0,s*.44,0)))
    parts.append(uv('Elbow',(s*1.73,.015,3.72),(.285,.28,.27)))
    parts.append(segment('Forearm',(s*1.72,-.01,3.75),(s*1.97,-.19,3.06),(.275,.27,.11)))
    parts.append(uv('Palm',(s*1.98,-.21,2.91),(.25,.245,.28)))
    parts.append(segment('Thumb',(s*1.79,-.37,3.05),(s*1.84,-.45,2.88),(.105,.12,.06)))
    for i in range(4):
        parts.append(uv('Curled grip finger',(s*1.98,-.395,2.75+i*.103),(.22,.125,.069)))
    parts.append(segment('Thigh',(s*.43,.08,2.65),(s*.65,-.00,1.79),(.40,.37,.13)))
    parts.append(uv('Quadriceps',(s*.57,-.18,2.24),(.34,.24,.50),rot=(0,s*.13,0)))
    parts.append(uv('Knee',(s*.68,-.055,1.64),(.29,.29,.30)))
    parts.append(segment('Calf',(s*.71,.06,1.55),(s*.77,.045,.72),(.285,.29,.10)))
    parts.append(uv('Foot',(s*.78,-.15,.48),(.31,.49,.25)))
body=union(parts,'ORC • unified muscular body',.032)

print('Sculpting head...',flush=True)
headparts=[uv('Cranium',(0,.012,5.67),(.59,.445,.65),col=FACE),
    box('Wide jaw',(0,-.135,5.20),(1.01,.80,.61),'skin',FACE,.19),
    uv('Muzzle',(0,-.475,5.38),(.44,.265,.205),col=FACE),
    uv('Chin',(0,-.375,5.08),(.435,.30,.215),col=FACE),
    uv('Nose bridge',(0,-.407,5.66),(.145,.17,.26),col=FACE),
    uv('Broad nose',(0,-.57,5.53),(.19,.208,.154),col=FACE)]
for s in [-1,1]:
    headparts.append(uv('Nose wing',(s*.152,-.563,5.49),(.117,.139,.084),col=FACE))
    headparts.append(uv('Cheek plane',(s*.389,-.302,5.47),(.213,.216,.222),col=FACE,rot=(0,-s*.25,0)))
    headparts.append(uv('Heavy brow',(s*.27,-.384,5.863),(.294,.192,.139),col=FACE,rot=(0,-s*.24,0)))
    v=[(s*.48,.016,5.88),(s*.81,.025,5.95),(s*1.16,.06,6.065),(s*.98,.006,5.65),(s*.62,-.06,5.43),(s*.76,-.172,5.72),
       (s*.48,.15,5.88),(s*.81,.145,5.95),(s*1.16,.10,6.065),(s*.98,.13,5.65),(s*.62,.10,5.43),(s*.75,.17,5.72)]
    f=[(0,1,5),(1,2,5),(2,3,5),(3,4,5),(4,0,5),(6,11,7),(7,11,8),(8,11,9),(9,11,10),(10,11,6)]
    f += [(i,(i+1)%5,(i+1)%5+6,i+6) for i in range(5)]
    headparts.append(mesh('Pointed ear',v,f,'skin',FACE))
head=union(headparts,'ORC • sculpted head & pointed ears',.019)

for s in [-1,1]:
    # Almond sockets, exposed amber eyes, and pin-point catchlights.
    uv('Deep eye socket',(s*.247,-.474,5.747),(.189,.072,.108),'skin_dark',FACE,rot=(0,-s*.18,0))
    uv('Amber eye',(s*.25,-.525,5.756),(.142,.053,.063),'eye',FACE,rot=(0,-s*.15,0))
    uv('Slit pupil',(s*.242,-.576,5.756),(.022,.012,.05),'mouth',FACE)
    uv('Catchlight',(s*.222,-.585,5.775),(.013,.008,.012),'glint',FACE,16,8)
    line('Lower eyelid',[(s*.105,-.528,5.729),(s*.23,-.565,5.697),(s*.37,-.49,5.731)],.025,'skin_dark',FACE)
    uv('Nostril',(s*.124,-.709,5.485),(.054,.018,.031),'mouth',FACE)
    mesh('Inset ear cartilage',[(s*.62,-.103,5.83),(s*1.024,.001,5.977),(s*.855,-.083,5.69),(s*.672,-.117,5.54),(s*.773,-.18,5.735)],[(0,1,4),(1,2,4),(2,3,4),(3,0,4)],'ear',FACE)
    tube('Curved ivory tusk',[(s*.325,-.645,5.15),(s*.39,-.756,5.255),(s*.427,-.805,5.39),(s*.428,-.806,5.51),(s*.399,-.782,5.615)],[.104,.087,.061,.034,.003],'bone',FACE,16)
    cone('Tusk brass cuff',(s*.328,-.654,5.156),(s*.36,-.709,5.207),.108,.10,'gold',FACE,16)
line('Set grim mouth',[(-.40,-.624,5.261),(-.27,-.725,5.254),(0,-.76,5.239),(.27,-.725,5.254),(.40,-.624,5.261)],.024,'mouth',FACE)
line('Heavy lower lip',[(-.25,-.70,5.168),(0,-.715,5.143),(.25,-.70,5.168)],.035,'skin_dark',FACE)
for x in [-.14,0,.14]: cone('Small lower tooth',(x,-.743,5.205),(x,-.753,5.283),.038,.022,'bone',FACE,10)
# One healed scar and two short cheek stripes.
line('Healed eye scar',[(.34,-.498,5.978),(.329,-.56,5.886)],.013,'scar',FACE)
line('Healed cheek scar',[(.33,-.545,5.684),(.365,-.508,5.572),(.412,-.439,5.489)],.013,'scar',FACE)
for k in range(2):
    line('Cheek warpaint',[(-.42+k*.055,-.476-k*.035,5.49),(-.405+k*.065,-.544-k*.025,5.395)],.025,'red',FACE)
# Ear rings, lying in the XZ plane.
for s in [-1,1]:
    pts=[(s*.842+.105*cos(a*2*pi/48),-.048,5.512+.142*sin(a*2*pi/48)) for a in range(48)]
    line('Brass ear hoop',pts,.029,'gold',FACE,True)
# A broad swept charcoal crest, carved as overlapping locks.
uv('Hair crown',(0,.06,6.183),(.255,.385,.18),'hair',FACE)
for i in range(6):
    y=-.25+i*.115
    z=6.21+.10*sin((i+1)*pi/7)
    tube('Swept mohawk lock',[(0,y,z-.05),(0,y+.05,z+.19),(0,y+.18,z+.35),(0,y+.31,z+.28)],[.19,.163,.104,.008],'hair',FACE,10)
for s in [-1,1]:
    for j in range(3):
        tube('Temple swept lock',[(s*.40,.04+j*.11,5.96),(s*.42,.16+j*.11,6.06),(s*.34,.31+j*.1,6.02)],[.095,.08,.003],'hair',FACE,10)

print('Forging armor...',flush=True)
# Closed elliptical belts and wraps use lofted rings.
loft('Broad waist belt',[(2.80,.71,.465,.025),(3.075,.707,.465,.025)],'leather',ARMOR)
for z in [2.82,3.055]:
    line('Belt brass piping',[(.72*cos(i*2*pi/80),.025+.473*sin(i*2*pi/80),z) for i in range(80)],.022,'gold',ARMOR,True)
box('Buckle iron seat',(0,-.482,2.946),(.48,.12,.31),'darkmetal',bevel=.04)
box('Buckle brass rim',(0,-.56,2.946),(.39,.074,.28),'gold',bevel=.035)
box('Buckle inset',(0,-.604,2.946),(.275,.028,.18),'darkmetal',bevel=.02)
# A small geometric iron-jaw sigil on the belt.
mesh('Buckle fang emblem',[(-.095,-.627,3.015),(.095,-.627,3.015),(.085,-.63,2.932),(.037,-.632,2.861),(0,-.633,2.904),(-.037,-.632,2.861),(-.085,-.63,2.932)],[(0,1,2,3,4,5,6)],'bone',ARMOR,.008)
for s in [-1,1]:
    for k in range(3): uv('Belt rivet',(s*(.28+k*.14),-.469+(.014+k*.025),2.946),(.035,.022,.035),'gold',ARMOR,16,8)

# Radial overlapping leather tassets. The center front is a long crimson tabard.
for k in range(12):
    a=2*pi*k/12
    if sin(a)<-.85: continue
    da=.225
    zbottom=2.18+(.06 if k%2 else -.06)
    vs=[(.735*cos(a-da),.025+.487*sin(a-da),2.83),(.735*cos(a+da),.025+.487*sin(a+da),2.83),
        (.99*cos(a-da),.025+.64*sin(a-da),2.55),(.99*cos(a+da),.025+.64*sin(a+da),2.55),
        (1.08*cos(a+da),.025+.69*sin(a+da),zbottom+.06),(1.12*cos(a),.025+.72*sin(a),zbottom-.10),(1.08*cos(a-da),.025+.69*sin(a-da),zbottom+.06)]
    o=mesh('Layered leather tasset %02d'%k,vs,[(0,1,3,2),(2,3,4,5,6)],'leather' if k%3 else 'red',ARMOR,.015)
    solid=o.modifiers.new('Leather thickness','SOLIDIFY'); solid.thickness=.055
    line('Tasset seam',[vs[1],vs[3],vs[4],vs[5],vs[6],vs[2],vs[0]],.012,'gold',ARMOR)

v=[(-.43,-.494,2.81),(0,-.547,2.81),(.43,-.494,2.81),(-.38,-.611,2.25),(0,-.671,2.19),(.38,-.611,2.25),(-.27,-.61,1.91),(-.09,-.679,1.77),(.13,-.67,1.85),(.30,-.61,1.99)]
f=[(0,1,4,3),(1,2,5,4),(3,4,7,6),(4,5,9,8,7)]
o=mesh('Crimson split war tabard',v,f,'red',ARMOR)
sol=o.modifiers.new('Woven cloth thickness','SOLIDIFY'); sol.thickness=.035
bev=o.modifiers.new('Soft cloth hem','BEVEL'); bev.width=.018; bev.segments=2
line('Tabard hem',[v[0],v[3],v[6],v[7],v[8],v[9],v[5],v[2]],.018,'gold',ARMOR)
mesh('Tabard angular clan rune',[(-.10,-.681,2.56),(0,-.69,2.45),(.10,-.681,2.56),(.055,-.695,2.32),(0,-.705,2.26),(-.055,-.695,2.32)],[(0,1,2,3,4,5)],'bone',ARMOR)

# Cross-body leather harness hugging the pecs.
strap=[(-1.03,-.255,4.72),(-.84,-.476,4.49),(-.57,-.627,4.24),(-.24,-.553,3.96),(.08,-.516,3.67),(.40,-.464,3.31),(.56,-.425,3.13)]
vs=[]
for x,y,z in strap: vs += [(x-.09,y-.012,z-.105),(x+.09,y-.012,z+.105)]
o=mesh('Diagonal leather baldric',vs,[(i*2,i*2+1,i*2+3,i*2+2) for i in range(len(strap)-1)],'leather',ARMOR)
sol=o.modifiers.new('Heavy leather thickness','SOLIDIFY'); sol.thickness=.075
bev=o.modifiers.new('Worn strap edges','BEVEL'); bev.width=.024; bev.segments=2
for sign in [-1,1]:
    line('Harness stitched edge',[(x+sign*.074,y-.044,z+sign*.087) for x,y,z in strap],.009,'gold',ARMOR)
for x,y,z in strap[1:-1]: uv('Harness stud',(x,y-.053,z),(.032,.025,.032),'gold',ARMOR,16,8)
box('Harness clasp',(-.30,-.623,4.015),(.31,.095,.25),'gold',bevel=.025,rot=(0,-.80,0))
box('Harness clasp opening',(-.30,-.68,4.015),(.19,.021,.14),'leather',bevel=.008,rot=(0,-.80,0))
# Back section of the same strap.
line('Rear harness',[(-1.04,.22,4.71),(-.68,.456,4.21),(-.1,.458,3.71),(.56,.399,3.13)],.10,'leather',ARMOR)

# Layered asymmetric pauldron shell.
def cap(name,center,radii,end,mat):
    n=12; levels=5
    vs=[(center[0],center[1],center[2]+radii[2])]
    for j in range(1,levels+1):
        t=end*j/levels
        vs.extend([(center[0]+radii[0]*sin(t)*cos(i*2*pi/n),center[1]+radii[1]*sin(t)*sin(i*2*pi/n),center[2]+radii[2]*cos(t)) for i in range(n)])
    fs=[(0,1+i,1+(i+1)%n) for i in range(n)]
    for j in range(levels-1):
        a=1+j*n; b=a+n
        fs.extend([(a+i,b+i,b+(i+1)%n,a+(i+1)%n) for i in range(n)])
    o=mesh(name,vs,fs,mat,ARMOR,.025)
    sol=o.modifiers.new('Thick forged shell','SOLIDIFY'); sol.thickness=.095
    return o
cap('Pauldron lower iron skirt',(1.12,.025,4.47),(.69,.58,.49),1.82,'darkmetal')
cap('Pauldron sculpted steel dome',(1.10,.008,4.53),(.67,.575,.49),1.5,'steel')
for z,rx,ry in [(4.38,.69,.59),(4.565,.67,.585)]:
    line('Pauldron rolled brass border',[(1.10+rx*cos(i*2*pi/12),.008+ry*sin(i*2*pi/12),z) for i in range(12)],.043,'gold',ARMOR,True)
for i in range(12):
    a=i*2*pi/12
    uv('Pauldron perimeter rivet',(1.10+.637*cos(a),.008+.558*sin(a),4.62),(.045,.045,.045),'edge',ARMOR,16,8)
for i,(a,b,r) in enumerate([
    ((1.10,.015,4.992),(1.14,.01,5.60),.15),
    ((1.52,.03,4.88),(1.94,.05,5.29),.135),
    ((1.32,-.38,4.84),(1.52,-.65,5.29),.115),
    ((1.29,.40,4.84),(1.44,.67,5.28),.115)]):
    av,bv=Vector(a),Vector(b)
    cone('Spike brass socket',av,av+(bv-av)*.19,r*1.20,r*1.10,'gold')
    cone('Pauldron ivory spike',av+(bv-av)*.12,bv,r,.006,'bone')
# Opposite shoulder: modest leather wrap exposes green silhouette.
segment('Right upper arm leather band',(-1.29,.014,4.29),(-1.43,.0,4.07),(.34,.345,.0),'leather',ARMOR)
for z,x in [(4.27,-1.30),(4.11,-1.42)]:
    uv('Upper armband brass clasp',(x,-.322,z),(.080,.041,.047),'gold',ARMOR)

for s in [-1,1]:
    # Boots and greaves.
    box('Broad leather boot',(s*.775,-.16,.505),(.72,1.08,.48),'leather',bevel=.115)
    box('Forged boot sole',(s*.775,-.17,.31),(.75,1.11,.13),'darkmetal',bevel=.055)
    for j in range(3):
        box('Overlapping boot toe plate',(s*.775,-.50+j*.145,.727+j*.024),(.65,.255,.13),'steel',bevel=.035)
    segment('Boot upper shaft',(s*.76,.01,.63),(s*.72,.03,1.36),(.315,.32,.04),'leather',ARMOR)
    for z in [.78,1.17]:
        line('Greave leather retaining strap',[(s*.75+.32*cos(i*2*pi/40),.028+.326*sin(i*2*pi/40),z) for i in range(40)],.044,'leather',ARMOR,True)
        box('Boot strap buckle',(s*1.035,-.10,z),(.10,.12,.12),'gold',bevel=.02)
    cx=s*.73
    vs=[(cx-.24,-.236,1.43),(cx,-.34,1.53),(cx+.24,-.236,1.43),(cx+.23,-.26,.82),(cx,-.383,.68),(cx-.23,-.26,.82),(cx,-.41,1.12)]
    fs=[(0,1,6),(1,2,6),(2,3,6),(3,4,6),(4,5,6),(5,0,6)]
    o=mesh('Ridged steel shin guard',vs,fs,'steel',ARMOR,.017)
    sol=o.modifiers.new('Greave plate thickness','SOLIDIFY');sol.thickness=.075
    line('Greave brass border',vs[:6],.022,'gold',ARMOR,True)
    line('Greave center ridge',[vs[1],vs[6],vs[4]],.018,'edge',ARMOR)
    uv('Kneecap underwrap',(s*.681,-.09,1.655),(.31,.297,.265),'leather',ARMOR)
    uv('Steel kneecap',(s*.681,-.308,1.68),(.25,.14,.20),'steel',ARMOR,16,12)
    for dx in [-.165,.165]: uv('Knee rivet',(s*.681+dx,-.395,1.68),(.028,.025,.028),'gold',ARMOR,12,8)
    # Bracer along the actual forearm axis.
    a=Vector((s*1.78,-.06,3.62));b=Vector((s*1.91,-.17,3.20))
    cone('Bracer leather sleeve',a,b,.317,.284,'leather',ARMOR,16)
    for t in [.10,.86]:
        p=a.lerp(b,t);q=a.lerp(b,t+.12)
        cone('Bracer iron retaining band',p,q,.328-(t*.036),.328-((t+.12)*.036),'darkmetal',ARMOR,16)
    cx=s*1.87
    vs=[(cx-.22,-.344,3.66),(cx+.20,-.344,3.64),(cx+.19,-.414,3.20),(cx,-.468,3.12),(cx-.20,-.414,3.23),(cx,-.482,3.43)]
    o=mesh('Angular vambrace face',vs,[(0,1,5),(1,2,5),(2,3,5),(3,4,5),(4,0,5)],'steel',ARMOR,.012)
    sol=o.modifiers.new('Bracer thickness','SOLIDIFY');sol.thickness=.055
    line('Bracer brass edge',vs[:5],.024,'gold',ARMOR,True)
    for dx in [-.14,.14]:
        for z in [3.29,3.57]: uv('Bracer rivet',(cx+dx,-.437,z),(.033,.023,.033),'edge',ARMOR,12,8)
    if s==-1:
        for z in [3.33,3.57]: cone('Bracer short spike',(-2.07,-.13,z),(-2.40,-.13,z+.10),.09,.003,'bone')

# A short bone necklace at the neck opening.
neckpts=[(.38*cos(pi+i*pi/32),-.015+.33*sin(pi+i*pi/32),4.99-.22*sin(i*pi/32)) for i in range(33)]
line('Neck trophy cord',neckpts,.029,'leather',ARMOR)
for x in [-.23,-.12,0,.12,.23]:
    z=4.79+abs(x)*.25
    cone('Necklace bone tooth',(x,-.337,z),(x*.90,-.375,z-.17),.046,.007,'bone',ARMOR,10)

from weapon_kit import build_weapons
build_weapons(WEAPONS,M)

print('Lighting display...',flush=True)
cone('Basalt display plinth',(0,0,.03),(0,0,.23),2.67,2.67,'stone',STAGE,96)
cone('Plinth lower black iron ring',(0,0,-.025),(0,0,.055),2.72,2.72,'darkmetal',STAGE,96)
cone('Plinth fine brass lip',(0,0,.055),(0,0,.077),2.735,2.735,'gold',STAGE,96)
line('Plinth inset circle',[(2.49*cos(i*2*pi/128),2.49*sin(i*2*pi/128),.236) for i in range(128)],.018,'gold',STAGE,True)
for i in range(24):
    a=i*2*pi/24
    r=2.57
    line('Plinth radial inlay',[(r*cos(a),r*sin(a),.241),((r+.07)*cos(a),(r+.07)*sin(a),.241)],.013,'gold',STAGE)
# A few restrained chips and seams in the stone top.
for a in [.4,1.8,3.0,4.0,5.15]:
    line('Basalt hairline seam',[(2.45*cos(a),2.45*sin(a),.237),(2.20*cos(a+.025),2.20*sin(a+.025),.237),(1.96*cos(a+.015),1.96*sin(a+.015),.237)],.008,'darkmetal',STAGE)

bpy.ops.mesh.primitive_plane_add(size=200,location=(0,0,-.095))
put(bpy.context.object,'Studio ground','floor',STUDIO)

def track(o,p): o.rotation_euler=(Vector(p)-o.location).to_track_quat('-Z','Y').to_euler()
def camera(name,loc,target,ortho):
    d=bpy.data.cameras.new(name);o=bpy.data.objects.new(name,d);STUDIO.objects.link(o);o.location=loc
    track(o,target);d.type='ORTHO';d.ortho_scale=ortho;d.lens=52
    return o
hero=camera('CAM • Hero three quarter',(8.4,-18,8.2),(-.20,0,3.20),8.20)
front=camera('CAM • Front inspection',(0,-20,5.8),(0,0,3.30),7.70)
back=camera('CAM • Back inspection',(-8,18,7.0),(0,0,3.1),8.05)
def area(name,loc,target,power,color,size):
    d=bpy.data.lights.new(name,'AREA');o=bpy.data.objects.new(name,d);STUDIO.objects.link(o)
    o.location=loc;d.energy=power;d.color=color;d.shape='DISK';d.size=size;track(o,target)
area('KEY • warm softbox',(-4,-6,10),(0,0,3.4),1500,(1,.80,.59),5)
area('FILL • cool softbox',(5,-4,6),(0,0,3.5),1050,(.58,.79,1),4)
area('RIM • glacial edge',(3,4,8),(0,0,3.8),2050,(.32,.72,1),3.5)
area('TOP • warm crown',(-3,2,10),(0,0,3.4),1300,(1,.47,.20),3)
area('FACE • eye softbox',(0,-6,5.7),(0,0,5.4),180,(1,.89,.69),2)
scene=bpy.context.scene
scene.camera=hero
scene.world.color=(.06,.06,.06)
scene.world.use_nodes=True
scene.world.node_tree.nodes.get('Background').inputs['Color'].default_value=(.08,.115,.15,1)
scene.world.node_tree.nodes.get('Background').inputs['Strength'].default_value=.3
scene.render.engine='CYCLES'
scene.cycles.samples=48
scene.cycles.use_denoising=True
scene.cycles.max_bounces=6
scene.render.resolution_x=1000
scene.render.resolution_y=1200
scene.render.resolution_percentage=70 if '--draft' in sys.argv else 100
scene.render.image_settings.file_format='PNG'
scene.render.film_transparent=False
scene.view_settings.view_transform='AgX'
scene.render.filepath=os.path.join(ROOT,'orc_warrior_preview.png')
scene.unit_settings.system='METRIC'
scene['asset_name']='GROM / IRON TUSK — original stylized orc warrior'
scene['asset_notes']='Editable static character sculpture. Separate armor and weapons. No animation rig or UV texture bake. Front faces -Y.'
# Native viewport opens on the character in material-colored solid mode.
bpy.ops.object.select_all(action='DESELECT')
body.select_set(True);bpy.context.view_layer.objects.active=body
for screen in bpy.data.screens:
    for a in screen.areas:
        if a.type=='VIEW_3D':
            a.spaces.active.region_3d.view_rotation=hero.rotation_euler.to_quaternion()
            a.spaces.active.region_3d.view_distance=10.2
            a.spaces.active.region_3d.view_location=(0,0,3.15)
            a.spaces.active.clip_end=300
            a.spaces.active.shading.type='MATERIAL'
            a.spaces.active.overlay.show_overlays=False
            a.spaces.active.region_3d.view_perspective='CAMERA'
# Keep generation script within the Blender document for easy revision.
for script in ['build_orc.py','weapon_kit.py']:
    bpy.data.texts.load(os.path.join(ROOT,script))
blendpath=os.path.join(ROOT,'orc_warrior.blend')
bpy.ops.wm.save_as_mainfile(filepath=blendpath)

# Character-only GLB for downstream preview/import. No studio or plinth.
bpy.ops.object.select_all(action='DESELECT')
for col in [BODY,FACE,ARMOR,WEAPONS]:
    for o in col.objects:
        if o.type in {'MESH','CURVE'}: o.select_set(True)
bpy.ops.export_scene.gltf(filepath=os.path.join(ROOT,'orc_warrior.glb'),export_format='GLB',use_selection=True,export_apply=True,export_cameras=False,export_lights=False)
bpy.ops.object.select_all(action='DESELECT');body.select_set(True);bpy.context.view_layer.objects.active=body
deps=bpy.context.evaluated_depsgraph_get()
stats={}
for col in [BODY,FACE,ARMOR,WEAPONS]:
    tris=0
    for o in col.objects:
        if o.type=='MESH':
            evaluated=o.evaluated_get(deps);me=evaluated.to_mesh();me.calc_loop_triangles();tris+=len(me.loop_triangles);evaluated.to_mesh_clear()
    stats[col.name]={'objects':len(col.objects),'triangles':tris}
with open(os.path.join(ROOT,'asset_stats.json'),'w',encoding='utf8') as f: json.dump(stats,f,indent=2,ensure_ascii=False)
print(json.dumps(stats,ensure_ascii=False),flush=True)
if '--no-render' not in sys.argv:
    print('Rendering hero preview...',flush=True)
    bpy.ops.render.render(write_still=True)
print('ORC_BUILD_COMPLETE',flush=True)
