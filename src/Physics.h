#pragma once
#include "Renderer.h"
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <Box2D/Dynamics/World.hpp>

constexpr float PI = 22.0f / 7.0f;

class MyDebugDraw;
class Object;
class Mario;

class ContactListener {
public:
  virtual void OnBeginContact(b2::Fixture *self, b2::Fixture *other) = 0;
  virtual void OnEndContact(b2::Fixture *self, b2::Fixture *other) = 0;
};

enum class FixtureDataType { Mario, MapTile, Object };

struct FixtureData {
  FixtureDataType type;
  ContactListener *listener;
  bool isBreakable = false;

  union {
    Mario *mario;
    Object *object;
    struct {
      int mapX, mapY;
      b2::Body* body;
    };
  };
};

enum CollisionCategory {
  CATEGORY_NOTHING = 0,
  CATEGORY_MARIO = 0x0001,
  CATEGORY_MAP_TILE = 0x0002,
  CATEGORY_ENEMY = 0x0004,
  CATEGORY_COIN = 0x0008,
  CATEGORY_MARIO_GROUND_SENSOR = 0x0010,
};

// Наш класс слушателя контактов для Box2D мира
class MyContactListener : public b2::ContactListener {
public:
  void BeginContact(b2::Contact *contact) override;
  void EndContact(b2::Contact *contact) override;
};

class Physics {
public:
  static void Init();
  static void Update(float deltaTime, int velocityIterations, int positionIterations);
  static void DebugDraw(Renderer &renderer);

  static b2::World *world;
  static MyDebugDraw *debugDraw;
};
