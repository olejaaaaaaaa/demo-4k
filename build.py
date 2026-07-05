import os
import subprocess
import platform
import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parent
TOOLS = ROOT / "tools"
BUILD_DIR = ROOT / "build"

if BUILD_DIR.exists():
    shutil.rmtree(BUILD_DIR)

BUILD_DIR.mkdir()

MAKE = TOOLS / "make-3.81-bin" / "bin" / "make.exe"

ARCH = "Win64" if platform.machine().endswith("64") else "Win32"

CRINKLER = TOOLS / "crinkler30a" / ARCH / "crinkler.exe"

SOURCES = [
    "src/main.c", 
    "src/audio.c"
]

OUTPUT = "demo.exe"

env = os.environ.copy()
env["PATH"] = str(MAKE.parent) + ";" + str(CRINKLER.parent) + ";" + env["PATH"]

CC = shutil.which("clang-cl.exe")
if not CC:
    print("[X] clang-cl.exe not found in PATH Install LLVM or add in PATH")
    exit(1)

version = subprocess.run([CC, "--version"], capture_output=True, text=True)
print("[*] Compiler:", version.stdout.splitlines()[0])

if not CRINKLER.exists():
    print("[X] crinkler.exe not found in path:", CRINKLER)
    exit(1)

KIT_ROOT = Path(r"C:\Program Files (x86)\Windows Kits\10\Lib")
versions = sorted([d for d in KIT_ROOT.iterdir() if (d / "um" / "x86").exists()], reverse=True)

if not versions:
    print("[X] Windows SDK (x86) not found in", KIT_ROOT)
    exit(1)

sdk_ver = versions[0]
um_lib = sdk_ver / "um" / "x86"
ucrt_lib = sdk_ver / "ucrt" / "x86"

print("[*] Windows SDK:", sdk_ver.name)

objs = [Path(s).with_suffix(".obj").name for s in SOURCES]
sources_from_build = ["../" + s for s in SOURCES]

makefile = f"""CC = clang-cl.exe
LINK = crinkler.exe

CFLAGS = -target i686-pc-windows-msvc -O1 -GS- -c -Wno-main-return-type

LFLAGS = /SUBSYSTEM:WINDOWS /ENTRY:main /COMPMODE:SLOW /ORDERTRIES:4000 /LIBPATH:"{um_lib}" /LIBPATH:"{ucrt_lib}"

LIBS = kernel32.lib user32.lib gdi32.lib opengl32.lib winmm.lib

OUTPUT = {OUTPUT}
OBJS = {' '.join(objs)}

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
\t$(LINK) $(LFLAGS) $(OBJS) $(LIBS) /OUT:$(OUTPUT)

"""

for src, obj in zip(sources_from_build, objs):
    makefile += f"{obj}: {src}\n\t$(CC) $(CFLAGS) {src} -o {obj}\n\n"

makefile += """run: $(OUTPUT)
\t-taskkill /F /IM $(OUTPUT) >nul 2>&1
\t$(OUTPUT)
\t$(MAKE) clean

clean:
\tdel /Q *.obj $(OUTPUT)
"""

(BUILD_DIR / "Makefile").write_text(makefile, encoding="utf-8")
print("[*] Makefile generate", BUILD_DIR)

print("[*] Running make...")
subprocess.run([str(MAKE), "all"], env=env, cwd=BUILD_DIR)