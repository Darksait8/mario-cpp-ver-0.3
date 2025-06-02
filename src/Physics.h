#pragma once
#include "Renderer.h"
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <Box2D/Dynamics/World.hpp>

constexpr float PI = 22.0f / 7.0f;

class MyDebugDraw;
class Object;
class Mario;
class Map; // Forward declaration for Map class

class ContactListener {
public:
  virtual void OnBeginContact(b2::Fixture *self, b2::Fixture *other) = 0;
  virtual void OnEndContact(b2::Fixture *self, b2::Fixture *other) = 0;
};

enum class FixtureDataType { Mario, MapTile, Object, EnemyTopSensor };

struct FixtureData {
  FixtureDataType type;
  ContactListener *listener;
  bool isBreakable = false;

  // Флаго-специфичные поля для анимации
  bool isFlag = false;
  bool isFlagTop = false;
  bool isDescending = false; // Flag to indicate if the flag is descending
  float flagDescentProgress = 0.0f; // Прогресс спуска (в пикселях)
  float flagDescentSpeed = 100.0f; // Скорость спуска (пикселей в секунду)

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
  // Constructor that takes a pointer to the Map instance
  MyContactListener(Map* mapInstance) : mapInstance(mapInstance) {}

  void BeginContact(b2::Contact *contact) override;
  void EndContact(b2::Contact *contact) override;
private:
  Map* mapInstance; // Pointer to the Map instance
};

class Physics {
public:
  static void Init(Map* mapInstance);
  static void Update(float deltaTime, int velocityIterations, int positionIterations);
  static void DebugDraw(Renderer &renderer);

  static b2::World *world;
  static MyDebugDraw *debugDraw;
};
