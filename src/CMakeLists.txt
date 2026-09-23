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
    ui/layout_engine.cpp
    ui/theme.cpp
    ui/components/text_box.cpp
    ui/components/border.cpp
    ui/components/list.cpp
    audio/miniaudio_engine.cpp
    audio/ffmpeg_decoder.cpp
    audio/playback_controller.cpp
    ${PLATFORM_SOURCES}
)

target_include_directories(fugue PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


