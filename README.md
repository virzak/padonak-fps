# padonak-fps

Padonak FPS, a small OpenGL first-person shooter from 2003, and the graphics libraries it is built from. Recovered from CVS working copies (the CVS repository itself was lost, but most files carry their CVS history in their `$Log` comment blocks).

- `fps` - "Padonak FPS", a small first-person shooter. See `fps/README` for the controls.
- `grutil` - shared point, vector, matrix and colour classes.
- `subdiv` - polyhedron subdivision surfaces, with a GLUT viewer in `subdiv/ui_glut`.
- `surface_paster_cpp` - surface pasting library (`lib`), with a Tcl/Tk and Togl editor in `ui_tcltk`.

`fps` compiles sources from the other three directories directly, so they are kept together in one repository.

The game uses SDL3 for its window, input and OpenGL context. The build is CMake (`CMakeLists.txt` and `CMakePresets.json` at the root). Build output goes to `artifacts/<preset>/`. The game loads `data/` relative to the current directory, so run it from `fps`.

## Build and run on Windows

Requires Visual Studio 2026 with the "Desktop development with C++" workload, which includes CMake and vcpkg. vcpkg builds SDL3 and libjpeg-turbo on the first configure.

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

Install a compiler, CMake and the SDL3, OpenGL and libjpeg development packages (Ubuntu 26.04 or later, for SDL3):

```sh
sudo apt-get install build-essential cmake libsdl3-dev libgl1-mesa-dev \
  libglu1-mesa-dev libjpeg-dev
```

Then:

```sh
cmake --preset linux
cmake --build --preset linux
cd fps
../artifacts/linux/fps
```

On Windows, run this inside a WSL distribution such as `wsl -d Ubuntu`; the window opens on the Windows desktop through WSLg.

## VS Code

Open the repository folder; VS Code recommends the C/C++ and EditorConfig extensions. Tasks (Terminal > Run Task) are `configure`, `build` (the default build task, Ctrl+Shift+B), `build debug` and `run`. On Windows they run through `scripts/vsdev.ps1`, which enters the Visual Studio developer environment first, so no developer prompt is needed.

To debug, pick `fps (Windows)` or `fps (Linux / WSL)` in Run and Debug and press F5. Both build first and start the game in `fps`. The Linux configuration needs `gdb` (`sudo apt-get install gdb`); in WSL, open the folder with the WSL extension.

## Original build files

The original autoconf build (`fps/configure`, `fps/Makefile.in`) is still in the tree but expects `libgrutil`, `libsubdiv` and `libpatch` to be built and installed first. The Visual Studio 2010 project and its 32-bit `jpeg.lib` were replaced by CMake and are in the git history.
