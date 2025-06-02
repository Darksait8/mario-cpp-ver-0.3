#include "Game.h"
#include "Map.h"
#include "Mario.h"
#include "Physics.h"
#include "Resources.h"
#include "Enemy.h"
#include "Coin.h"

#include <filesystem>
#include <SFML/Audio.hpp>
#include <iostream>

class Coin; // Forward declaration

Camera &GetCamera() { return data->camera; }
bool &GetPaused() { return data->paused; }

void Restart() {
  // Проверяем количество жизней перед рестартом
  if (data->lives > 1) {
    data->lives--; // Уменьшаем количество жизней
    data->score = 0; // Сбрасываем очки
    data->mario.coins = 0; // Сбрасываем количество монет у Марио (предполагая, что coins это член Mario)
    Physics::Init(&data->map);

    data->mario.position =
        data->map.CreateFromImage(data->mapImage, data->objects, &data->mario);
    data->mario.mapInstance = &data->map;
    data->mario.isDead = false;
    data->paused = false;

    data->mario.Begin();
    for (auto &object : data->objects) {
      object->Begin();
    }

    data->music.play();
  } else {
    // Жизни закончились, устанавливаем флаг Game Over
    if (Resources::sounds.count("mario-smert.mp3")) {
      data->deathSound.play(); // Воспроизводим звук смерти через член структуры Data
    }
    data->isGameOver = true;
    data->music.stop(); // Останавливаем фоновую музыку
  }
}

void Begin() {
  data = new Data();

  for (auto &file :
       std::filesystem::directory_iterator("resources/textures/")) {
    if (file.is_regular_file() && (file.path().extension() == ".png" ||
                                   file.path().extension() == ".jpg")) {
      std::string filename = file.path().filename().string();
      if (Resources::textures[filename].loadFromFile(file.path().string())) {
          std::cout << "Loaded texture: " << filename << std::endl;
      } else {
          std::cerr << "Failed to load texture: " << filename << std::endl;
      }
    }
  }

  // Загружаем текстуру Game Over (death.jpg)
  if (std::filesystem::exists("./resources/textures/death.jpg")) {
      Resources::textures["death.jpg"].loadFromFile("./resources/textures/death.jpg");
  } else {
      // Удаляем std::cerr << "Error: death.jpg not found in resources/textures/" << std::endl;
  }

  // std::cout << "Loaded " << Resources::textures.size() << " textures." << std::endl; // Комментируем вывод

  for (auto &file :
       std::filesystem::directory_iterator("./resources/sounds/")) {
    if (file.is_regular_file() && (file.path().extension() == ".ogg" ||
                                   file.path().extension() == ".wav" ||
                                   file.path().extension() == ".mp3")) {
      Resources::sounds[file.path().filename().string()].loadFromFile(
          file.path().string());
    }
  }

  // Debug: Check if the death sound is loaded
  if (Resources::sounds.count("mario-smert.mp3")) {
      // Удаляем std::cout << "Successfully loaded mario-smert.mp3" << std::endl;
      data->deathSound.setBuffer(Resources::sounds["mario-smert.mp3"]); // Устанавливаем буфер для deathSound
  } else {
      // Удаляем std::cerr << "Error: mario-smert.mp3 not found in Resources::sounds" << std::endl;
  }

  data->music.openFromFile("./resources/sounds/music.ogg");
  data->music.setLoop(true);
  data->music.setVolume(25);

  // data->font.loadFromFile("./resources/font.ttf"); // Закомментируем старую строку
  if (!data->font.loadFromFile("./resources/font.ttf")) {
      // Удаляем std::cerr << "Error: Failed to load font from ./resources/font.ttf" << std::endl;
      // Можно добавить какую-то обработку ошибки, например, завершение игры или использование шрифта по умолчанию
  }

  data->coinsText.setFillColor(sf::Color::White);
  data->coinsText.setOutlineColor(sf::Color::Black);
  data->coinsText.setOutlineThickness(1.0f);
  data->coinsText.setScale(0.1f, 0.1f);

  data->scoreText.setFont(data->font);
  data->scoreText.setFillColor(sf::Color::White);
  data->scoreText.setOutlineColor(sf::Color::Black);
  data->scoreText.setOutlineThickness(1.0f);
  data->scoreText.setScale(0.1f, 0.1f);

  data->livesText.setFont(data->font);
  data->livesText.setFillColor(sf::Color::White);
  data->livesText.setOutlineColor(sf::Color::Black);
  data->livesText.setOutlineThickness(1.0f);
  data->livesText.setScale(0.1f, 0.1f);

  data->timeText.setFont(data->font);
  data->timeText.setFillColor(sf::Color::White);
  data->timeText.setOutlineColor(sf::Color::Black);
  data->timeText.setOutlineThickness(1.0f);
  data->timeText.setScale(0.1f, 0.1f);

  data->backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
  data->backgroundShape.setOrigin(0.5f, 0.5f);

  data->mapImage.loadFromFile("./resources/level11.png");

  Restart();
}

void Update(float deltaTime) {
  // Если Марио мертв и игра еще не окончена, вызываем рестарт (или Game Over)
  if (data->mario.isDead && !data->isGameOver)
    Restart();

  if (data->paused)
    return;

  // Если игра окончена, пропускаем остальную логику обновления
  if (data->isGameOver) {
      // Здесь можно добавить логику для Game Over экрана (например, проверка ввода для рестарта/выста)
      return;
  }

  // Check if victory sequence has started
  if (data->map.victorySequenceStarted) {
      // If victory sequence just started, configure Mario
      if (!data->mario.inVictorySequence) {
          data->mario.inVictorySequence = true;
          // Deactivate Mario's physics body to manually control position
          if (data->mario.body) {
              data->mario.body->SetActive(false);
          }
          // Set initial vertical position for sliding down the flagpole
          // Assuming Mario's position is already set near the top of the flagpole when touching the flag
          // data->mario.position.y = ... ; // Maybe adjust if needed
          data->mario.facingLeft = false; // Ensure facing right
          // TODO: Start Mario's sliding down animation
           data->mario.textureToDraw = Resources::textures["idle.png"]; // Placeholder texture
      }

      // --- Victory Sequence Logic ---

      // Define the Y coordinate where Mario is considered on the ground after sliding down the pole
      // This value should be based on your level design and tile size
      // For example, if flagpole base is at y=Y and Mario's height is H, ground Y is Y - H/2
      // Let's use the Y position of the bottom-most flagpole tile for simplicity for now,
      // assuming Mario's origin is centered.
      float groundY = -1.0f; // Initialize with an invalid value
       if (data->mario.mapInstance && data->map.topFlagData) {
           int flagColumn = data->map.topFlagData->mapX;
           int flagRow = data->map.topFlagData->mapY;
           int lowestFlagpoleRow = flagRow;
            // Find the lowest flagpole tile in the flag's column
           for (int y = flagRow + 1; y < data->mario.mapInstance->grid[flagColumn].size(); ++y) {
               if (data->mario.mapInstance->grid[flagColumn][y] == &Resources::textures["flaghtock.png"]) {
                   lowestFlagpoleRow = y;
               } else {
                   break; // Stop if we encounter a non-flagpole tile
               }
           }
           // Calculate ground Y based on the lowest flagpole tile
           groundY = lowestFlagpoleRow * data->mario.mapInstance->cellSize + data->mario.mapInstance->cellSize / 2.0f;
       }


      // If Mario is above the ground Y, slide him down
      if (groundY > 0 && data->mario.position.y < groundY) {
          float slideSpeed = 5.0f; // Adjust slide speed as needed
          data->mario.position.y += slideSpeed * deltaTime;
          // Cap position at groundY
          if (data->mario.position.y > groundY) {
              data->mario.position.y = groundY;
          }
           // TODO: Update sliding animation
             data->mario.textureToDraw = Resources::textures["idle.png"]; // Placeholder texture

      } else {
          // Mario has reached the ground, start walking right
           float victoryWalkSpeed = 3.0f; // Adjust speed as needed
           data->mario.position.x += victoryWalkSpeed * deltaTime;
           // TODO: Update walking animation
           data->mario.runAnimation.Update(deltaTime);
           data->mario.textureToDraw = data->mario.runAnimation.GetTexture();

          // Check for end of walk (e.g., reach a certain x-coordinate near the end of the map)
          // Define the X coordinate where Mario walks off screen
          float endOfWalkX = (data->mario.mapInstance ? data->mario.mapInstance->grid.size() * data->mario.mapInstance->cellSize + 1.0f : data->mario.position.x + 100.0f); // Example: 1 unit past the end of the map grid

          if (data->mario.position.x >= endOfWalkX) {
              data->mario.victoryWalkComplete = true;
              // Stop Mario's movement (optional, as he is now off-screen or game is ending)
              // Maybe hide Mario?
              // Transition to victory screen
              data->isVictory = true;
              // Optionally stop music or play victory sound
              data->music.stop();
          }
      }

      // Keep camera centered on Mario during the sequence
       data->camera.position = data->mario.position;

      // Update the map (for animations, etc.) - Map update should happen regardless for flag animation
      data->map.Update(deltaTime); // Keep map update outside the condition to ensure flag animation runs

      // Skip normal update logic
      return;
  }

  // If the victory sequence is not started, perform normal game updates
  // Увеличиваем время
  data->timeElapsed += deltaTime;

  Physics::Update(deltaTime, 16, 6);

  data->mario.Update(deltaTime); // Normal Mario update (input, etc.)

  // Update the map (for animations, etc.)
  data->map.Update(deltaTime);

  // Camera update should happen in both cases to follow Mario
  data->camera.position = data->mario.position;

  // Проверяем, нужно ли разрушить блок - this should only happen during normal gameplay
  if (!data->map.victorySequenceStarted) {
    if (data->mario.shouldBreakBlock) {
      // Вызываем метод разрушения блока у карты (меняем текстуру)
      data->map.BreakBlock(data->mario.blockToBreakX, data->mario.blockToBreakY, &Resources::textures["nomoney.png"]);
      
      // Изменяем тип физического тела блока на статический, чтобы он остался на месте, но не реагировал на физику
      if (data->mario.bodyToBreak) {
          data->mario.bodyToBreak->SetType(b2::BodyType::staticBody);

          // Находим фикстуру блока и помечаем ее как неразрушаемую
          b2::Fixture* blockFixture = data->mario.bodyToBreak->GetFixtureList(); // Предполагаем, что у блока одна фикстура
          if (blockFixture) {
              FixtureData* fixtureData = static_cast<FixtureData*>(blockFixture->GetUserData());
              if (fixtureData) {
                  fixtureData->isBreakable = false; // Блок больше не разрушаемый
              }
          }
      }

      // Создаем новую монетку в позиции разрушенного блока
      Coin* newCoin = new Coin(&data->mario); // Pass Mario pointer
      newCoin->position = sf::Vector2f(
          data->mario.blockToBreakX * data->map.cellSize + data->map.cellSize / 2.0f,
          data->mario.blockToBreakY * data->map.cellSize + data->map.cellSize / 2.0f);
      newCoin->initialPosition = newCoin->position; // Сохраняем начальную позицию
      data->objects.push_back(newCoin);
      newCoin->Begin(); // Инициализируем монетку (создает физическое тело и запускает анимацию)

      // Даем монетке начальную скорость вверх и уменьшаем гравитацию
      if (newCoin->body) { // Убеждаемся, что физическое тело создано
          newCoin->body->SetLinearVelocity(b2::Vec2(0.0f, -7.0f)); // Уменьшаем скорость вверх
          newCoin->body->SetGravityScale(0.5f); // Оставляем уменьшенную гравитацию для плавности
      }

      // Сбрасываем флаг и поля у Марио
      data->mario.shouldBreakBlock = false;
      data->mario.blockToBreakX = -1;
      data->mario.blockToBreakY = -1;
      data->mario.bodyToBreak = nullptr;
    }
  }

  // Удаляем объекты, помеченные как isDead, после шага физики - this should only happen during normal gameplay
  data->objects.erase(
      std::remove_if(data->objects.begin(), data->objects.end(),
                     [](Object *obj) {
                       // Удаляем врагов, помеченных как isDead
                       if (obj->tag == "enemy") {
                         return dynamic_cast<Enemy*>(obj)->IsDead();
                       }
                       // Удаляем монетки, помеченные как isCollected
                       if (obj->tag == "coin") {
                         return dynamic_cast<Coin*>(obj)->isCollected;
                       }
                       return false; // Не удаляем другие объекты
                     }),
      data->objects.end());

  for (auto &object : data->objects) {
    object->Update(deltaTime);
  }
}

void Render(Renderer &renderer) {
  // Если игра окончена, отображаем Game Over экран
  if (data->isGameOver) {
      renderer.target.clear(sf::Color::Black); // Очищаем экран

      // !!! Сохраняем текущий вид и устанавливаем вид по умолчанию для отрисовки UI !!!
      sf::View originalView = renderer.target.getView();
      renderer.target.setView(renderer.target.getDefaultView());

      // Выводим текстовое сообщение GAME OVER
      sf::Text gameOverText("GAME OVER", data->font, 36); // Размер шрифта
      gameOverText.setFillColor(sf::Color::White);

      // Получаем размеры окна (render target)
      sf::Vector2u windowSize = renderer.target.getSize();

      // Устанавливаем начало координат текста в его центр для правильного позиционирования
      gameOverText.setOrigin(gameOverText.getLocalBounds().width / 2.0f, gameOverText.getLocalBounds().height / 2.0f);

      // Устанавливаем позицию текста в центр окна
      gameOverText.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);

      renderer.target.draw(gameOverText);

      // !!! Восстанавливаем оригинальный вид !!!
      renderer.target.setView(originalView);

      return; // Пропускаем обычную отрисовку уровня и объектов
  }

  // Обычная отрисовка игры
  renderer.Draw(Resources::textures["sky.png"], data->camera.position,
                data->camera.GetViewSize());

  data->map.Draw(renderer);

  for (auto &object : data->objects) {
    object->Render(renderer);
  }

  data->mario.Draw(renderer);

  Physics::DebugDraw(renderer);
}

void RenderUI(Renderer &renderer) {
  // Не отображаем UI, если игра окончена
  if (data->isGameOver) {
      return;
  }

  // Отображаем экран победы, если игра выиграна
  if (data->isVictory) {
      renderer.target.clear(sf::Color::Black); // Очищаем экран (или оставьте последний кадр игры)

      // !!! Сохраняем текущий вид и устанавливаем вид по умолчанию для отрисовки UI !!!
      sf::View originalView = renderer.target.getView();
      renderer.target.setView(renderer.target.getDefaultView());

      // Получаем размеры окна (render target)
      sf::Vector2u windowSize = renderer.target.getSize();

      // Выводим текстовое сообщение VICTORY!
      sf::Text victoryText("VICTORY!", data->font, 36); // Размер шрифта
      victoryText.setFillColor(sf::Color::White);
      victoryText.setOrigin(victoryText.getLocalBounds().width / 2.0f, victoryText.getLocalBounds().height / 2.0f);
      victoryText.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f - 50); // Position above stats
      renderer.target.draw(victoryText);

      // Display Score, Time, and Coins
      sf::Text statsText("", data->font, 24);
      statsText.setFillColor(sf::Color::White);

      // Score
      statsText.setString("SCORE: " + std::to_string(data->score));
      statsText.setOrigin(statsText.getLocalBounds().width / 2.0f, statsText.getLocalBounds().height / 2.0f);
      statsText.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);
      renderer.target.draw(statsText);

      // Time
      statsText.setString("TIME: " + std::to_string(static_cast<int>(data->timeElapsed)));
      statsText.setOrigin(statsText.getLocalBounds().width / 2.0f, statsText.getLocalBounds().height / 2.0f);
      statsText.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f + 30);
      renderer.target.draw(statsText);

      // Coins
      statsText.setString("COINS: " + std::to_string(data->mario.GetCoins()));
      statsText.setOrigin(statsText.getLocalBounds().width / 2.0f, statsText.getLocalBounds().height / 2.0f);
      statsText.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f + 60);
      renderer.target.draw(statsText);

      // !!! Восстанавливаем оригинальный вид !!!
      renderer.target.setView(originalView);

      return; // Пропускаем обычную отрисовку UI
  }

  sf::Vector2f viewSize = data->camera.GetViewSize();
  // Начальная позиция UI блоков (верхний край видимой области камеры)
  sf::Vector2f uiTopLeft = -viewSize / 2.0f + sf::Vector2f(1.0f, 1.0f); // Небольшой отступ от края

  // Позиции для каждого элемента (центр секции)
  float horizontalPadding = 2.0f; // Добавляем горизонтальный отступ
  float sectionWidth = (viewSize.x - horizontalPadding * 5) / 4.0f; // Учитываем отступы между секциями и по краям

  float scoreX = uiTopLeft.x + horizontalPadding + sectionWidth * 0.5f;
  float coinX = uiTopLeft.x + horizontalPadding * 2 + sectionWidth * 1.5f;
  float timeX = uiTopLeft.x + horizontalPadding * 3 + sectionWidth * 2.5f;
  float livesX = uiTopLeft.x + horizontalPadding * 4 + sectionWidth * 3.5f;

  float verticalOffset = 2.5f; // Увеличиваем отступ для значений под надписями

  // Отображение Score
  data->scoreText.setString("SCORE");
  data->scoreText.setOrigin(data->scoreText.getLocalBounds().width / 2.0f, 0.0f); // Выравнивание по центру
  data->scoreText.setPosition(scoreX, uiTopLeft.y);
  renderer.target.draw(data->scoreText);

  data->scoreText.setString(std::to_string(data->score));
  data->scoreText.setOrigin(data->scoreText.getLocalBounds().width / 2.0f, 0.0f); // Выравнивание по центру
  data->scoreText.setPosition(scoreX, uiTopLeft.y + verticalOffset);
  renderer.target.draw(data->scoreText);

  // Отображение Coin
  data->coinsText.setString("COIN");
  data->coinsText.setOrigin(data->coinsText.getLocalBounds().width / 2.0f, 0.0f); // Выравнивание по центру
  data->coinsText.setPosition(coinX, uiTopLeft.y);
  renderer.target.draw(data->coinsText);

  data->coinsText.setString(std::to_string(data->mario.GetCoins()));
  data->coinsText.setOrigin(data->coinsText.getLocalBounds().width / 2.0f, 0.0f); // Выравнивание по центру
  data->coinsText.setPosition(coinX, uiTopLeft.y + verticalOffset);
  renderer.target.draw(data->coinsText);

  // Отображение Time
  data->timeText.setString("TIME");
  data->timeText.setOrigin(data->timeText.getLocalBounds().width / 2.0f, 0.0f); // Выравнивание по центру
  data->timeText.setPosition(timeX, uiTopLeft.y);
  renderer.target.draw(data->timeText);

  data->timeText.setString(std::to_string(static_cast<int>(data->timeElapsed)));
  data->timeText.setOrigin(data->timeText.getLocalBounds().width / 2.0f, 0.0f); // Выравнивание по центру
  data->timeText.setPosition(timeX, uiTopLeft.y + verticalOffset);
  renderer.target.draw(data->timeText);

  // Отображение Lives
  data->livesText.setString("LIVES");
  data->livesText.setOrigin(data->livesText.getLocalBounds().width / 2.0f, 0.0f); // Выравнивание по центру
  data->livesText.setPosition(livesX, uiTopLeft.y);
  renderer.target.draw(data->livesText);

  data->livesText.setString(std::to_string(data->lives));
  data->livesText.setOrigin(data->livesText.getLocalBounds().width / 2.0f, 0.0f); // Выравнивание по центру
  data->livesText.setPosition(livesX, uiTopLeft.y + verticalOffset);
  renderer.target.draw(data->livesText);

  if (data->paused) {
    data->backgroundShape.setScale(data->camera.GetViewSize());
    renderer.target.draw(data->backgroundShape);
  }
}

void DeleteObject(Object *object) {
  const auto &it =
      std::find(data->objects.begin(), data->objects.end(), object);
  if (it != data->objects.end()) {
    delete *it;
    data->objects.erase(it);
  }
}

void AddScore(int scoreToAdd) {
    if (data) { // Проверяем, инициализирована ли data
        data->score += scoreToAdd;
    }
}

void End() {
  delete data;
  Resources::textures.clear();
  Resources::sounds.clear();
}
