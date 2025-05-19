#pragma once
#include <Box2D/Dynamics/World.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include "Renderer.h"

constexpr float M_PI = 22.0f / 7.0f;

class MyDebugDraw;
class Object;
class Mario;

class ContactListener
{
public:
	virtual void OnBeginContact(box2d::Fixture* self, box2d::Fixture* other) = 0;
	virtual void OnEndContact(box2d::Fixture* self, box2d::Fixture* other) = 0;
};

enum class FixtureDataType
{
	Mario,
	MapTile,
	Object
};

struct FixtureData
{
	FixtureDataType type;
	ContactListener* listener;

	union
	{
		Mario* mario;
		Object* object;
		struct { int mapX, mapY; };
	};
};

class Physics
{
public:
	static void Init();
	static void Update(float deltaTime);
	static void DebugDraw(Renderer& renderer);

	static box2d::World* world;
	static MyDebugDraw* debugDraw;
};
