#pragma once

#include "Animation.h"
#include "Physics.h"
#include "Renderer.h"
#include <Box2D/Collision/Shapes/PolygonShape.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

class Map; // Forward declaration

class Mario : public ContactListener {
public:
  void Begin();
  void Update(float deltaTime);
  void Draw(Renderer &renderer);

  virtual void OnBeginContact(b2::Fixture *self, b2::Fixture *other) override;
  virtual void OnEndContact(b2::Fixture *self, b2::Fixture *other) override;

  size_t GetCoins();

  sf::Vector2f position{};
  float angle{};

  // Новые поля для сигнализации разрушения блока
  bool shouldBreakBlock = false;
  int blockToBreakX = -1;
  int blockToBreakY = -1;

  b2::Body* bodyToBreak = nullptr; // Новое поле для хранения тела блока для разрушения

  // Делаем coins публичным, чтобы Game мог к нему обращаться
  size_t coins = 1; // Инициализируем счетчик монет с 1

  // Поля для анимации смерти
  bool isDead{};
  bool isDying = false;
  float deathAnimationTimer = 0.0f;
  sf::Texture* deathTexture = nullptr;
  float deathSpeedY = 0.0f; // Скорость по Y для анимации смерти

  bool isFlying = false; // Флаг полета

  bool facingLeft{};
  bool isInvincible = false; // Флаг неуязвимости
  float invincibilityTimer = 0.0f; // Таймер неуязвимости
  const float invincibilityDuration = 3.0f; // Длительность неуязвимости в секундах

  b2::Filter originalFilter; // To store the original collision filter
  b2::Filter originalGroundSensorFilter; // To store the original ground sensor filter

  bool inVictorySequence = false;
  bool victoryWalkComplete = false;
  bool isSlidingDownFlag = false;
  bool isVictoryDancing = false;
  float victoryDanceTimer = 0.0f;
  const float victoryDanceDuration = 2.0f;
  Map* mapInstance = nullptr;

  // Перемещаем эти поля из private в public
  Animation runAnimation{};
  sf::Texture textureToDraw{};
  b2::Body *body{};

  // Перемещаем isGrounded в public
  size_t isGrounded{};

private:
  sf::Sound jumpSound;
  sf::Sound deathSound;

  FixtureData fixtureData{};
  b2::Fixture *groundFixture{};
};
