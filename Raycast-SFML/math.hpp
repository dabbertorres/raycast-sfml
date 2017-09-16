#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include "world.hpp"

namespace math
{
    struct RayHit
    {
        sf::Vector2u hitPoint;
        enum class Side
        {
            NorthSouth,
            EastWest,
        } side;
    };

    RayHit raycast(sf::Vector2f start, sf::Vector2f dir) noexcept;
    float planeDistance(sf::Vector2f start, sf::Vector2f dir, RayHit ray) noexcept;
}
