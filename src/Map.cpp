#include "Coin.h"
#include "Enemy.h"
#include "Map.h"
#include "Physics.h"
#include "Resources.h"

#include <Box2D/Collision/Shapes/PolygonShape.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <iostream>

// Удаляем инициализацию статического поля grid
// std::vector<std::vector<sf::Texture*>> Map::grid;

Map::Map(float cellSize) : cellSize(cellSize) {
    if (Resources::getTexture("sky.png").getNativeHandle() != 0) { // Check if the texture is actually loaded
        backgroundSprite.setTexture(Resources::getTexture("sky.png"));
        backgroundSprite.setScale(1.0f, 1.0f); // Убедитесь, что это правильный масштаб
    } else {
        std::cerr << "Error: sky.png texture is found in resources but is invalid (getNativeHandle() is 0) in Map constructor." << std::endl;
    }
}

void Map::CreateCheckerboard(size_t width, size_t height) {
  grid = std::vector(width, std::vector(height, (sf::Texture *)nullptr));

  bool last = 0;
  for (auto &column : grid) {
    for (auto &cell : column) {
      last = !last;
      if (last)
        cell = &Resources::getTexture("brick.png");
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
        if (Resources::getTexture("brick.png").getNativeHandle() != 0) {
            grid[x][y] = &Resources::getTexture("brick.png");
        } else {
            grid[x][y] = nullptr; // Fallback if texture not loaded
            std::cerr << "Warning: 'brick.png' texture not found in resources. Tile at " << x << "," << y << " will be empty." << std::endl;
        }
      } else if (color == sf::Color::Yellow) {
        object = new Coin(mario);
      } else if (color == sf::Color::Blue) {
        object = new Enemy();
      } else if (
          std::abs(color.r - 255)<=5 &&
          std::abs(color.g - 175) <= 5 &&
          std::abs(color.b - 0) <= 5) {
        if (Resources::getTexture("kirpich.png").getNativeHandle() != 0) {
            grid[x][y] = &Resources::getTexture("kirpich.png");
        } else {
            grid[x][y] = nullptr;
            std::cerr << "Warning: 'kirpich.png' texture not found in resources. Tile at " << x << "," << y << " will be empty." << std::endl;
        }
      } else if (
          std::abs(color.r - 220) <= 5 &&
          std::abs(color.g - 220) <= 5 &&
          std::abs(color.b - 220) <= 5 &&
          color.a == 255) {
        if (Resources::getTexture("monetki.png").getNativeHandle() != 0) {
            grid[x][y] = &Resources::getTexture("monetki.png");
        } else {
            grid[x][y] = nullptr;
            std::cerr << "Warning: 'monetki.png' texture not found in resources. Tile at " << x << "," << y << " will be empty." << std::endl;
        }
      } else if (
          std::abs(color.r - 0) <= 5 && std::abs(color.g - 255) <= 5 && std::abs(color.b - 0) <= 5 && color.a == 255) { // pile1.png color
        if (Resources::getTexture("pile1.png").getNativeHandle() != 0) {
            grid[x][y] = &Resources::getTexture("pile1.png");
        } else {
            grid[x][y] = nullptr;
            std::cerr << "Warning: 'pile1.png' texture not found in resources. Tile at " << x << "," << y << " will be empty." << std::endl;
        }
      } else if (
          std::abs(color.r - 0) <= 5 && std::abs(color.g - 205) <= 5 && std::abs(color.b - 0) <= 5 && color.a == 255) { // pile1_2.png color
        if (Resources::getTexture("pile1_2.png").getNativeHandle() != 0) {
            grid[x][y] = &Resources::getTexture("pile1_2.png");
        } else {
            grid[x][y] = nullptr;
            std::cerr << "Warning: 'pile1_2.png' texture not found in resources. Tile at " << x << "," << y << " will be empty." << std::endl;
        }
      } else if (
          std::abs(color.r - 0) <= 5 && std::abs(color.g - 145) <= 5 && std::abs(color.b - 0) <= 5 && color.a == 255) { // pile2.png color
        if (Resources::getTexture("pile2.png").getNativeHandle() != 0) {
            grid[x][y] = &Resources::getTexture("pile2.png");
        } else {
            grid[x][y] = nullptr;
            std::cerr << "Warning: 'pile2.png' texture not found in resources. Tile at " << x << "," << y << " will be empty." << std::endl;
        }
      } else if (
          std::abs(color.r - 0) <= 5 && std::abs(color.g - 83) <= 5 && std::abs(color.b - 0) <= 5 && color.a == 255) { // pile2_2.png color
        if (Resources::getTexture("pile2_2.png").getNativeHandle() != 0) {
            grid[x][y] = &Resources::getTexture("pile2_2.png");
        } else {
            grid[x][y] = nullptr;
            std::cerr << "Warning: 'pile2_2.png' texture not found in resources. Tile at " << x << "," << y << " will be empty." << std::endl;
        }
      } else if (
          std::abs(color.r - 255) <= 5 &&
          std::abs(color.g - 0) <= 5 &&
          std::abs(color.b - 255) <= 5 &&
          color.a == 255) {
        if (Resources::getTexture("flag.png").getNativeHandle() != 0) {
            grid[x][y] = &Resources::getTexture("flag.png");
        } else {
            grid[x][y] = nullptr;
            std::cerr << "Warning: 'flag.png' texture not found in resources. Tile at " << x << "," << y << " will be empty." << std::endl;
        }
      } else if (
          std::abs(color.r - 255) <= 15 &&
          std::abs(color.g - 0) <= 15 &&
          std::abs(color.b - 161) <= 15 &&
          color.a == 255) {
        if (Resources::getTexture("flaghtock.png").getNativeHandle() != 0) {
            grid[x][y] = &Resources::getTexture("flaghtock.png");
        } else {
            grid[x][y] = nullptr;
            std::cerr << "Warning: 'flaghtock.png' texture not found in resources. Tile at " << x << "," << y << " will be empty." << std::endl;
        }
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
        fixtureData->isBreakable = (grid[x][y] == &Resources::getTexture("monetki.png"));
        if (fixtureData->isBreakable) {
          // Удаляем std::cout << "Set isBreakable=true..." << std::endl;
        }

        b2::FixtureDef fixtureDef{};
        fixtureDef.shape = &shape;
        fixtureDef.userData = fixtureData;
        fixtureDef.friction = 0.0f;
        body->CreateFixture(&fixtureDef);

        // Check if this is a flag or flag pole and initialize FixtureData accordingly
        if (grid[x][y] == &Resources::getTexture("flag.png") || grid[x][y] == &Resources::getTexture("flaghtock.png")) {
            fixtureData->isFlag = true;
            // If it's the top flag block, mark it and store the pointer
            if (grid[x][y] == &Resources::getTexture("flag.png")) {
                fixtureData->isFlagTop = true;
                topFlagData = fixtureData;
                topFlagData->flagDescentSpeed = 7.0f; // Adjust this value to control descent speed (4x slower)
            }
        }
      }
    }
  }

  // Second pass: Correct pipe textures for top blocks
  for (size_t x = 0; x < grid.size(); x++) {
    for (size_t y = 0; y < grid[x].size(); y++) {
      // Check if the current block is a pipe block
      // Make sure the texture exists before accessing it
      bool isCurrentPipe = false;
      if (grid[x][y]) { // Only check if the pointer is not null
          isCurrentPipe = (grid[x][y] == &Resources::getTexture("pile1.png") ||
                               grid[x][y] == &Resources::getTexture("pile1_2.png") ||
                               grid[x][y] == &Resources::getTexture("pile2.png") ||
                               grid[x][y] == &Resources::getTexture("pile2_2.png"));
      }

      if (isCurrentPipe) {
        // Check if there is a pipe block directly above this one
        bool isPipeAbove = false;
        if (y > 0 && grid[x][y - 1] != nullptr) {
          bool isAbovePipe = false;
          if (grid[x][y-1] == &Resources::getTexture("pile1.png")) isAbovePipe = true;
          if (grid[x][y-1] == &Resources::getTexture("pile1_2.png")) isAbovePipe = true;
          if (grid[x][y-1] == &Resources::getTexture("pile2.png")) isAbovePipe = true;
          if (grid[x][y-1] == &Resources::getTexture("pile2_2.png")) isAbovePipe = true;
          if (isAbovePipe) {
            isPipeAbove = true;
          }
        }

        // If this is a top pipe block (either first row or no pipe block above)
        if (y == 0 || !isPipeAbove) {
            sf::Color color = image.getPixel(x, y); // Get original color for top texture selection
            if (std::abs(color.r - 0) <= 5 && std::abs(color.g - 145) <= 5 && std::abs(color.b - 0) <= 5 && color.a == 255) { // pile2.png color (top left)
                if (Resources::getTexture("pile2.png").getNativeHandle() != 0) {
                    grid[x][y] = &Resources::getTexture("pile2.png");
                } else {
                    grid[x][y] = nullptr;
                    std::cerr << "Warning: 'pile2.png' texture not found in resources. Tile at " << x << "," << y << " will be empty." << std::endl;
                }
            } else if (std::abs(color.r - 0) <= 5 && std::abs(color.g - 83) <= 5 && std::abs(color.b - 0) <= 5 && color.a == 255) { // pile2_2.png color (top right)
                if (Resources::getTexture("pile2_2.png").getNativeHandle() != 0) {
                    grid[x][y] = &Resources::getTexture("pile2_2.png");
                } else {
                    grid[x][y] = nullptr;
                    std::cerr << "Warning: 'pile2_2.png' texture not found in resources. Tile at " << x << "," << y << " will be empty." << std::endl;
                }
            } else if (std::abs(color.r - 0) <= 5 && std::abs(color.g - 255) <= 5 && std::abs(color.b - 0) <= 5 && color.a == 255) { // pile1.png color (bottom left, but at top)
                if (Resources::getTexture("pile2.png").getNativeHandle() != 0) {
                    grid[x][y] = &Resources::getTexture("pile2.png"); // Assume it should be top left
                } else {
                    grid[x][y] = nullptr;
                    std::cerr << "Warning: 'pile2.png' texture not found in resources. Tile at " << x << "," << y << " will be empty." << std::endl;
                }
            } else if (std::abs(color.r - 0) <= 5 && std::abs(color.g - 205) <= 5 && std::abs(color.b - 0) <= 5 && color.a == 255) { // pile1_2.png color (bottom right, but at top)
                if (Resources::getTexture("pile2_2.png").getNativeHandle() != 0) {
                    grid[x][y] = &Resources::getTexture("pile2_2.png"); // Assume it should be top right
                } else {
                    grid[x][y] = nullptr;
                    std::cerr << "Warning: 'pile2_2.png' texture not found in resources. Tile at " << x << "," << y << " will be empty." << std::endl;
                }
            }
        }
      }
    }
  }

  return marioPosition;
}

void Map::ClearGrid() {
    grid.clear();
}

void Map::Draw(Renderer &renderer) {
  // Удаляем std::cout << "Map::Draw - Grid size..." << std::endl;

  // Отрисовываем фоновый спрайт
  if (Resources::getTexture("sky.png").getNativeHandle() != 0) {
      backgroundSprite.setTexture(Resources::getTexture("sky.png"));

      // Получаем текущий вид
      sf::View currentView = renderer.target.getView();

      // Устанавливаем позицию спрайта в центр текущего вида
      backgroundSprite.setPosition(currentView.getCenter().x - currentView.getSize().x / 2.0f,
                                   currentView.getCenter().y - currentView.getSize().y / 2.0f);

      // Устанавливаем масштаб спрайта, чтобы он покрывал весь вид
      backgroundSprite.setScale(currentView.getSize().x / backgroundSprite.getLocalBounds().width,
                               currentView.getSize().y / backgroundSprite.getLocalBounds().height);

      renderer.target.draw(backgroundSprite);
  }

  int x = 0;
  for (const auto &column : grid) {
    int y = 0;
    for (const auto &cell : column) {
      if (cell) {
        // Calculate the drawing position
        sf::Vector2f drawPosition = sf::Vector2f(cellSize * x + cellSize / 2.0f,
                                                  cellSize * y + cellSize / 2.0f);

        // If this is the top flag block and its state needs visual update (descending or finished descending)
        if (topFlagData && topFlagData->mapX == x && topFlagData->mapY == y) {
             // Apply the descent progress if it's descending or has finished descending
            if (topFlagData->isDescending || topFlagData->flagDescentProgress > 0.0f) {
                 drawPosition.y += topFlagData->flagDescentProgress;
            }
        }

        // Удаляем std::cout << "Drawing block at..." << std::endl; // Можно добавить, если нужно больше деталей
        renderer.Draw(*cell,
                      drawPosition,
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

// Implementation of method to start the flag descent animation
void Map::StartFlagDescent() {
    if (topFlagData) {
        topFlagData->isDescending = true;
    }
}

// Implementation of method to update map state, including flag animation
void Map::Update(float deltaTime) {
    // Update flag animation
    if (topFlagData && topFlagData->isDescending) {
        // Calculate the maximum descent amount (e.g., height of the flagpole)
        // This might require knowing the total height of the flagpole, which we don't explicitly store yet.
        // For now, let's assume a fixed maximum descent or calculate it based on map structure above the flag.
        // A simple approach for now: descend a fixed distance or until it reaches a certain y coordinate.

        // Example: Descend by speed * deltaTime
        topFlagData->flagDescentProgress += topFlagData->flagDescentSpeed * deltaTime;

        // Calculate the correct maximum descent based on the flagpole height
        float maxDescent = 0.0f;
        int flagColumn = topFlagData->mapX;
        int flagRow = topFlagData->mapY;

        // Find the lowest flagpole tile in the flag's column
        int lowestFlagpoleRow = flagRow;
        for (int y = flagRow + 1; y < grid[flagColumn].size(); ++y) {
            if (grid[flagColumn][y] == &Resources::getTexture("flaghtock.png")) {
                lowestFlagpoleRow = y;
            } else {
                // Stop if we encounter a non-flagpole tile
                break;
            }
        }

        // Calculate max descent based on the distance to the lowest flagpole tile
        maxDescent = (lowestFlagpoleRow - flagRow) * cellSize;


        // Cap the descent progress
        if (topFlagData->flagDescentProgress >= maxDescent) {
            topFlagData->flagDescentProgress = maxDescent;
            // Stop the animation and signal victory
            topFlagData->isDescending = false;
            victorySequenceStarted = true;
        }
    }
}
