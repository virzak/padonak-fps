cmake_minimum_required(VERSION 3.25)

project(PadonakFps LANGUAGES C CXX)

# The version is the git commit, read when CMake configures. It goes into
# the window title, and on the web into a build meta tag and build-info.json.
set(FPS_VERSION unknown)
find_package(Git QUIET)
if(GIT_FOUND)
  # safe.directory: in a container the checkout belongs to another user
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" -c safe.directory=* rev-parse --short HEAD
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE git_sha OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE git_result ERROR_QUIET)
  if(git_result EQUAL 0)
    set(FPS_VERSION "${git_sha}")
  endif()
endif()
string(TIMESTAMP FPS_BUILD_TIME "%Y-%m-%dT%H:%M:%SZ" UTC)

# Configure again when HEAD moves, so the version follows commits.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git/HEAD")
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS .git/HEAD)
  file(READ "${CMAKE_CURRENT_SOURCE_DIR}/.git/HEAD" git_head)
  if(git_head MATCHES "^ref: ([^\n]+)" AND
      EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git/${CMAKE_MATCH_1}")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
      ".git/${CMAKE_MATCH_1}")
  endif()
endif()

# fps compiles the grutil sources directly. The subdiv and
# surface_paster_cpp libraries are kept in the repository but the game no
# longer builds them: it never drew their objects (that code was commented
# out in 2003) and they use fixed-function OpenGL.
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
  fps/src/render.cpp
  fps/src/stain.cpp
  fps/src/textures.cpp
  fps/src/timer.cpp
  fps/src/touch.cpp
  fps/src/ui.cpp
  fps/src/wall.cpp
  fps/src/window.cpp
  fps/src/world.cpp
  fps/src/xray.cpp
  grutil/src/util.cpp
)

target_include_directories(fps PRIVATE
  fps
  fps/src
  grutil/include
)
# grutil's few fixed-function drawing helpers are not used by the game
target_compile_definitions(fps PRIVATE GRUTIL_NO_GL)
target_compile_definitions(fps PRIVATE FPS_VERSION="${FPS_VERSION}")

if(EMSCRIPTEN)
  # The browser build: SDL3 and libjpeg are Emscripten ports, OpenGL ES 3.0
  # is WebGL 2, and data/ is bundled into fps.data next to the page.
  set(web_ports --use-port=sdl3 --use-port=libjpeg)
  set(web_shell "${CMAKE_CURRENT_BINARY_DIR}/shell.html")
  configure_file(fps/web/shell.html "${web_shell}" @ONLY)
  file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/build-info.json"
    CONTENT "{\n  \"version\": \"@FPS_VERSION@\",\n  \"builtAt\": \"@FPS_BUILD_TIME@\"\n}\n"
    @ONLY)
  target_compile_options(fps PRIVATE ${web_ports})
  target_link_options(fps PRIVATE ${web_ports}
    -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2
    -sALLOW_MEMORY_GROWTH=1
    "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/fps/data@data"
    "--shell-file=${web_shell}"
  )
  set_target_properties(fps PROPERTIES SUFFIX .html LINK_DEPENDS "${web_shell}")
else()
  # OpenGL itself is loaded at run time through SDL and glad, so it is not
  # linked.
  find_package(SDL3 REQUIRED CONFIG)
  find_package(JPEG REQUIRED)
  target_link_libraries(fps PRIVATE SDL3::SDL3 JPEG::JPEG)
  target_sources(fps PRIVATE third_party/glad/src/gl.c)
  target_include_directories(fps PRIVATE third_party/glad/include)
endif()

if(WIN32)
  set_target_properties(fps PROPERTIES WIN32_EXECUTABLE ON)
  target_sources(fps PRIVATE fps/fps.rc fps/fps.manifest)
  target_compile_definitions(fps PRIVATE WIN32 _WINDOWS _CRT_SECURE_NO_WARNINGS)
else()
  target_link_libraries(fps PRIVATE m)
endif()

if(MSVC)
  target_compile_options(fps PRIVATE /permissive /W1)
else()
  # 2003-era code: keep its many old-style warnings quiet
  target_compile_options(fps PRIVATE -w)
endif()

# The game loads data/ relative to the working directory.
set_target_properties(fps PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/fps"
)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT fps)
