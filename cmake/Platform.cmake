function(setup_platform target)
    if(WIN32)
        set(FUGUE_PLATFORM "Windows")
        target_compile_definitions(${target} PRIVATE FUGUE_PLATFORM_WINDOWS=1)
        target_sources(${target} PRIVATE 
            ${CMAKE_SOURCE_DIR}/src/platform/filesystem_win.cpp
            ${CMAKE_SOURCE_DIR}/src/platform/dynamic_lib_win.cpp
        )
    elseif(APPLE)
        set(FUGUE_PLATFORM "macOS")
        target_compile_definitions(${target} PRIVATE FUGUE_PLATFORM_MACOS=1)
        target_sources(${target} PRIVATE 
            ${CMAKE_SOURCE_DIR}/src/platform/filesystem_posix.cpp
            ${CMAKE_SOURCE_DIR}/src/platform/dynamic_lib_posix.cpp
        )
    elseif(UNIX)
        if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
            set(FUGUE_PLATFORM "FreeBSD")
            target_compile_definitions(${target} PRIVATE FUGUE_PLATFORM_FREEBSD=1)
        elseif(CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
            set(FUGUE_PLATFORM "OpenBSD")
            target_compile_definitions(${target} PRIVATE FUGUE_PLATFORM_OPENBSD=1)
        elseif(ANDROID)
            set(FUGUE_PLATFORM "Termux")
            target_compile_definitions(${target} PRIVATE FUGUE_PLATFORM_TERMUX=1)
        else()
            set(FUGUE_PLATFORM "Linux")
            target_compile_definitions(${target} PRIVATE FUGUE_PLATFORM_LINUX=1)
        endif()
        target_sources(${target} PRIVATE 
            ${CMAKE_SOURCE_DIR}/src/platform/filesystem_posix.cpp
            ${CMAKE_SOURCE_DIR}/src/platform/dynamic_lib_posix.cpp
        )
    endif()
endfunction()
