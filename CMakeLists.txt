cmake_minimum_required(VERSION 3.25)

project(OldGfx LANGUAGES CXX)

# fps compiles the grutil, subdiv and surface_paster_cpp sources directly,
# the same way the original Visual Studio project did.
add_executable(fps
  fps/src/background.cpp
  fps/src/camera.cpp
  fps/src/engine.cpp
  fps/src/gamelevel_poly_01.cpp
  fps/src/gr.cpp
  fps/src/light.cpp
  fps/src/mobject.cpp
  fps/src/object.cpp
  fps/src/objgroups.cpp
  fps/src/output.cpp
  fps/src/player.cpp
  fps/src/polygon.cpp
  fps/src/stain.cpp
  fps/src/textures.cpp
  fps/src/timer.cpp
  fps/src/ui.cpp
  fps/src/wall.cpp
  fps/src/window.cpp
  fps/src/world.cpp
  fps/src/xray.cpp
  grutil/src/util.cpp
  subdiv/src/polyhedron.cpp
  subdiv/src/subdiv.cpp
  surface_paster_cpp/lib/patch.cpp
)

target_include_directories(fps PRIVATE
  fps
  fps/src
  grutil/include
  subdiv/include
  surface_paster_cpp/include
)

find_package(OpenGL REQUIRED)
find_package(JPEG REQUIRED)
target_link_libraries(fps PRIVATE OpenGL::GL OpenGL::GLU JPEG::JPEG)

if(WIN32)
  set_target_properties(fps PROPERTIES WIN32_EXECUTABLE ON)
  target_sources(fps PRIVATE fps/fps.rc fps/fps.manifest)
  target_compile_definitions(fps PRIVATE WIN32 _WINDOWS _CRT_SECURE_NO_WARNINGS)
else()
  find_package(X11 REQUIRED)
  target_link_libraries(fps PRIVATE X11::X11 X11::Xxf86vm X11::Xt m)
endif()

if(MSVC)
  target_compile_options(fps PRIVATE /permissive /W1)
else()
  # 2003-era code: accept its non-standard constructs quietly. It also
  # relies on pre-C++11 template name lookup.
  set_target_properties(fps PROPERTIES CXX_STANDARD 98 CXX_EXTENSIONS ON)
  target_compile_options(fps PRIVATE -fpermissive -w)
endif()

# The game loads data/ relative to the working directory.
set_target_properties(fps PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/fps"
)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT fps)
