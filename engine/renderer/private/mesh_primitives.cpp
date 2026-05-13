#include "mesh_primitives.hpp"

#include <array>
#include <glm/gtc/constants.hpp>

void AddTriangle(std::vector<bb::u32>& indices, std::array<bb::u32, 3> triangle)
{
    indices.emplace_back(triangle[0]);
    indices.emplace_back(triangle[1]);
    indices.emplace_back(triangle[2]);
}

CPUMesh<Vertex> GenerateUVSphere(bb::u32 slices, bb::u32 stacks, float radius)
{
    CPUMesh<Vertex> mesh;

    bb::u32 totalVertices = 2 + (stacks - 1) * slices;

    mesh.vertices.reserve(totalVertices);

    // TODO: Consider this based on the total amount of indices instead.
    mesh.materialIndex = 0;

    for (bb::u32 i = 0; i <= stacks; ++i)
    {
        float theta = i * glm::pi<float>() / stacks;

        for (bb::u32 j = 0; j <= slices; ++j)
        {
            float phi = j * (glm::pi<float>() * 2.0f) / slices;
            glm::vec3 point {
                cosf(phi) * sinf(theta),
                cosf(theta),
                sinf(phi) * sinf(theta)
            };

            float u = static_cast<float>(j) / slices;
            float v = static_cast<float>(i) / stacks;
            glm::vec2 texCoords { u, v };
            glm::vec3 position { point * radius };

            mesh.vertices.emplace_back(position, point, glm::vec4 {}, texCoords);
        }
    }

    using Triangle = std::array<bb::u32, 3>;
    for (bb::u32 i = 0; i < stacks; ++i)
    {
        for (bb::u32 j = 0; j < slices; ++j)
        {
            bb::u32 first = i * (slices + 1) + j;
            bb::u32 second = first + slices + 1;

            AddTriangle(mesh.indices, Triangle { first, second, first + 1 });
            AddTriangle(mesh.indices, Triangle { second, second + 1, first + 1 });
        }
    }

    mesh.boundingRadius = radius;
    mesh.boundingBox = { glm::vec3 { -radius }, glm::vec3 { radius } };
    return mesh;
}
