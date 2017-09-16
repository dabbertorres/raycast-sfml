#include "world.hpp"

namespace world
{
    // lazy as hell
    #define E Square::Empty
    #define O Square::Outer
    #define I Square::Inner1
    #define J Square::Inner2
    #define C Square::Column

    static const std::array<Square, width * height> mapData =
    {
        O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O,
        O, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, I, I, I, I, I, E, E, E, E, C, E, C, E, C, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, I, E, E, E, I, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, I, E, E, E, I, E, E, E, E, C, E, E, E, C, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, I, E, E, E, I, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, I, I, E, I, I, E, E, E, E, C, E, C, E, C, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, J, J, J, J, J, J, J, J, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, J, E, J, E, E, E, E, J, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, J, E, E, E, E, E, E, J, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, J, E, J, E, E, E, E, J, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, J, E, J, J, J, J, J, J, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, J, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, J, J, J, J, J, J, J, J, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, I, I, I, I, I, E, E, E, E, C, E, C, E, C, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, I, E, E, E, I, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, O,
        O, E, E, E, E, E, I, E, E, E, I, E, E, E, E, C, E, E, E, C, E, E, E, E, E, E, E, E, E, E, E, O,
        O, O, O, O, O, O, I, O, O, O, I, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O, O,
    };

    // I do clean up after myself, however
    #undef E
    #undef O
    #undef I
    #undef J
    #undef C

    sf::Color color(sf::Vector2u pos) noexcept
    {
        switch (map(pos))
        {
            case Square::Empty:
                return sf::Color::Transparent;

            case Square::Outer:
                return sf::Color::Blue;

            case Square::Inner1:
                return sf::Color::Red;

            case Square::Inner2:
                return sf::Color::Green;

            case Square::Column:
                return sf::Color::Yellow;
        }
    }

    Square map(sf::Vector2u pos) noexcept
    {
        return mapData[static_cast<size_t>(pos.x) + static_cast<size_t>(pos.y) * width];
    }

    Square map(sf::Vector2f pos) noexcept
    {
        return mapData[static_cast<size_t>(pos.x) + static_cast<size_t>(pos.y) * width];
    }
}
