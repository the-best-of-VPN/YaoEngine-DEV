# Python 脚本依赖配置

当前构建使用 pybind11 与普通 CPython x64。默认 SDK 目录是
`YaoEngine-core/python`，本机已补齐为 CPython 3.12.14。
`ScriptEngine.h` / `ScriptEngine.cpp` 的脚本加载、解释器管理和帧回调接口仍待实现；
此配置负责让这些实现具备完整的编译、链接和运行条件。

## 生成工程

在仓库根目录执行 `build.bat`。需要显示独立验证项目时执行：

```powershell
./build.bat --python-smoke
```

也可以直接调用 Premake，避免批处理结尾的暂停：

```powershell
./Script/premake5.exe --file=premake5.lua --python-smoke vs2026
```

用其他 SDK 时，可传入完整安装目录：

```powershell
./build.bat --python-home="D:/SDK/Python312" --python-smoke
```

SDK 选择顺序为 `--python-home`、`YAO_PYTHON_HOME` 环境变量、仓库默认目录。
相对路径以仓库根目录为基准。版本从 `include/patchlevel.h` 读取；
脚本会校验 Python 3.9 以上版本、必要文件和运行时 DLL 的 x64 架构。

默认 SDK 的主要文件如下；使用其他版本时库名、DLL 名要与头文件匹配：

```text
YaoEngine-core/python/
  include/Python.h
  include/pyconfig.h
  include/patchlevel.h
  libs/python312.lib
  python312.dll
  python3.dll
  vcruntime140.dll
  vcruntime140_1.dll
  python.exe
  Lib/
  DLLs/
  LICENSE.txt
```

本机补充的头文件、导入库、DLLs、解释器和许可证来自已有 CPython 3.12.14
运行时，保留了仓库原有 `Lib`，没有复制第三方 `site-packages`。
当前依赖未包含 Tcl/Tk 数据目录；若脚本需要 Tk 界面，应另外配置完整 Tcl/Tk。
现有 `.gitignore` 忽略 DLL、LIB、EXE；其他机器需要自行准备完整 SDK 或指定
`--python-home`，仅复制这些构建脚本不会带上二进制依赖。

## 编译和链接

`Script/Python.lua` 集中管理 SDK。核心库与三个宿主项目都调用
`YaoPython.configure()`，它会设置：

- pybind11 与 CPython 的头文件路径；
- CPython 导入库目录及与头文件版本对应的 `python3XY` 链接项；
- pybind11 项目依赖和 MSVC `/utf-8` 编码。

保持现有 `staticruntime "On"`，不会单独把核心库切到不同运行库。
Debug 使用 C++ Debug 运行库，Release / Dist 使用 C++ Release 运行库。
三种配置都链接普通 CPython；没有强制启用 `Py_DEBUG`，也不会寻找
本机不存在的 `python312_d.lib`。通过 pybind11 入口头文件使用 Python API。

## 运行时部署

编辑器、运行程序和启动器调用 `YaoPython.deploy()`，成功构建后自动生成：

```text
<应用输出目录>/
  <应用>.exe
  python312.dll
  python3.dll
  vcruntime140*.dll
  python312._pth
  python/
    Lib/
    DLLs/
    LICENSE.txt
```

Python 标准库置于 `python/Lib`，避免与 Windows 上 Mono 的 `lib` 目录混合。
`python312._pth` 由 `Script/python._pth` 复制，按 DLL 所在目录解析相对路径；
无需依赖开发机 `PYTHONHOME`、`PYTHONPATH` 或 Python 注册表配置。
此文件启用隔离路径并允许 `import site`，第三方包装在 `python/Lib/site-packages`。
引擎项目自己的脚本目录应在创建解释器后显式加入 `sys.path`。

新增宿主项目时，调用 `YaoPython.configure()` 和 `YaoPython.deploy()` 即可复用。
构建会复制所选 SDK 的整个 `Lib`；更换 Python 版本后应清理旧的应用输出目录，
避免残留上一版本的 DLL、`._pth` 或扩展模块。

## 独立验证

生成 `--python-smoke` 工程后，在 Visual Studio 中单独生成 `PythonSmoke`，
分别验证 Debug、Release、Dist。它不链接核心库、CUDA 或 Mono。
输出位于 `build/bin/<配置>-windows-x86_64/PythonSmoke/`。

该程序检查：

- pybind11 嵌入模块注册及 C++ 函数调用；
- 解释器按隔离路径初始化，并从输出目录加载标准库；
- `encodings`、`json`、`socket`、`ssl`、`sqlite3` 的导入；
- SQLite 内存数据库查询及 Python 异常报告。

可以从任意工作目录运行该 EXE，成功时输出 `Python smoke passed`。
此验证覆盖 Python 工具链与部署，不代表现有引擎业务代码已经完成编译或脚本接入。

参考：[CPython 模块搜索路径与 ._pth](https://docs.python.org/3.12/library/sys_path_init.html#pth-files)、
[pybind11 嵌入解释器](https://pybind11.readthedocs.io/en/stable/advanced/embedding.html)。
