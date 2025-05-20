#pragma once
#include "Renderer.h"
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <Box2D/Dynamics/World.hpp>

constexpr float M_PI = 22.0f / 7.0f;

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

  union {
    Mario *mario;
    Object *object;
    struct {
      int mapX, mapY;
    };
  };
};

class Physics {
public:
  static void Init();
  static void Update(float deltaTime);
  static void DebugDraw(Renderer &renderer);

  static b2::World *world;
  static MyDebugDraw *debugDraw;
};
