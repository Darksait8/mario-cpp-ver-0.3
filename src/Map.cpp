#include "Coin.h"
#include "Enemy.h"
#include "Map.h"
#include "Physics.h"
#include "Resources.h"

#include <Box2D/Collision/Shapes/PolygonShape.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>

// Удаляем инициализацию статического поля grid
// std::vector<std::vector<sf::Texture*>> Map::grid;

Map::Map(float cellSize) : cellSize(cellSize) {}

// Удаляем реализацию RemoveBlock
// void Map::RemoveBlock(int x, int y) {
//     if (x >= 0 && x < grid.size() && y >= 0 && y < grid[x].size()) {
//         grid[x][y] = nullptr;
//     }
// }

void Map::CreateCheckerboard(size_t width, size_t height) {
  grid = std::vector(width, std::vector(height, (sf::Texture *)nullptr));

  bool last = 0;
  for (auto &column : grid) {
    for (auto &cell : column) {
      last = !last;
      if (last)
        cell = &Resources::textures["brick.png"];
    }
    if (width % 2 == 0)
      last = !last;
  }
}

sf::Vector2f Map::CreateFromImage(const sf::Image &image,
                                  std::vector<Object *> &objects, class Mario* mario) {
  objects.clear();
  grid.clear();
  grid = std::vector(image.getSize().x,
                     std::vector(image.getSize().y, (sf::Texture *)nullptr));

  // Удаляем std::cout << "Map::CreateFromImage - Grid size..." << std::endl;

  sf::Vector2f marioPosition{};

  for (size_t x = 0; x < grid.size(); x++) {
    for (size_t y = 0; y < grid[x].size(); y++) {
      sf::Color color = image.getPixel(x, y);
      Object *object = nullptr;

      if (color == sf::Color::Red) {
        marioPosition = sf::Vector2f(cellSize * x + cellSize / 2.0f,
                                     cellSize * y + cellSize / 2.0f);
        continue;
      } else if (color == sf::Color::Black) {
        grid[x][y] = &Resources::textures["brick.png"];
      } else if (color == sf::Color::Yellow) {
        object = new Coin(mario);
      } else if (color == sf::Color::Blue) {
        object = new Enemy();
      } else if (
          std::abs(color.r - 255) <= 5 &&
          std::abs(color.g - 175) <= 5 &&
          std::abs(color.b - 0) <= 5) {
        grid[x][y] = &Resources::textures["kirpich.png"];
      } else if (
          std::abs(color.r - 220) <= 5 &&
          std::abs(color.g - 220) <= 5 &&
          std::abs(color.b - 220) <= 5 &&
          color.a == 255) {
        grid[x][y] = &Resources::textures["monetki.png"];
      } else if (
          std::abs(color.r - 0) <= 5 &&
          std::abs(color.g - 255) <= 5 &&
          std::abs(color.b - 0) <= 5) {
        grid[x][y] = &Resources::textures["pile1.png"];
      } else if (
          std::abs(color.r - 0) <= 5 &&
          std::abs(color.g - 255) <= 5 &&
          std::abs(color.b - 155) <= 5) {
        grid[x][y] = &Resources::textures["pile1_2.png"];
      } else if (
          std::abs(color.r - 157) <= 5 &&
          std::abs(color.g - 240) <= 5 &&
          std::abs(color.b - 255) <= 5) {
        grid[x][y] = &Resources::textures["pile2.png"];
      } else if (
          std::abs(color.r - 11) <= 5 &&
          std::abs(color.g - 96) <= 5 &&
          std::abs(color.b - 111) <= 5) {
        grid[x][y] = &Resources::textures["pile2_2.png"];
      } else if (
          std::abs(color.r - 61) <= 5 &&
          std::abs(color.g - 80) <= 5 &&
          std::abs(color.b - 48) <= 5) {
        grid[x][y] = &Resources::textures["flag.png"];
      } else if (
          std::abs(color.r - 218) <= 5 &&
          std::abs(color.g - 255) <= 5 &&
          std::abs(color.b - 193) <= 5) {
        grid[x][y] = &Resources::textures["flaghtock.png"];
      }


      if (object) {
        // Удаляем std::cout << "  Created object with tag..." << std::endl;
        object->position = sf::Vector2f(cellSize * x + cellSize / 2.0f,
                                        cellSize * y + cellSize / 2.0f);
        objects.push_back(object);
      } else if (grid[x][y]) {
        b2::BodyDef bodyDef{};
        bodyDef.position = b2::Vec2(cellSize * x + cellSize / 2.0f,
                                    cellSize * y + cellSize / 2.0f);
        auto body = Physics::world->CreateBody(&bodyDef);
        b2::PolygonShape shape;
        shape.SetAsBox(cellSize / 2.0f, cellSize / 2.0f);

        FixtureData *fixtureData = new FixtureData();
        fixtureData->type = FixtureDataType::MapTile;
        fixtureData->mapX = x;
        fixtureData->mapY = y;
        fixtureData->body = body;
        fixtureData->listener = nullptr;
        fixtureData->isBreakable = (grid[x][y] == &Resources::textures["monetki.png"]);
        if (fixtureData->isBreakable) {
          // Удаляем std::cout << "Set isBreakable=true..." << std::endl;
        }

        b2::FixtureDef fixtureDef{};
        fixtureDef.shape = &shape;
        fixtureDef.userData = fixtureData;
        fixtureDef.friction = 0.0f;
        body->CreateFixture(&fixtureDef);
      }
    }
  }

  return marioPosition;
}

void Map::Draw(Renderer &renderer) {
  // Удаляем std::cout << "Map::Draw - Grid size..." << std::endl;
  int x = 0;
  for (const auto &column : grid) {
    int y = 0;
    for (const auto &cell : column) {
      if (cell) {
        // Удаляем std::cout << "Drawing block at..." << std::endl; // Можно добавить, если нужно больше деталей
        renderer.Draw(*cell,
                      sf::Vector2f(cellSize * x + cellSize / 2.0f,
                                   cellSize * y + cellSize / 2.0f),
                      sf::Vector2f(cellSize, cellSize));
      } else {
        // Удаляем std::cout << "Map::Draw - Cell at..." << std::endl;
      }
      y++;
    }
    x++;
  }
}

// Реализация метода разрушения блока
void Map::BreakBlock(int x, int y, sf::Texture* newTexture) {
    if (x >= 0 && x < grid.size() && y >= 0 && y < grid[x].size()) {
        // Меняем текстуру блока
        grid[x][y] = newTexture;
    }
}
