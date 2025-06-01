#include "Physics.h"

#include <Box2D/Common/Math.hpp>
#include <Box2D/Dynamics/World.hpp>
#include <Box2D/Common/Draw.hpp>
#include <SFML/Graphics.hpp>

b2::World *Physics::world{};
MyDebugDraw *Physics::debugDraw{};


class MyDebugDraw
	: public b2::Draw
{
public:
	MyDebugDraw(sf::RenderTarget& target)
		: target(target)
	{
	}

	virtual void DrawPolygon(const b2::Vec2* vertices, b2::int32 vertexCount,
		const b2::Color& color) override
	{
		sf::ConvexShape shape(vertexCount);
		for (int i = 0; i < vertexCount; i++)
		{
			shape.setPoint(i, sf::Vector2f(vertices[i].x, vertices[i].y));
		}

		shape.setFillColor(sf::Color::Transparent);
		shape.setOutlineThickness(0.02f);
		shape.setOutlineColor(sf::Color((sf::Uint8)(color.r * 255),
			(sf::Uint8)(color.g * 255), (sf::Uint8)(color.b * 255),
			(sf::Uint8)(color.a * 255)));
		target.draw(shape);
	}

	virtual void DrawSolidPolygon(const b2::Vec2* vertices, b2::int32 vertexCount,
		const b2::Color& color) override
	{
		sf::ConvexShape shape(vertexCount);
		for (int i = 0; i < vertexCount; i++)
		{
			shape.setPoint(i, sf::Vector2f(vertices[i].x, vertices[i].y));
		}

		shape.setFillColor(sf::Color((sf::Uint8)(color.r * 255),
			(sf::Uint8)(color.g * 255), (sf::Uint8)(color.b * 255),
			(sf::Uint8)(color.a * 120)));
		target.draw(shape);
	}

	virtual void DrawCircle(const b2::Vec2& center, float radius, const b2::Color& color) override
	{
		sf::CircleShape circle(radius);
		circle.setPosition(center.x, center.y);
		circle.setOrigin(radius, radius);
		circle.setFillColor(sf::Color::Transparent);
		circle.setOutlineThickness(0.02f);
		circle.setOutlineColor(sf::Color((sf::Uint8)(color.r * 255),
			(sf::Uint8)(color.g * 255), (sf::Uint8)(color.b * 255),
			(sf::Uint8)(color.a * 255)));
		target.draw(circle);
	}

	virtual void DrawSolidCircle(const b2::Vec2& center, float radius, const b2::Vec2& axis, const b2::Color& color) override
	{
		sf::CircleShape circle(radius);
		circle.setPosition(center.x, center.y);
		circle.setOrigin(radius, radius);
		circle.setFillColor(sf::Color((sf::Uint8)(color.r * 255),
			(sf::Uint8)(color.g * 255), (sf::Uint8)(color.b * 255),
			(sf::Uint8)(color.a * 120)));
		target.draw(circle);

		b2::Vec2 p = center + (radius * axis);
		DrawSegment(center, p, color);
	}

	virtual void DrawSegment(const b2::Vec2& p1, const b2::Vec2& p2, const b2::Color& color) override
	{
		sf::VertexArray va(sf::Lines, 2);
		sf::Color sfColor = sf::Color((sf::Uint8)(color.r * 255),
			(sf::Uint8)(color.g * 255), (sf::Uint8)(color.b * 255),
			(sf::Uint8)(color.a * 120));

		va[0].position = sf::Vector2f(p1.x, p1.y);
		va[0].color = sfColor;

		va[1].position = sf::Vector2f(p2.x, p2.y);
		va[1].color = sfColor;

		target.draw(va);
	}

	virtual void DrawTransform(const b2::Transform& xf) override
	{
		b2::Vec2 p = xf.p;
		b2::Vec2 px = p + (0.5f * xf.q.GetXAxis());
		b2::Vec2 py = p + (0.5f * xf.q.GetYAxis());

		DrawSegment(p, px, b2::Color(1.0f, 0.0f, 0.0f));
		DrawSegment(p, py, b2::Color(0.0f, 1.0f, 0.0f));
	}

private:
	sf::RenderTarget& target;
};

void Physics::Init() {
  if (world)
    delete world;

  world = new b2::World(b2::Vec2(0.f, 9.8f)); // Укажите параметры, если нужно
}

void Physics::Update(float deltaTime) {
  world->Step(deltaTime, 8, 3); // Используйте правильный метод вашей версии
  // Box2D
}

void Physics::DebugDraw(Renderer &renderer) {
  if (!debugDraw) {
    debugDraw = new MyDebugDraw(renderer.target);
    debugDraw->SetFlags(0u); // Если нужно
    world->SetDebugDraw(debugDraw); // Если нужно
  }

  world->DrawDebugData(); // Если нужно
}
