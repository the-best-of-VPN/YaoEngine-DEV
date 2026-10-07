# 骨骼与挥砍动作

打开 `orc_warrior_rigged.blend`，按 **空格** 播放。时间轴为 **1–72 帧，24 fps，约 3 秒**，首尾回到同一站姿，可循环。

动作顺序：抬斧蓄力 → 快速向前斜劈 → 余势 → 回收。躯干配合转动，盾手抬起防御，双脚由 IK 固定。

## 文件

- `orc_warrior_rigged.blend`：含 36 根骨骼、完整蒙皮、手脚 IK、可编辑挥砍 Action、材质和展示场景。
- `orc_warrior_attack.glb`：角色、27 根变形骨骼与烘焙后的挥砍动作；不含 Blender 控制器、灯光、底座。
- `orc_warrior_attack.mp4`：720 × 810、24 fps 动画预览。
- `orc_warrior_attack.gif`：循环动画缩略预览。
- `rig_orc.py`、`attack_motion.py`：可复现骨骼、蒙皮和动作的源脚本，也嵌入了 Blender 文件。

## 修改动作

选中 `ORC_RIG • IK character controls`，进入 **Pose Mode**。

| 控制骨 | 用途 |
| --- | --- |
| `CTRL_hand.R` | 斧手位置与握持角度 |
| `CTRL_hand.L` | 盾手位置与角度 |
| `CTRL_elbow.R/L` | 手肘弯曲方向 |
| `CTRL_foot.R/L` | 脚部落点与角度 |
| `CTRL_knee.R/L` | 膝盖朝向 |
| `pelvis / spine / chest` | 重心和躯干姿态 |
| `neck / head` | 头颈姿态 |
| `weapon.axe / weapon.shield` | 装备握持位置微调 |
| `skirt.front / back / R / L` | 裙甲调整 |

Dope Sheet 或 Graph Editor 中的 Action 名为 `Orc | Heavy Axe Slash`。控制器每帧有关键帧；身体使用骨骼热权重，装备使用刚性权重，胸带使用脊柱混合权重。手部保持现有握拳造型，未制作独立手指和面部表情骨骼。

原始静态工程 `orc_warrior.blend` 仍保留。动画版胸带经过小幅调整，以适应抬臂动作。GLB 保留基础 PBR 材质，程序化微表面纹理由 Blender 工程呈现。

## 重新生成

在本目录执行，生成结果会覆盖相应动画版文件：

```powershell
& 'D:\blender\blender.exe' --background '.\orc_warrior.blend' --python '.\rig_orc.py' -- --contact
& 'D:\blender\blender.exe' --background '.\orc_warrior_rigged.blend' --python '.\validate_orc_rig.py'
& 'D:\blender\blender.exe' --background '.\orc_warrior_rigged.blend' --python '.\render_attack.py'
```
