#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

using namespace sf;
using namespace std;


class Camera{
    public:
        Vector3f position = Vector3f(0.0, 0.0, 0.0);
        Vector3f direction = Vector3f(1.0, 0.0, 0.0);
        Vector2f mouse_position = Vector2f(0.0, 0.0);
        float speed = 0.05;

        void checkMovements(){
            if (Keyboard::isKeyPressed(Keyboard::W)){
                position += direction * speed;
            }
            if (Keyboard::isKeyPressed(Keyboard::S)){
                position -= direction * speed;
            }
            if (Keyboard::isKeyPressed(Keyboard::A)){
                position += speed * Vector3f(direction.z, 0, direction.x);
            }
            if (Keyboard::isKeyPressed(Keyboard::D)){
                position -= speed * Vector3f(direction.z, 0, direction.x);
            }
            if (Keyboard::isKeyPressed(Keyboard::Space)){
                position += speed * Vector3f(0, 1, 0);
            }
            if (Keyboard::isKeyPressed(Keyboard::LControl)){
                position += speed * Vector3f(0, -1, 0);
            }
        }
        
        void checkMouseMovements(int WIDTH, int HEIGHT){
            float mouse_delta_x = float(Mouse::getPosition().x - WIDTH / 2);
            float mouse_delta_y = float(Mouse::getPosition().y - HEIGHT / 2);
            Mouse::setPosition({WIDTH / 2, HEIGHT / 2});
            mouse_position = Vector2f(mouse_position.x + mouse_delta_x, mouse_position.y + mouse_delta_y);
        }
};


int main()
{
    int WIDTH = 800, HEIGHT = 600;
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Ray Tracing");

    Texture texture;
    texture.create(WIDTH, HEIGHT);
    Sprite sprite(texture);

    Shader shader;
    shader.loadFromFile("shader.frag", Shader::Fragment);

    Camera camera;


    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear();

        //cout << camera.position.x << ' ' << camera.position.y << ' ' << camera.position.z << '\n';

        shader.setUniform("player_pos", camera.position);
        shader.setUniform("player_mouse_position", camera.mouse_position);
        shader.setUniform("resolution", Vector2f(float(WIDTH), float(HEIGHT)));

        window.draw(sprite, &shader);

        camera.checkMovements();
        camera.checkMouseMovements(WIDTH, HEIGHT);

        window.display();
    }
}