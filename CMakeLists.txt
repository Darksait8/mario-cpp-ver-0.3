cmake_minimum_required(VERSION 3.10)
project(MarioGame)

set(CMAKE_CXX_STANDARD 17)

# Добавляем пути к заголовочным файлам
include_directories(${CMAKE_SOURCE_DIR}/include)

# Находим все исходные файлы
file(GLOB SOURCES 
    "*.cpp"
    "*.h"
)

# Исключаем Math.cpp из основного списка исходников, так как он может быть включен через Box2D_SOURCES
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Math.cpp")

file(GLOB_RECURSE BOX2D_SOURCES "${CMAKE_SOURCE_DIR}/include/Box2D/*.cpp")

add_executable(MarioGame ${SOURCES} ${BOX2D_SOURCES})

# Линкуем SFML
target_link_libraries(MarioGame
    ${CMAKE_SOURCE_DIR}/lib/sfml-graphics.lib
    ${CMAKE_SOURCE_DIR}/lib/sfml-window.lib
    ${CMAKE_SOURCE_DIR}/lib/sfml-system.lib
    ${CMAKE_SOURCE_DIR}/lib/sfml-audio.lib
    ${CMAKE_SOURCE_DIR}/lib/sfml-main.lib
)

# Линкуем Box2D
# target_link_libraries(MarioGame ${CMAKE_SOURCE_DIR}/lib/Box2D.lib)

# Копируем SFML DLL в выходную папку сборки
# Убедитесь, что путь к папке bin ваших SFML DLL указан правильно ниже.
# file(COPY "C:/Users/User/Desktop/SFML-2.5.1/bin/sfml-graphics-2.dll" DESTINATION "$<TARGET_FILE_DIR:MarioGame>")
# file(COPY "C:/Users/User/Desktop/SFML-2.5.1/bin/sfml-window-2.dll" DESTINATION "$<TARGET_FILE_DIR:MarioGame>")
# file(COPY "C:/Users/User/Desktop/SFML-2.5.1/bin/sfml-system-2.dll" DESTINATION "$<TARGET_FILE_DIR:MarioGame>")

# Используем add_custom_command для копирования DLL после сборки
add_custom_command(TARGET MarioGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "C:/Users/User/Desktop/SFML-2.5.1/bin/sfml-graphics-2.dll"
    "$<TARGET_FILE_DIR:MarioGame>/sfml-graphics-2.dll"
    COMMENT "Copying sfml-graphics-2.dll"
)
add_custom_command(TARGET MarioGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "C:/Users/User/Desktop/SFML-2.5.1/bin/sfml-window-2.dll"
    "$<TARGET_FILE_DIR:MarioGame>/sfml-window-2.dll"
    COMMENT "Copying sfml-window-2.dll"
)
add_custom_command(TARGET MarioGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "C:/Users/User/Desktop/SFML-2.5.1/bin/sfml-system-2.dll"
    "$<TARGET_FILE_DIR:MarioGame>/sfml-system-2.dll"
    COMMENT "Copying sfml-system-2.dll"
)

# Если вы используете другие модули SFML (аудио, сеть) или у вас Debug версия,
# возможно, потребуется скопировать соответствующие DLL (например, sfml-audio-2.dll, sfml-graphics-d-2.dll и т.д.)
add_custom_command(TARGET MarioGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "C:/Users/User/Desktop/SFML-2.5.1/bin/sfml-audio-2.dll"
    "$<TARGET_FILE_DIR:MarioGame>/sfml-audio-2.dll"
    COMMENT "Copying sfml-audio-2.dll"
)
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

add_custom_command(TARGET MarioGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "C:/Users/User/Desktop/SFML-2.5.1/bin/OpenAL32.dll"
    "$<TARGET_FILE_DIR:MarioGame>/OpenAL32.dll"
    COMMENT "Copying OpenAL32.dll"
)

add_custom_command(TARGET MarioGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/resources"
    "$<TARGET_FILE_DIR:MarioGame>/resources"
    COMMENT "Copying resources directory"
)