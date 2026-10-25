# OldGfx

Old OpenGL projects from 2003-2010, recovered from CVS working copies (the CVS repository itself was lost, but most files carry their CVS history in their `$Log` comment blocks).

- `fps` - "Padonak FPS", a small first-person shooter. See `fps/README` for the controls.
- `grutil` - shared point, vector, matrix and colour classes.
- `subdiv` - polyhedron subdivision surfaces, with a GLUT viewer in `subdiv/ui_glut`.
- `surface_paster_cpp` - surface pasting library (`lib`), with a Tcl/Tk and Togl editor in `ui_tcltk`.

`fps` compiles sources from the other three directories directly, so they are kept together in one repository.

The build is CMake (`CMakeLists.txt` and `CMakePresets.json` at the root). Build output goes to `artifacts/<preset>/`. The game loads `data/` relative to the current directory, so run it from `fps`.

## Build and run on Windows

Requires Visual Studio 2026 with the "Desktop development with C++" workload, which includes CMake and vcpkg. vcpkg builds libjpeg-turbo on the first configure.

From a Developer PowerShell for VS 2026:

```powershell
cmake --preset windows
cmake --build --preset windows
cd fps
..\artifacts\windows\Release\fps.exe
```

To debug, open the generated `artifacts\windows\OldGfx.slnx` after the configure step; `fps` is the startup project and runs in the `fps` directory.

Outside a developer prompt, set `VCPKG_ROOT` to the vcpkg that ships with Visual Studio (`<VS install>\VC\vcpkg`) or to your own vcpkg clone first.

## Build and run on Linux or WSLg

Install a compiler, CMake and the OpenGL, X11 and libjpeg development packages (Ubuntu):

```sh
sudo apt-get install build-essential cmake libgl1-mesa-dev libglu1-mesa-dev \
  libx11-dev libxext-dev libxxf86vm-dev libxt-dev libjpeg-dev
```

Then:

```sh
cmake --preset linux
cmake --build --preset linux
cd fps
../artifacts/linux/fps
```

On Windows, run this inside a WSL distribution such as `wsl -d Ubuntu`; the window opens on the Windows desktop through WSLg.

## Original build files

The original autoconf build (`fps/configure`, `fps/Makefile.in`) is still in the tree but expects `libgrutil`, `libsubdiv` and `libpatch` to be built and installed first. The Visual Studio 2010 project and its 32-bit `jpeg.lib` were replaced by CMake and are in the git history.
