#pragma once

#include "audio/decoder.hpp"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

namespace fugue::audio {

class FFmpegDecoder : public IDecoder {
public:
    FFmpegDecoder();
    ~FFmpegDecoder() override;

    auto open(const std::filesystem::path& path) -> std::expected<TrackInfo, std::string> override;
    auto decode(std::span<float> output_buffer) -> DecoderResult override;
    auto seek(float position_secs) -> std::expected<void, std::string> override;
    auto close() -> void override;

private:
    auto read_and_decode_frame() -> bool;

    AVFormatContext* format_ctx_{nullptr};
    AVCodecContext* codec_ctx_{nullptr};
    SwrContext* swr_ctx_{nullptr};
    
    int audio_stream_idx_{-1};
    AVFrame* frame_{nullptr};
    AVPacket* packet_{nullptr};
    
    std::vector<float> resample_buf_;
    size_t resample_buf_head_{0};
    size_t resample_buf_tail_{0};

    uint32_t target_sample_rate_{48000};
    uint8_t target_channels_{2};
    bool eof_reached_{false};
};

} // namespace fugue::audio
