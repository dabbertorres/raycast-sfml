#include <vector>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Event.hpp>

#include "world.hpp"
#include "math.hpp"

using Screen = std::vector<sf::Vertex>;

void render(Screen& screen, const world::Player& player) noexcept;

// forward/backward movement
void move(world::Player& player, float dt) noexcept;
void rotate(world::Player& player, float dt) noexcept;

void inputPress(world::Player& player, sf::Keyboard::Key key) noexcept;
void inputRelease(world::Player& player, sf::Keyboard::Key key) noexcept;

void initScreen(Screen& screen) noexcept;

const sf::Vector2f renderSize{ 640, 360 };
const sf::VideoMode windowMode{ 1280, 720 };

constexpr auto vertsPerColumn = 6u;

int main(int argc, char** argv)
{
    sf::View renderView{ renderSize / 2.f, renderSize };

    sf::Font font;
    font.loadFromFile("cour.ttf");

    sf::Text frameTimeText{ "", font };
    frameTimeText.setPosition(10.f, 10.f);
    frameTimeText.setFillColor(sf::Color::Black);

    constexpr auto frameTimeBufLen = 7;
    char frameTimeBuf[frameTimeBufLen + 1] = { '\0' };

    // So, partial lie. This isn't "full" raycasting. I mean, there is raycasting. But I'm not pushing
    // a whole screen buffer every frame. I'm only pushing vertices to draw lines. So. Yeah.

    // we're drawing 3 lines per x coordinate. 2 vertices per line
    // line 1 (v0, v1): ceiling
    // line 2 (v2, v3): walls/empty/etc
    // line 3 (v4, v5): floor
    std::vector<sf::Vertex> screen{ static_cast<size_t>(renderSize.x * 2 * 3) };

    initScreen(screen);

    world::Player player{ {15.f, 15.f}, {-1.f, 0.f}, {0.f, 0.66f} };

    // loaded our resources - now open the window
    sf::RenderWindow window{ windowMode, "Raycaster" };

    //window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(false);
    window.setView(renderView);

    sf::Clock clock;
    auto lastTime = clock.getElapsedTime();

    ///////////////////////////////////////////////////////////////////////
    // let's get started already!
    ///////////////////////////////////////////////////////////////////////

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
        std::snprintf(frameTimeBuf, sizeof(frameTimeBuf), "%0*.*f", frameTimeBufLen, frameTimeBufLen - 2, dt);
        frameTimeText.setString(frameTimeBuf);

        move(player, dt);
        rotate(player, dt);

        ///////////////////////////////////////////////////////////////////////
        // rendering
        ///////////////////////////////////////////////////////////////////////

        render(screen, player);

        ///////////////////////////////////////////////////////////////////////
        // display!
        ///////////////////////////////////////////////////////////////////////

        window.clear();

        window.setView(renderView);
        window.draw(screen.data(), screen.size(), sf::PrimitiveType::Lines);

        window.setView(window.getDefaultView());
        window.draw(frameTimeText);

        window.display();
    }

    return 0;
}

void render(Screen& screen, const world::Player& player) noexcept
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

        auto idx = x * vertsPerColumn;
        auto& ceilBot  = screen[idx + 1];
        auto& wallTop  = screen[idx + 2];
        auto& wallBot  = screen[idx + 3];
        auto& floorTop = screen[idx + 4];

        auto halfHeight = renderSize.y / dist / 2.f;
        wallTop.position.y = centerLine - halfHeight;
        wallBot.position.y = centerLine + halfHeight;

        ceilBot.position.y = wallTop.position.y;
        floorTop.position.y = wallBot.position.y;
        
        auto color = world::color(ray.hitPoint);
        wallTop.color = color;
        wallBot.color = color;
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

void initScreen(Screen& screen) noexcept
{
    // init each pair of vertices' positions to a single column
    // and make them transparent by default
    for (auto x = 0u; x < screen.size(); ++x)
    {
        screen[x].position.x = static_cast<float>(x / vertsPerColumn);
        
        // now, the top ceiling and the bottom floor y component never change.
        // and ceiling and floor colors never change. so set them all here too.

        switch (x % 6)
        {
            case 0:
                screen[x].position.y = 0.f;
            case 1:
                screen[x].color = world::ceilingColor;
                break;

            case 5:
                screen[x].position.y = renderSize.y;
            case 4:
                screen[x].color = world::floorColor;
                break;

            default:
                break;
        }
    }
}
