import json
import os
from pathlib import Path
import subprocess
import sys

root = Path(__file__).resolve().parents[2]
log_dir = Path(__file__).resolve().parent
run_dir = log_dir / "unrelated-cwd"
run_dir.mkdir(exist_ok=True)
msbuild = Path(r"D:/vs/va product/MSBuild/Current/Bin/MSBuild.exe")
project = root / "Script/PythonSmoke/PythonSmoke.vcxproj"
build_env = dict(os.environ)
build_env["Path"] = build_env.pop("PATH", "")
run_env = {k: v for k, v in build_env.items() if k.upper() not in {"PYTHONHOME", "PYTHONPATH", "PATH"}}
windows = Path(os.environ.get("SystemRoot", r"C:\Windows"))
run_env["Path"] = ";".join(str(p) for p in [windows / "System32", windows, windows / "System32/Wbem"])
results = []
for config in sys.argv[1:] or ["Debug", "Release", "Dist"]:
    assert config in {"Debug", "Release", "Dist"}
    cmd = [str(msbuild), str(project), "/m:1", "/nodeReuse:false",
           f"/p:Configuration={config}", "/p:Platform=x64", "/p:BuildProjectReferences=false",
           "/nologo", "/verbosity:minimal"]
    build = subprocess.run(cmd, cwd=root, env=build_env, capture_output=True)
    build_text = (build.stdout + build.stderr).decode("mbcs", errors="replace")
    (log_dir / f"{config}-build.log").write_text(build_text, encoding="utf-8")
    item = {"configuration": config, "platform": "x64", "build_exit": build.returncode}
    print(f"{config} build exit: {build.returncode}", flush=True)
    print(build_text[-7000:], flush=True)
    if build.returncode == 0:
        output_dir = root / "build/bin" / f"{config}-windows-x86_64" / "PythonSmoke"
        exe = output_dir / "PythonSmoke.exe"
        run = subprocess.run([str(exe)], cwd=run_dir, env=run_env, capture_output=True, timeout=30)
        run_text = (run.stdout + run.stderr).decode("utf-8", errors="replace")
        (log_dir / f"{config}-run.log").write_text(run_text, encoding="utf-8")
        expected_encodings = str(output_dir / "python/Lib/encodings/__init__.py")
        item.update({"run_exit": run.returncode, "cwd": str(run_dir), "path": run_env["Path"],
                     "pythonhome_present": "PYTHONHOME" in run_env,
                     "pythonpath_present": "PYTHONPATH" in run_env,
                     "output": run_text,
                     "encodings_in_output": expected_encodings.casefold() in run_text.casefold(),
                     "passed": run.returncode == 0 and "Python smoke passed" in run_text
                               and expected_encodings.casefold() in run_text.casefold()})
        print(run_text, flush=True)
    else:
        item["passed"] = False
    results.append(item)
    (log_dir / "results.json").write_text(json.dumps(results, indent=2), encoding="utf-8")
sys.exit(0 if all(item["passed"] for item in results) else 1)
