# 兽人战士 · Iron Tusk

原创风格化兽人战士，使用 Blender 5.1 本地程序化建模。

**骨骼与挥砍动画版：** 打开 `orc_warrior_rigged.blend`，按空格播放。含 36 根骨骼、手脚 IK 与 72 帧挥砍动作；使用说明见 `ANIMATION.md`。以下文件描述保留的原始静态版本。

- `orc_warrior.blend`：可编辑 Blender 工程，含材质、摄影棚灯光、展示底座和三个相机。
- `orc_warrior.glb`：角色与装备的独立模型，便于导入其他 3D 软件；不包含摄影棚和底座。
- `orc_warrior_preview.png`：1400 × 1680 成品渲染。
- `orc_warrior_front.png`、`orc_warrior_back.png`：正面与背面检查图。
- `build_orc.py`、`weapon_kit.py`：建模源脚本，也保存在 Blender 工程的 Text 数据中。

角色拥有绿色肌肉体型、尖耳、弯曲獠牙、鬃发、黄眼、耳环、骨牙项链、不对称尖刺肩甲、胸带、红色战裙、护腕、护胫，以及单刃战斧和木质包铁圆盾。

Blender 内按身体、头部、盔甲、武器、底座和摄影棚分为六个 Collection。角色面向 -Y，Z 为上轴。工程无需外部贴图或链接库。

这是用于展示和继续加工的静态模型，未绑定骨骼、未制作动画或烘焙贴图。身体是合并后的雕塑网格，装备保留独立对象。GLB 保留基础 PBR 材质，Blender 程序化微表面纹理不在 GLB 中复现。

重新生成会覆盖此目录内的生成结果：

```powershell
& 'D:\blender\blender.exe' --background --factory-startup --python '.\build_orc.py'
& 'D:\blender\blender.exe' --background '.\orc_warrior.blend' --python '.\finalize_orc.py'
```
