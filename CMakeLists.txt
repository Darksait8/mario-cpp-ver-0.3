cmake_minimum_required(VERSION 3.10)
project(MarioGame)

# After requiring the standard you want, you should always set it to be required (usually),
# because CMake falls back to an older standard if it is not supported. Also, I have turned
# off C++ extensions (non-standard stuff), but you can turn it on if you need it, but you might
# sacrifice portability. It is better to use standard C++ unless you have a reason not to.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# You should basically never use include_directories, since that applies project-wide; you
# should use target_include_directories instead where you need it.
#include_directories(${CMAKE_SOURCE_DIR}/include)

# It is generally better to have sources not in the root directory, as it leads to clutter.
# So I have moved them into a src/ directory.
file(GLOB SOURCES "src/*.cpp" "src/*.h")

# This is not necessary, since I have deleted the Math.cpp/hpp files. I am not sure why you would
# put them in your sources, since they are provided by Box2D.
# Исключаем Math.cpp из основного списка исходников, так как он может быть включен через Box2D_SOURCES
#list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Math.cpp")

# There is no need to include the Box2D sources here. We'll see a better way to add Box2D later.
# Compiling it as a part of your executable is not very good.
#file(GLOB_RECURSE BOX2D_SOURCES "${CMAKE_SOURCE_DIR}/include/Box2D/*.cpp")

# You only need to add your sources here.
add_executable(MarioGame ${SOURCES})

# This whole way you were adding SFML and Box2D is basically bad. You should never use your own PC's
# paths here. The purpose of a build system is to allow it to run on multiple platforms.
# Using something like FetchContent or git submodules would be better. I'll explain how to use the
# latter below.

# Линкуем SFML
#target_link_libraries(MarioGame
#    ${CMAKE_SOURCE_DIR}/lib/sfml-graphics.lib
#    ${CMAKE_SOURCE_DIR}/lib/sfml-window.lib
#    ${CMAKE_SOURCE_DIR}/lib/sfml-system.lib
#    ${CMAKE_SOURCE_DIR}/lib/sfml-audio.lib
#    ${CMAKE_SOURCE_DIR}/lib/sfml-main.lib
#)
#
## Линкуем Box2D
## target_link_libraries(MarioGame ${CMAKE_SOURCE_DIR}/lib/Box2D.lib)
#
## Копируем SFML DLL в выходную папку сборки
## Убедитесь, что путь к папке bin ваших SFML DLL указан правильно ниже.
## file(COPY "C:/Users/User/Desktop/SFML-2.5.1/bin/sfml-graphics-2.dll" DESTINATION "$<TARGET_FILE_DIR:MarioGame>")
## file(COPY "C:/Users/User/Desktop/SFML-2.5.1/bin/sfml-window-2.dll" DESTINATION "$<TARGET_FILE_DIR:MarioGame>")
## file(COPY "C:/Users/User/Desktop/SFML-2.5.1/bin/sfml-system-2.dll" DESTINATION "$<TARGET_FILE_DIR:MarioGame>")
#
## Используем add_custom_command для копирования DLL после сборки
#add_custom_command(TARGET MarioGame POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#    "C:/Users/User/Desktop/SFML-2.5.1/bin/sfml-graphics-2.dll"
#    "$<TARGET_FILE_DIR:MarioGame>/sfml-graphics-2.dll"
#    COMMENT "Copying sfml-graphics-2.dll"
#)
#add_custom_command(TARGET MarioGame POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#    "C:/Users/User/Desktop/SFML-2.5.1/bin/sfml-window-2.dll"
#    "$<TARGET_FILE_DIR:MarioGame>/sfml-window-2.dll"
#    COMMENT "Copying sfml-window-2.dll"
#)
#add_custom_command(TARGET MarioGame POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#    "C:/Users/User/Desktop/SFML-2.5.1/bin/sfml-system-2.dll"
#    "$<TARGET_FILE_DIR:MarioGame>/sfml-system-2.dll"
#    COMMENT "Copying sfml-system-2.dll"
#)

# Если вы используете другие модули SFML (аудио, сеть) или у вас Debug версия,
# возможно, потребуется скопировать соответствующие DLL (например, sfml-audio-2.dll, sfml-graphics-d-2.dll и т.д.)
#add_custom_command(TARGET MarioGame POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#    "C:/Users/User/Desktop/SFML-2.5.1/bin/sfml-audio-2.dll"
#    "$<TARGET_FILE_DIR:MarioGame>/sfml-audio-2.dll"
#    COMMENT "Copying sfml-audio-2.dll"
#)
# add_custom_command(TARGET MarioGame POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#     "C:/Users/User/Desktop/SFML-2.5.1/bin/sfml-graphics-d-2.dll"
#     "$<TARGET_FILE_DIR:MarioGame>/sfml-graphics-d-2.dll"
#     COMMENT "Copying sfml-graphics-d-2.dll"
# )
# add_custom_command(TARGET MarioGame POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#     "C:/Users/User/Desktop/SFML-2.5.1/bin/sfml-window-d-2.dll"
#     "$<TARGET_FILE_DIR:MarioGame>/sfml-window-d-2.dll"
#     COMMENT "Copying sfml-window-d-2.dll"
# )
# add_custom_command(TARGET MarioGame POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#     "C:/Users/User/Desktop/SFML-2.5.1/bin/sfml-system-d-2.dll"
#     "$<TARGET_FILE_DIR:MarioGame>/sfml-system-d-2.dll"
#     COMMENT "Copying sfml-system-d-2.dll"
# )
#
#add_custom_command(TARGET MarioGame POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#    "C:/Users/User/Desktop/SFML-2.5.1/bin/OpenAL32.dll"
#    "$<TARGET_FILE_DIR:MarioGame>/OpenAL32.dll"
#    COMMENT "Copying OpenAL32.dll"
#)
#
#add_custom_command(TARGET MarioGame POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#    "${CMAKE_SOURCE_DIR}/resources"
#    "$<TARGET_FILE_DIR:MarioGame>/resources"
#    COMMENT "Copying resources directory"
#)

# Now, for adding of the dependencies, using git submodules is a fairly easy and robust way.
# To add a library using git submodules, you can run this command:
# git submodule add <url-of-the-repository> <path-where-you-want-to-add-it>
#
# For example, for SFML and Box2D:
# git submodule add https://github.com/SFML external/SFML
# git submodule add https://github.com/eXpl0it3r/Box2D external/Box2D
# (I have put these in an external/ directory to keep it organized.)
# For the SFML one, you can run `git checkout 2.6.2` in the SFML directory,
# because your code is written for SFML 2 while the latest is SFML 3 and it
# is not compatible.
#
# However, you do NOT need to run these, since I already added the submodules.
# But you do need to do one of these:
# a) Clone the repository from scratch, using git clone but pass in the --recurse-submodules flag.
# b) Run: `git submodule init` AND `git submodule update` if you don't want to clone again.

# By default, it will try to build examples, but we don't need those.
set(BOX2D_BUILD_EXAMPLES OFF)

# It is better to just build the static library, so we set it to off for both Box2D and SFML.
set(BOX2D_BUILD_SHARED OFF)
set(BUILD_SHARED_LIBS OFF)

# Now, to use this in CMake, it is remarkably simple:
# This will build both, and the SYSTEM means that you can include this with `<>` instead of `""`.
add_subdirectory(external/Box2D SYSTEM)
add_subdirectory(external/SFML SYSTEM)

# Now, to actually use these with your project, it is quite simple:
target_link_libraries(MarioGame box2d sfml-graphics sfml-audio)

# The Box2D library does not seem to have its includes set correctly. To fix this, we can add
# them manually:
target_include_directories(MarioGame PRIVATE external/Box2D/include/)

# In order to use the SFML audio, we need OpenAL32.dll on Windows. The `if(WIN32)` ensures
# this only runs on windows and copies the correct file.
if(WIN32)
    add_custom_command(
        TARGET MarioGame
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:MarioGame>
        VERBATIM)
endif()

# We also need to copy the resources/ folder.
set_property(TARGET MarioGame PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
