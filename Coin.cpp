#include "Coin.h"
#include "Resources.h"
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Collision/Shapes/PolygonShape.hpp>
#include <Box2D/Dynamics/Fixture.hpp>

Coin::~Coin()
{
	if (body) {
		Physics::world->Destroy(body);
		body = nullptr;
	}
}

void Coin::Begin()
{
	tag = "coin";

	animation = Animation(1.4f,
		{
			AnimFrame(1.3f, Resources::textures["coin14.png"]),
			AnimFrame(1.2f, Resources::textures["coin13.png"]),
			AnimFrame(1.1f, Resources::textures["coin12.png"]),
			AnimFrame(1.0f, Resources::textures["coin11.png"]),
			AnimFrame(0.9f, Resources::textures["coin10.png"]),
			AnimFrame(0.8f, Resources::textures["coin9.png"]),
			AnimFrame(0.7f, Resources::textures["coin8.png"]),
			AnimFrame(0.6f, Resources::textures["coin7.png"]),
			AnimFrame(0.5f, Resources::textures["coin6.png"]),
			AnimFrame(0.4f, Resources::textures["coin5.png"]),
			AnimFrame(0.3f, Resources::textures["coin4.png"]),
			AnimFrame(0.2f, Resources::textures["coin3.png"]),
			AnimFrame(0.1f, Resources::textures["coin2.png"]),
			AnimFrame(0.0f, Resources::textures["coin1.png"]),
		});

	box2d::BodyDef bodyDef{};
	bodyDef.type = box2d::BodyType::Dynamic;
	bodyDef.position = box2d::Vec2(position.x, position.y);
	body = Physics::world->CreateBody(bodyDef);

	auto shape = std::make_shared<box2d::PolygonShape>();
	shape->SetAsBox(0.4f, 0.4f);
	shape->SetDensity(0.0f);

	FixtureData* fixtureData = new FixtureData();
	fixtureData->type = FixtureDataType::Object;
	fixtureData->object = this;

	box2d::FixtureDef fixtureDef{};
	fixtureDef.UseUserData(fixtureData);
	fixtureDef.UseIsSensor(true);
	body->CreateFixture(shape, fixtureDef);
}

void Coin::Update(float deltaTime)
{
	animation.Update(deltaTime);
}

void Coin::Render(Renderer& renderer)
{
	renderer.Draw(animation.GetTexture(), position, sf::Vector2f(0.8f, 0.8f));
}
