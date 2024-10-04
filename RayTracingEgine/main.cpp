#include <SFML/Graphics.hpp>

using namespace std;

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "3D engine", sf::Style::Fullscreen);
    
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    sf::Shader shader;
    shader.loadFromFile("fragment_shader.frag", sf::Shader::Fragment);

    sf::RectangleShape shape(sf::Vector2f(window.getSize().x, window.getSize().y));
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed) window.close();
        }
        window.clear();


        window.draw(shape, &shader);


        window.display();
    }

    return 0;
}