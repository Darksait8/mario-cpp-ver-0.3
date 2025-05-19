#include "Physics.h"

#include <SFML/Graphics.hpp>
#include <Box2D/Dynamics/World.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <Box2D/Dynamics/WorldCallbacks.hpp>
#include <Box2D/Dynamics/Contacts/Contact.hpp>
#include <Box2D/Common/Math.hpp>
#include <Box2D/Common/Settings.hpp>
#include <Box2D/Dynamics/StepConf.hpp>
#include <Box2D/Dynamics/StepStats.hpp>
#include <Box2D/Dynamics/BodyDef.hpp>
#include <Box2D/Dynamics/FixtureDef.hpp>
#include <Box2D/Dynamics/Joints/Joint.hpp>
#include <Box2D/Dynamics/Joints/RevoluteJoint.hpp>
#include <Box2D/Dynamics/Joints/PrismaticJoint.hpp>
#include <Box2D/Dynamics/Joints/DistanceJoint.hpp>
#include <Box2D/Dynamics/Joints/PulleyJoint.hpp>
#include <Box2D/Dynamics/Joints/MouseJoint.hpp>
#include <Box2D/Dynamics/Joints/GearJoint.hpp>
#include <Box2D/Dynamics/Joints/WheelJoint.hpp>
#include <Box2D/Dynamics/Joints/WeldJoint.hpp>
#include <Box2D/Dynamics/Joints/FrictionJoint.hpp>
#include <Box2D/Dynamics/Joints/RopeJoint.hpp>
#include <Box2D/Dynamics/Joints/MotorJoint.hpp>

box2d::World* Physics::world{};
MyDebugDraw* Physics::debugDraw{};

class MyDebugDraw
{
public:
	MyDebugDraw(sf::RenderTarget& target)
		: target(target)
	{
	}

	void DrawPolygon(const box2d::Vec2* vertices, int32_t vertexCount, const sf::Color& color)
	{
		sf::ConvexShape shape(vertexCount);
		for (int i = 0; i < vertexCount; i++)
		{
			shape.setPoint(i, sf::Vector2f(vertices[i].x, vertices[i].y));
		}

		shape.setFillColor(sf::Color::Transparent);
		shape.setOutlineThickness(0.02f);
		shape.setOutlineColor(color);
		target.draw(shape);
	}

	// Аналогично реализуйте остальные методы отрисовки, используя sf::Color и box2d::Vec2

private:
	sf::RenderTarget& target;
};

// Реализуйте свой контакт-листенер, если нужно

void Physics::Init()
{
	if (world)
		delete world;

	world = new box2d::World(); // Укажите параметры, если нужно
}

void Physics::Update(float deltaTime)
{
	// world->Step(deltaTime, 8, 3); // Используйте правильный метод вашей версии Box2D
}

void Physics::DebugDraw(Renderer& renderer)
{
	if (!debugDraw)
	{
		debugDraw = new MyDebugDraw(renderer.target);
		// debugDraw->SetFlags(0u); // Если нужно
		// world->SetDebugDraw(debugDraw); // Если нужно
	}

	// world->DebugDraw(); // Если нужно
}
