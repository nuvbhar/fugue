set(FFMPEG_ROOT "${CMAKE_SOURCE_DIR}/third_party/ffmpeg")

find_path(FFmpeg_INCLUDE_DIR libavformat/avformat.h PATHS ${FFMPEG_ROOT}/include NO_DEFAULT_PATH)
find_library(FFmpeg_AVFORMAT_LIBRARY avformat PATHS ${FFMPEG_ROOT}/lib NO_DEFAULT_PATH)
find_library(FFmpeg_AVCODEC_LIBRARY avcodec PATHS ${FFMPEG_ROOT}/lib NO_DEFAULT_PATH)
find_library(FFmpeg_AVUTIL_LIBRARY avutil PATHS ${FFMPEG_ROOT}/lib NO_DEFAULT_PATH)
find_library(FFmpeg_SWRESAMPLE_LIBRARY swresample PATHS ${FFMPEG_ROOT}/lib NO_DEFAULT_PATH)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFmpeg
    REQUIRED_VARS FFmpeg_INCLUDE_DIR FFmpeg_AVFORMAT_LIBRARY FFmpeg_AVCODEC_LIBRARY FFmpeg_AVUTIL_LIBRARY FFmpeg_SWRESAMPLE_LIBRARY
)

if(FFmpeg_FOUND AND NOT TARGET FFmpeg::avformat)
    add_library(FFmpeg::avformat UNKNOWN IMPORTED)
    set_target_properties(FFmpeg::avformat PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_AVFORMAT_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIR}"
    )

    add_library(FFmpeg::avcodec UNKNOWN IMPORTED)
    set_target_properties(FFmpeg::avcodec PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_AVCODEC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIR}"
    )

    add_library(FFmpeg::avutil UNKNOWN IMPORTED)
    set_target_properties(FFmpeg::avutil PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_AVUTIL_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIR}"
    )

    add_library(FFmpeg::swresample UNKNOWN IMPORTED)
    set_target_properties(FFmpeg::swresample PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_SWRESAMPLE_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIR}"
    )
endif()
