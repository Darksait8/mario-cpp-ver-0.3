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
#include <windows.h>

class Coin; // Forward declaration

Camera &GetCamera() { return data->camera; }
bool &GetPaused() { return data->paused; }

void Restart() {
  // Проверяем количество жизней перед рестартом
  if (data->lives > 1) {
    data->lives--; // Уменьшаем количество жизней
    data->score = 0; // Сбрасываем очки
    data->mario.coins = 0; // Сбрасываем количество монет у Марио (предполагая, что coins это член Mario)
    data->hasStartedWalking = false; // Reset walking flag
    data->walkingStartTime = 0.0f; // Reset walking timer
    Physics::Init(&data->map);

    data->mario.position = data->map.CreateFromImage(data->mapImage, data->objects, &data->mario);
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
    data->deathSound.setBuffer(Resources::getSound("mario-smert.mp3"));
    data->isGameOver = true;
    data->music.stop(); // Останавливаем фоновую музыку
  }
}

void StartNewGame() {
    // Reset all relevant game data for a completely new game
    // data->lives = 3; // Starting lives (УБРАНО, чтобы не сбрасывать жизни после выбора сложности)
    data->score = 0;
    data->mario.coins = 0;
    data->timeElapsed = 0.0f;
    data->isGameOver = false;
    data->isVictory = false; // Ensure victory flag is reset
    data->map.victorySequenceStarted = false; // Reset map victory sequence
    data->mario.inVictorySequence = false; // Reset Mario victory sequence
    data->mario.isSlidingDownFlag = false;
    data->mario.isVictoryDancing = false;
    data->mario.victoryWalkComplete = false;
    data->camera.position = sf::Vector2f(0, 0); // Reset camera position
    data->hasStartedWalking = false; // Reset walking flag for new game
    data->walkingStartTime = 0.0f; // Reset walking timer for new game

    // Clear existing objects and re-initialize physics
    for (auto& obj : data->objects) {
        delete obj;
    }
    data->objects.clear();
    Physics::Init(&data->map);

    // Recreate the map and place Mario
    // data->map.LoadMapFromFile("./resources/level11.png"); // Reload the map (this method does not exist)
    // First, clear the map grid before recreating from image
    data->map.ClearGrid();
    data->mario.position = data->map.CreateFromImage(data->mapImage, data->objects, &data->mario);
    data->mario.mapInstance = &data->map;
    data->mario.isDead = false;
    data->paused = false; // Ensure game is not paused
    data->isInMenu = false; // Exit menu

    data->mario.Begin(); // Re-initialize Mario
    for (auto &object : data->objects) {
      object->Begin(); // Re-initialize game objects
    }

    data->music.play(); // Start game music
}

void Begin(sf::RenderWindow& window) {
  data = new Data(); // САМАЯ ПЕРВАЯ СТРОКА!
  // СНАЧАЛА загружаем все текстуры и звуки
  for (auto &file :
       std::filesystem::directory_iterator("resources/textures/")) {
    if (file.is_regular_file() && (file.path().extension() == ".png" ||
                                   file.path().extension() == ".jpg")) {
      std::string filename = file.path().filename().string();
      sf::Image tempImage; // Создаем временный объект sf::Image
      if (!tempImage.loadFromFile(file.path().string())) {
          std::cerr << "Error: Failed to load image data for texture: " << filename << std::endl;
          continue; // Пропускаем эту текстуру, если изображение не загружено
      }
      if (tempImage.getSize().x == 0 || tempImage.getSize().y == 0) {
          std::cerr << "Error: Image data for texture is empty: " << filename << std::endl;
          continue; // Пропускаем эту текстуру, если изображение пусто
      }

      sf::Texture tempTexture; // Создаем временную текстуру
      if (tempTexture.loadFromImage(tempImage)) {
          Resources::getTexture(filename) = std::move(tempTexture); // Перемещаем загруженную текстуру в карту ресурсов
          std::cout << "Loaded texture: " << filename << std::endl;
      } else {
          std::cerr << "Failed to load texture from image for: " << filename << std::endl;
      }
    }
  }

  if (std::filesystem::exists("./resources/textures/death.jpg")) {
      sf::Image deathImage; // Временный объект sf::Image для death.jpg
      if (!deathImage.loadFromFile("./resources/textures/death.jpg")) {
          std::cerr << "Error: Failed to load image data for death.jpg." << std::endl;
      } else if (deathImage.getSize().x == 0 || deathImage.getSize().y == 0) {
          std::cerr << "Error: Image data for death.jpg is empty." << std::endl;
      } else {
          sf::Texture deathTexture; // Временная текстура для death.jpg
          if (deathTexture.loadFromImage(deathImage)) {
              Resources::getTexture("death.jpg") = std::move(deathTexture);
              std::cout << "Loaded texture: death.jpg" << std::endl;
          } else {
              std::cerr << "Failed to load texture from image for death.jpg." << std::endl;
          }
      }
  }

  for (auto &file :
       std::filesystem::directory_iterator("./resources/sounds/")) {
    if (file.is_regular_file() && (file.path().extension() == ".ogg" ||
                                   file.path().extension() == ".wav" ||
                                   file.path().extension() == ".mp3")) {
      Resources::getSound(file.path().filename().string()).loadFromFile(
          file.path().string());
    }
  }

  // Теперь создаём data и всё остальное
  data->window = &window; // Инициализируем указатель на окно
  data->window->setActive(true); // Убеждаемся, что контекст окна активен для операций SFML
  data->isInMenu = true; // Начинаем с меню
  data->gameMenu.initializeActions(data); // Инициализируем действия меню здесь
  data->gameMenu.loadUsers(); // Загружаем данные пользователей при запуске

  // Populate English strings - REMOVED (now hardcoded Russian)
  // data->englishStrings["coinsText"] = sf::String(L"монеты");
  // data->englishStrings["scoreText"] = sf::String(L"очки");
  // data->englishStrings["livesText"] = sf::String(L"жизни");
  // data->englishStrings["timeText"] = sf::String(L"время");
  // data->englishStrings["gameOverText"] = sf::String(L"ИГРА ОКОНЧЕНА");
  // data->englishStrings["restartOrExitText"] = sf::String(L"нажмите R для рестарта или ESC для выхода");
  // data->englishStrings["languageButtonText_EN"] = sf::String(L"RU"); // Button text for English locale

  // Populate Russian strings - REMOVED (now hardcoded Russian)
  // data->russianStrings["coinsText"] = sf::String(L"монеты");
  // data->russianStrings["scoreText"] = sf::String(L"очки");
  // data->russianStrings["livesText"] = sf::String(L"жизни");
  // data->russianStrings["timeText"] = sf::String(L"время");
  // data->russianStrings["gameOverText"] = sf::String(L"ИГРА ОКОНЧЕНА");
  // data->russianStrings["restartOrExitText"] = sf::String(L"нажмите R для рестарта или ESC для выхода");
  // data->russianStrings["languageButtonText_RU"] = sf::String(L"EN"); // Button text for Russian locale

  // Initialize backgroundDimmer
  data->backgroundDimmer.setSize(sf::Vector2f(data->window->getDefaultView().getSize().x, data->window->getDefaultView().getSize().y));
  data->backgroundDimmer.setFillColor(sf::Color(0, 0, 0, 0)); // Прозрачный черный для начала

  data->deathSound.setBuffer(Resources::getSound("mario-smert.mp3"));

  data->music.openFromFile("./resources/sounds/music.ogg");
  data->music.setLoop(true);
  data->music.setVolume(25);

  // Загрузка шрифта
  if (!data->font.loadFromFile("C:/Users/User/Desktop/mario-cpp-ver-0.3/resources/troika.otf")) {
    std::cerr << "Error loading font resources/troika.otf" << std::endl;
    std::cout << "DEBUG: Failed to load font resources/troika.otf" << std::endl;
    return;
  }
  std::cout << "DEBUG: Successfully loaded font resources/troika.otf" << std::endl;

  // Устанавливаем шрифт для всех объектов sf::Text после его успешной загрузки
  data->coinsText.setFont(data->font);
  data->coinsText.setFillColor(sf::Color::White);
  data->coinsText.setOutlineColor(sf::Color::Black);
  data->coinsText.setOutlineThickness(1.0f);
  data->coinsText.setCharacterSize(32);

  data->coinsValueText.setFont(data->font);
  data->coinsValueText.setFillColor(sf::Color::White);
  data->coinsValueText.setOutlineColor(sf::Color::Black);
  data->coinsValueText.setOutlineThickness(1.0f);
  data->coinsValueText.setCharacterSize(32);

  data->scoreText.setFont(data->font);
  data->scoreText.setFillColor(sf::Color::White);
  data->scoreText.setOutlineColor(sf::Color::Black);
  data->scoreText.setOutlineThickness(1.0f);
  data->scoreText.setCharacterSize(32);

  data->scoreValueText.setFont(data->font);
  data->scoreValueText.setFillColor(sf::Color::White);
  data->scoreValueText.setOutlineColor(sf::Color::Black);
  data->scoreValueText.setOutlineThickness(1.0f);
  data->scoreValueText.setCharacterSize(32);

  data->livesText.setFont(data->font);
  data->livesText.setFillColor(sf::Color::White);
  data->livesText.setOutlineColor(sf::Color::Black);
  data->livesText.setOutlineThickness(1.0f);
  data->livesText.setCharacterSize(32);

  data->livesValueText.setFont(data->font);
  data->livesValueText.setFillColor(sf::Color::White);
  data->livesValueText.setOutlineColor(sf::Color::Black);
  data->livesValueText.setOutlineThickness(1.0f);
  data->livesValueText.setCharacterSize(32);

  data->timeText.setFont(data->font);
  data->timeText.setFillColor(sf::Color::White);
  data->timeText.setOutlineColor(sf::Color::Black);
  data->timeText.setOutlineThickness(1.0f);
  data->timeText.setCharacterSize(32);

  data->timeValueText.setFont(data->font);
  data->timeValueText.setFillColor(sf::Color::White);
  data->timeValueText.setOutlineColor(sf::Color::Black);
  data->timeValueText.setOutlineThickness(1.0f);
  data->timeValueText.setCharacterSize(32);

  data->backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
  data->backgroundShape.setOrigin(0.5f, 0.5f);

  if (!data->mapImage.loadFromFile("./resources/level11.png")) {
      std::cerr << "Error: Failed to load map image from ./resources/level11.png." << std::endl;
      // Здесь можно добавить логику для обработки ошибки, например, выход из приложения
      // exit(1); 
  } else if (data->mapImage.getSize().x == 0 || data->mapImage.getSize().y == 0) {
      std::cerr << "Error: Loaded map image is empty: ./resources/level11.png." << std::endl;
  } else {
      std::cout << "DEBUG: mapImage loaded successfully. Size: " << data->mapImage.getSize().x << "x" << data->mapImage.getSize().y << std::endl;
  }

  // Initialize language button text - REMOVED (not needed with hardcoded Russian)
  // data->languageButtonText.setFont(data->font);
  // data->languageButtonText.setCharacterSize(30);
  // data->languageButtonText.setFillColor(sf::Color::White);
  // data->languageButtonText.setOutlineColor(sf::Color::Black);
  // data->languageButtonText.setOutlineThickness(1.0f);

  // Автоматический выбор языка системы - REMOVED
  // LANGID langId = GetUserDefaultUILanguage();
  // if (PRIMARYLANGID(langId) == LANG_RUSSIAN) {
  //     data->currentLanguage = Language::RUSSIAN;
  // } else {
  //     data->currentLanguage = Language::ENGLISH;
  // }

  // UpdateLocalizedStrings(); // Call to set initial localized strings AFTER window is initialized - REMOVED

  Restart();
}

void Update(float deltaTime) {
  // std::cout << "Game::Update - DeltaTime: " << deltaTime << ", isInMenu: " << data->isInMenu << ", isVictory: " << data->isVictory << ", GameMenuState: " << static_cast<int>(data->gameMenu.getCurrentState()) << std::endl; // Debug print

  if (data->isInMenu) {
    data->gameMenu.update();
    return;
  }

  // Если Марио мертв и игра еще не окончена, вызываем рестарт (или Game Over)
  if (data->mario.isDead && !data->isGameOver)
    Restart();

  if (data->paused)
    return;

  // Если игра окончена, пропускаем остальную логику обновления
  if (data->isGameOver) {
      // Здесь можно добавить логику для Game Over экрана (например, проверка ввода для рестарта/выхода)
      return;
  }

  // Check if Mario has started walking
  if (!data->hasStartedWalking && (data->mario.body->GetLinearVelocity().x != 0 || data->mario.body->GetLinearVelocity().y != 0)) {
      data->hasStartedWalking = true;
      data->walkingStartTime = data->timeElapsed;
  }

  // Check if victory sequence has started
  if (data->map.victorySequenceStarted) {
      // If victory sequence just started, configure Mario
      if (!data->mario.inVictorySequence) {
          data->mario.inVictorySequence = true;
          data->mario.isSlidingDownFlag = true;
          // Deactivate Mario's physics body to manually control position
          if (data->mario.body) {
              data->mario.body->SetActive(false);
          }
          data->mario.facingLeft = false; // Ensure facing right

          // Play victory sound if available, only once
          if (!data->isVictory) {
              sf::Sound victorySound;
              victorySound.setBuffer(Resources::getSound("victory.wav"));
              victorySound.play();
              data->isVictory = true; // Set flag to indicate sound has played
          }
          data->music.stop(); // Stop background music
      }

      // --- Victory Sequence Logic ---
      float groundY = -1.0f;
       if (data->mario.mapInstance && data->map.topFlagData) {
           int flagColumn = data->map.topFlagData->mapX;
           int flagRow = data->map.topFlagData->mapY;
           int lowestFlagpoleRow = flagRow;
           for (int y = flagRow + 1; y < data->mario.mapInstance->grid[flagColumn].size(); ++y) {
               if (data->mario.mapInstance->grid[flagColumn][y] == &Resources::getTexture("flaghtock.png")) {
                   lowestFlagpoleRow = y;
               } else {
                  break;
               }
           }
           groundY = lowestFlagpoleRow * data->mario.mapInstance->cellSize + data->mario.mapInstance->cellSize / 2.0f;
       }

      // If Mario is currently sliding down the flag
      if (data->mario.isSlidingDownFlag) {
          float slideSpeed = 5.0f;
          data->mario.position.y += slideSpeed * deltaTime; // Move Mario down

          // Once Mario reaches or passes the ground Y, stop sliding
          if (data->mario.position.y >= groundY) {
              const float marioSpriteOffsetY = 16.0f; // Смещение вниз для корректного положения на земле
              data->mario.position.y = groundY + marioSpriteOffsetY; // Зафиксировать на земле с учетом смещения
              data->mario.isSlidingDownFlag = false; // Stop sliding
              // Start walking right by ensuring a horizontal velocity for Mario's visual position
              // Note: Mario's body is inactive, so we update his position directly
              data->mario.body->SetLinearVelocity(b2::Vec2(3.0f, 0.0f)); // Set a dummy velocity for animation (body is inactive anyway)
              // Ensure Mario's vertical velocity is zeroed out in his body, as it's inactive but good practice
              if (data->mario.body) {
                  data->mario.body->SetLinearVelocity(b2::Vec2(data->mario.body->GetLinearVelocity().x, 0.0f));
              }
              // Start the victory screen timer when Mario starts walking
              data->startVictoryScreenTimer = true;
              data->victoryScreenTimer = 0.0f;
          }
      } else if (!data->mario.isVictoryDancing) { // If not sliding and not dancing, Mario is walking
          float victoryWalkSpeed = 3.0f;
          const float marioSpriteOffsetY =1.0f; // Смещение вниз для корректного положения на земле
          data->mario.position.x += victoryWalkSpeed * deltaTime;
          data->mario.position.y = groundY + marioSpriteOffsetY; // ВСЕГДА фиксируем на земле с учетом смещения!
      }

      // Update Mario victory sequence (handles sliding, walking, dancing)
      data->mario.Update(deltaTime);

      // If victory screen timer is active, count it down
      if (data->startVictoryScreenTimer) {
          data->victoryScreenTimer += deltaTime;
          if (data->victoryScreenTimer >= 2.0f) { // 2 second delay
              data->isInMenu = true;
              data->gameMenu.setState(GameMenuState::VICTORY);
              data->startVictoryScreenTimer = false; // Reset flag
              data->isVictory = false; // Reset isVictory for next win
          }
      }
      // Camera should follow Mario during victory sequence
       data->camera.position = data->mario.position;
      data->map.Update(deltaTime); // Update map (for animations, etc.) even during victory
      return; // Skip normal game update if in victory sequence
  }

  // Normal game updates (only if not in victory sequence and not in menu/paused/game over)
  data->timeElapsed += deltaTime; // Update game time

  Physics::Update(deltaTime, 16, 6); // Physics update

  data->mario.Update(deltaTime); // Normal Mario update (input, etc.)

  data->map.Update(deltaTime); // Map update

  // Handle block breaking
    if (data->mario.shouldBreakBlock) {
      data->map.BreakBlock(data->mario.blockToBreakX, data->mario.blockToBreakY, &Resources::getTexture("nomoney.png"));
      
      if (data->mario.bodyToBreak) {
          data->mario.bodyToBreak->SetType(b2::BodyType::staticBody);
        b2::Fixture* blockFixture = data->mario.bodyToBreak->GetFixtureList();
          if (blockFixture) {
              FixtureData* fixtureData = static_cast<FixtureData*>(blockFixture->GetUserData());
              if (fixtureData) {
                fixtureData->isBreakable = false;
              }
          }
      }

    Coin* newCoin = new Coin(&data->mario);
      newCoin->position = sf::Vector2f(
          data->mario.blockToBreakX * data->map.cellSize + data->map.cellSize / 2.0f,
          data->mario.blockToBreakY * data->map.cellSize + data->map.cellSize / 2.0f);
    newCoin->initialPosition = newCoin->position;
      data->objects.push_back(newCoin);
    newCoin->Begin();

    if (newCoin->body) {
        newCoin->body->SetLinearVelocity(b2::Vec2(0.0f, -7.0f));
        newCoin->body->SetGravityScale(0.5f);
      }

      data->mario.shouldBreakBlock = false;
      data->mario.blockToBreakX = -1;
      data->mario.blockToBreakY = -1;
      data->mario.bodyToBreak = nullptr;
  }

  // Remove dead/collected objects
  data->objects.erase(
      std::remove_if(data->objects.begin(), data->objects.end(),
                     [](Object *obj) {
                       if (obj->tag == "enemy") {
                         return dynamic_cast<Enemy*>(obj)->IsDead();
                       }
                       if (obj->tag == "coin") {
                         return dynamic_cast<Coin*>(obj)->isCollected;
                       }
                       return false;
                     }),
      data->objects.end());

  for (auto &object : data->objects) {
    object->Update(deltaTime);
  }

  // Camera update (should be consistent, whether in victory or normal game)
  data->camera.position = data->mario.position;
}

void Render(Renderer &renderer, sf::RenderWindow& window) {
  // Если мы находимся в любом меню (главное, авторизация, опции, лидерборд), отрисовываем только меню
  if (data->isInMenu) {
    data->gameMenu.render(window);
    return;
  }

  // Отрисовываем игровой мир, если игра не на паузе (кроме Game Over/Victory, которые имеют свою логику)
  // или если игра на паузе, но не в меню паузы (т.е. на Game Over/Victory экране)
  if (!data->paused || data->isGameOver) { // Removed data->isVictory here
    renderer.target.clear(sf::Color(146, 208, 255));
    data->map.Draw(renderer);
    for (auto &object : data->objects) {
      object->Render(renderer);
    }
    data->mario.Draw(renderer);
  }

  // Если игра окончена, отображаем Game Over экран
  if (data->isGameOver) {
      // !!! Сохраняем текущий вид и устанавливаем вид по умолчанию для отрисовки UI !!!
      sf::View originalView = renderer.target.getView();
      renderer.target.setView(renderer.target.getDefaultView());

      renderer.target.draw(data->backgroundDimmer); // Затемняем экран

      sf::Text gameOverText(sf::String(L"ИГРА ОКОНЧЕНА"), data->font, 60);
      gameOverText.setFillColor(sf::Color::Red);
      gameOverText.setOrigin(gameOverText.getLocalBounds().width / 2, gameOverText.getLocalBounds().height / 2);
      gameOverText.setPosition(renderer.target.getSize().x / 2, renderer.target.getSize().y / 2 - 50);
      renderer.Draw(gameOverText);

      sf::Text restartText(sf::String(L"нажмите R для рестарта или ESC для выхода"), data->font, 24);
      restartText.setFillColor(sf::Color::White);
      restartText.setOrigin(restartText.getLocalBounds().width / 2, restartText.getLocalBounds().height / 2);
      restartText.setPosition(renderer.target.getSize().x / 2, renderer.target.getSize().y / 2 + 50);
      renderer.Draw(restartText);

      // !!! Восстанавливаем оригинальный вид !!!
      renderer.target.setView(originalView);
      return;
  }

  // Если игра на паузе, затемняем экран и отрисовываем меню паузы (или опций)
  if (data->paused && (data->gameMenu.getCurrentState() == GameMenuState::PAUSE || data->gameMenu.getCurrentState() == GameMenuState::OPTIONS)) {
      // Сохраняем текущий вид окна
      sf::View originalView = renderer.target.getView();
      // Устанавливаем вид по умолчанию для отрисовки затемнения и меню
      renderer.target.setView(renderer.target.getDefaultView());

      // Затемнение для паузы/опций будет отрисовываться внутри GameMenu::renderOptions/renderPauseMenu
      // renderer.target.draw(data->backgroundDimmer); // Удалил отсюда
      data->gameMenu.render(window); // Отрисовываем меню паузы/опций

      // Восстанавливаем оригинальный вид окна
      renderer.target.setView(originalView);
  }
}

void RenderUI(Renderer &renderer) {
  if (data->isInMenu) { // Если мы в меню, не отрисовываем UI игры
      return;
  }
  if (data->paused) { // Если игра на паузе, не отрисовываем UI
      return;
  }

  // Сохраняем текущий вид UI
  sf::View originalUIView = renderer.target.getView();

  // Устанавливаем вид по умолчанию для отрисовки UI
  renderer.target.setView(renderer.target.getDefaultView());

  // Затемняющий фон для регулировки яркости (глобально)
  sf::Vector2f viewSize = renderer.target.getView().getSize();
  sf::Vector2f viewCenter = renderer.target.getView().getCenter();
  sf::RectangleShape dimmer(sf::Vector2f(viewSize.x, viewSize.y));
  dimmer.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>((1.0f - data->brightness) * 255)));
  dimmer.setPosition(viewCenter.x - viewSize.x / 2, viewCenter.y - viewSize.y / 2);
  renderer.target.draw(dimmer);

  float windowWidth = renderer.target.getSize().x;
  float topLabelY = 20.0f; // Y position for labels
  float topValueY = 50.0f; // Y position for values

  // Calculate segment centers for 4 equal parts of the screen
  float segmentWidth = windowWidth / 4.0f;
  float segmentCenter1 = segmentWidth * 0.5f; // Center of the first segment
  float segmentCenter2 = segmentWidth * 1.5f; // Center of the second segment
  float segmentCenter3 = segmentWidth * 2.5f; // Center of the third segment
  float segmentCenter4 = segmentWidth * 3.5f; // Center of the fourth segment

  // SCORE - Centered in the first segment
  data->scoreText.setString(sf::String(L"ОЧКИ"));
  data->scoreText.setOrigin(data->scoreText.getLocalBounds().width / 2.0f, 0); // Center align label
  data->scoreText.setPosition(segmentCenter1, topLabelY);
  renderer.Draw(data->scoreText);

  data->scoreValueText.setString(std::to_string(data->score));
  data->scoreValueText.setOrigin(data->scoreValueText.getLocalBounds().width / 2.0f, 0); // Center align value
  data->scoreValueText.setPosition(segmentCenter1, topValueY);
  renderer.Draw(data->scoreValueText);

  // COINS - Centered in the second segment
  data->coinsText.setString(sf::String(L"монеты"));
  data->coinsText.setOrigin(data->coinsText.getLocalBounds().width / 2.0f, 0); // Center align label
  data->coinsText.setPosition(segmentCenter2, topLabelY);
  renderer.Draw(data->coinsText);

  data->coinsValueText.setString(std::to_string(data->mario.coins));
  data->coinsValueText.setOrigin(data->coinsValueText.getLocalBounds().width / 2.0f, 0); // Center align value
  data->coinsValueText.setPosition(segmentCenter2, topValueY);
  renderer.Draw(data->coinsValueText);

  // LIVES - Centered in the third segment
  data->livesText.setString(sf::String(L"ЖИЗНИ"));
  data->livesText.setOrigin(data->livesText.getLocalBounds().width / 2.0f, 0); // Center align label
  data->livesText.setPosition(segmentCenter3, topLabelY);
  renderer.Draw(data->livesText);

  data->livesValueText.setString(std::to_string(data->lives));
  data->livesValueText.setOrigin(data->livesValueText.getLocalBounds().width / 2.0f, 0); // Center align value
  data->livesValueText.setPosition(segmentCenter3, topValueY);
  renderer.Draw(data->livesValueText);

  // TIME - Centered in the fourth segment
  data->timeText.setString(sf::String(L"ВРЕМЯ"));
  data->timeText.setOrigin(data->timeText.getLocalBounds().width / 2.0f, 0); // Center align label
  data->timeText.setPosition(segmentCenter4, topLabelY);
  renderer.Draw(data->timeText);

  data->timeValueText.setString(std::to_string(static_cast<int>(data->timeElapsed)));
  data->timeValueText.setOrigin(data->timeValueText.getLocalBounds().width / 2.0f, 0); // Center align value
  data->timeValueText.setPosition(segmentCenter4, topValueY);
  renderer.Draw(data->timeValueText);

  // Render language button - REMOVED (not needed with hardcoded Russian)
  // renderer.Draw(data->languageButtonText);

  // Restore original view
  renderer.target.setView(originalUIView);
}

void HandleEvent(sf::Event event) {
  // Передаем событие SFML в меню, если мы находимся в меню или на паузе
  if (data->isInMenu || (data->paused && (data->gameMenu.getCurrentState() == GameMenuState::PAUSE || data->gameMenu.getCurrentState() == GameMenuState::OPTIONS))) {
    data->gameMenu.handleInput(event); // Передаем событие SFML
    return;
  }

  // Handle language button click - REMOVED
  // if (event.type == sf::Event::MouseButtonPressed) {
  //     if (event.mouseButton.button == sf::Mouse::Left) {
  //         sf::Vector2f mousePos = data->window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), data->camera.GetUIView());
  //         if (data->languageButtonText.getGlobalBounds().contains(mousePos)) {
  //             if (data->currentLanguage == Language::ENGLISH) {
  //                 data->currentLanguage = Language::RUSSIAN;
  //             } else {
  //                 data->currentLanguage = Language::ENGLISH;
  //             }
  //             UpdateLocalizedStrings(); // Update all displayed texts
  //         }
  //     }
  // }

  // Если игра не в меню (т.е. идет геймплей или Game Over/Victory Screen)
  if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
      if (!data->paused) { // Если игра не на паузе, ставим на паузу
          data->paused = true;
          data->gameMenu.setState(GameMenuState::PAUSE); // Переходим в меню паузы
      } else if (data->isGameOver) {
          // Если игра на паузе из-за Game Over, ESC выходит из игры
          exit(0); // Временно для Game Over, позже можно добавить меню
      } else if (data->gameMenu.getCurrentState() == GameMenuState::PAUSE) {
          // Если игра на паузе и мы в меню паузы, то ESC обрабатывается пунктами меню
          // Не делаем ничего здесь, так как handleInput уже обработает
      }
  }

  // Здесь может быть дополнительная обработка событий для игрового процесса
}

void DeleteObject(Object *object) {
  delete object;
  object = nullptr;
}

void AddScore(int scoreToAdd) { data->score += scoreToAdd; }

// Helper function to get localized text - REMOVED
// sf::String GetLocalizedText(const std::string& key) {
//     if (!data) return sf::String(L"");
// 
//     if (data->currentLanguage == Language::ENGLISH) {
//         if (data->englishStrings.count(key)) {
//             return data->englishStrings[key];
//         }
//     } else { // RUSSIAN
//         if (data->russianStrings.count(key)) {
//             return data->russianStrings[key];
//         }
//     }
//     return sf::String(L"KEY_NOT_FOUND");
// }

void End() {
  data->gameMenu.saveUsers(); // Сохраняем данные пользователей при завершении
  delete data;
  Resources::clear();
}

// void UpdateLocalizedStrings() { // REMOVED
//     // Update game UI texts
//     if (data) {
//         data->coinsText.setString(GetLocalizedText("coinsText"));
//         data->scoreText.setString(GetLocalizedText("scoreText"));
//         data->livesText.setString(GetLocalizedText("livesText"));
//         data->timeText.setString(GetLocalizedText("timeText"));
//         // Update language button text based on current language
//         data->languageButtonText.setString(GetLocalizedText(data->currentLanguage == Language::ENGLISH ? "languageButtonText_EN" : "languageButtonText_RU"));
//         // Position the language button in the bottom left corner
//         data->languageButtonText.setPosition(10, data->window->getSize().y - data->languageButtonText.getLocalBounds().height - 10);
//     }
// }
