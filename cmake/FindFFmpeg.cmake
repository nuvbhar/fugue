set(FFMPEG_ROOT "${CMAKE_SOURCE_DIR}/third_party/ffmpeg")

find_path(FFmpeg_INCLUDE_DIR libavformat/avformat.h PATHS ${FFMPEG_ROOT}/include NO_DEFAULT_PATH)

find_library(FFmpeg_AVFORMAT_LIBRARY avformat PATHS ${FFMPEG_ROOT}/lib NO_DEFAULT_PATH)
find_library(FFmpeg_AVCODEC_LIBRARY avcodec PATHS ${FFMPEG_ROOT}/lib NO_DEFAULT_PATH)
find_library(FFmpeg_AVUTIL_LIBRARY avutil PATHS ${FFMPEG_ROOT}/lib NO_DEFAULT_PATH)
find_library(FFmpeg_SWRESAMPLE_LIBRARY swresample PATHS ${FFMPEG_ROOT}/lib NO_DEFAULT_PATH)

if(WIN32)
    find_file(FFmpeg_AVFORMAT_DLL avformat-61.dll PATHS ${FFMPEG_ROOT}/include/libavformat ${FFMPEG_ROOT}/bin NO_DEFAULT_PATH)
    find_file(FFmpeg_AVCODEC_DLL avcodec-61.dll PATHS ${FFMPEG_ROOT}/include/libavcodec ${FFMPEG_ROOT}/bin NO_DEFAULT_PATH)
    find_file(FFmpeg_AVUTIL_DLL avutil-59.dll PATHS ${FFMPEG_ROOT}/include/libavutil ${FFMPEG_ROOT}/bin NO_DEFAULT_PATH)
    find_file(FFmpeg_SWRESAMPLE_DLL swresample-5.dll PATHS ${FFMPEG_ROOT}/include/libswresample ${FFMPEG_ROOT}/bin NO_DEFAULT_PATH)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFmpeg
    REQUIRED_VARS FFmpeg_INCLUDE_DIR FFmpeg_AVFORMAT_LIBRARY FFmpeg_AVCODEC_LIBRARY FFmpeg_AVUTIL_LIBRARY FFmpeg_SWRESAMPLE_LIBRARY
)

if(FFmpeg_FOUND AND NOT TARGET FFmpeg::avformat)
    add_library(FFmpeg::avformat SHARED IMPORTED)
    set_target_properties(FFmpeg::avformat PROPERTIES
        IMPORTED_IMPLIB "${FFmpeg_AVFORMAT_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "Bcrypt.lib;Secur32.lib;ws2_32.lib"
    )
    if(WIN32 AND FFmpeg_AVFORMAT_DLL)
        set_property(TARGET FFmpeg::avformat PROPERTY IMPORTED_LOCATION "${FFmpeg_AVFORMAT_DLL}")
    endif()

    add_library(FFmpeg::avcodec SHARED IMPORTED)
    set_target_properties(FFmpeg::avcodec PROPERTIES
        IMPORTED_IMPLIB "${FFmpeg_AVCODEC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIR}"
    )
    if(WIN32 AND FFmpeg_AVCODEC_DLL)
        set_property(TARGET FFmpeg::avcodec PROPERTY IMPORTED_LOCATION "${FFmpeg_AVCODEC_DLL}")
    endif()

    add_library(FFmpeg::avutil SHARED IMPORTED)
    set_target_properties(FFmpeg::avutil PROPERTIES
        IMPORTED_IMPLIB "${FFmpeg_AVUTIL_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIR}"
    )
    if(WIN32 AND FFmpeg_AVUTIL_DLL)
        set_property(TARGET FFmpeg::avutil PROPERTY IMPORTED_LOCATION "${FFmpeg_AVUTIL_DLL}")
    endif()

    add_library(FFmpeg::swresample SHARED IMPORTED)
    set_target_properties(FFmpeg::swresample PROPERTIES
        IMPORTED_IMPLIB "${FFmpeg_SWRESAMPLE_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIR}"
    )
    if(WIN32 AND FFmpeg_SWRESAMPLE_DLL)
        set_property(TARGET FFmpeg::swresample PROPERTY IMPORTED_LOCATION "${FFmpeg_SWRESAMPLE_DLL}")
    endif()
endif()
