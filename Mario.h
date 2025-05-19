#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <Box2D/Collision/Shapes/PolygonShape.hpp>
#include "Animation.h"
#include "Renderer.h"
#include "Physics.h"

class Mario
	: public ContactListener
{
public:
	void Begin();
	void Update(float deltaTime);
	void Draw(Renderer& renderer);

	virtual void OnBeginContact(box2d::Fixture* self, box2d::Fixture* other) override;
	virtual void OnEndContact(box2d::Fixture* self, box2d::Fixture* other) override;

	size_t GetCoins();

	sf::Vector2f position{};
	float angle{};
	bool isDead{};
private:
	Animation runAnimation{};
	sf::Texture textureToDraw{};
	sf::Sound jumpSound{};

	FixtureData fixtureData{};
	box2d::Body* body{};
	box2d::Fixture* groundFixture{};

	size_t isGrounded{};
	bool facingLeft{};
	size_t coins{};
};
