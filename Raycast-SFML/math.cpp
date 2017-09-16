#include "math.hpp"

#include <cmath>

namespace math
{
    RayHit raycast(sf::Vector2f start, sf::Vector2f dir) noexcept
    {
        sf::Vector2u map = static_cast<sf::Vector2u>(start);

        sf::Vector2f dirSqd = { dir.x * dir.x, dir.y * dir.y };
        sf::Vector2f delta = { std::sqrtf(1.f + dirSqd.y / dirSqd.x), std::sqrtf(1.f + dirSqd.x / dirSqd.y) };

        sf::Vector2f sideDist;
        sf::Vector2i step;

        if (dir.x < 0)
        {
            step.x = -1;
            sideDist.x = (start.x - static_cast<float>(map.x)) * delta.x;
        }
        else
        {
            step.x = 1;
            sideDist.x = (static_cast<float>(map.x) + 1.f - start.x) * delta.x;
        }

        if (dir.y < 0)
        {
            step.y = -1;
            sideDist.y = (start.y - static_cast<float>(map.y)) * delta.y;
        }
        else
        {
            step.y = 1;
            sideDist.y = (static_cast<float>(map.y) + 1.f - start.y) * delta.y;
        }

        // Digital Differential Analysis
        RayHit ray;

        bool hit = false;
        while (!hit)
        {
            if (sideDist.x < sideDist.y)
            {
                sideDist.x += delta.x;
                map.x += step.x;
                ray.side = RayHit::Side::EastWest;
            }
            else
            {
                sideDist.y += delta.y;
                map.y += step.y;
                ray.side = RayHit::Side::NorthSouth;
            }

            hit = world::map(map) != world::Square::Empty;
        }
        
        ray.hitPoint = map;
        return ray;
    }

    float planeDistance(sf::Vector2f start, sf::Vector2f dir, RayHit ray) noexcept
    {
        sf::Vector2i step = { dir.x < 0 ? -1 : 1, dir.y < 0 ? -1 : 1 };

        auto calc = [](size_t hit, float start, int step, float dir)
        {
            return (static_cast<float>(hit) - start + static_cast<float>(1 - step) / 2.f) / dir;
        };

        switch (ray.side)
        {
            case RayHit::Side::EastWest:
                return calc(ray.hitPoint.x, start.x, step.x, dir.x);

            case RayHit::Side::NorthSouth:
                return calc(ray.hitPoint.y, start.y, step.y, dir.y);
        }
    }
}
