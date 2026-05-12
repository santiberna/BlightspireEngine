#pragma once

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

#include "file_io.hpp"

template <class T>
class DataStore
{
public:
    DataStore(std::string path)
        : _path(std::move(path))
    {
        ReadFile();
    }

    void Write()
    {
        if (auto stream = fileIO::OpenWriteStream(_path))
        {
            cereal::JSONOutputArchive archive { stream.value() };
            archive(cereal::make_nvp("data", data));
        }
    }

    void SetPathAndReimport(const std::string& path)
    {
        _path = path;
        ReadFile();
    }

    const std::string GetCurrentPath() const
    {
        return _path;
    }

    ~DataStore() = default;

    T data;

private:
    void ReadFile()
    {
        if (auto stream = fileIO::OpenReadStream(_path))
        {
            cereal::JSONInputArchive archive { stream.value() };
            archive(cereal::make_nvp("data", data));
        }
    }

    std::string _path;
};
