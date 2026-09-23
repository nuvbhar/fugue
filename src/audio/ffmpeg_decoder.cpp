#include "audio/ffmpeg_decoder.hpp"
#include <algorithm>
#include <iostream>

namespace fugue::audio {

FFmpegDecoder::~FFmpegDecoder() {
    close();
}

auto FFmpegDecoder::open(const std::filesystem::path& path) -> std::expected<TrackInfo, std::string> {
    close();

    std::string path_str = path.string();

    format_ctx_ = avformat_alloc_context();
    if (avformat_open_input(&format_ctx_, path_str.c_str(), nullptr, nullptr) != 0) {
        return std::unexpected("Could not open file: " + path_str);
    }

    if (avformat_find_stream_info(format_ctx_, nullptr) < 0) {
        return std::unexpected("Could not find stream info");
    }

    audio_stream_idx_ = av_find_best_stream(format_ctx_, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (audio_stream_idx_ < 0) {
        return std::unexpected("Could not find audio stream");
    }

    AVStream* stream = format_ctx_->streams[audio_stream_idx_];
    const AVCodec* codec = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!codec) {
        return std::unexpected("Could not find decoder");
    }

    codec_ctx_ = avcodec_alloc_context3(codec);
    if (!codec_ctx_) {
        return std::unexpected("Could not allocate codec context");
    }

    if (avcodec_parameters_to_context(codec_ctx_, stream->codecpar) < 0) {
        return std::unexpected("Could not copy codec parameters to context");
    }

    if (avcodec_open2(codec_ctx_, codec, nullptr) < 0) {
        return std::unexpected("Could not open codec");
    }

    packet_ = av_packet_alloc();
    frame_ = av_frame_alloc();

    AVChannelLayout target_layout;
    av_channel_layout_default(&target_layout, target_channels_);

    swr_alloc_set_opts2(&swr_ctx_,
                        &target_layout, AV_SAMPLE_FMT_FLT, target_sample_rate_,
                        &codec_ctx_->ch_layout, codec_ctx_->sample_fmt, codec_ctx_->sample_rate,
                        0, nullptr);
                        
    if (!swr_ctx_ || swr_init(swr_ctx_) < 0) {
        return std::unexpected("Could not initialize resampler");
    }

    TrackInfo info;
    info.path = path;
    info.sample_rate = target_sample_rate_;
    info.channels = target_channels_;
    
    if (format_ctx_->duration != AV_NOPTS_VALUE) {
        info.duration_secs = static_cast<float>(format_ctx_->duration) / AV_TIME_BASE;
    }

    auto get_meta = [&](const char* key) -> std::string {
        AVDictionaryEntry* tag = av_dict_get(format_ctx_->metadata, key, nullptr, 0);
        return tag ? tag->value : "";
    };

    info.title = get_meta("title");
    info.artist = get_meta("artist");
    info.album = get_meta("album");

    if (info.title.empty()) {
        info.title = path.filename().string();
    }

    return info;
}

auto FFmpegDecoder::read_and_decode_frame() -> bool {
    while (av_read_frame(format_ctx_, packet_) >= 0) {
        if (packet_->stream_index == audio_stream_idx_) {
            int ret = avcodec_send_packet(codec_ctx_, packet_);
            if (ret < 0) {
                av_packet_unref(packet_);
                return false;
            }

            ret = avcodec_receive_frame(codec_ctx_, frame_);
            if (ret >= 0) {
                int out_samples = swr_get_out_samples(swr_ctx_, frame_->nb_samples);
                size_t out_size = out_samples * target_channels_;
                
                if (resample_buf_.size() < resample_buf_tail_ + out_size) {
                    resample_buf_.resize(resample_buf_tail_ + out_size * 2);
                }

                uint8_t* out_data = reinterpret_cast<uint8_t*>(resample_buf_.data() + resample_buf_tail_);
                int converted_samples = swr_convert(swr_ctx_, &out_data, out_samples, 
                                                    const_cast<const uint8_t**>(frame_->data), frame_->nb_samples);
                                                    
                if (converted_samples > 0) {
                    resample_buf_tail_ += converted_samples * target_channels_;
                }
                
                av_packet_unref(packet_);
                return true;
            }
        }
        av_packet_unref(packet_);
    }
    return false;
}

auto FFmpegDecoder::decode(std::span<float> output_buffer) -> DecoderResult {
    DecoderResult result;
    size_t to_fill = output_buffer.size();
    size_t filled = 0;
    
    while (filled < to_fill) {
        size_t available = resample_buf_tail_ - resample_buf_head_;
        if (available > 0) {
            size_t chunk = (std::min)(available, to_fill - filled);
            std::copy_n(resample_buf_.data() + resample_buf_head_, chunk, output_buffer.data() + filled);
            resample_buf_head_ += chunk;
            filled += chunk;
            
            if (resample_buf_head_ == resample_buf_tail_) {
                resample_buf_head_ = 0;
                resample_buf_tail_ = 0;
            }
        } else {
            bool has_more = read_and_decode_frame();
            if (!has_more) {
                result.end_of_file = true;
                break;
            }
        }
    }
    
    result.frames_decoded = filled;
    return result;
}

auto FFmpegDecoder::seek(float position_secs) -> std::expected<void, std::string> {
    if (!format_ctx_) return std::unexpected("Decoder not opened");
    
    int64_t target_ts = static_cast<int64_t>(position_secs * AV_TIME_BASE);
    if (avformat_seek_file(format_ctx_, -1, INT64_MIN, target_ts, INT64_MAX, 0) < 0) {
        return std::unexpected("Seek failed");
    }
    
    avcodec_flush_buffers(codec_ctx_);
    resample_buf_head_ = 0;
    resample_buf_tail_ = 0;
    
    return {};
}

auto FFmpegDecoder::close() -> void {
    if (swr_ctx_) {
        swr_free(&swr_ctx_);
    }
    if (codec_ctx_) {
        avcodec_free_context(&codec_ctx_);
    }
    if (format_ctx_) {
        avformat_close_input(&format_ctx_);
    }
    if (packet_) {
        av_packet_free(&packet_);
    }
    if (frame_) {
        av_frame_free(&frame_);
    }
    
    audio_stream_idx_ = -1;
    resample_buf_head_ = 0;
    resample_buf_tail_ = 0;
}

} // namespace fugue::audio
