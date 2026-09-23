set(PLATFORM_SOURCES "")
if (WIN32)
    list(APPEND PLATFORM_SOURCES 
        platform/dynamic_lib_win.cpp
        platform/filesystem_win.cpp
        terminal/platform/win_terminal.cpp
    )
else()
    list(APPEND PLATFORM_SOURCES 
        platform/dynamic_lib_posix.cpp
        platform/filesystem_posix.cpp
        terminal/platform/posix_terminal.cpp
    )
endif()

add_executable(fugue 
    main.cpp
    terminal/screen_buffer.cpp
    terminal/escape.cpp
    terminal/renderer.cpp
    terminal/input.cpp
    terminal/capabilities.cpp
    ${PLATFORM_SOURCES}
)

target_include_directories(fugue PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


