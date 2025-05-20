#include "Physics.h"

#include <Box2D/Common/Math.hpp>
#include <Box2D/Dynamics/World.hpp>
#include <SFML/Graphics.hpp>

b2::World *Physics::world{};
MyDebugDraw *Physics::debugDraw{};

class MyDebugDraw {
public:
  MyDebugDraw(sf::RenderTarget &target) : target(target) {}

  void DrawPolygon(const b2::Vec2 *vertices, int32_t vertexCount,
                   const sf::Color &color) {
    sf::ConvexShape shape(vertexCount);
    for (int i = 0; i < vertexCount; i++) {
      shape.setPoint(i, sf::Vector2f(vertices[i].x, vertices[i].y));
    }

    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineThickness(0.02f);
    shape.setOutlineColor(color);
    target.draw(shape);
  }

  // Аналогично реализуйте остальные методы отрисовки, используя sf::Color и
  // box2d::Vec2

private:
  sf::RenderTarget &target;
};

// Реализуйте свой контакт-листенер, если нужно

void Physics::Init() {
  if (world)
    delete world;

  world = new b2::World(b2::Vec2(0.f, -9.8f)); // Укажите параметры, если нужно
}

void Physics::Update(float deltaTime) {
  // world->Step(deltaTime, 8, 3); // Используйте правильный метод вашей версии
  // Box2D
}

void Physics::DebugDraw(Renderer &renderer) {
  if (!debugDraw) {
    debugDraw = new MyDebugDraw(renderer.target);
    // debugDraw->SetFlags(0u); // Если нужно
    // world->SetDebugDraw(debugDraw); // Если нужно
  }

  // world->DebugDraw(); // Если нужно
}
