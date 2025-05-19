#include "Enemy.h"
#include "Game.h"
#include "Resources.h"
#include <Box2D/Collision/Shapes/DiskShape.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <memory>

void Enemy::Begin()
{
	animation = Animation(0.5f,
		{
			AnimFrame(0.25f, Resources::textures["enemy2.png"]),
			AnimFrame(0.00f, Resources::textures["enemy1.png"]),
		});

	tag = "enemy";
	
	fixtureData.object = this;
	fixtureData.type = FixtureDataType::Object;

	box2d::BodyDef bodyDef{};
	bodyDef.type = box2d::BodyType::Dynamic;
	bodyDef.position = box2d::Vec2(position.x, position.y);
	// bodyDef.fixedRotation = true; // если есть аналог
	body = Physics::world->CreateBody(bodyDef);

	auto diskShape = std::make_shared<box2d::DiskShape>();
	diskShape->SetRadius(0.5f);

	box2d::FixtureDef fixtureDef{};
	fixtureDef.UseUserData(&fixtureData);

	body->CreateFixture(diskShape, fixtureDef);
}

void Enemy::Update(float deltaTime)
{
	if (isDead)
	{
		destroyTimer += deltaTime;
		if (destroyTimer >= 2.0f)
			DeleteObject(this);

		return;
	}

	animation.Update(deltaTime);

	auto velocity = box2d::GetLinearVelocity(*body);

	if (std::abs(velocity.x) <= 0.02f)
		movement *= -1.0f;

	velocity.x = movement;

	box2d::SetLinearVelocity(*body, velocity);

	auto xf = body->GetTransformation();
	position = sf::Vector2f(xf.p.x, xf.p.y);

	angle = static_cast<float>(box2d::GetAngle(*body)); 
}

void Enemy::Render(Renderer& renderer)
{
	renderer.Draw(animation.GetTexture(), !isDead ? position : sf::Vector2f(position.x,
		position.y + 0.45f), sf::Vector2f(1.0f, isDead ? 0.2f : 1.0f), angle);
}

void Enemy::Die()
{
	isDead = true;
	Physics::world->Destroy(body);
}

bool Enemy::IsDead()
{
	return isDead;
}
