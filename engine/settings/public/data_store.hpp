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
        if (auto stream = fileIO::OpenReadStream(_path))
        {
            cereal::JSONInputArchive archive { stream.value() };
            archive(cereal::make_nvp("data", data));
        }
    }

    void Write()
    {
        if (auto stream = fileIO::OpenWriteStream(_path))
        {
            cereal::JSONOutputArchive archive { stream.value() };
            archive(cereal::make_nvp("data", data));
        }
    }

    ~DataStore() = default;

    T data;

private:
    std::string _path;
};
