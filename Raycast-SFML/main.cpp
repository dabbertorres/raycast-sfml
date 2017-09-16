#include <iostream>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Event.hpp>

#include "world.hpp"
#include "math.hpp"

void render(std::vector<sf::Vertex>& screen, const world::Player& player, const sf::Vector2f& renderSize);

// forward/backward movement
void move(world::Player& player, float dt) noexcept;
void rotate(world::Player& player, float dt) noexcept;

void inputPress(world::Player& player, sf::Keyboard::Key key) noexcept;
void inputRelease(world::Player& player, sf::Keyboard::Key key) noexcept;

int main(int argc, char** argv)
{
    const sf::Vector2f renderSize{ 640, 360 };
    const sf::VideoMode windowMode{ 1280, 720 };
    const sf::Color clearColor{ 127, 127, 127 };

    sf::View renderView{ renderSize / 2.f, renderSize };

    sf::Font font;
    font.loadFromFile("cour.ttf");

    sf::Text frameTimeText{ "", font };
    frameTimeText.setPosition(10.f, 10.f);
    frameTimeText.setFillColor(sf::Color::Black);

    constexpr auto frameTimeBufLen = 7;
    char frameTimeBuf[frameTimeBufLen] = { '\0' };

    // we're drawing one line per x coordinate. Only need two points for a line
    std::vector<sf::Vertex> screen{ static_cast<size_t>(renderSize.x * 2) };

    // init each pair of vertices' positions to a single column
    // and make them transparent by default
    for (auto x = 0u; x < screen.size(); ++x)
    {
        screen[x].position.x = static_cast<float>(x / 2);
        screen[x].color = sf::Color::Transparent;
    }

    world::Player player{ {15.f, 15.f}, {-1.f, 0.f}, {0.f, 0.66f} };

    // loaded our resources - now open the window
    sf::RenderWindow window{ windowMode, "Raycaster" };

    //window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(false);
    window.setView(renderView);

    sf::Clock clock;
    auto lastTime = clock.getElapsedTime();

    while (window.isOpen())
    {
        ///////////////////////////////////////////////////////////////////////
        // event handling
        ///////////////////////////////////////////////////////////////////////

        sf::Event ev;
        while (window.pollEvent(ev))
        {
            switch (ev.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::KeyPressed:
                    if (ev.key.code == sf::Keyboard::Escape)
                        window.close();
                    else
                        inputPress(player, ev.key.code);
                    break;

                case sf::Event::KeyReleased:
                    inputRelease(player, ev.key.code);
                    break;

                default:
                    break;
            }
        }

        ///////////////////////////////////////////////////////////////////////
        // update
        ///////////////////////////////////////////////////////////////////////

        auto nowTime = clock.getElapsedTime();
        auto frameTime = nowTime - lastTime;
        lastTime = nowTime;

        float dt = frameTime.asSeconds();
        std::snprintf(frameTimeBuf, frameTimeBufLen, "%0*.*f", frameTimeBufLen, frameTimeBufLen - 2, dt);
        frameTimeText.setString(frameTimeBuf);

        move(player, dt);
        rotate(player, dt);

        ///////////////////////////////////////////////////////////////////////
        // rendering
        ///////////////////////////////////////////////////////////////////////

        render(screen, player, renderSize);

        ///////////////////////////////////////////////////////////////////////
        // display!
        ///////////////////////////////////////////////////////////////////////

        window.clear(clearColor);

        window.setView(renderView);
        window.draw(screen.data(), screen.size(), sf::PrimitiveType::Lines);

        window.setView(window.getDefaultView());
        window.draw(frameTimeText);

        window.display();
    }

    return 0;
}

void render(std::vector<sf::Vertex>& screen, const world::Player& player, const sf::Vector2f& renderSize)
{
    const auto centerLine = renderSize.y / 2.f;

    // cast a ray on each vertical line of the viewport,
    // then set vertices for that line accordingly
    for (auto x = 0u; x < static_cast<decltype(x)>(renderSize.x); ++x)
    {
        // transform to camera space
        float cameraX = 2.f * x / renderSize.x - 1;
        auto rayDir = player.face + player.cameraPlane * cameraX;

        auto ray = math::raycast(player.position, rayDir);
        auto dist = math::planeDistance(player.position, rayDir, ray);

        auto idx = x * 2;
        auto& top = screen[idx];
        auto& bot = screen[idx + 1];

        auto halfHeight = renderSize.y / dist / 2.f;
        top.position.y = centerLine - halfHeight;
        bot.position.y = centerLine + halfHeight;
        
        auto color = world::color(ray.hitPoint);
        top.color = color;
        bot.color = color;
    }
}

void move(world::Player& player, float dt) noexcept
{
    sf::Vector2f newPos = player.position + player.face * player.move * player.moveSpeed * dt;
    if (world::map(sf::Vector2f{ newPos.x, player.position.y }) == world::Square::Empty)
        player.position.x = newPos.x;

    if (world::map(sf::Vector2f{ player.position.x, newPos.y }) == world::Square::Empty)
        player.position.y = newPos.y;
}

void rotate(world::Player& player, float dt) noexcept
{
    float rotation = player.rotate * player.rotSpeed * dt;

    float cos = std::cos(rotation);
    float sin = std::sin(rotation);

    float faceX = player.face.x;
    player.face.x = faceX * cos - player.face.y * sin;
    player.face.y = faceX * sin + player.face.y * cos;

    float planeX = player.cameraPlane.x;
    player.cameraPlane.x = planeX * cos - player.cameraPlane.y * sin;
    player.cameraPlane.y = planeX * sin + player.cameraPlane.y * cos;
}

void inputPress(world::Player& player, sf::Keyboard::Key key) noexcept
{
    switch (key)
    {
        case sf::Keyboard::Up:
        case sf::Keyboard::W:
            player.move += 1.f;
            break;

        case sf::Keyboard::Down:
        case sf::Keyboard::S:
            player.move -= 1.f;
            break;

        case sf::Keyboard::Left:
        case sf::Keyboard::A:
            player.rotate += 1.f;
            break;

        case sf::Keyboard::Right:
        case sf::Keyboard::D:
            player.rotate -= 1.f;
            break;

        default:
            break;
    }
}

void inputRelease(world::Player& player, sf::Keyboard::Key key) noexcept
{
    switch (key)
    {
        case sf::Keyboard::Up:
        case sf::Keyboard::W:
            player.move -= 1.f;
            break;

        case sf::Keyboard::Down:
        case sf::Keyboard::S:
            player.move += 1.f;
            break;

        case sf::Keyboard::Left:
        case sf::Keyboard::A:
            player.rotate -= 1.f;
            break;

        case sf::Keyboard::Right:
        case sf::Keyboard::D:
            player.rotate += 1.f;
            break;

        default:
            break;
    }
}
