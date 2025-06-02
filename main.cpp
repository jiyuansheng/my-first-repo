#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <cmath>
#include <random>
#include <iostream>
#include <cstdint>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Particle {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float lifetime;
    float maxLifetime;
    sf::Color color;

    Particle(sf::Vector2f pos, sf::Vector2f vel, float life, sf::Color col)
            : position(pos), velocity(vel), lifetime(life), maxLifetime(life), color(col) {}

    void update(float deltaTime) {
        position += velocity * deltaTime;
        lifetime -= deltaTime;
        float alpha = (lifetime / maxLifetime) * 255.f;
        color.a = static_cast<uint8_t>(std::clamp(alpha, 0.f, 255.f));
    }

    bool isDead() const {
        return lifetime <= 0;
    }
};

bool isInsideHeart(float x, float y, float size) {
    float nx = x / size;
    float ny = y / size;
    float eq = (nx * nx + ny * ny - 1);
    eq = eq * eq * eq - nx * nx * ny * ny * ny;
    return eq <= 0;
}

sf::VertexArray createHeart(float size, float beatFactor, sf::Color color) {
    sf::VertexArray heart(sf::TriangleFan);

    heart.append(sf::Vertex(sf::Vector2f(0.f, 0.f), color));

    const int numPoints = 100;
    for (int i = 0; i <= numPoints; ++i) {
        float angle = (i / static_cast<float>(numPoints)) * 2 * M_PI;
        float r = 0.f;

        for (float testR = 0; testR < size * 3.f; testR += 0.5f) {
            float x = testR * std::cos(angle);
            float y = testR * std::sin(angle);
            if (isInsideHeart(x, y * beatFactor, size)) {
                r = testR;
            } else if (r > 0.f) {
                break;
            }
        }

        float x = r * std::cos(angle);
        float y = -r * std::sin(angle) / beatFactor;
        heart.append(sf::Vertex(sf::Vector2f(x, y), color));
    }

    return heart;
}

int main() {
    sf::String title = sf::String::fromUtf8(
            u8"跳动的粉色爱心",
            u8"跳动的粉色爱心" + std::char_traits<char>::length(u8"跳动的粉色爱心")
    );
    sf::RenderWindow window(sf::VideoMode(800, 600), title);
    window.setFramerateLimit(60);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(0, 2 * M_PI);
    std::uniform_real_distribution<float> speedDist(20.0f, 50.0f);
    std::uniform_real_distribution<float> lifeDist(1.0f, 3.0f);
    std::uniform_int_distribution<int> greenOffset(0, 50);
    std::uniform_int_distribution<int> blueOffset(0, 75);

    std::vector<Particle> particles;

    const float baseSize = 100.0f;
    float beatTimer = 0.0f;

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float deltaTime = clock.restart().asSeconds();
        beatTimer += deltaTime;

        float beatFactor = 1.0f + 0.1f * std::sin(beatTimer * 4.0f);
        float heartSize = baseSize * (1.0f + 0.05f * std::sin(beatTimer * 4.0f));

        sf::VertexArray heart = createHeart(heartSize, beatFactor, sf::Color(255, 105, 180));

        // 粒子生成逻辑
        if (particles.size() < 500) {
            for (int i = 0; i < 5; ++i) {
                float angle = angleDist(gen);
                float speed = speedDist(gen);
                float r = heartSize * (0.9f + 0.2f * std::sin(angle));

                float x = r * std::cos(angle);
                float y = -r * std::sin(angle) / beatFactor;

                sf::Vector2f position(x + 400, y + 300);
                sf::Vector2f velocity(speed * std::cos(angle), speed * std::sin(angle));

                float lifetime = lifeDist(gen);

                int green = std::min(255, 105 + greenOffset(gen));
                int blue  = std::min(255, 180 + blueOffset(gen));

                sf::Color color(255, green, blue, 255);
                particles.emplace_back(position, velocity, lifetime, color);
            }
        }

        for (auto it = particles.begin(); it != particles.end();) {
            it->update(deltaTime);
            if (it->isDead())
                it = particles.erase(it);
            else
                ++it;
        }

        // 绘制部分
        window.clear(sf::Color(30, 30, 30));

        sf::Transform transform;
        transform.translate(400.f, 300.f);
        window.draw(heart, transform);

        for (const auto& particle : particles) {
            sf::CircleShape circle(2);
            circle.setFillColor(particle.color);
            circle.setPosition(particle.position - sf::Vector2f(2, 2));
            window.draw(circle);
        }

        window.display();
    }

    return 0;
}