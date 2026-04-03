#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>

bool loadModel(const std::filesystem::path& path, fastgltf::Asset& outAsset)
{
    fastgltf::Parser parser { fastgltf::Extensions::KHR_lights_punctual | fastgltf::Extensions::KHR_texture_transform };

    auto data = fastgltf::GltfDataBuffer::FromPath(path);
    if (data.error() != fastgltf::Error::None)
    {
        spdlog::error("Failed to read file: {}", fastgltf::getErrorMessage(data.error()));
        return false;
    }

    constexpr auto options = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble | fastgltf::Options::LoadExternalBuffers | fastgltf::Options::LoadExternalImages;

    auto asset = parser.loadGltf(data.get(), path.parent_path(), options);
    if (asset.error() != fastgltf::Error::None)
    {
        spdlog::error("Failed to load model: {}", fastgltf::getErrorMessage(asset.error()));
        return false;
    }

    outAsset = std::move(asset.get());
    return true;
}

std::span<const std::byte> buffer_span(const fastgltf::Buffer& buffer)
{
    fastgltf::visitor v = {
        [](const fastgltf::sources::Vector& vector) -> std::span<const std::byte>
        {
            return vector.bytes;
        },
        [](const fastgltf::sources::ByteView& byteView) -> std::span<const std::byte>
        {
            return byteView.bytes;
        },
        [](const fastgltf::sources::Array& array) -> std::span<const std::byte>
        {
            // Raw bytes in a std::array
            return std::span<const std::byte>(array.bytes);
        },
        [](const fastgltf::sources::BufferView&) -> std::span<const std::byte>
        {
            return {};
        },
        [](const fastgltf::sources::URI&) -> std::span<const std::byte>
        {
            return {};
        },
        [](const std::monostate&) -> std::span<const std::byte>
        {
            return {};
        },
        [](const fastgltf::sources::CustomBuffer&) -> std::span<const std::byte>
        {
            return {};
        },
        [](const fastgltf::sources::Fallback&) -> std::span<const std::byte>
        {
            return {};
        }, // catch-all for safety
    };

    return std::visit(v, buffer.data);
}

bool isPNG(const std::byte* data)
{
    return data[0] == std::byte(0x89) && data[1] == std::byte(0x50) && // 'P'
        data[2] == std::byte(0x4E) && // 'N'
        data[3] == std::byte(0x47); // 'G'
}

int main(int argc, char* argv[])
{
    fastgltf::Asset asset;

    if (argc < 3)
    {
        spdlog::info("Provide <input> <output_dir> paths");
        return 1;
    }

    const char* input = argv[1];
    std::string output = argv[2];

    if (!loadModel(input, asset))
    {
        return 1;
    }

    std::unordered_map<std::string, int> name_counts;
    for (auto& image : asset.images)
    {
        fastgltf::visitor v {
            [](fastgltf::sources::Vector& vector) -> std::span<const std::byte>
            {
                return std::span<std::byte>(vector.bytes);
            },
            [&asset](fastgltf::sources::BufferView& bufferView) -> std::span<const std::byte>
            {
                // Embedded in a buffer view (e.g. GLB)
                auto& view = asset.bufferViews.at(bufferView.bufferViewIndex);
                auto& buffer = asset.buffers.at(view.bufferIndex);
                auto span = buffer_span(buffer);

                return span.subspan(view.byteOffset, view.byteLength);
            },
            [](fastgltf::sources::Array& array) -> std::span<const std::byte>
            {
                // Raw bytes in a std::array
                return std::span<std::byte>(array.bytes);
            },
            [](fastgltf::sources::ByteView& byteView) -> std::span<const std::byte>
            {
                // Non-owning view into bytes
                return byteView.bytes;
            },
            [](auto&) -> std::span<const std::byte>
            { return {}; } // catch-all for safety
        };

        std::span<const std::byte> image_data = std::visit(v, image.data);

        if (isPNG(image_data.data()))
        {
            std::string name = image.name.c_str();

            int count = name_counts[name]++;
            if (count > 0)
            {
                name += "_" + std::to_string(count);
            }

            std::filesystem::path path = output + "/images/" + name + ".png";
            std::filesystem::create_directories(path.parent_path());
            std::ofstream file { path, std::ios::binary };
            file.write((const char*)(image_data.data()), image_data.size());
        }
        else
        {
            spdlog::error("Found non PNG file");
        }
    }

    spdlog::info("Loaded {} meshes", asset.meshes.size());
    spdlog::info("Loaded {} textures", asset.images.size());
    return 0;
}