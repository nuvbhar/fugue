#pragma once

#include <vector>
#include <atomic>
#include <cstdint>
#include <span>

namespace fugue::audio {

// Lock-free single-producer single-consumer ring buffer for audio samples
class RingBuffer {
public:
    explicit RingBuffer(size_t capacity) 
        : capacity_(capacity), buffer_(capacity) {}

    auto push(std::span<const float> data) -> size_t {
        size_t current_tail = tail_.load(std::memory_order_relaxed);
        size_t current_head = head_.load(std::memory_order_acquire);
        
        size_t available = capacity_ - (current_tail - current_head);
        size_t to_write = (std::min)(data.size(), available);
        
        if (to_write == 0) return 0;
        
        size_t tail_index = current_tail % capacity_;
        
        if (tail_index + to_write <= capacity_) {
            std::copy_n(data.begin(), to_write, buffer_.begin() + tail_index);
        } else {
            size_t part1 = capacity_ - tail_index;
            size_t part2 = to_write - part1;
            std::copy_n(data.begin(), part1, buffer_.begin() + tail_index);
            std::copy_n(data.begin() + part1, part2, buffer_.begin());
        }
        
        tail_.store(current_tail + to_write, std::memory_order_release);
        return to_write;
    }

    auto pop(std::span<float> data) -> size_t {
        size_t current_head = head_.load(std::memory_order_relaxed);
        size_t current_tail = tail_.load(std::memory_order_acquire);
        
        size_t available = current_tail - current_head;
        size_t to_read = (std::min)(data.size(), available);
        
        if (to_read == 0) return 0;
        
        size_t head_index = current_head % capacity_;
        
        if (head_index + to_read <= capacity_) {
            std::copy_n(buffer_.begin() + head_index, to_read, data.begin());
        } else {
            size_t part1 = capacity_ - head_index;
            size_t part2 = to_read - part1;
            std::copy_n(buffer_.begin() + head_index, part1, data.begin());
            std::copy_n(buffer_.begin(), part2, data.begin() + part1);
        }
        
        head_.store(current_head + to_read, std::memory_order_release);
        return to_read;
    }

    auto available_read() const -> size_t {
        return tail_.load(std::memory_order_acquire) - head_.load(std::memory_order_acquire);
    }
    
    auto available_write() const -> size_t {
        return capacity_ - available_read();
    }
    
    auto clear() -> void {
        head_.store(0, std::memory_order_relaxed);
        tail_.store(0, std::memory_order_relaxed);
    }

private:
    size_t capacity_;
    std::vector<float> buffer_;
    alignas(64) std::atomic<size_t> head_{0};
    alignas(64) std::atomic<size_t> tail_{0};
};

} // namespace fugue::audio
