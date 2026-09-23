#include "dynamic_lib.hpp"
#include <windows.h>
#include <system_error>

namespace fugue::platform {

auto DynamicLib::open(const std::filesystem::path& path) -> std::expected<DynamicLib, std::string> {
    HMODULE handle = LoadLibraryW(path.c_str());
    if (!handle) {
        auto ec = std::error_code(GetLastError(), std::system_category());
        return std::unexpected("Failed to load library: " + path.string() + " (" + ec.message() + ")");
    }
    return DynamicLib(handle);
}

auto DynamicLib::raw_symbol(std::string_view name) -> void* {
    if (!handle_) return nullptr;
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(handle_), name.data()));
}

auto DynamicLib::close() -> void {
    if (handle_) {
        FreeLibrary(static_cast<HMODULE>(handle_));
        handle_ = nullptr;
    }
}

DynamicLib::~DynamicLib() {
    close();
}

DynamicLib::DynamicLib(DynamicLib&& other) noexcept : handle_(other.handle_) {
    other.handle_ = nullptr;
}

DynamicLib& DynamicLib::operator=(DynamicLib&& other) noexcept {
    if (this != &other) {
        close();
        handle_ = other.handle_;
        other.handle_ = nullptr;
    }
    return *this;
}

} // namespace fugue::platform
