#pragma once

#include "game/Level.h"

#include <algorithm>
#include <cmath>
#include <span>

namespace ag
{
    // 정적 박스의 합집합에서 노출된 면만 생성한다. 충돌 데이터는 그대로 둔다.
    inline void BuildBoxSurface(std::span<const Wall> boxes, std::vector<float>& vertices, std::vector<unsigned int>& indices)
    {
        constexpr float epsilon = 0.0001f;
        struct Bounds
        {
            glm::vec3 min;
            glm::vec3 max;
        };
        struct Rect
        {
            float left;
            float right;
            float bottom;
            float top;
        };

        vertices.clear();
        indices.clear();
        std::vector<Bounds> bounds;
        bounds.reserve(boxes.size());
        for (const auto& box : boxes)
            bounds.push_back({ box.center - box.size * 0.5f, box.center + box.size * 0.5f });

        std::vector<Rect> exposed;
        std::vector<Rect> remainder;
        for (std::size_t i = 0; i < bounds.size(); ++i)
        {
            for (int axis = 0; axis < 3; ++axis)
            {
                const int u = (axis + 1) % 3;
                const int v = (axis + 2) % 3;
                for (const bool positive : { false, true })
                {
                    const Bounds& box = bounds[i];
                    const float plane = positive ? box.max[axis] : box.min[axis];
                    const float orientedPlane = positive ? plane : -plane;
                    exposed = { { box.min[u], box.max[u], box.min[v], box.max[v] } };

                    for (std::size_t j = 0; j < bounds.size() && !exposed.empty(); ++j)
                    {
                        if (i == j)
                            continue;
                        const Bounds& other = bounds[j];
                        const float otherNear = positive ? other.min[axis] : -other.max[axis];
                        const float otherFar = positive ? other.max[axis] : -other.min[axis];
                        // 같은 높이의 면은 뒤쪽 박스가 맡고, 바깥을 덮는 박스는 내부 면을 제거한다.
                        const bool claimsPlane = otherFar > orientedPlane + epsilon
                            || (j > i && std::abs(otherFar - orientedPlane) <= epsilon);
                        if (otherNear > orientedPlane + epsilon || !claimsPlane)
                            continue;
                        if (other.min[u] >= box.max[u] || other.max[u] <= box.min[u]
                            || other.min[v] >= box.max[v] || other.max[v] <= box.min[v])
                            continue;

                        remainder.clear();
                        const auto keep = [&](float l, float r, float b, float t)
                        {
                            if (r - l > epsilon && t - b > epsilon)
                                remainder.push_back({ l, r, b, t });
                        };
                        for (const Rect& face : exposed)
                        {
                            const float left = std::max(face.left, other.min[u]);
                            const float right = std::min(face.right, other.max[u]);
                            const float bottom = std::max(face.bottom, other.min[v]);
                            const float top = std::min(face.top, other.max[v]);
                            if (right - left <= epsilon || top - bottom <= epsilon)
                            {
                                remainder.push_back(face);
                                continue;
                            }
                            keep(face.left, left, face.bottom, face.top);
                            keep(right, face.right, face.bottom, face.top);
                            keep(left, right, face.bottom, bottom);
                            keep(left, right, top, face.top);
                        }
                        exposed.swap(remainder);
                    }

                    glm::vec3 normal{ 0.0f };
                    normal[axis] = positive ? 1.0f : -1.0f;
                    const glm::vec3 color = boxes[i].color;
                    for (const Rect& face : exposed)
                    {
                        const auto base = static_cast<unsigned int>(vertices.size() / 9);
                        const auto append = [&](float x, float y)
                        {
                            glm::vec3 point{ 0.0f };
                            point[axis] = plane;
                            point[u] = x;
                            point[v] = y;
                            vertices.insert(vertices.end(), { point.x, point.y, point.z,
                                color.r, color.g, color.b, normal.x, normal.y, normal.z });
                        };
                        append(face.left, face.bottom);
                        append(face.right, face.bottom);
                        append(face.right, face.top);
                        append(face.left, face.top);
                        if (positive)
                            indices.insert(indices.end(), { base, base + 1, base + 2, base, base + 2, base + 3 });
                        else
                            indices.insert(indices.end(), { base, base + 2, base + 1, base, base + 3, base + 2 });
                    }
                }
            }
        }
    }
}
