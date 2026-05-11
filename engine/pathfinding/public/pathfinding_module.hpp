#pragma once

#include "module_interface.hpp"
#include "renderer.hpp"
#include <glm/glm.hpp>
#include <queue>
#include <unordered_map>

struct PathNode
{
    glm::vec3 centre;
};

struct ComputedPath
{
    ComputedPath() = default;
    ~ComputedPath() = default;

    std::vector<PathNode> waypoints;
};

struct TriangleNode
{
    float totalCost;
    float estimateToGoal;
    float totalEstimatedCost;
    bb::u32 triangleIndex;
    bb::u32 parentTriangleIndex;

    bool operator<(const TriangleNode& rhs)
    {
        return this->totalCost < rhs.totalCost;
    }
    bool operator>(const TriangleNode& rhs)
    {
        return this->totalCost > rhs.totalCost;
    }

    bool operator==(const TriangleNode& rhs)
    {
        return this->triangleIndex == rhs.triangleIndex;
    }
};

inline bool operator<(const TriangleNode& lhs, const TriangleNode& rhs)
{
    return lhs.totalCost < rhs.totalCost;
}

inline bool operator>(const TriangleNode& lhs, const TriangleNode& rhs)
{
    return lhs.totalCost > rhs.totalCost;
}

inline bool operator==(const TriangleNode& lhs, const TriangleNode& rhs)
{
    return lhs.triangleIndex == rhs.triangleIndex;
}

template <class _Ty, class _Container = std::vector<_Ty>, class _Pr = std::less<typename _Container::value_type>>
struct IterablePriorityQueue : public std::priority_queue<_Ty, _Container, _Pr>
{
public:
    typename std::vector<_Ty>::iterator begin()
    {
        return this->c.begin();
    }

    typename std::vector<_Ty>::iterator end()
    {
        return this->c.end();
    }

    typename _Container::iterator find(const _Ty& item)
    {
        return std::find(this->c.begin(), this->c.end(), item);
    }
};

class PathfindingModule : public ModuleInterface
{
    ModuleTickOrder Init(Engine& engine) final;
    void Shutdown(Engine& engine) final;
    void Tick(Engine& engine) final;

public:
    PathfindingModule();
    ~PathfindingModule();

    NON_COPYABLE(PathfindingModule);
    NON_MOVABLE(PathfindingModule);

    bb::i32 SetNavigationMesh(std::string_view filePath);
    ComputedPath FindPath(glm::vec3 startPos, glm::vec3 endPos);

    bool SetDebugDrawState(bool state) { return _debugDraw = state; }
    bool GetDebugDrawState() const { return _debugDraw; }

    const std::vector<glm::vec3>& GetDebugLines() const { return _debugLines; }

private:
    float Heuristic(glm::vec3 startPos, glm::vec3 endPos);
    float DirectedHeuristic(glm::vec3 startPos, glm::vec3 endPos, glm::vec3 finalPosition);
    ComputedPath ReconstructPath(const bb::u32 finalTriangleIndex, std::unordered_map<bb::u32, TriangleNode>& nodes);

    struct TriangleInfo
    {
        bb::u32 indices[3] = {
            std::numeric_limits<bb::u32>::max(),
            std::numeric_limits<bb::u32>::max(),
            std::numeric_limits<bb::u32>::max()
        };

        glm::vec3 centre = glm::vec3 { 0.0f };

        bb::u32 adjacentTriangleIndices[3] = {
            std::numeric_limits<bb::u32>::max(),
            std::numeric_limits<bb::u32>::max(),
            std::numeric_limits<bb::u32>::max()
        };
        bb::u8 adjacentTriangleCount = 0;
    };

    std::vector<TriangleInfo> _triangles;
    std::unordered_map<bb::u32, bb::u32[3]> _trianglesToNeighbours;
    glm::mat4 _inverseTransform = glm::mat4(1.0f);

    CPUMesh<Vertex> _mesh = {};

    bool _debugDraw = false;
    std::vector<glm::vec3> _debugLines;
    std::vector<ComputedPath> _computedPaths;
};
