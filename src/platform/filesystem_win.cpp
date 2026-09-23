#include "filesystem.hpp"
#include <windows.h>
#include <shlobj.h>

namespace fugue::platform {

static auto get_known_folder(REFKNOWNFOLDERID folder_id) -> std::expected<std::filesystem::path, std::string> {
    PWSTR path = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(folder_id, 0, nullptr, &path))) {
        std::filesystem::path result(path);
        CoTaskMemFree(path);
        return result / "fugue";
    }
    return std::unexpected("SHGetKnownFolderPath failed");
}

auto config_dir() -> std::expected<std::filesystem::path, std::string> {
    return get_known_folder(FOLDERID_RoamingAppData);
}

auto data_dir() -> std::expected<std::filesystem::path, std::string> {
    return get_known_folder(FOLDERID_LocalAppData);
}

auto cache_dir() -> std::expected<std::filesystem::path, std::string> {
    return data_dir().transform([](const auto& p) { return p / "cache"; });
}

auto plugin_dir() -> std::expected<std::filesystem::path, std::string> {
    return config_dir().transform([](const auto& p) { return p / "plugins"; });
}

auto ensure_dirs_exist() -> std::expected<void, std::string> {
    auto config = config_dir();
    if (!config) return std::unexpected(config.error());
    auto data = data_dir();
    if (!data) return std::unexpected(data.error());
    auto cache = cache_dir();
    if (!cache) return std::unexpected(cache.error());
    auto plugin = plugin_dir();
    if (!plugin) return std::unexpected(plugin.error());

    std::error_code ec;
    std::filesystem::create_directories(*config, ec);
    if (ec) return std::unexpected("Failed to create config dir: " + ec.message());
    
    std::filesystem::create_directories(*data, ec);
    if (ec) return std::unexpected("Failed to create data dir: " + ec.message());
    
    std::filesystem::create_directories(*cache, ec);
    if (ec) return std::unexpected("Failed to create cache dir: " + ec.message());
    
    std::filesystem::create_directories(*plugin, ec);
    if (ec) return std::unexpected("Failed to create plugin dir: " + ec.message());
    
    return {};
}

} // namespace fugue::platform
