#include "Coin.h"
#include "Resources.h"
#include <Box2D/Collision/Shapes/PolygonShape.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>

Coin::~Coin() {
  if (body) {
    Physics::world->DestroyBody(body);
    body = nullptr;
  }
}

void Coin::Begin() {
  tag = "coin";

  animation =
      Animation(1.4f, {
                          AnimFrame(1.3f, Resources::getTexture("coin14.png")),
                          AnimFrame(1.2f, Resources::getTexture("coin13.png")),
                          AnimFrame(1.1f, Resources::getTexture("coin12.png")),
                          AnimFrame(1.0f, Resources::getTexture("coin11.png")),
                          AnimFrame(0.9f, Resources::getTexture("coin10.png")),
                          AnimFrame(0.8f, Resources::getTexture("coin9.png")),
                          AnimFrame(0.7f, Resources::getTexture("coin8.png")),
                          AnimFrame(0.6f, Resources::getTexture("coin7.png")),
                          AnimFrame(0.5f, Resources::getTexture("coin6.png")),
                          AnimFrame(0.4f, Resources::getTexture("coin5.png")),
                          AnimFrame(0.3f, Resources::getTexture("coin4.png")),
                          AnimFrame(0.2f, Resources::getTexture("coin3.png")),
                          AnimFrame(0.1f, Resources::getTexture("coin2.png")),
                          AnimFrame(0.0f, Resources::getTexture("coin1.png")),
                      });

  b2::BodyDef bodyDef{};
  bodyDef.type = b2::BodyType::dynamicBody;
  bodyDef.gravityScale = 0.0f;
  bodyDef.position = b2::Vec2(position.x, position.y);
  body = Physics::world->CreateBody(&bodyDef);

  b2::PolygonShape shape;
  shape.SetAsBox(0.4f, 0.4f);

  fixtureData.type = FixtureDataType::Object;
  fixtureData.object = this;
  fixtureData.listener = nullptr;

  b2::FixtureDef fixtureDef{};
  fixtureDef.shape = &shape;
  fixtureDef.userData = &fixtureData;
  fixtureDef.isSensor = true;
  body->CreateFixture(&fixtureDef);
}

void Coin::Update(float deltaTime) {
  animation.Update(deltaTime);

  // Логика для анимации вылета монетки вверх и возврата обратно
  if (body) {
    // Начальная позиция монетки (там, где был блок)
    sf::Vector2f startPosition = initialPosition; // Используем сохраненную начальную позицию

    // Проверяем, если монетка двигается вверх (скорость по Y отрицательная)
    if (body->GetLinearVelocity().y < 0) {
        // Пока двигаемся вверх, ничего особенного не делаем (скорость задана при создании)
    } else { // Монетка начала падать (скорость по Y стала положительной или нулевой)
        // Теперь нужно, чтобы она стремилась вернуться к начальной позиции Y
        // Можно установить гравитацию обратно в нормальное значение или управлять скоростью напрямую
        // Давайте просто установим ей скорость вниз, чтобы она вернулась
        body->SetLinearVelocity(b2::Vec2(0.0f, 5.0f)); // Скорость вниз (положительная по Y)
        body->SetGravityScale(0.0f); // Отключаем гравитацию, чтобы управлять движением вручную

        // Проверяем, вернулась ли монетка примерно на начальную позицию по Y
        if (body->GetPosition().y >= startPosition.y - 0.1f) { // Сравниваем с небольшим допуском
            if (!isCollected) { // Only increment coins the first time
                mario->coins++; // Increment Mario's coin count
            }
            isCollected = true; // Помечаем для удаления
            // Опционально: остановить физическое тело, чтобы оно не падало дальше
            body->SetLinearVelocity(b2::Vec2(0.0f, 0.0f));
            body->SetType(b2::BodyType::staticBody);
        }
    }
    // Обновляем позицию монетки на основе физического тела
    position = sf::Vector2f(body->GetPosition().x, body->GetPosition().y);
  }
}

void Coin::Render(Renderer &renderer) {
  renderer.Draw(animation.GetTexture(), position, sf::Vector2f(0.8f, 0.8f));
}
