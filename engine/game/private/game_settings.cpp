#include "game_settings.hpp"

#include <cereal/archives/json.hpp>
#include <fstream>

GameSettings GameSettings::FromFile(const std::string& path)
{
    GameSettings out {};
    if (auto stream = std::ifstream { path })
    {
        cereal::JSONInputArchive ar { stream };

        try
        {
            ar(out);
        }
        catch (const std::exception& e)
        {
            spdlog::warn("Outdated settings file, reverting to defaults.");
            out.SaveToFile(path);
        }
    }
    else
    {
        out.SaveToFile(path);
    }
    return out;
}
void GameSettings::SaveToFile(const std::string& path) const
{
    if (auto stream = std::ofstream { path })
    {
        cereal::JSONOutputArchive ar { stream };
        ar(*this);
    }
    else
    {
        spdlog::error("Error serializing user settings: {}", path);
    }
}
