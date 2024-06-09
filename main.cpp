#include <SFML/Graphics.hpp>

using namespace sf;

int main() {
    RenderWindow window(VideoMode(1920, 1080), "3D engine", Style::Fullscreen);
    CircleShape shape(100.f);
    shape.setFillColor(Color::Green);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    
}
