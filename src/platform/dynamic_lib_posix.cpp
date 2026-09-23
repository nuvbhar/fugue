#include "dynamic_lib.hpp"
#include <dlfcn.h>

namespace fugue::platform {

auto DynamicLib::open(const std::filesystem::path& path) -> std::expected<DynamicLib, std::string> {
    void* handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!handle) {
        return std::unexpected(std::string("Failed to load library: ") + path.string() + " (" + dlerror() + ")");
    }
    return DynamicLib(handle);
}

auto DynamicLib::raw_symbol(std::string_view name) -> void* {
    if (!handle_) return nullptr;
    return dlsym(handle_, name.data());
}

auto DynamicLib::close() -> void {
    if (handle_) {
        dlclose(handle_);
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
