"""Bind Iron Tusk, add editable IK controls, and create the heavy axe attack.

Run against the original saved static asset. Writes only the new rigged variant.
"""
import bpy
import math
import os
import sys
import json
from mathutils import Vector, Matrix

ROOT = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, ROOT)
scene = bpy.context.scene
bpy.context.preferences.filepaths.save_version = 0
scene.frame_set(1)

def find_collection(prefix):
    return next(c for c in bpy.data.collections if c.name.startswith(prefix))

body = bpy.data.objects['ORC • unified muscular body']
face_col = find_collection('02 ')
armor_col = find_collection('03 ')
weapon_col = find_collection('04 ')
character = [body] + list(face_col.objects) + list(armor_col.objects) + list(weapon_col.objects)
rig_col = bpy.data.collections.new('00 • RIG — select controls in Pose Mode')
scene.collection.children.link(rig_col)
data = bpy.data.armatures.new('Iron Tusk | humanoid skeleton')
rig = bpy.data.objects.new('ORC_RIG • IK character controls', data)
rig_col.objects.link(rig)
rig.show_in_front = True
data.display_type = 'OCTAHEDRAL'
rig['instructions'] = 'Pose Mode: move cyan hand/foot IK controls; yellow poles set knee/elbow direction. Spine, head and skirt bones use FK. Space plays frames 1–72.'
bpy.ops.object.select_all(action='DESELECT')
rig.select_set(True)
bpy.context.view_layer.objects.active = rig
bpy.ops.object.mode_set(mode='EDIT')

bone_defs = {}
def bone(name, head, tail, parent=None, deform=True, kind='Deform'):
    eb = data.edit_bones.new(name)
    eb.head, eb.tail = head, tail
    if parent:
        eb.parent = data.edit_bones[parent]
    eb.use_deform = deform
    bone_defs[name] = {'head':list(head), 'tail':list(tail), 'parent':parent, 'kind':kind}
    return eb

bone('root',(0,0,.23),(0,0,.7),deform=False,kind='Controls')
bone('pelvis',(0,0,2.62),(0,0,3.05),'root')
bone('spine',(0,0,3.05),(0,0,3.73),'pelvis')
bone('chest',(0,0,3.73),(0,0,4.62),'spine')
bone('neck',(0,.04,4.62),(0,.04,5.12),'chest')
bone('head',(0,.04,5.12),(0,.04,6.20),'neck')

pole_positions = {}
def pole_position(start, mid, end, distance):
    start,mid,end = Vector(start),Vector(mid),Vector(end)
    axis = (end-start).normalized()
    perp = mid-start-axis*(mid-start).dot(axis)
    return mid+perp.normalized()*distance

for side,s in [('R',-1),('L',1)]:
    sh=(s*1.10,0,4.48); el=(s*1.73,0,3.72); wr=(s*1.97,-.17,3.115); tip=(s*1.98,-.23,2.78)
    hip=(s*.43,.05,2.68); knee=(s*.68,-.055,1.64); ankle=(s*.775,.03,.68); toes=(s*.775,-.51,.4)
    bone('clavicle.'+side,(s*.14,0,4.57),sh,'chest')
    bone('upper_arm.'+side,sh,el,'clavicle.'+side)
    bone('forearm.'+side,el,wr,'upper_arm.'+side)
    bone('hand.'+side,wr,tip,'forearm.'+side)
    bone('thigh.'+side,hip,knee,'pelvis')
    bone('shin.'+side,knee,ankle,'thigh.'+side)
    bone('foot.'+side,ankle,toes,'shin.'+side)
    bone('toe.'+side,toes,(s*.775,-.68,.4),'foot.'+side)
    bone('CTRL_hand.'+side,wr,tip,'root',False,'Controls')
    bone('CTRL_foot.'+side,ankle,toes,'root',False,'Controls')
    for label,p in [('elbow',pole_position(sh,el,wr,1.8)), ('knee',pole_position(hip,knee,ankle,1.7))]:
        pole_positions[label+'.'+side]=p
        bone('CTRL_'+label+'.'+side,p,p+Vector((0,0,.24)),'root',False,'Controls')

# Equipment bones are enabled after heat skinning, so they cannot steal body weights.
equipment = [
    ('weapon.axe',(-1.98,-.37,2.94),(-1.98,-.37,3.65),'hand.R'),
    ('weapon.shield',(2.05,-.55,3.30),(2.05,-1.0,3.30),'hand.L'),
    ('skirt.front',(0,-.51,2.82),(0,-.65,2.12),'pelvis'),
    ('skirt.back',(0,.51,2.82),(0,.65,2.12),'pelvis'),
    ('skirt.R',(-.70,0,2.82),(-1.02,0,2.12),'pelvis'),
    ('skirt.L',(.70,0,2.82),(1.02,0,2.12),'pelvis')]
for n,h,t,p in equipment: bone(n,h,t,p,False,'Equipment')
bpy.ops.object.mode_set(mode='OBJECT')

groups = {n:data.collections.new(n) for n in ['Controls','Deform','Equipment']}
for n,info in bone_defs.items():
    groups[info['kind']].assign(data.bones[n])
    pb=rig.pose.bones[n]
    pb.rotation_mode='QUATERNION'
    pb.color.palette='THEME04' if info['kind']=='Controls' else ('THEME03' if info['kind']=='Equipment' else 'THEME02')

print('Binding body with bone heat...',flush=True)
bpy.ops.object.select_all(action='DESELECT')
body.select_set(True);rig.select_set(True)
bpy.context.view_layer.objects.active=rig
auto_error=None
try:
    bpy.ops.object.parent_set(type='ARMATURE_AUTO')
except RuntimeError as e:
    auto_error=str(e)
    print('Bone heat fallback:',auto_error,flush=True)

deform_names=[n for n in bone_defs if data.bones[n].use_deform]
def closest_segment(p,a,b):
    d=b-a
    t=max(0,min(1,(p-a).dot(d)/d.length_squared))
    return (p-(a+t*d)).length

# Repair missing assignments deterministically if heat has missed a disconnected point.
repairs=0
for v in body.data.vertices:
    existing=[g for g in v.groups if body.vertex_groups[g.group].name in deform_names and g.weight>1e-6]
    if not existing:
        p=body.matrix_world@v.co
        best=min(deform_names,key=lambda n:closest_segment(p,data.bones[n].head_local,data.bones[n].tail_local))
        vg=body.vertex_groups.get(best) or body.vertex_groups.new(name=best)
        vg.add([v.index],1,'REPLACE'); repairs+=1

def ensure_armature(o):
    world=o.matrix_world.copy()
    o.parent=rig
    o.matrix_world=world
    mod=next((m for m in o.modifiers if m.type=='ARMATURE'),None)
    if not mod: mod=o.modifiers.new('Rig deformation','ARMATURE')
    mod.object=rig
    mod.use_deform_preserve_volume=True
    return mod
ensure_armature(body)
body['rig_binding']='Bone heat skinning, normalized per vertex'

# Normalize all body weights; controls and rigid attachments never enter this pass.
for v in body.data.vertices:
    relevant=[g for g in v.groups if body.vertex_groups[g.group].name in deform_names]
    total=sum(g.weight for g in relevant)
    if total:
        for g in relevant: body.vertex_groups[g.group].add([v.index],g.weight/total,'REPLACE')
for n,h,t,p in equipment: data.bones[n].use_deform=True

def center(o):
    return sum((o.matrix_world@Vector(v) for v in o.bound_box),Vector())/8

def to_mesh(o):
    if o.type=='CURVE':
        bpy.ops.object.select_all(action='DESELECT');o.select_set(True)
        bpy.context.view_layer.objects.active=o
        bpy.ops.object.convert(target='MESH')
    return o

binding_counts={}
def rigid_bind(o,n):
    to_mesh(o)
    o.vertex_groups.clear()
    vg=o.vertex_groups.new(name=n)
    vg.add(list(range(len(o.data.vertices))),1,'REPLACE')
    ensure_armature(o)
    o['rig_binding']='Rigid: '+n
    binding_counts[n]=binding_counts.get(n,0)+1

def torso_weights(p):
    z=p.z
    if z<=3.05: return {'pelvis':1}
    if z<3.62:
        t=(z-3.05)/.57
        return {'pelvis':1-t,'spine':t}
    if z<4.15:
        t=(z-3.62)/.53
        return {'spine':1-t,'chest':t}
    return {'chest':1}

def soft_bind(o):
    to_mesh(o);o.vertex_groups.clear()
    g={n:o.vertex_groups.new(name=n) for n in ['pelvis','spine','chest']}
    for v in o.data.vertices:
        p=o.matrix_world@v.co
        # Route the harness above the pec instead of over the rotating deltoid.
        t=max(0,min(1,(p.z-4.20)/.50))
        t=t*t*(3-2*t)
        if p.x<0 and t>0:
            p.x+=.34*t
            p.y+=(-.22 if p.y<0 else .15)*t
            v.co=o.matrix_world.inverted()@p
        for n,w in torso_weights(p).items():
            if w>1e-6:g[n].add([v.index],w,'REPLACE')
    ensure_armature(o)
    o['rig_binding']='Flexible: pelvis / spine / chest'

print('Binding face, armor and hand-held weapons...',flush=True)
for o in list(face_col.objects):rigid_bind(o,'head')
for o in list(weapon_col.objects):rigid_bind(o,'weapon.axe' if o.name.startswith('Axe |') else 'weapon.shield')
for o in list(armor_col.objects):
    n=o.name; p=center(o);side='R' if p.x<0 else 'L'
    if n.startswith(('Diagonal leather','Harness','Rear harness')):
        soft_bind(o)
    elif n.startswith(('Crimson split','Tabard')):
        rigid_bind(o,'skirt.front')
    elif n.startswith(('Layered leather tasset','Tasset seam')):
        target=('skirt.'+side) if abs(p.x)>.72*abs(p.y) else ('skirt.front' if p.y<0 else 'skirt.back')
        rigid_bind(o,target)
    elif n.startswith(('Pauldron','Spike brass')):
        rigid_bind(o,'upper_arm.L')
    elif n.startswith(('Right upper arm','Upper armband')):
        rigid_bind(o,'upper_arm.R')
    elif n.startswith(('Bracer','Angular vambrace')):
        rigid_bind(o,'forearm.'+side)
    elif n.startswith(('Broad leather boot','Forged boot sole','Overlapping boot toe')):
        rigid_bind(o,'foot.'+side)
    elif n.startswith(('Boot upper','Boot strap','Greave','Ridged steel shin','Kneecap','Steel kneecap','Knee rivet')):
        rigid_bind(o,'shin.'+side)
    elif n.startswith(('Neck trophy','Necklace')):
        rigid_bind(o,'neck')
    else:
        rigid_bind(o,'pelvis')

print('Setting up stable hand and foot IK...',flush=True)
ik_errors={}
for side in ['R','L']:
    for upper,lower,end,pole,effector in [('upper_arm.','forearm.','hand.','elbow.','hand.'),('thigh.','shin.','foot.','knee.','foot.')]:
        pb=rig.pose.bones[lower+side]
        ik=pb.constraints.new('IK');ik.name='Two-bone IK • '+end+side
        ik.target=rig;ik.subtarget='CTRL_'+end+side
        ik.pole_target=rig;ik.pole_subtarget='CTRL_'+pole+side
        ik.chain_count=2;ik.use_stretch=False;ik.iterations=128
        pb.ik_stretch=0;rig.pose.bones[upper+side].ik_stretch=0
        expected=Vector(bone_defs[lower+side]['head'])
        # Fit pole angle numerically against the exact rest elbow/knee. Bone roll independent.
        def error(angle):
            ik.pole_angle=angle
            bpy.context.view_layer.update()
            return (rig.pose.bones[lower+side].head-expected).length
        angles=[-math.pi+i*math.tau/48 for i in range(48)]
        best=min(angles,key=error)
        step=math.tau/48
        for k in range(9):
            options=[best-step*.5,best,best+step*.5]
            best=min(options,key=error);step*=.5
        ik_errors[lower+side]=error(best)
        cr=rig.pose.bones[effector+side].constraints.new('COPY_ROTATION')
        cr.name='World-space grip / planted foot rotation'
        cr.target=rig;cr.subtarget='CTRL_'+end+side
        cr.target_space='WORLD';cr.owner_space='WORLD';cr.mix_mode='REPLACE'
bpy.context.view_layer.update()
print('Rest-pose IK errors:',ik_errors,flush=True)

# Render-invisible control shapes for practical manual posing.
widgets=bpy.data.collections.new('07 • Control widgets (not rendered)')
scene.collection.children.link(widgets)
widgets.hide_render=True
def widget(name,kind):
    if kind=='ring':
        verts=[(math.cos(i*math.tau/32),0,math.sin(i*math.tau/32)) for i in range(32)]
        edges=[(i,(i+1)%32) for i in range(32)]
    else:
        verts=[(-1,0,0),(0,0,1),(1,0,0),(0,0,-1),(0,-.5,0),(0,.5,0)]
        edges=[(0,1),(1,2),(2,3),(3,0),(0,4),(1,4),(2,4),(3,4),(0,5),(1,5),(2,5),(3,5)]
    me=bpy.data.meshes.new(name);me.from_pydata(verts,edges,[])
    o=bpy.data.objects.new(name,me);widgets.objects.link(o)
    o.hide_render=True;o.hide_set(True)
    return o
ring=widget('WGT • ring','ring');diamond=widget('WGT • diamond','diamond')
for n in bone_defs:
    if n.startswith('CTRL_') or n=='root':
        pb=rig.pose.bones[n]
        pb.custom_shape=ring if ('foot' in n or n=='root') else diamond
        pb.use_custom_shape_bone_size=False
        scale=1.1 if n=='root' else (.33 if 'foot' in n else .22)
        pb.custom_shape_scale_xyz=(scale,scale,scale)

if '--bind-only' in sys.argv:
    print('BIND_CHECK',json.dumps({'auto_weight_error':auto_error,'repaired_vertices':repairs,'ik_errors':ik_errors}),flush=True)
    bpy.ops.wm.save_as_mainfile(filepath=os.path.join(ROOT,'rig_bind_check.blend'))
    raise SystemExit(0)

from attack_motion import build_attack
motion=build_attack(rig,scene)
scene.frame_start=1;scene.frame_end=72;scene.render.fps=24
scene.frame_set(1)
scene['asset_notes']='Rigged static sculpt with IK limbs and 72-frame heavy axe slash. Editable control keys; armor and weapons rigidly weighted. Original static asset retained separately.'
scene['animation_instructions']='SPACE to play. Select ORC_RIG, enter Pose Mode, move CTRL_hand/CTRL_foot controls. Action: Orc | Heavy Axe Slash.'

# Room for the wind-up and the complete axe sweep.
camera=bpy.data.objects['CAM • Hero three quarter'].copy()
camera.data=camera.data.copy()
camera.name='CAM • Attack view'
find_collection('06 ').objects.link(camera)
camera.location=(-10.6,-17.5,8.5)
target=Vector((-.20,-.30,3.45))
camera.rotation_euler=(target-camera.location).to_track_quat('-Z','Y').to_euler()
camera.data.ortho_scale=8.85
scene.camera=camera
scene.render.resolution_x=960;scene.render.resolution_y=1080;scene.render.resolution_percentage=100
scene.cycles.samples=24;scene.cycles.use_denoising=True
scene.render.image_settings.file_format='PNG'
scene.render.filepath=os.path.join(ROOT,'attack_frames','frame_')
os.makedirs(os.path.join(ROOT,'attack_frames'),exist_ok=True)

bpy.ops.object.select_all(action='DESELECT')
rig.select_set(True);bpy.context.view_layer.objects.active=rig
bpy.ops.object.mode_set(mode='POSE')
for pb in rig.pose.bones:pb.select=False
rig.data.bones.active=rig.data.bones['CTRL_hand.R']
rig.pose.bones['CTRL_hand.R'].select=True
for screen in bpy.data.screens:
    for a in screen.areas:
        if a.type=='VIEW_3D':
            space=a.spaces.active
            space.overlay.show_overlays=True;space.overlay.show_extras=False
            space.overlay.show_floor=False;space.overlay.show_axis_x=False;space.overlay.show_axis_y=False
            space.overlay.show_cursor=False
            space.shading.type='MATERIAL'
            space.region_3d.view_perspective='CAMERA'
        elif a.type=='DOPESHEET_EDITOR':
            a.spaces.active.dopesheet.show_only_selected=False
for script in ['rig_orc.py','attack_motion.py']:
    existing=bpy.data.texts.get(script)
    if existing:bpy.data.texts.remove(existing)
    bpy.data.texts.load(os.path.join(ROOT,script))
instructions=bpy.data.texts.new('START HERE • 骨骼与挥砍动画')
instructions.write('兽人战士：骨骼 / IK / 挥砍动作\n\n时间轴 1–72 帧，24 fps，按空格播放。\nPose Mode 下移动 CTRL_hand.R 控制斧手，CTRL_hand.L 控制盾手。\nCTRL_elbow / CTRL_knee 控制弯曲方向，CTRL_foot 固定脚部。\npelvis / spine / chest / neck / head 为 FK 骨骼。\nweapon.axe / weapon.shield 可微调装备，skirt 四向骨骼可修正裙甲。\n动画保存在 Orc | Heavy Axe Slash Action 中，可在 Dope Sheet / Graph Editor 编辑。\n')

out=os.path.join(ROOT,'orc_warrior_rigged.blend')
bpy.ops.wm.save_as_mainfile(filepath=out)
bpy.ops.object.mode_set(mode='OBJECT')

# Export a baked skeleton plus the action, excluding studio, plinth and widgets.
bpy.ops.object.select_all(action='DESELECT')
for o in character:o.select_set(True)
rig.select_set(True);bpy.context.view_layer.objects.active=rig
bpy.ops.export_scene.gltf(filepath=os.path.join(ROOT,'orc_warrior_attack.glb'),export_format='GLB',use_selection=True,
    export_apply=True,export_cameras=False,export_lights=False,export_animations=True,export_animation_mode='ACTIVE_ACTIONS',
    export_skins=True,export_def_bones=True,export_force_sampling=True,export_bake_animation=True,
    export_frame_range=True,export_frame_step=1,export_anim_slide_to_zero=True)

report={'bones':len(data.bones),'deform_bones':sum(b.use_deform for b in data.bones),'skinned_objects':len(character),
    'auto_weight_error':auto_error,'repaired_body_vertices':repairs,'rest_ik_error':ik_errors,
    'rigid_bindings':binding_counts,'motion':motion}
with open(os.path.join(ROOT,'rig_report.json'),'w',encoding='utf8') as f:json.dump(report,f,indent=2,ensure_ascii=False)
print('RIG_BUILD_COMPLETE',json.dumps(report,ensure_ascii=False),flush=True)
if '--contact' in sys.argv:
    scene.render.resolution_percentage=55;scene.cycles.samples=16
    os.makedirs(os.path.join(ROOT,'attack_contact'),exist_ok=True)
    for frame in [1,13,22,28,35,46,60,72]:
        scene.frame_set(frame)
        scene.render.filepath=os.path.join(ROOT,'attack_contact','pose_%02d.png'%frame)
        bpy.ops.render.render(write_still=True)
