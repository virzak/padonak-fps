# OldGfx

Old OpenGL projects from 2003-2010, recovered from CVS working copies (the CVS repository itself was lost, but most files carry their CVS history in their `$Log` comment blocks).

- `fps` - "Padonak FPS", a small first-person shooter. See `fps/README` for the controls.
- `grutil` - shared point, vector, matrix and colour classes.
- `subdiv` - polyhedron subdivision surfaces, with a GLUT viewer in `subdiv/ui_glut`.
- `surface_paster_cpp` - surface pasting library (`lib`), with a Tcl/Tk and Togl editor in `ui_tcltk`.

`fps` compiles sources from the other three directories directly, so they are kept together in one repository.

## Build and run on Linux or WSLg

Install a compiler and the OpenGL, X11 and libjpeg development packages (Ubuntu):

```sh
sudo apt-get install build-essential libgl1-mesa-dev libglu1-mesa-dev \
  libx11-dev libxxf86vm-dev libxt-dev libjpeg-dev
```

Then, from `fps` (the game loads `data/` relative to the current directory):

```sh
cd fps
make -f Makefile.wsl run
```

On Windows, run this inside a WSL distribution such as `wsl -d Ubuntu`; the window opens on the Windows desktop through WSLg.

## Build on Windows

Open `fps/fps.sln` in Visual Studio. The project still has include paths from the original machines (`E:\cygwin\home\virzak\...`, `C:\Code\...`) and has not been updated to build from this layout.

The original autoconf build (`fps/configure`, `fps/Makefile.in`) expects `libgrutil`, `libsubdiv` and `libpatch` to be built and installed first; `Makefile.wsl` replaces it.
