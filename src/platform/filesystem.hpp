#pragma once

#include <filesystem>
#include <expected>
#include <string>

namespace fugue::platform {

auto config_dir() -> std::expected<std::filesystem::path, std::string>;
auto data_dir() -> std::expected<std::filesystem::path, std::string>;
auto cache_dir() -> std::expected<std::filesystem::path, std::string>;
auto plugin_dir() -> std::expected<std::filesystem::path, std::string>;
auto ensure_dirs_exist() -> std::expected<void, std::string>;

} // namespace fugue::platform
