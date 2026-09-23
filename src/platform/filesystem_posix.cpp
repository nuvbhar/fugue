#include "filesystem.hpp"
#include <cstdlib>

namespace fugue::platform {

static auto get_env_or_default(const char* env_var, const std::filesystem::path& default_suffix) -> std::expected<std::filesystem::path, std::string> {
    if (const char* env_val = std::getenv(env_var); env_val && *env_val) {
        std::filesystem::path p(env_val);
        if (p.is_absolute()) return p / "fugue";
    }
    
    if (const char* home = std::getenv("HOME"); home && *home) {
        std::filesystem::path p(home);
        if (p.is_absolute()) return p / default_suffix / "fugue";
    }
    
    return std::unexpected(std::string("Cannot determine absolute path for ") + env_var);
}

auto config_dir() -> std::expected<std::filesystem::path, std::string> {
    return get_env_or_default("XDG_CONFIG_HOME", ".config");
}

auto data_dir() -> std::expected<std::filesystem::path, std::string> {
    return get_env_or_default("XDG_DATA_HOME", ".local/share");
}

auto cache_dir() -> std::expected<std::filesystem::path, std::string> {
    return get_env_or_default("XDG_CACHE_HOME", ".cache");
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
