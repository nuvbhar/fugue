#pragma once

#include <filesystem>
#include <expected>
#include <string>
#include <string_view>

namespace fugue::platform {

class DynamicLib {
public:
    static auto open(const std::filesystem::path& path) -> std::expected<DynamicLib, std::string>;

    template <typename Func>
    auto symbol(std::string_view name) -> std::expected<Func*, std::string> {
        auto* sym = raw_symbol(name);
        if (!sym) {
            return std::unexpected("Failed to find symbol: " + std::string(name));
        }
        return reinterpret_cast<Func*>(sym);
    }

    auto close() -> void;

    ~DynamicLib();

    // Move-only
    DynamicLib(DynamicLib&& other) noexcept;
    DynamicLib& operator=(DynamicLib&& other) noexcept;
    DynamicLib(const DynamicLib&) = delete;
    DynamicLib& operator=(const DynamicLib&) = delete;

private:
    explicit DynamicLib(void* handle) : handle_(handle) {}
    auto raw_symbol(std::string_view name) -> void*;

    void* handle_{nullptr};
};

} // namespace fugue::platform
