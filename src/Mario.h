#pragma once

#include "Animation.h"
#include "Physics.h"
#include "Renderer.h"
#include <Box2D/Collision/Shapes/PolygonShape.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

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

private:
  Animation runAnimation{};
  sf::Texture textureToDraw{};
  sf::Sound jumpSound;
  sf::Sound deathSound;

  FixtureData fixtureData{};
  b2::Body *body{};
  b2::Fixture *groundFixture{};

  size_t isGrounded{};
  bool facingLeft{};
  bool isInvincible = false; // Флаг неуязвимости
  float invincibilityTimer = 0.0f; // Таймер неуязвимости
  const float invincibilityDuration = 3.0f; // Длительность неуязвимости в секундах
};
