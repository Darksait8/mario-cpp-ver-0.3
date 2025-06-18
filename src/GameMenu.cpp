#include "GameMenu.h"
#include "Game.h"
#include "Resources.h" // Для доступа к ресурсам (текстуры, шрифты)
#include <iostream>
#include <fstream>
#include <algorithm>
// #include <conio.h> // Больше не нужен для SFML ввода

GameMenuItem::GameMenuItem(const sf::String& textString, sf::Font& font, std::function<void()> action)
    : action(action) {
    text.setFont(font);
    text.setString(textString);
    text.setCharacterSize(48); // Увеличиваем базовый размер шрифта
    text.setFillColor(sf::Color::White);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(1.0f);
    text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f); // Центрируем начало координат текста
}

void GameMenuItem::execute() const {
    if (action) {
        action();
    }
}

// const std::string& GameMenuItem::getText() const { // Больше не нужен
//     return text;
// }

GameMenu::GameMenu() : currentState(GameMenuState::AUTH), selectedIndex(0), gameDataRef(nullptr), currentLeaderboardDisplay(LeaderboardDisplayType::SCORE) {
    // Загрузка шрифта
    if (!font.loadFromFile("C:/Users/User/Desktop/mario-cpp-ver-0.3/resources/troika.otf")) {
        std::cerr << "Error: Failed to load font from ./resources/troika.otf" << std::endl;
    }

    // Загрузка фонового изображения
    backgroundSprite.setTexture(Resources::getTexture("mainmenu.png"));

    // Инициализация текста заголовка
    titleText.setFont(font);
    titleText.setString("REMAKE SUPER MARIO BROS.");
    titleText.setCharacterSize(64);
    titleText.setFillColor(sf::Color::Red);
    titleText.setOutlineColor(sf::Color::Yellow);
    titleText.setOutlineThickness(2.0f);

    // Инициализация текста инструкций
    instructionsText.setFont(font);
    instructionsText.setString(L"W/S - выбрать, Enter - подтвердить, ESC - вернуться");
    instructionsText.setCharacterSize(32);
    instructionsText.setFillColor(sf::Color::White);
    instructionsText.setOutlineColor(sf::Color::Black);
    instructionsText.setOutlineThickness(1.0f);

    // Инициализация элементов авторизации
    usernameLabel.setFont(font);
    usernameLabel.setString(L"Имя пользователя:");
    usernameLabel.setCharacterSize(32);
    usernameLabel.setFillColor(sf::Color::White);

    passwordLabel.setFont(font);
    passwordLabel.setString(L"пароль:");
    passwordLabel.setCharacterSize(32);
    passwordLabel.setFillColor(sf::Color::White);

    usernameInputBox.setSize(sf::Vector2f(400, 50));
    usernameInputBox.setFillColor(sf::Color(50, 50, 50, 200));
    usernameInputBox.setOutlineThickness(2);
    usernameInputBox.setOutlineColor(sf::Color::White);

    passwordInputBox.setSize(sf::Vector2f(400, 50));
    passwordInputBox.setFillColor(sf::Color(50, 50, 50, 200));
    passwordInputBox.setOutlineThickness(2);
    passwordInputBox.setOutlineColor(sf::Color::White);

    loginButtonRect.setSize(sf::Vector2f(200, 60));
    loginButtonRect.setFillColor(sf::Color(70, 70, 70, 200));
    loginButtonRect.setOutlineThickness(2);
    loginButtonRect.setOutlineColor(sf::Color::White);

    loginButtonText.setFont(font);
    loginButtonText.setString(L"войти");
    loginButtonText.setCharacterSize(36);
    loginButtonText.setFillColor(sf::Color::White);

    registerButtonRect.setSize(sf::Vector2f(200, 60));
    registerButtonRect.setFillColor(sf::Color(70, 70, 70, 200));
    registerButtonRect.setOutlineThickness(2);
    registerButtonRect.setOutlineColor(sf::Color::White);

    registerButtonText.setFont(font);
    registerButtonText.setString(L"регстрация");
    registerButtonText.setCharacterSize(32);
    registerButtonText.setFillColor(sf::Color::White);

    activeInputField = InputField::NONE;
    usernameInput = "";
    passwordInput = "";

    // Initialize messageText
    messageText.setFont(font);
    messageText.setCharacterSize(24);
    messageText.setFillColor(sf::Color::Red);
    messageText.setString("");
}

void GameMenu::initializeActions(Data* gameData) {
    gameDataRef = gameData;
    items.clear();
    addMenuItem(L"начать иру", [this]() {
        if (gameDataRef) {
            gameDataRef->isInMenu = false;
            StartNewGame();
        }
    });
    addMenuItem(L"настройки", [this]() {
        setState(GameMenuState::OPTIONS);
    });
    addMenuItem(L"таблица лидеров", [this]() {
        setState(GameMenuState::CREDITS);
    });
    addMenuItem(L"выход", []() {
        exit(0);
    });
}

void GameMenu::update() {
    // Обновление состояния меню (может быть пустым для простых меню)
}

void GameMenu::render(sf::RenderWindow& window) {
    // Сохраняем текущий вид окна
    sf::View originalView = window.getView();

    // Устанавливаем вид по умолчанию для отрисовки меню
    window.setView(window.getDefaultView());

    // Отрисовка в зависимости от текущего состояния
    switch (currentState) {
        case GameMenuState::MAIN_MENU:
            if (Resources::getTexture("mainmenu.png").getNativeHandle() != 0) {
                backgroundSprite.setTexture(Resources::getTexture("mainmenu.png"));
                sf::Vector2u textureSize = Resources::getTexture("mainmenu.png").getSize();
                sf::Vector2u windowSize = window.getSize();
                backgroundSprite.setScale(
                    static_cast<float>(windowSize.x) / textureSize.x,
                    static_cast<float>(windowSize.y) / textureSize.y
                );
                backgroundSprite.setPosition(0, 0);
                window.draw(backgroundSprite);
            }
            renderMainMenu(window);
            break;
        case GameMenuState::OPTIONS:
            if (Resources::getTexture("options.png").getNativeHandle() != 0) {
                backgroundSprite.setTexture(Resources::getTexture("options.png"));
                sf::Vector2u textureSize = Resources::getTexture("options.png").getSize();
                sf::Vector2u windowSize = window.getSize();
                backgroundSprite.setScale(
                    static_cast<float>(windowSize.x) / textureSize.x,
                    static_cast<float>(windowSize.y) / textureSize.y
                );
                backgroundSprite.setPosition(0, 0);
                window.draw(backgroundSprite);
            }
            renderOptions(window);
            break;
        case GameMenuState::CREDITS:
            if (Resources::getTexture("leaderboard.png").getNativeHandle() != 0) {
                backgroundSprite.setTexture(Resources::getTexture("leaderboard.png"));
                sf::Vector2u textureSize = Resources::getTexture("leaderboard.png").getSize();
                sf::Vector2u windowSize = window.getSize();
                backgroundSprite.setScale(
                    static_cast<float>(windowSize.x) / textureSize.x,
                    static_cast<float>(windowSize.y) / textureSize.y
                );
                backgroundSprite.setPosition(0, 0);
                window.draw(backgroundSprite);
            }
            renderCredits(window);
            break;
        case GameMenuState::AUTH:
            if (Resources::getTexture("authorization.png").getNativeHandle() != 0) {
                backgroundSprite.setTexture(Resources::getTexture("authorization.png"));
                sf::Vector2u textureSize = Resources::getTexture("authorization.png").getSize();
                sf::Vector2u windowSize = window.getSize();
                backgroundSprite.setScale(
                    static_cast<float>(windowSize.x) / textureSize.x,
                    static_cast<float>(windowSize.y) / textureSize.y
                );
                backgroundSprite.setPosition(0, 0);
                window.draw(backgroundSprite);
            }
            renderAuth(window);
            break;
        case GameMenuState::PAUSE:
            if (Resources::getTexture("pause.png").getNativeHandle() != 0) {
                backgroundSprite.setTexture(Resources::getTexture("pause.png"));
                std::cout << "Drawing pause.png background. Texture size: " << Resources::getTexture("pause.png").getSize().x << "x" << Resources::getTexture("pause.png").getSize().y << std::endl;
                sf::Vector2u textureSize = Resources::getTexture("pause.png").getSize();
                sf::Vector2u windowSize = window.getSize();
                backgroundSprite.setScale(
                    static_cast<float>(windowSize.x) / textureSize.x,
                    static_cast<float>(windowSize.y) / textureSize.y
                );
                backgroundSprite.setPosition(0, 0);
                window.draw(backgroundSprite);
            } else {
                std::cerr << "Error: pause.png texture not found in resources." << std::endl;
            }
            renderPauseMenu(window);
            break;
        case GameMenuState::VICTORY:
            if (Resources::getTexture("victory.png").getNativeHandle() != 0) {
                backgroundSprite.setTexture(Resources::getTexture("victory.png"));
                sf::Vector2u textureSize = Resources::getTexture("victory.png").getSize();
                sf::Vector2u windowSize = window.getSize();
                backgroundSprite.setScale(
                    static_cast<float>(windowSize.x) / textureSize.x,
                    static_cast<float>(windowSize.y) / textureSize.y
                );
                backgroundSprite.setPosition(0, 0);
                window.draw(backgroundSprite);
            }
            renderVictoryScreen(window);
            break;
        case GameMenuState::EXIT:
            // В случае выхода, возможно, здесь ничего не нужно рисовать,
            // или показать сообщение "Выход..."
            break;
    }

    // Восстанавливаем оригинальный вид окна
    window.setView(originalView);
}

void GameMenu::handleInput(sf::Event event) {
    // std::cout << "GameMenu::handleInput - Current State: " << static_cast<int>(currentState) << std::endl; // Отладочное сообщение

    switch (currentState) {
        case GameMenuState::MAIN_MENU:
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::W) {
                    selectedIndex = (selectedIndex > 0) ? selectedIndex - 1 : items.size() - 1;
                } else if (event.key.code == sf::Keyboard::S) {
                    selectedIndex = (selectedIndex + 1) % items.size();
                } else if (event.key.code == sf::Keyboard::Enter) {
                    items[selectedIndex].execute();
                }
            } else if (event.type == sf::Event::MouseMoved) {
                sf::Vector2f mousePos = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
                for (size_t i = 0; i < items.size(); ++i) {
                    if (items[i].text.getGlobalBounds().contains(mousePos)) {
                        selectedIndex = i;
                        break;
                    }
                }
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                    for (size_t i = 0; i < items.size(); ++i) {
                        if (items[i].text.getGlobalBounds().contains(mousePos)) {
                            items[i].execute();
                            break;
                        }
                    }
                }
            }
            break;

        case GameMenuState::AUTH:
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                if (usernameInputBox.getGlobalBounds().contains(mousePos)) {
                    activeInputField = InputField::USERNAME;
                } else if (passwordInputBox.getGlobalBounds().contains(mousePos)) {
                    activeInputField = InputField::PASSWORD;
                } else if (loginButtonRect.getGlobalBounds().contains(mousePos)) {
                    if (loginUser(usernameInput, passwordInput)) {
                        messageText.setString("Login successful!");
                        messageText.setFillColor(sf::Color::Green);
                        setState(GameMenuState::MAIN_MENU);
                    } else {
                        messageText.setString("Login failed. Invalid username or password.");
                        messageText.setFillColor(sf::Color::Red);
                    }
                    usernameInput = "";
                    passwordInput = "";
                    activeInputField = InputField::NONE;
                } else if (registerButtonRect.getGlobalBounds().contains(mousePos)) {
                    if (registerUser(usernameInput, passwordInput)) {
                        messageText.setString("Registration successful! You can now log in.");
                        messageText.setFillColor(sf::Color::Green);
                        setState(GameMenuState::MAIN_MENU);
                    } else {
                        messageText.setString("Registration failed. Try a different username or shorter password.");
                        messageText.setFillColor(sf::Color::Red);
                    }
                    usernameInput = "";
                    passwordInput = "";
                    activeInputField = InputField::NONE;
                } else {
                    activeInputField = InputField::NONE;
                    messageText.setString("");
                }
            } else if (event.type == sf::Event::TextEntered) {
                if (activeInputField != InputField::NONE) {
                    if (event.text.unicode < 128) {
                        if (event.text.unicode == '\b') {
                            if (activeInputField == InputField::USERNAME && !usernameInput.empty()) {
                                usernameInput.pop_back();
                            } else if (activeInputField == InputField::PASSWORD && !passwordInput.empty()) {
                                passwordInput.pop_back();
                            }
                        } else if (event.text.unicode != '\r' && event.text.unicode != '\n') {
                            if (activeInputField == InputField::USERNAME) {
                                usernameInput += static_cast<char>(event.text.unicode);
                            } else if (activeInputField == InputField::PASSWORD) {
                                passwordInput += static_cast<char>(event.text.unicode);
                            }
                        }
                    }
                    messageText.setString("");
                }
            } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                exit(0);
                messageText.setString("");
                usernameInput = "";
                passwordInput = "";
                activeInputField = InputField::NONE;
            }
            break;

        case GameMenuState::PAUSE:
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::W) {
                    selectedIndex = (selectedIndex > 0) ? selectedIndex - 1 : items.size() - 1;
                } else if (event.key.code == sf::Keyboard::S) {
                    selectedIndex = (selectedIndex + 1) % items.size();
                } else if (event.key.code == sf::Keyboard::Enter) {
                    items[selectedIndex].execute();
                } else if (event.key.code == sf::Keyboard::Escape) {
                    if (gameDataRef) {
                        gameDataRef->paused = false;
                        gameDataRef->music.play();
                    }
                }
            } else if (event.type == sf::Event::MouseMoved) {
                sf::Vector2f mousePos = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
                for (size_t i = 0; i < items.size(); ++i) {
                    if (items[i].text.getGlobalBounds().contains(mousePos)) {
                        selectedIndex = i;
                        break;
                    }
                }
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                    for (size_t i = 0; i < items.size(); ++i) {
                        if (items[i].text.getGlobalBounds().contains(mousePos)) {
                            items[i].execute();
                            break;
                        }
                    }
                }
            }
            break;

        case GameMenuState::OPTIONS:
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::W) {
                    if (activeSlider != OptionSliderType::NONE) {
                        activeSlider = OptionSliderType::NONE;
                    }
                    selectedIndex = (selectedIndex > 0) ? selectedIndex - 1 : items.size() - 1;
                } else if (event.key.code == sf::Keyboard::S) {
                    if (activeSlider != OptionSliderType::NONE) {
                        activeSlider = OptionSliderType::NONE;
                    }
                    selectedIndex = (selectedIndex + 1) % items.size();
                } else if (event.key.code == sf::Keyboard::Enter) {
                    if (activeSlider != OptionSliderType::NONE) {
                        // Handle slider value change here if needed
                        activeSlider = OptionSliderType::NONE; // Deactivate slider
                    } else {
                        items[selectedIndex].execute();
                    }
                } else if (event.key.code == sf::Keyboard::Left) {
                    if (activeSlider == OptionSliderType::VOLUME) {
                        gameDataRef->volume = std::max(0.0f, gameDataRef->volume - 5.0f);
                        gameDataRef->music.setVolume(gameDataRef->volume);
                    } else if (activeSlider == OptionSliderType::BRIGHTNESS) {
                        gameDataRef->brightness = std::max(0.0f, gameDataRef->brightness - 0.1f);
                    }
                } else if (event.key.code == sf::Keyboard::Right) {
                    if (activeSlider == OptionSliderType::VOLUME) {
                        gameDataRef->volume = std::min(100.0f, gameDataRef->volume + 5.0f);
                        gameDataRef->music.setVolume(gameDataRef->volume);
                    } else if (activeSlider == OptionSliderType::BRIGHTNESS) {
                        gameDataRef->brightness = std::min(1.0f, gameDataRef->brightness + 0.1f);
                    }
                } else if (event.key.code == sf::Keyboard::Escape) {
                    setState(previousState);
                }
            } else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                // Check for slider clicks first
                if (volumeTrack.getGlobalBounds().contains(mousePos)) {
                    activeSlider = OptionSliderType::VOLUME;
                } else if (brightnessTrack.getGlobalBounds().contains(mousePos)) {
                    activeSlider = OptionSliderType::BRIGHTNESS;
                } else {
                    activeSlider = OptionSliderType::NONE;
                    for (size_t i = 0; i < items.size(); ++i) {
                        if (items[i].text.getGlobalBounds().contains(mousePos)) {
                            selectedIndex = i;
                            items[selectedIndex].execute();
                            break;
                        }
                    }
                }
            } else if (event.type == sf::Event::MouseMoved && activeSlider != OptionSliderType::NONE) {
                float mouseX = event.mouseMove.x;
                if (activeSlider == OptionSliderType::VOLUME) {
                    float trackStartX = volumeTrack.getPosition().x - volumeTrack.getSize().x / 2.0f;
                    float trackEndX = volumeTrack.getPosition().x + volumeTrack.getSize().x / 2.0f;
                    float percentage = (mouseX - trackStartX) / volumeTrack.getSize().x;
                    gameDataRef->volume = std::max(0.0f, std::min(100.0f, percentage * 100.0f));
                    gameDataRef->music.setVolume(gameDataRef->volume);
                } else if (activeSlider == OptionSliderType::BRIGHTNESS) {
                    float trackStartX = brightnessTrack.getPosition().x - brightnessTrack.getSize().x / 2.0f;
                    float trackEndX = brightnessTrack.getPosition().x + brightnessTrack.getSize().x / 2.0f;
                    float percentage = (mouseX - trackStartX) / brightnessTrack.getSize().x;
                    gameDataRef->brightness = std::max(0.0f, std::min(1.0f, percentage));
                }
            } else if (event.type == sf::Event::MouseButtonReleased) {
                activeSlider = OptionSliderType::NONE;
            }
            break;

        case GameMenuState::CREDITS:
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::W) {
                    currentLeaderboardDisplay = static_cast<LeaderboardDisplayType>((static_cast<int>(currentLeaderboardDisplay) + 2) % 3);
                } else if (event.key.code == sf::Keyboard::S) {
                    currentLeaderboardDisplay = static_cast<LeaderboardDisplayType>((static_cast<int>(currentLeaderboardDisplay) + 1) % 3);
                } else if (event.key.code == sf::Keyboard::Escape) {
                    setState(GameMenuState::MAIN_MENU);
                }
            } else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                // Check for clicks on leaderboard categories
                if (scoreLeaderboardRect.getGlobalBounds().contains(mousePos)) {
                    currentLeaderboardDisplay = LeaderboardDisplayType::SCORE;
                } else if (timeLeaderboardRect.getGlobalBounds().contains(mousePos)) {
                    currentLeaderboardDisplay = LeaderboardDisplayType::FASTEST_TIME;
                } else if (coinsLeaderboardRect.getGlobalBounds().contains(mousePos)) {
                    currentLeaderboardDisplay = LeaderboardDisplayType::MOST_COINS;
                }
            }
            break;

        case GameMenuState::VICTORY:
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) {
                    std::cout << "DEBUG: Victory Screen - Enter pressed. loggedInUsername: " << loggedInUsername << std::endl;
                    std::cout << "DEBUG: Current game score: " << gameDataRef->score << ", coins: " << gameDataRef->mario.coins << std::endl;
                    // Before starting a new game, save the current user's score
                    if (gameDataRef && loggedInUsername != "") {
                        for (auto& user : users) {
                            if (user.username == loggedInUsername) {
                                std::cout << "DEBUG: Found user " << user.username << ". Old score: " << user.score << ", old coins: " << user.coins << std::endl;
                                // Update score only if current score is higher
                                if (gameDataRef->score > user.score) {
                                    user.score = gameDataRef->score;
                                    std::cout << "DEBUG: Updated user score to: " << user.score << std::endl;
                                }
                                // Update coins (assuming coins are cumulative or highest achieved)
                                // For now, let's just update if current coins are higher
                                if (gameDataRef->mario.coins > user.coins) {
                                    user.coins = gameDataRef->mario.coins;
                                    std::cout << "DEBUG: Updated user coins to: " << user.coins << std::endl;
                                }
                                // Update fastestTime if current time is lower and not 0 (0 means not set yet)
                                if (gameDataRef->timeElapsed > 0 && (user.fastestTime == 0 || gameDataRef->timeElapsed < user.fastestTime)) {
                                    user.fastestTime = gameDataRef->timeElapsed;
                                    std::cout << "DEBUG: Updated user fastestTime to: " << user.fastestTime << std::endl;
                                }
                                // Update mostCoins if current coins are higher
                                if (gameDataRef->mario.coins > user.mostCoins) {
                                    user.mostCoins = gameDataRef->mario.coins;
                                    std::cout << "DEBUG: Updated user mostCoins to: " << user.mostCoins << std::endl;
                                }
                                break;
                            }
                        }
                        saveUsers(); // Save updated users to file
                    }
                    // Do NOT call StartNewGame() here, as it resets the entire game. Instead, transition to main menu.
                    gameDataRef->isInMenu = true; // Ensure we are in menu mode
                    setState(GameMenuState::MAIN_MENU); // Transition to Main Menu state
                    // Reset game flags relevant to victory/game state
                    gameDataRef->isGameOver = false; 
                    gameDataRef->isVictory = false;
                    gameDataRef->map.victorySequenceStarted = false;
                    gameDataRef->mario.inVictorySequence = false;
                    gameDataRef->mario.isSlidingDownFlag = false;
                    gameDataRef->mario.isVictoryDancing = false;
                    gameDataRef->mario.victoryWalkComplete = false;
                    gameDataRef->startVictoryScreenTimer = false;
                    gameDataRef->hasStartedWalking = false; 
                    gameDataRef->walkingStartTime = 0.0f; 
                } else if (event.key.code == sf::Keyboard::Escape) {
                    std::cout << "DEBUG: Victory Screen - Escape pressed. loggedInUsername: " << loggedInUsername << std::endl;
                    std::cout << "DEBUG: Current game score: " << gameDataRef->score << ", coins: " << gameDataRef->mario.coins << std::endl;
                    // Before returning to main menu, save the current user's score
                    if (gameDataRef && loggedInUsername != "") {
                        for (auto& user : users) {
                            if (user.username == loggedInUsername) {
                                std::cout << "DEBUG: Found user " << user.username << ". Old score: " << user.score << ", old coins: " << user.coins << std::endl;
                                if (gameDataRef->score > user.score) {
                                    user.score = gameDataRef->score;
                                    std::cout << "DEBUG: Updated user score to: " << user.score << std::endl;
                                }
                                if (gameDataRef->mario.coins > user.coins) {
                                    user.coins = gameDataRef->mario.coins;
                                    std::cout << "DEBUG: Updated user coins to: " << user.coins << std::endl;
                                }
                                // Update fastestTime if current time is lower and not 0
                                if (gameDataRef->timeElapsed > 0 && (user.fastestTime == 0 || gameDataRef->timeElapsed < user.fastestTime)) {
                                    user.fastestTime = gameDataRef->timeElapsed;
                                    std::cout << "DEBUG: Updated user fastestTime to: " << user.fastestTime << std::endl;
                                }
                                // Update mostCoins if current coins are higher
                                if (gameDataRef->mario.coins > user.mostCoins) {
                                    user.mostCoins = gameDataRef->mario.coins;
                                    std::cout << "DEBUG: Updated user mostCoins to: " << user.mostCoins << std::endl;
                                }
                                break;
                            }
                        }
                        saveUsers(); // Save updated users to file
                    }
                    // Do NOT call StartNewGame() here, as it resets the entire game. Instead, transition to main menu.
                    gameDataRef->isInMenu = true; // Ensure we are in menu mode
                    setState(GameMenuState::MAIN_MENU); // Transition to Main Menu state
                    // Reset game flags relevant to victory/game state
                    gameDataRef->isGameOver = false; 
                    gameDataRef->isVictory = false;
                    gameDataRef->map.victorySequenceStarted = false;
                    gameDataRef->mario.inVictorySequence = false;
                    gameDataRef->mario.isSlidingDownFlag = false;
                    gameDataRef->mario.isVictoryDancing = false;
                    gameDataRef->mario.victoryWalkComplete = false;
                    gameDataRef->startVictoryScreenTimer = false;
                    gameDataRef->hasStartedWalking = false; 
                    gameDataRef->walkingStartTime = 0.0f; 
                }
            }
            break;

        default:
            // No specific input handling for other states like EXIT
            break;
    }
}

void GameMenu::addMenuItem(const sf::String& textString, std::function<void()> action) {
    items.emplace_back(textString, font, action);
    // Выравнивание по центру, предполагая, что 800 - это ширина окна
    float xPos = 800 / 2.0f;
    float yPos = 200.0f + items.size() * 60.0f;
    items.back().text.setPosition(xPos, yPos);
}

void GameMenu::setState(GameMenuState newState) {
    previousState = currentState; // Сохраняем текущее состояние как предыдущее
    currentState = newState;
    setupMenuForState(newState); // Call setupMenuForState after changing state
    // selectedIndex = 0; // This is now handled by setupMenuForState
}

GameMenuState GameMenu::getCurrentState() const {
    return currentState;
}

void GameMenu::renderMainMenu(sf::RenderWindow& window) {
    sf::Vector2f viewCenter = window.getView().getCenter(); // Declare viewCenter here
    sf::Vector2f viewSize = window.getView().getSize(); // Also ensure viewSize is declared if used

    // Отрисовка фонового изображения
    backgroundSprite.setScale(
        viewSize.x / backgroundSprite.getTexture()->getSize().x,
        viewSize.y / backgroundSprite.getTexture()->getSize().y
    );
    backgroundSprite.setPosition(viewCenter.x - viewSize.x / 2, viewCenter.y - viewSize.y / 2); // Позиционируем фон на весь экран
    window.draw(backgroundSprite);

    float startY = viewCenter.y - 60; // Изменяем начальную позицию для пунктов меню, чтобы учесть отсутствие заголовка и большее расстояние
    float lineHeight = 60.0f; // Расстояние между пунктами

    for (size_t i = 0; i < items.size(); ++i) {
        items[i].text.setFillColor(sf::Color::White); // Сброс цвета для невыбранных
        if (i == selectedIndex) {
            items[i].text.setFillColor(sf::Color::Yellow); // Выделение выбранного пункта
        }
        items[i].text.setOrigin(items[i].text.getLocalBounds().width / 2.0f, items[i].text.getLocalBounds().height / 2.0f);
        items[i].text.setPosition(viewCenter.x, startY + i * lineHeight);
        window.draw(items[i].text);
    }

    // Позиционируем инструкции
    instructionsText.setOrigin(instructionsText.getLocalBounds().width / 2.0f, instructionsText.getLocalBounds().height / 2.0f);
    instructionsText.setPosition(viewCenter.x, viewCenter.y + viewSize.y / 2.5f); // Изменяем смещение для инструкций, учитывая увеличенное меню
    window.draw(instructionsText);
}

void GameMenu::renderOptions(sf::RenderWindow& window) {
    sf::Vector2f viewCenter = window.getView().getCenter();
    sf::Vector2f viewSize = window.getView().getSize();

    // Затемняющий фон для регулировки яркости
    sf::RectangleShape dimmer(sf::Vector2f(viewSize.x, viewSize.y));
    dimmer.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>((1.0f - gameDataRef->brightness) * 255)));
    dimmer.setPosition(viewCenter.x - viewSize.x / 2, viewCenter.y - viewSize.y / 2);
    window.draw(dimmer);

    // --- Ползунок Яркость ---
    sf::Text brightnessLabel(L"Яркость", font, 32);
    brightnessLabel.setFillColor(activeSlider == OptionSliderType::BRIGHTNESS ? sf::Color::Yellow : sf::Color::White);
    brightnessLabel.setOutlineColor(sf::Color::Black);
    brightnessLabel.setOutlineThickness(2.0f);
    brightnessLabel.setOrigin(brightnessLabel.getLocalBounds().width / 2.0f, brightnessLabel.getLocalBounds().height / 2.0f);
    brightnessLabel.setPosition(viewCenter.x, viewCenter.y - 60);

    sf::RectangleShape brightnessTrack(sf::Vector2f(300, 8));
    brightnessTrack.setFillColor(sf::Color(100, 100, 100));
    brightnessTrack.setOrigin(brightnessTrack.getSize().x / 2, brightnessTrack.getSize().y / 2);
    brightnessTrack.setPosition(viewCenter.x, viewCenter.y - 20);

    sf::CircleShape brightnessThumb(12);
    brightnessThumb.setFillColor(sf::Color::White);
    float bx = viewCenter.x - 150 + 300 * gameDataRef->brightness;
    brightnessThumb.setPosition(bx - 12, viewCenter.y - 32);

    // --- Ползунок Звук ---
    sf::Text volumeLabel(L"Звук", font, 32);
    volumeLabel.setFillColor(activeSlider == OptionSliderType::VOLUME ? sf::Color::Yellow : sf::Color::White);
    volumeLabel.setOutlineColor(sf::Color::Black);
    volumeLabel.setOutlineThickness(2.0f);
    volumeLabel.setOrigin(volumeLabel.getLocalBounds().width / 2.0f, volumeLabel.getLocalBounds().height / 2.0f);
    volumeLabel.setPosition(viewCenter.x, viewCenter.y + 40);

    sf::RectangleShape volumeTrack(sf::Vector2f(300, 8));
    volumeTrack.setFillColor(sf::Color(100, 100, 100));
    volumeTrack.setOrigin(volumeTrack.getSize().x / 2, volumeTrack.getSize().y / 2);
    volumeTrack.setPosition(viewCenter.x, viewCenter.y + 80);

    sf::CircleShape volumeThumb(12);
    volumeThumb.setFillColor(sf::Color::White);
    float vx = viewCenter.x - 150 + 300 * (gameDataRef->volume / 100.0f);
    volumeThumb.setPosition(vx - 12, viewCenter.y + 68);

    // Рисуем всё
    window.draw(brightnessLabel);
    window.draw(brightnessTrack);
    window.draw(brightnessThumb);
    window.draw(volumeLabel);
    window.draw(volumeTrack);
    window.draw(volumeThumb);
}

void GameMenu::renderCredits(sf::RenderWindow& window) {
    sf::Vector2f viewCenter = window.getView().getCenter();
    sf::Vector2f viewSize = window.getView().getSize();

    // Инициализация прямоугольников для заголовков таблицы лидеров
    float leaderboardRectWidth = 300.0f;
    float leaderboardRectHeight = 50.0f;
    float leaderboardRectSpacing = 20.0f;
    float startX = viewCenter.x - (leaderboardRectWidth * 1.5f + leaderboardRectSpacing); // Adjust starting X for 3 columns

    scoreLeaderboardRect.setSize(sf::Vector2f(leaderboardRectWidth, leaderboardRectHeight));
    scoreLeaderboardRect.setPosition(startX, viewCenter.y - viewSize.y / 3.0f + 100); // Примерное положение
    timeLeaderboardRect.setSize(sf::Vector2f(leaderboardRectWidth, leaderboardRectHeight));
    timeLeaderboardRect.setPosition(startX + leaderboardRectWidth + leaderboardRectSpacing, viewCenter.y - viewSize.y / 3.0f + 100);
    coinsLeaderboardRect.setSize(sf::Vector2f(leaderboardRectWidth, leaderboardRectHeight));
    coinsLeaderboardRect.setPosition(startX + (leaderboardRectWidth + leaderboardRectSpacing) * 2, viewCenter.y - viewSize.y / 3.0f + 100);

    // Отрисовка фонового изображения
    backgroundSprite.setScale(
        viewSize.x / backgroundSprite.getTexture()->getSize().x,
        viewSize.y / backgroundSprite.getTexture()->getSize().y
    );
    backgroundSprite.setPosition(viewCenter.x - viewSize.x / 2, viewCenter.y - viewSize.y / 2);
    window.draw(backgroundSprite);

    // Подписи категорий
    sf::Text scoreLabel(L"очки", font, 28);
    sf::Text timeLabel(L"время", font, 28);
    sf::Text coinsLabel(L"монеты", font, 28);
    scoreLabel.setFillColor(currentLeaderboardDisplay == LeaderboardDisplayType::SCORE ? sf::Color::Yellow : sf::Color::White);
    timeLabel.setFillColor(currentLeaderboardDisplay == LeaderboardDisplayType::FASTEST_TIME ? sf::Color::Yellow : sf::Color::White);
    coinsLabel.setFillColor(currentLeaderboardDisplay == LeaderboardDisplayType::MOST_COINS ? sf::Color::Yellow : sf::Color::White);
    scoreLabel.setOutlineColor(sf::Color::Black);
    timeLabel.setOutlineColor(sf::Color::Black);
    coinsLabel.setOutlineColor(sf::Color::Black);
    scoreLabel.setOutlineThickness(2.0f);
    timeLabel.setOutlineThickness(2.0f);
    coinsLabel.setOutlineThickness(2.0f);
    scoreLabel.setOrigin(scoreLabel.getLocalBounds().width / 2.0f, scoreLabel.getLocalBounds().height / 2.0f);
    timeLabel.setOrigin(timeLabel.getLocalBounds().width / 2.0f, timeLabel.getLocalBounds().height / 2.0f);
    coinsLabel.setOrigin(coinsLabel.getLocalBounds().width / 2.0f, coinsLabel.getLocalBounds().height / 2.0f);
    scoreLabel.setPosition(scoreLeaderboardRect.getPosition().x + scoreLeaderboardRect.getSize().x / 2, scoreLeaderboardRect.getPosition().y + scoreLeaderboardRect.getSize().y / 2);
    timeLabel.setPosition(timeLeaderboardRect.getPosition().x + timeLeaderboardRect.getSize().x / 2, timeLeaderboardRect.getPosition().y + timeLeaderboardRect.getSize().y / 2);
    coinsLabel.setPosition(coinsLeaderboardRect.getPosition().x + coinsLeaderboardRect.getSize().x / 2, coinsLeaderboardRect.getPosition().y + coinsLeaderboardRect.getSize().y / 2);

    // Надпись о текущем пользователе
    if (!loggedInUsername.empty() && gameDataRef) {
        sf::Text currentUserScoreText;
        currentUserScoreText.setFont(font);
        sf::String currentUserDisplay = sf::String(L"выбранный пользователь: ") + sf::String(loggedInUsername) + sf::String(L" — ");
        switch (currentLeaderboardDisplay) {
            case LeaderboardDisplayType::SCORE:
                currentUserDisplay += sf::String(std::to_string(gameDataRef->score));
                break;
            case LeaderboardDisplayType::FASTEST_TIME:
                currentUserDisplay += sf::String(std::to_string(static_cast<int>(gameDataRef->timeElapsed))) + sf::String(L"s");
                break;
            case LeaderboardDisplayType::MOST_COINS:
                currentUserDisplay += sf::String(std::to_string(gameDataRef->mario.coins));
                break;
        }
        currentUserScoreText.setString(currentUserDisplay);
        currentUserScoreText.setCharacterSize(30);
        currentUserScoreText.setFillColor(sf::Color::Yellow);
        currentUserScoreText.setOutlineColor(sf::Color::Black);
        currentUserScoreText.setOutlineThickness(1.0f);
        currentUserScoreText.setOrigin(currentUserScoreText.getLocalBounds().width / 2.0f, currentUserScoreText.getLocalBounds().height / 2.0f);
        currentUserScoreText.setPosition(viewCenter.x, viewCenter.y - 40);
        window.draw(currentUserScoreText);
    }

    // Надпись "Топ игроков:"
    sf::Text topLabel(L"лучшие игроки:", font, 26);
    topLabel.setFillColor(sf::Color::White);
    topLabel.setOutlineColor(sf::Color::Black);
    topLabel.setOutlineThickness(1.0f);
    topLabel.setOrigin(topLabel.getLocalBounds().width / 2.0f, topLabel.getLocalBounds().height / 2.0f);
    topLabel.setPosition(viewCenter.x, viewCenter.y + 10);
    window.draw(topLabel);

    // Сортировка и вывод топа (без дублирования текущего пользователя)
    std::vector<User> sortedUsers = users;
    std::sort(sortedUsers.begin(), sortedUsers.end(), [this](const User& a, const User& b) {
        if (currentLeaderboardDisplay == LeaderboardDisplayType::SCORE) {
            return a.score > b.score;
        } else if (currentLeaderboardDisplay == LeaderboardDisplayType::FASTEST_TIME) {
            if (a.fastestTime == 0) return false;
            if (b.fastestTime == 0) return true;
            return a.fastestTime < b.fastestTime;
        } else {
            return a.mostCoins > b.mostCoins;
        }
    });

    size_t shown = 0;
    float userListStartY = viewCenter.y + 50;
    float lineHeight = 40.0f;
    for (size_t i = 0; i < sortedUsers.size() && shown < 10; ++i) {
        if (!loggedInUsername.empty() && sortedUsers[i].username == loggedInUsername) continue; // не дублируем
        sf::Text userEntryText;
        userEntryText.setFont(font);
        sf::String entryString = sf::String(std::to_string(shown + 1)) + sf::String(L". ") + sf::String(sortedUsers[i].username) + sf::String(L": ");
        switch (currentLeaderboardDisplay) {
            case LeaderboardDisplayType::SCORE:
                entryString += sf::String(std::to_string(sortedUsers[i].score));
                break;
            case LeaderboardDisplayType::FASTEST_TIME:
                entryString += sf::String(std::to_string(static_cast<int>(sortedUsers[i].fastestTime))) + sf::String(L"s");
                break;
            case LeaderboardDisplayType::MOST_COINS:
                entryString += sf::String(std::to_string(sortedUsers[i].mostCoins));
                break;
        }
        userEntryText.setString(entryString);
        userEntryText.setCharacterSize(24);
        userEntryText.setFillColor(sf::Color::White);
        userEntryText.setOutlineColor(sf::Color::Black);
        userEntryText.setOutlineThickness(1.0f);
        userEntryText.setOrigin(userEntryText.getLocalBounds().width / 2.0f, userEntryText.getLocalBounds().height / 2.0f);
        userEntryText.setPosition(viewCenter.x, userListStartY + shown * lineHeight);
        window.draw(userEntryText);
        ++shown;
    }

    // Render clickable rectangles for leaderboard categories
    scoreLeaderboardRect.setFillColor(currentLeaderboardDisplay == LeaderboardDisplayType::SCORE ? sf::Color(100, 100, 250, 150) : sf::Color(50, 50, 150, 150));
    timeLeaderboardRect.setFillColor(currentLeaderboardDisplay == LeaderboardDisplayType::FASTEST_TIME ? sf::Color(100, 100, 250, 150) : sf::Color(50, 50, 150, 150));
    coinsLeaderboardRect.setFillColor(currentLeaderboardDisplay == LeaderboardDisplayType::MOST_COINS ? sf::Color(100, 100, 250, 150) : sf::Color(50, 50, 150, 150));
    window.draw(scoreLeaderboardRect);
    window.draw(timeLeaderboardRect);
    window.draw(coinsLeaderboardRect);
    window.draw(scoreLabel);
    window.draw(timeLabel);
    window.draw(coinsLabel);
}

void GameMenu::renderAuth(sf::RenderWindow& window) {
    sf::Vector2f viewCenter = window.getView().getCenter();
    sf::Vector2f viewSize = window.getView().getSize();

    // Отрисовка фонового изображения
    backgroundSprite.setScale(
        viewSize.x / backgroundSprite.getTexture()->getSize().x,
        viewSize.y / backgroundSprite.getTexture()->getSize().y
    );
    backgroundSprite.setPosition(viewCenter.x - viewSize.x / 2, viewCenter.y - viewSize.y / 2); // Позиционируем фон на весь экран
    window.draw(backgroundSprite);

    // Позиционирование заголовка

    // Позиционирование полей ввода
    float inputWidth = 400;
    float inputHeight = 50;
    float inputSpacing = 20;

    usernameInputBox.setPosition(viewCenter.x - inputWidth / 2, viewCenter.y - inputHeight - inputSpacing / 2);
    passwordInputBox.setPosition(viewCenter.x - inputWidth / 2, viewCenter.y + inputSpacing / 2);

    usernameLabel.setOrigin(usernameLabel.getLocalBounds().width / 2.0f, usernameLabel.getLocalBounds().height / 2.0f);
    usernameLabel.setPosition(usernameInputBox.getPosition().x - usernameLabel.getLocalBounds().width / 2.0f - 100, usernameInputBox.getPosition().y + usernameInputBox.getSize().y / 2.0f - usernameLabel.getLocalBounds().height / 2.0f);

    passwordLabel.setOrigin(passwordLabel.getLocalBounds().width / 2.0f, passwordLabel.getLocalBounds().height / 2.0f);
    passwordLabel.setPosition(passwordInputBox.getPosition().x - passwordLabel.getLocalBounds().width / 2.0f - 100, passwordInputBox.getPosition().y + passwordInputBox.getSize().y / 2.0f - passwordLabel.getLocalBounds().height / 2.0f);

    // Позиционирование текста в полях ввода
    sf::Text usernameText(usernameInput, font, 30);
    usernameText.setFillColor(sf::Color::White);
    usernameText.setPosition(usernameInputBox.getPosition().x + 10, usernameInputBox.getPosition().y + 10);

    sf::Text passwordText(std::string(passwordInput.length(), '*'), font, 30); // Show asterisks for password
    passwordText.setFillColor(sf::Color::White);
    passwordText.setPosition(passwordInputBox.getPosition().x + 10, passwordInputBox.getPosition().y + 10);

    // Позиционирование кнопок
    float buttonWidth = 200;
    float buttonHeight = 60;
    float buttonSpacing = 20;

    loginButtonRect.setPosition(viewCenter.x - buttonWidth - buttonSpacing / 2, viewCenter.y + inputHeight + inputSpacing + buttonSpacing);
    registerButtonRect.setPosition(viewCenter.x + buttonSpacing / 2, viewCenter.y + inputHeight + inputSpacing + buttonSpacing);

    loginButtonText.setOrigin(loginButtonText.getLocalBounds().width / 2.0f, loginButtonText.getLocalBounds().height / 2.0f);
    loginButtonText.setPosition(loginButtonRect.getPosition().x + loginButtonRect.getSize().x / 2.0f, loginButtonRect.getPosition().y + loginButtonRect.getSize().y / 2.0f);

    registerButtonText.setOrigin(registerButtonText.getLocalBounds().width / 2.0f, registerButtonText.getLocalBounds().height / 2.0f);
    registerButtonText.setPosition(registerButtonRect.getPosition().x + registerButtonRect.getSize().x / 2.0f, registerButtonRect.getPosition().y + registerButtonRect.getSize().y / 2.0f);

    // Позиционирование сообщения
    messageText.setOrigin(messageText.getLocalBounds().width / 2.0f, messageText.getLocalBounds().height / 2.0f);
    messageText.setPosition(viewCenter.x, registerButtonRect.getPosition().y + registerButtonRect.getSize().y + 50); // Below buttons

    // Отрисовка элементов
    window.draw(usernameInputBox);
    window.draw(passwordInputBox);
    window.draw(usernameLabel);
    window.draw(passwordLabel);
    window.draw(usernameText);
    window.draw(passwordText);
    window.draw(loginButtonRect);
    window.draw(loginButtonText);
    window.draw(registerButtonRect);
    window.draw(registerButtonText);
    window.draw(messageText);

    // Highlight active input field
    if (activeInputField == InputField::USERNAME) {
        usernameInputBox.setOutlineColor(sf::Color::Yellow);
        passwordInputBox.setOutlineColor(sf::Color::White);
    } else if (activeInputField == InputField::PASSWORD) {
        usernameInputBox.setOutlineColor(sf::Color::White);
        passwordInputBox.setOutlineColor(sf::Color::Yellow);
    } else {
        usernameInputBox.setOutlineColor(sf::Color::White);
        passwordInputBox.setOutlineColor(sf::Color::White);
    }

    // Highlight selected button (if no input field is active)
    if (activeInputField == InputField::NONE) {
        if (selectedIndex == 0) { // Login button
            loginButtonRect.setOutlineColor(sf::Color::Yellow);
            registerButtonRect.setOutlineColor(sf::Color::White);
        } else if (selectedIndex == 1) { // Register button
            loginButtonRect.setOutlineColor(sf::Color::White);
            registerButtonRect.setOutlineColor(sf::Color::Yellow);
        }
    }

}

void GameMenu::renderPauseMenu(sf::RenderWindow& window) {
    sf::Vector2f viewCenter = window.getView().getCenter(); // Declare viewCenter here
    sf::Vector2f viewSize = window.getView().getSize(); // Also ensure viewSize is declared if used

    // Отрисовка затемняющего фона - это должен делать Game::Render()
    // Render options title


    float startY = viewCenter.y - 50;
    float lineHeight = 60.0f;

    for (size_t i = 0; i < items.size(); ++i) {
        items[i].text.setFillColor(sf::Color::White); // Сброс цвета для невыбранных
        if (i == selectedIndex) {
            items[i].text.setFillColor(sf::Color::Yellow); // Выделение выбранного пункта
        }
        items[i].text.setOrigin(items[i].text.getLocalBounds().width / 2.0f, items[i].text.getLocalBounds().height / 2.0f);
        items[i].text.setPosition(viewCenter.x, startY + i * lineHeight);
        window.draw(items[i].text);
    }
}

void GameMenu::renderVictoryScreen(sf::RenderWindow& window) {
    sf::Vector2f viewCenter = window.getView().getCenter();
    sf::Vector2f viewSize = window.getView().getSize();

    // Отрисовка фонового изображения
    backgroundSprite.setScale(
        viewSize.x / backgroundSprite.getTexture()->getSize().x,
        viewSize.y / backgroundSprite.getTexture()->getSize().y
    );
    backgroundSprite.setPosition(viewCenter.x - viewSize.x / 2, viewCenter.y - viewSize.y / 2); // Позиционируем фон на весь экран
    window.draw(backgroundSprite);


    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(40);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setString(sf::String(L"очки: ") + std::to_string(gameDataRef->score));
    scoreText.setOrigin(scoreText.getLocalBounds().width / 2.0f, scoreText.getLocalBounds().height / 2.0f);
    scoreText.setPosition(viewCenter.x, viewCenter.y - 100); // Below title
    window.draw(scoreText);

    sf::Text coinsText;
    coinsText.setFont(font);
    coinsText.setCharacterSize(40);
    coinsText.setFillColor(sf::Color::White);
    coinsText.setString(sf::String(L"монеты: ") + std::to_string(gameDataRef->mario.coins));
    coinsText.setOrigin(coinsText.getLocalBounds().width / 2.0f, coinsText.getLocalBounds().height / 2.0f);
    coinsText.setPosition(viewCenter.x, viewCenter.y - 50); // Below score
    window.draw(coinsText);

    sf::Text timeText;
    timeText.setFont(font);
    timeText.setCharacterSize(40);
    timeText.setFillColor(sf::Color::White);
    timeText.setString(sf::String(L"время: ") + std::to_string(static_cast<int>(gameDataRef->timeElapsed)) + L"s");
    timeText.setOrigin(timeText.getLocalBounds().width / 2.0f, timeText.getLocalBounds().height / 2.0f);
    timeText.setPosition(viewCenter.x, viewCenter.y); // Below coins
    window.draw(timeText);

    // Instructions text
    instructionsText.setString(L"нажмите ENTER чтобы продолжить");
    instructionsText.setOrigin(instructionsText.getLocalBounds().width / 2.0f, instructionsText.getLocalBounds().height / 2.0f);
    instructionsText.setPosition(viewCenter.x, viewCenter.y + 150); // Below stats
    window.draw(instructionsText);
}

// User management functions implementation
std::string GameMenu::hashPassword(const std::string& password) {
    std::hash<std::string> hasher;
    size_t hashValue = hasher(password);
    return std::to_string(hashValue);
}

bool GameMenu::registerUser(const std::string& username, const std::string& password) {
    // Check if username already exists
    for (const auto& user : users) {
        if (user.username == username) {
            std::cerr << "Error: Username already exists!" << std::endl;
            return false;
        }
    }

    if (username.length() >= 50 || password.length() >= 50) {
        std::cerr << "Error: Username or password too long! Max 49 characters." << std::endl;
        return false;
    }

    User newUser;
    std::strncpy(newUser.username, username.c_str(), sizeof(newUser.username) - 1);
    std::string hashedPassword = hashPassword(password);
    std::strncpy(newUser.passwordHash, hashedPassword.c_str(), sizeof(newUser.passwordHash) - 1);
    newUser.score = 0; // Initial score for new user

    users.push_back(newUser);
    saveUsers();
    std::cout << "User " << username << " registered successfully!" << std::endl;
    return true;
}

bool GameMenu::loginUser(const std::string& username, const std::string& password) {
    std::string hashedPassword = hashPassword(password);
    for (const auto& user : users) {
        if (user.username == username && user.passwordHash == hashedPassword) {
            loggedInUsername = username;
            std::cout << "User " << username << " logged in successfully!" << std::endl;
            return true;
        }
    }
    std::cerr << "Error: Invalid username or password!" << std::endl;
    return false;
}

void GameMenu::loadUsers() {
    std::cout << "DEBUG: Attempting to load users from users.bin..." << std::endl;
    std::ifstream file("users.bin", std::ios::binary | std::ios::in);
    if (file.is_open()) {
        users.clear();
        User user;
        int loadedUserCount = 0;
        while (file.read(reinterpret_cast<char*>(&user), sizeof(User))) {
            users.push_back(user);
            loadedUserCount++;
        }
        file.close();
        std::cout << "DEBUG: Successfully loaded " << loadedUserCount << " users from users.bin" << std::endl;
    } else {
        std::cerr << "WARNING: users.bin not found or could not be opened for reading. Starting with no users." << std::endl;
    }
}

void GameMenu::saveUsers() {
    std::cout << "DEBUG: Attempting to save " << users.size() << " users to users.bin..." << std::endl;
    std::ofstream file("users.bin", std::ios::binary | std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        for (const auto& user : users) {
            file.write(reinterpret_cast<const char*>(&user), sizeof(User));
        }
        file.close();
        std::cout << "DEBUG: Successfully saved users to users.bin" << std::endl;
    } else {
        std::cerr << "ERROR: Could not save users to users.bin" << std::endl;
    }
}

void GameMenu::initializePauseMenuActions() {
    items.clear();
    addMenuItem(L"продолжить", [this]() {
        gameDataRef->paused = false;
        gameDataRef->music.play();
    });
    addMenuItem(L"настройки", [this]() {
        setState(GameMenuState::OPTIONS);
    });
    addMenuItem(L"выйти в главное меню", [this]() {
        gameDataRef->paused = false;
        gameDataRef->isInMenu = true;
        gameDataRef->currentLevel = 0; // Reset level
        gameDataRef->music.stop();
        gameDataRef->gameMenu.setState(GameMenuState::MAIN_MENU); // Set menu state to main menu
        gameDataRef->gameMenu.initializeActions(gameDataRef); // Reinitialize main menu items
    });
    addMenuItem(L"выход", []() {
        exit(0);
    });
    selectedIndex = 0; // Reset selected index
}

void GameMenu::setupMenuForState(GameMenuState newState) {
    items.clear(); // Clear existing menu items
    currentState = newState; // Set the new state
    switch (newState) {
        case GameMenuState::MAIN_MENU:
            initializeActions(gameDataRef); // Call the existing initialization for main menu
            break;
        case GameMenuState::PAUSE:
            initializePauseMenuActions(); // Setup items for pause menu
            break;
        case GameMenuState::OPTIONS:
            initializeOptionsActions(); // Setup items for options menu
            break;
        case GameMenuState::AUTH:
            // Authorization menu items are handled differently and don't use addMenuItem
            // No explicit menu items here, handled by input fields and buttons
            break;
        case GameMenuState::VICTORY:
            items.clear();
            addMenuItem(L"играть снова", [this]() {
                gameDataRef->currentLevel = 0; // Reset level to 0
                gameDataRef->isInMenu = false; // Exit menu
                gameDataRef->music.play();
            });
            addMenuItem(L"главное меню", [this]() {
                gameDataRef->currentLevel = 0; // Reset level to 0
                gameDataRef->isInMenu = true; // Stay in menu
                gameDataRef->music.stop();
                gameDataRef->gameMenu.setState(GameMenuState::MAIN_MENU);
                gameDataRef->gameMenu.initializeActions(gameDataRef);
            });
            break;
        default:
            // Handle other states or do nothing
            break;
    }
}

void GameMenu::initializeOptionsActions() {
    // std::cout << "GameMenu::initializeOptionsActions called." << std::endl; // Отладочное сообщение
    if (!gameDataRef) {
        std::cerr << "Error: gameDataRef is null in initializeOptionsActions." << std::endl;
        return;
    }
    items.clear();

    // Initialize Menu Items for navigation (Volume, Brightness, Back)
    addMenuItem(std::string("Music: ") + (gameDataRef->isMusicOn ? "ON" : "OFF"), [this]() {
        // Toggle music here, update the label (volumeLabel)
        if (gameDataRef->music.getStatus() == sf::Music::Playing) {
            gameDataRef->music.pause();
            volumeLabel.setString("Music: OFF");
        } else {
            gameDataRef->music.play();
            volumeLabel.setString("Music: ON");
        }
    });
    addMenuItem("Change Volume (NYI)", [this]() {
        activeSlider = OptionSliderType::VOLUME; // Change this to VOLUME slider, as it's the first option
    });
    // Remove the Brightness menu item, as the brightness slider is always active below
    // addMenuItem(GetLocalizedMenuText("brightnessOption"), [this]() {
    //     activeSlider = OptionSliderType::BRIGHTNESS;
    // });
    addMenuItem("Back", [this]() {
        activeSlider = OptionSliderType::NONE; // Deactivate slider when going back
        setState(previousState); // Return to previous state
    });

    // Initialize Slider UI elements (These will be drawn in renderOptions)
    float centerX = 600.0f; // Assuming 1200x900 window
    float startY = 400.0f; // Moved down from 300.0f
    float sliderWidth = 400.0f;
    float sliderHeight = 20.0f;
    float sliderSpacing = 80.0f;

    // Volume Slider initialization
    volumeLabel.setFont(font);
    // volumeLabel.setString(GetLocalizedMenuText("musicToggle")); // Now handled in updateLocalizedMenuStrings
    volumeLabel.setCharacterSize(36);
    volumeLabel.setFillColor(sf::Color::White);
    volumeLabel.setOrigin(volumeLabel.getLocalBounds().width / 2.0f, volumeLabel.getLocalBounds().height / 2.0f);
    volumeLabel.setPosition(centerX - sliderWidth / 2.0f - 100, startY);

    volumeTrack.setSize(sf::Vector2f(sliderWidth, sliderHeight));
    volumeTrack.setFillColor(sf::Color(50, 50, 50, 200));
    volumeTrack.setOutlineThickness(2);
    volumeTrack.setOutlineColor(sf::Color::White);
    volumeTrack.setOrigin(sliderWidth / 2.0f, sliderHeight / 2.0f);
    volumeTrack.setPosition(centerX, startY);

    volumeThumb.setSize(sf::Vector2f(20, 30));
    volumeThumb.setFillColor(sf::Color::Yellow);
    volumeThumb.setOrigin(10.0f, 15.0f);

    volumeValueText.setFont(font);
    volumeValueText.setCharacterSize(36);
    volumeValueText.setFillColor(sf::Color::White);
    volumeValueText.setOrigin(volumeValueText.getLocalBounds().width / 2.0f, volumeValueText.getLocalBounds().height / 2.0f);
    volumeValueText.setPosition(centerX + sliderWidth / 2.0f + 50, startY);

    // Brightness Slider initialization
    brightnessLabel.setFont(font);
    // brightnessLabel.setString(GetLocalizedMenuText("changeVolume")); // Now handled in updateLocalizedMenuStrings
    brightnessLabel.setCharacterSize(36);
    brightnessLabel.setFillColor(sf::Color::White);
    brightnessLabel.setOrigin(brightnessLabel.getLocalBounds().width / 2.0f, brightnessLabel.getLocalBounds().height / 2.0f);
    brightnessLabel.setPosition(centerX - sliderWidth / 2.0f - 100, startY + sliderSpacing);

    brightnessTrack.setSize(sf::Vector2f(sliderWidth, sliderHeight));
    brightnessTrack.setFillColor(sf::Color(50, 50, 50, 200));
    brightnessTrack.setOutlineThickness(2);
    brightnessTrack.setOutlineColor(sf::Color::White);
    brightnessTrack.setOrigin(sliderWidth / 2.0f, sliderHeight / 2.0f);
    brightnessTrack.setPosition(centerX, startY + sliderSpacing);

    brightnessThumb.setSize(sf::Vector2f(20, 30));
    brightnessThumb.setFillColor(sf::Color::Yellow);
    brightnessThumb.setOrigin(10.0f, 15.0f);

    brightnessValueText.setFont(font);
    brightnessValueText.setCharacterSize(36);
    brightnessValueText.setFillColor(sf::Color::White);
    brightnessValueText.setOrigin(brightnessValueText.getLocalBounds().width / 2.0f, brightnessValueText.getLocalBounds().height / 2.0f);
    brightnessValueText.setPosition(centerX + sliderWidth / 2.0f + 50, startY + sliderSpacing);

    activeSlider = OptionSliderType::NONE; // Initially no slider is active
    selectedIndex = 0; // Reset selected index
} 