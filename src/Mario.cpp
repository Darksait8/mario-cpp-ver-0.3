#define _USE_MATH_DEFINES
#include "Mario.h"
#include "Object.h"
#include "Resources.h"
#include "Map.h"

#include "Coin.h"
#include "Enemy.h"
#include <Box2D/Collision/Shapes/CircleShape.hpp>
#include <Box2D/Collision/Shapes/PolygonShape.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <iostream>
#include <cmath>

const float movementSpeed = 7.0f;
const float jumpVelocity = 10.0f;

void Mario::Begin() {
  runAnimation =
      Animation(0.45f, {
                           AnimFrame(0.30f, Resources::getTexture("run3.png")),
                           AnimFrame(0.15f, Resources::getTexture("run2.png")),
                           AnimFrame(0.00f, Resources::getTexture("run1.png")),
                       });

  jumpSound.setBuffer(Resources::getSound("jump.wav"));
  jumpSound.setVolume(20);

  deathSound.setBuffer(Resources::getSound("mario-smert.mp3"));
  deathSound.setVolume(20);

  fixtureData.listener = this;
  fixtureData.mario = this;
  fixtureData.type = FixtureDataType::Mario;

  b2::BodyDef bodyDef{};
  bodyDef.type = b2::BodyType::dynamicBody;
  bodyDef.position = b2::Vec2(position.x, position.y - 1.3f);
  bodyDef.fixedRotation = true;
  body = Physics::world->CreateBody(&bodyDef);

  b2::CircleShape circleShape;
  circleShape.m_radius = 0.5f;

  b2::FixtureDef fixtureDef{};
  fixtureDef.shape = &circleShape;
  fixtureDef.userData = &fixtureData;
  fixtureDef.density = 1.f;
  fixtureDef.friction = 0.01f;
  fixtureDef.restitution = 0.1f;
  body->CreateFixture(&fixtureDef);
  originalFilter = body->GetFixtureList()->GetFilterData(); // Store filter for the first fixture

  b2::PolygonShape polygonShape;
  polygonShape.SetAsBox(0.4f, 0.8f);
  fixtureDef.shape = &polygonShape;
  fixtureDef.friction = 0.01f;
  fixtureDef.restitution = 0.1f;
  body->CreateFixture(&fixtureDef);
  originalFilter = body->GetFixtureList()->GetNext()->GetFilterData(); // Store filter for the second fixture (assuming two main fixtures)

  // Create a smaller ground sensor at the bottom of Mario's feet
  b2::PolygonShape groundSensorShape;
  groundSensorShape.SetAsBox(0.4f, 0.05f, b2::Vec2(0.0f, 0.8f),
                             0.0f);
  fixtureDef.shape = &groundSensorShape;
  fixtureDef.isSensor = true;
  groundFixture = body->CreateFixture(&fixtureDef);
  originalGroundSensorFilter = groundFixture->GetFilterData(); // Store filter for the ground sensor fixture
}

void Mario::Update(float deltaTime) {
  static bool wasDying = false;

  // Check if victory sequence has started in Map and transition to victory walk
  if (mapInstance && mapInstance->victorySequenceStarted && !inVictorySequence) {
      inVictorySequence = true;
      // Disable collisions for all of Mario's fixtures
      b2::Filter filter;
      filter.maskBits = CATEGORY_NOTHING; // Don't collide with anything
      for (b2::Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
          f->SetFilterData(filter);
      }
      // Set horizontal velocity for victory walk (initial impulse)
      body->SetLinearVelocity(b2::Vec2(3.0f, 0.0f));

      // Disable gravity during victory walk
      body->SetGravityScale(0.0f);

      // Ensure facing right for victory walk animation
      facingLeft = false;
  }

  // Victory sequence logic (when already in sequence)
  if (inVictorySequence) {
      // Position is manually updated in Game::Update during victory sequence
      // No need to update visual position from physics body here in Mario::Update

      if (isSlidingDownFlag) {
          textureToDraw = Resources::getTexture("slide.png");
          facingLeft = false;
      } else if (isVictoryDancing) {
          victoryDanceTimer += deltaTime;
          if (victoryDanceTimer >= victoryDanceDuration) {
              isVictoryDancing = false;
              victoryWalkComplete = true;
          }
          if (static_cast<int>(victoryDanceTimer * 5) % 2 == 0) {
              textureToDraw = Resources::getTexture("victory1.png");
          } else {
              textureToDraw = Resources::getTexture("victory2.png");
          }
      } else { // Mario is walking
          runAnimation.Update(deltaTime);
          textureToDraw = runAnimation.GetTexture();
      }

       float endOfWalkX = (mapInstance ? mapInstance->grid.size() * mapInstance->cellSize - 2.0f : position.x + 100.0f);
      if (position.x >= endOfWalkX && !isVictoryDancing) {
          isVictoryDancing = true;
          victoryDanceTimer = 0.0f;
          // If body is active, set its velocity to zero (though it should be inactive)
          if (body && body->IsActive()) { // <-- НОВАЯ СТРОКА: Убедитесь, что тело активно
              body->SetLinearVelocity(b2::Vec2(0.0f, 0.0f)); // Stop walking physics
          }
      }
      // Do NOT update position from physics body here during victory sequence
      // position = sf::Vector2f(body->GetPosition().x, body->GetPosition().y);
      // angle = static_cast<float>(body->GetAngle() * (180.0f / M_PI));

      return; // Skip normal update logic when in victory sequence
  }

  // Логика неуязвимости
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && !isInvincible) {
    isInvincible = true;
    invincibilityTimer = 0.0f;
    // Возможно, добавить визуальный эффект для неуязвимости здесь
  }

  if (isInvincible) {
    invincibilityTimer += deltaTime;
    if (invincibilityTimer >= invincibilityDuration) {
      isInvincible = false;
      invincibilityTimer = 0.0f;
      // Возможно, убрать визуальный эффект неуязвимости здесь
    }
  }

  if (isDying && !wasDying) {
    // Только что начали анимацию смерти
    body->SetActive(false); // Отключаем физическое тело
    body->SetType(b2::BodyType::staticBody); // Устанавливаем тип на статический
    body->SetLinearVelocity(b2::Vec2(0.0f, 0.0f)); // Обнуляем скорость
    body->SetAngularVelocity(0.0f); // Обнуляем угловую скорость
    deathSound.play();
  }

  if (isDying) {
    // Логика анимации смерти
    deathAnimationTimer += deltaTime;
    // Ручное обновление позиции для анимации смерти
    deathSpeedY += Physics::world->GetGravity().y * body->GetGravityScale() * deltaTime; // Имитация гравиции
    position.y += deathSpeedY * deltaTime; // Обновляем только вертикальную позицию
    // Горизонтальная позиция остается неизменной

    // Завершаем анимацию смерти через 0.5 секунды
    if (deathAnimationTimer >= 2.5f) {
      isDead = true; // Устанавливаем флаг окончательной смерти
      isDying = false; // Завершаем анимацию смерти
      body->SetActive(true); // Включаем физическое тело обратно
      body->SetType(b2::BodyType::dynamicBody); // Устанавливаем тип обратно на динамический
      deathAnimationTimer = 0.0f; // Сбрасываем таймер
    }
  } else { // Обычная логика обновления Марио (движение, прыжки и т.д.)
    // Логика полета
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
        if (!isFlying) {
            isFlying = true;
            body->SetGravityScale(0.0f); // Отключаем гравитацию во время полета
        }
        // Применяем постоянную вертикальную скорость вверх во время полета
        auto velocity = body->GetLinearVelocity();
        velocity.y = -movementSpeed; // Можно использовать другую скорость для полета
        body->SetLinearVelocity(velocity);
    } else {
        if (isFlying) {
            isFlying = false;
            body->SetGravityScale(1.0f); // Восстанавливаем гравитацию
        }
    }

    // Обычное горизонтальное движение
  float move = movementSpeed;

  runAnimation.Update(deltaTime);

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    move *= 2;

  auto velocity = body->GetLinearVelocity();
  velocity.x = 0.0f;

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    velocity.x += move;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    velocity.x -= move;

  // Логика прыжка (только если не летим)
  if (!isFlying && (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
       sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) &&
      isGrounded) {
    velocity.y = -jumpVelocity;
    jumpSound.play();
  }

  textureToDraw = runAnimation.GetTexture();

  if (velocity.x < -0.02f)
    facingLeft = true;
  else if (velocity.x > 0.02f)
    facingLeft = false;
  else
    textureToDraw = Resources::getTexture("idle.png");

  // Изменяем текстуру только если не летим
  if (!isGrounded && !isFlying)
    textureToDraw = Resources::getTexture("jump.png");

  body->SetLinearVelocity(velocity);

  // Update position and angle only if not in victory sequence
  if (!inVictorySequence) {
    position = sf::Vector2f(body->GetPosition().x, body->GetPosition().y);
    angle = static_cast<float>(body->GetAngle() * (180.0f / M_PI));
  }
  }

  // Check if Mario falls off the bottom of the level
  if (position.y > 32.0f && !isDying) {
      isDying = true;
      textureToDraw = Resources::getTexture("marioDeath.png");
      body->SetGravityScale(0.5f);
      deathSpeedY = -5.0f; // Give him a little upward bounce for death animation
  }

  wasDying = isDying; // Обновляем предыдущее состояние isDying
}

void Mario::Draw(Renderer &renderer) {
  renderer.Draw(textureToDraw, position,
                sf::Vector2f(facingLeft ? -1.0f : 1.0f, 2.0f), angle);
}

void Mario::OnBeginContact(b2::Fixture *self, b2::Fixture *other) {
  FixtureData *data = static_cast<FixtureData *>(other->GetUserData());

  if (!data)
    return;

  if (groundFixture == self && data->type == FixtureDataType::MapTile) {
    isGrounded++;
  } else if (data->type == FixtureDataType::MapTile && data->isBreakable) {
    b2::Vec2 marioPos = body->GetPosition();
    b2::Vec2 blockPos = data->body->GetPosition();

    if (marioPos.y > blockPos.y && body->GetLinearVelocity().y < 0) {
      // Устанавливаем флаг и сохраняем физическое тело блока
      shouldBreakBlock = true;
      blockToBreakX = data->mapX;
      blockToBreakY = data->mapY;
      bodyToBreak = data->body; // Сохраняем тело блока
    }
  } else if (data->type == FixtureDataType::Object &&
             data->object->tag == "coin") {
    // std::cout << "Contact with object tag: " << data->object->tag << std::endl;
    Coin *coin = dynamic_cast<Coin *>(data->object);
    if (coin && !coin->isCollected) {
      coin->isCollected = true;
      // std::cout << "coins = " << coins << "\n";
    }
  } else if (data->type == FixtureDataType::Object &&
             data->object->tag == "enemy") {
    // std::cout << "Contact with object tag: " << data->object->tag << std::endl;
    Enemy *enemy = dynamic_cast<Enemy *>(data->object);
    if (!enemy || enemy->IsDead() || isInvincible) // Добавлена проверка на неуязвимость Марио
      return;

    // Проверка на неуязвимость перед получением урона
    // if (isInvincible) {
    //   // Если Марио неуязвим, игнорируем контакт с врагом (для получения урона)
    //   return;
    // }

    if (groundFixture == self) { // Проверка, что контакт произошел с нижним сенсором Марио
      // std::cout << "Mario stomped on enemy!" << std::endl;
      enemy->Die();
      // Добавляем импульс вверх к телу Марио для отскока
      float bounceVelocity = -8.0f; // Скорость отскока вверх (отрицательное значение для движения вверх)
      body->SetLinearVelocity(b2::Vec2(body->GetLinearVelocity().x, bounceVelocity));
    } else { // Контакт с любой другой частью тела врага (не верхним сенсором)
      // std::cout << "Mario touched enemy and should die!" << std::endl;
      isDying = true;
      textureToDraw = Resources::getTexture("marioDeath.png");
      body->SetGravityScale(0.5f);
      deathSpeedY = -5.0f;
    }
  } else if (data->type == FixtureDataType::EnemyTopSensor) { // Проверка на контакт с верхним сенсором врага
       // std::cout << "Contact with enemy top sensor!" << std::endl;
       Enemy* enemy = dynamic_cast<Enemy*>(data->object); // Приводим обратно к типу Enemy
        if (enemy && !enemy->IsDead() && body->GetLinearVelocity().y > 0) { // Убеждаемся, что это враг, он жив, и Марио движется вниз
           // std::cout << "Mario stomped on enemy using top sensor!" << std::endl;
           enemy->Die();
           // Добавляем импульс вверх к телу Марио для отскока
           float bounceVelocity = -8.0f; // Скорость отскока вверх
           body->SetLinearVelocity(b2::Vec2(body->GetLinearVelocity().x, bounceVelocity));
       }
  }
}

void Mario::OnEndContact(b2::Fixture *self, b2::Fixture *other) {
  FixtureData *data = static_cast<FixtureData *>(other->GetUserData());

  if (!data)
    return;

  if (groundFixture == self && data->type == FixtureDataType::MapTile &&
      isGrounded > 0) {
    isGrounded--;
  }
}

size_t Mario::GetCoins() { return coins; }
