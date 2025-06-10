#include "GameMenu.h"
#include "Game.h"
#include "Resources.h" // Для доступа к ресурсам (текстуры, шрифты)
#include <iostream>
#include <fstream>
#include <algorithm>
// #include <conio.h> // Больше не нужен для SFML ввода

GameMenuItem::GameMenuItem(const std::string& textString, sf::Font& font, std::function<void()> action)
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
    if (!font.loadFromFile("./resources/troika.otf")) {
        std::cerr << "Error: Failed to load font from ./resources/troika.otf" << std::endl;
    }

    // Загрузка фонового изображения
    if (Resources::textures.count("mainmenu.png")) {
        backgroundSprite.setTexture(Resources::textures["mainmenu.png"]);
        // Масштабирование и позиционирование теперь будет происходить в render()
    } else {
        std::cerr << "Error: mainmenu.png texture not loaded for GameMenu background." << std::endl;
    }

    // Инициализация текста заголовка
    titleText.setFont(font);
    // titleText.setString("REMAKE SUPER MARIO BROSU."); // Удаляем текст заголовка
    titleText.setCharacterSize(64); // Увеличиваем размер заголовка
    titleText.setFillColor(sf::Color::Red);
    titleText.setOutlineColor(sf::Color::Yellow);
    titleText.setOutlineThickness(2.0f);
    // Позиционирование будет происходить в renderMainMenu

    // Инициализация текста инструкций
    instructionsText.setFont(font);
    // instructionsText.setString("W/S - Select, Enter - Confirm, ESC - Back"); // Moved to updateLocalizedMenuStrings
    instructionsText.setCharacterSize(32); // Увеличиваем размер инструкций
    instructionsText.setFillColor(sf::Color::White);
    instructionsText.setOutlineColor(sf::Color::Black);
    instructionsText.setOutlineThickness(1.0f);
    // Позиционирование будет происходить в renderMainMenu

    // Инициализация элементов авторизации
    usernameLabel.setFont(font);
    // usernameLabel.setString("Username:"); // Moved to updateLocalizedMenuStrings
    usernameLabel.setCharacterSize(32);
    usernameLabel.setFillColor(sf::Color::White);

    passwordLabel.setFont(font);
    // passwordLabel.setString("Password:"); // Moved to updateLocalizedMenuStrings
    passwordLabel.setCharacterSize(32);
    passwordLabel.setFillColor(sf::Color::White);

    usernameInputBox.setSize(sf::Vector2f(400, 50));
    usernameInputBox.setFillColor(sf::Color(50, 50, 50, 200)); // Темно-серый, полупрозрачный
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
    // loginButtonText.setString("Login"); // Moved to updateLocalizedMenuStrings
    loginButtonText.setCharacterSize(36);
    loginButtonText.setFillColor(sf::Color::White);

    registerButtonRect.setSize(sf::Vector2f(200, 60));
    registerButtonRect.setFillColor(sf::Color(70, 70, 70, 200));
    registerButtonRect.setOutlineThickness(2);
    registerButtonRect.setOutlineColor(sf::Color::White);

    registerButtonText.setFont(font);
    // registerButtonText.setString("Register"); // Moved to updateLocalizedMenuStrings
    registerButtonText.setCharacterSize(36);
    registerButtonText.setFillColor(sf::Color::White);

    activeInputField = InputField::NONE;
    usernameInput = "";
    passwordInput = "";

    // Initialize messageText
    messageText.setFont(font);
    messageText.setCharacterSize(24);
    messageText.setFillColor(sf::Color::Red); // Default to red for errors
    messageText.setString(""); // Start with empty message

    // Populate English menu strings
    menuEnglishStrings["instructionsText"] = "W/S - Select, Enter - Confirm, ESC - Back";
    menuEnglishStrings["usernameLabel"] = "Username:";
    menuEnglishStrings["passwordLabel"] = "Password:";
    menuEnglishStrings["loginButton"] = "Login";
    menuEnglishStrings["registerButton"] = "Register";
    menuEnglishStrings["loginSuccess"] = "Login successful!";
    menuEnglishStrings["loginFailed"] = "Login failed. Invalid username or password.";
    menuEnglishStrings["registrationSuccess"] = "Registration successful! You can now log in.";
    menuEnglishStrings["registrationFailed"] = "Registration failed. Try a different username or shorter password.";
    menuEnglishStrings["pauseTitle"] = "PAUSED";
    menuEnglishStrings["continueGame"] = "Continue";
    menuEnglishStrings["optionsMenu"] = "Options";
    menuEnglishStrings["exitToMainMenu"] = "Exit to Main Menu";
    menuEnglishStrings["victoryTitle"] = "VICTORY!";
    menuEnglishStrings["scoreDisplay"] = "Score: ";
    menuEnglishStrings["coinsDisplay"] = "Coins: ";
    menuEnglishStrings["timeDisplay"] = "Time: ";
    menuEnglishStrings["pressEnterToContinue"] = "Press ENTER to continue";
    menuEnglishStrings["pressEscToExitMenu"] = "Press ESC to exit to main menu";
    menuEnglishStrings["musicToggle"] = "Toggle Music";
    menuEnglishStrings["changeVolume"] = "Change Volume (NYI)";
    menuEnglishStrings["backButton"] = "Back";
    menuEnglishStrings["leaderboardTitle"] = "Leaderboard";
    menuEnglishStrings["toggleMusicOn"] = "Music: ON";
    menuEnglishStrings["toggleMusicOff"] = "Music: OFF";
    menuEnglishStrings["pressEscToReturn"] = "Press ESC to return";
    menuEnglishStrings["leaderboardScore"] = "Score:";
    menuEnglishStrings["leaderboardFastestTime"] = "Fastest Time:";
    menuEnglishStrings["leaderboardMostCoins"] = "Most Coins:";

    // Populate Russian menu strings
    menuRussianStrings["instructionsText"] = "W/S - Выбрать, Enter - Подтвердить, ESC - Назад";
    menuRussianStrings["usernameLabel"] = "Имя пользователя:";
    menuRussianStrings["passwordLabel"] = "Пароль:";
    menuRussianStrings["loginButton"] = "Вход";
    menuRussianStrings["registerButton"] = "Регистрация";
    menuRussianStrings["loginSuccess"] = "Вход успешен!";
    menuRussianStrings["loginFailed"] = "Вход не удался. Неверное имя пользователя или пароль.";
    menuRussianStrings["registrationSuccess"] = "Регистрация успешна! Теперь вы можете войти.";
    menuRussianStrings["registrationFailed"] = "Регистрация не удалась. Попробуйте другое имя пользователя или более короткий пароль.";
    menuRussianStrings["pauseTitle"] = "ПАУЗА";
    menuRussianStrings["continueGame"] = "Продолжить";
    menuRussianStrings["optionsMenu"] = "Настройки";
    menuRussianStrings["exitToMainMenu"] = "Выход в главное меню";
    menuRussianStrings["victoryTitle"] = "ПОБЕДА!";
    menuRussianStrings["scoreDisplay"] = "Очки: ";
    menuRussianStrings["coinsDisplay"] = "Монеты: ";
    menuRussianStrings["timeDisplay"] = "Время: ";
    menuRussianStrings["pressEnterToContinue"] = "Нажмите ENTER для продолжения";
    menuRussianStrings["pressEscToExitMenu"] = "Нажмите ESC для выхода в главное меню";
    menuRussianStrings["musicToggle"] = "Переключить музыку";
    menuRussianStrings["changeVolume"] = "Изменить громкость (Н/Р)";
    menuRussianStrings["backButton"] = "Назад";
    menuRussianStrings["leaderboardTitle"] = "Таблица лидеров";
    menuRussianStrings["toggleMusicOn"] = "Музыка: ВКЛ";
    menuRussianStrings["toggleMusicOff"] = "Музыка: ВЫКЛ";
    menuRussianStrings["pressEscToReturn"] = "Нажмите ESC для возврата";
    menuRussianStrings["leaderboardScore"] = "Очки:";
    menuRussianStrings["leaderboardFastestTime"] = "Лучшее время:";
    menuRussianStrings["leaderboardMostCoins"] = "Больше всего монет:";

    updateLocalizedMenuStrings(); // Initial update of menu strings
}

// Helper function to get localized menu text
std::string GameMenu::GetLocalizedMenuText(const std::string& key) {
    if (!gameDataRef) {
        // Handle the case where gameDataRef is not set, e.g., during initial setup before Game is fully initialized.
        // For debugging, you might print a warning:
        // std::cerr << "Warning: gameDataRef is null in GetLocalizedMenuText for key: " << key << std::endl;
        return "ERROR: gameDataRef NULL"; // Or a more appropriate fallback
    }

    if (gameDataRef->currentLanguage == Language::ENGLISH) {
        if (menuEnglishStrings.count(key)) {
            return menuEnglishStrings[key];
        }
    } else { // RUSSIAN
        if (menuRussianStrings.count(key)) {
            return menuRussianStrings[key];
        }
    }
    return "MENU_KEY_NOT_FOUND"; // Fallback for missing keys
}

void GameMenu::updateLocalizedMenuStrings() {
    // Update Main Menu items
    // Re-initialize actions to update menu item strings
    // This part should be handled by setupMenuForState which is called in setState
    // For general text updates, we can update them directly here.
    // Ensure gameDataRef is set before accessing currentLanguage
    if (gameDataRef) {
        // Update Auth menu texts
        usernameLabel.setString(GetLocalizedMenuText("usernameLabel"));
        passwordLabel.setString(GetLocalizedMenuText("passwordLabel"));
        loginButtonText.setString(GetLocalizedMenuText("loginButton"));
        registerButtonText.setString(GetLocalizedMenuText("registerButton"));
        instructionsText.setString(GetLocalizedMenuText("instructionsText"));

        // Update Options menu texts (labels only, actual menu items are handled by initializeOptionsActions)
        // Ensure these SFML text objects are properly initialized in the constructor
        volumeLabel.setString(GetLocalizedMenuText(gameDataRef->isMusicOn ? "toggleMusicOn" : "toggleMusicOff"));
        brightnessLabel.setString(GetLocalizedMenuText("changeVolume")); // This is still brightness (NYI)

        // Update Pause menu texts (items are handled by initializePauseMenuActions)

        // Update Victory screen texts (handled directly in renderVictoryScreen for now)
        // No need to update here, as they are dynamic in renderVictoryScreen
    }

    // Since menu items are dynamically created/updated via initializeActions,
    // setupMenuForState and initializePauseMenuActions/initializeOptionsActions
    // should be called after language change to refresh the menu item strings.
    // This is already handled by calling updateLocalizedMenuStrings in setState.
}

void GameMenu::initializeActions(Data* gameData) {
    gameDataRef = gameData; // Set gameDataRef
    // Добавляем пункты главного меню
    items.clear(); // Clear existing items before adding main menu items
    addMenuItem(GetLocalizedMenuText("startGame"), [this]() { // Use [this]() to access member functions
        if (gameDataRef) {
            gameDataRef->isGameStarted = true;
            gameDataRef->resetGame = true; // Signal to reset the game state
        }
    });
    
    addMenuItem(GetLocalizedMenuText("optionsMenu"), [this]() {
        setState(GameMenuState::OPTIONS);
    });
    
    addMenuItem(GetLocalizedMenuText("leaderboardTitle"), [this]() {
        setState(GameMenuState::CREDITS);
    });
    
    addMenuItem(GetLocalizedMenuText("exit"), []() {
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
            if (Resources::textures.count("mainmenu.png")) {
                backgroundSprite.setTexture(Resources::textures["mainmenu.png"]);
                sf::Vector2u textureSize = Resources::textures["mainmenu.png"].getSize();
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
            if (Resources::textures.count("options.png")) {
                backgroundSprite.setTexture(Resources::textures["options.png"]);
                sf::Vector2u textureSize = Resources::textures["options.png"].getSize();
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
            if (Resources::textures.count("leaderboard.png")) {
                backgroundSprite.setTexture(Resources::textures["leaderboard.png"]);
                sf::Vector2u textureSize = Resources::textures["leaderboard.png"].getSize();
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
            if (Resources::textures.count("authorization.png")) {
                backgroundSprite.setTexture(Resources::textures["authorization.png"]);
                sf::Vector2u textureSize = Resources::textures["authorization.png"].getSize();
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
            if (Resources::textures.count("pause.png")) {
                backgroundSprite.setTexture(Resources::textures["pause.png"]);
                // Добавлено отладочное сообщение
                std::cout << "Drawing pause.png background. Texture size: " << Resources::textures["pause.png"].getSize().x << "x" << Resources::textures["pause.png"].getSize().y << std::endl;
                sf::Vector2u textureSize = Resources::textures["pause.png"].getSize();
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
            if (Resources::textures.count("victory.png")) {
                backgroundSprite.setTexture(Resources::textures["victory.png"]);
                sf::Vector2u textureSize = Resources::textures["victory.png"].getSize();
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
                        messageText.setString(GetLocalizedMenuText("loginSuccess"));
                        messageText.setFillColor(sf::Color::Green);
                        setState(GameMenuState::MAIN_MENU);
                    } else {
                        messageText.setString(GetLocalizedMenuText("loginFailed"));
                        messageText.setFillColor(sf::Color::Red);
                    }
                    usernameInput = "";
                    passwordInput = "";
                    activeInputField = InputField::NONE;
                } else if (registerButtonRect.getGlobalBounds().contains(mousePos)) {
                    if (registerUser(usernameInput, passwordInput)) {
                        messageText.setString(GetLocalizedMenuText("registrationSuccess"));
                        messageText.setFillColor(sf::Color::Green);
                        setState(GameMenuState::MAIN_MENU);
                    } else {
                        messageText.setString(GetLocalizedMenuText("registrationFailed"));
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

void GameMenu::addMenuItem(const std::string& textString, std::function<void()> action) {
    items.emplace_back(textString, font, action); // Передаем текст и шрифт
}

void GameMenu::setState(GameMenuState newState) {
    previousState = currentState; // Сохраняем текущее состояние как предыдущее
    currentState = newState;
    setupMenuForState(newState); // Call setupMenuForState after changing state
    updateLocalizedMenuStrings(); // Update menu strings after state change
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

    // Позиционируем заголовок
    titleText.setOrigin(titleText.getLocalBounds().width / 2.0f, titleText.getLocalBounds().height / 2.0f);
    titleText.setPosition(viewCenter.x, viewCenter.y - viewSize.y / 3.0f); // Изменяем смещение для заголовка
    window.draw(titleText);

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
    sf::Vector2f viewCenter = window.getView().getCenter(); // Declare viewCenter here
    sf::Vector2f viewSize = window.getView().getSize(); // Also ensure viewSize is declared if used

    // Отрисовка затемняющего фона для регулировки яркости
    // Чем меньше brightness (от 0.0 до 1.0), тем темнее будет затемняющий слой
    sf::RectangleShape dimmer(sf::Vector2f(viewSize.x, viewSize.y));
    dimmer.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>((1.0f - gameDataRef->brightness) * 255)));
    dimmer.setPosition(viewCenter.x - viewSize.x / 2, viewCenter.y - viewSize.y / 2);
    window.draw(dimmer);

    // Render options title
    titleText.setString(GetLocalizedMenuText("optionsMenu"));
    titleText.setOrigin(titleText.getLocalBounds().width / 2.0f, titleText.getLocalBounds().height / 2.0f);
    titleText.setPosition(viewCenter.x, viewCenter.y - 300); // Higher position for the title
    window.draw(titleText);

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

    // Update and draw music toggle text
    if (gameDataRef) {
        volumeLabel.setString(GetLocalizedMenuText(gameDataRef->isMusicOn ? "toggleMusicOn" : "toggleMusicOff"));
        volumeLabel.setOrigin(volumeLabel.getLocalBounds().width / 2.0f, volumeLabel.getLocalBounds().height / 2.0f);
        volumeLabel.setPosition(viewCenter.x, startY + 0 * lineHeight);
        // We don't draw it here directly, as it's part of the menu items handled above
    }

    // Draw instructions text (ESC to return)
    instructionsText.setString(GetLocalizedMenuText("pressEscToReturn"));
    instructionsText.setOrigin(instructionsText.getLocalBounds().width / 2.0f, instructionsText.getLocalBounds().height / 2.0f);
    instructionsText.setPosition(viewCenter.x, viewCenter.y + viewSize.y / 2.5f); // Position below the menu items
    window.draw(instructionsText);

    // Render volume slider (if applicable)
    // For now, only the toggle music option is active, sliders are not yet implemented visually.

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
    backgroundSprite.setPosition(viewCenter.x - viewSize.x / 2, viewCenter.y - viewSize.y / 2); // Позиционируем фон на весь экран
    window.draw(backgroundSprite);

    // Render leaderboard title
    titleText.setString(GetLocalizedMenuText("leaderboardTitle"));
    titleText.setOrigin(titleText.getLocalBounds().width / 2.0f, titleText.getLocalBounds().height / 2.0f);
    titleText.setPosition(viewCenter.x, viewCenter.y - 300); // Higher position for the title
    window.draw(titleText);

    // Render leaderboard data
    float startY = viewCenter.y - 200;
    float lineHeight = 40.0f;

    // Display current user's score at the top if logged in
    if (!loggedInUsername.empty() && gameDataRef) {
        sf::Text currentUserScoreText;
        currentUserScoreText.setFont(font);
        std::string currentUserDisplay = loggedInUsername + ": ";
        switch (currentLeaderboardDisplay) {
            case LeaderboardDisplayType::SCORE:
                currentUserDisplay += std::to_string(gameDataRef->score);
                break;
            case LeaderboardDisplayType::FASTEST_TIME:
                currentUserDisplay += std::to_string(static_cast<int>(gameDataRef->timeElapsed)) + "s";
                break;
            case LeaderboardDisplayType::MOST_COINS:
                currentUserDisplay += std::to_string(gameDataRef->mario.coins);
                break;
        }
        currentUserScoreText.setString(currentUserDisplay);
        currentUserScoreText.setCharacterSize(30);
        currentUserScoreText.setFillColor(sf::Color::Yellow);
        currentUserScoreText.setOutlineColor(sf::Color::Black);
        currentUserScoreText.setOutlineThickness(1.0f);
        currentUserScoreText.setOrigin(currentUserScoreText.getLocalBounds().width / 2.0f, currentUserScoreText.getLocalBounds().height / 2.0f);
        currentUserScoreText.setPosition(viewCenter.x, startY - 50); // Position above the list
        window.draw(currentUserScoreText);
    }

    // Sort users based on selected display type
    std::vector<User> sortedUsers = users;
    std::sort(sortedUsers.begin(), sortedUsers.end(), [this](const User& a, const User& b) {
        if (currentLeaderboardDisplay == LeaderboardDisplayType::SCORE) {
            return a.score > b.score; // Higher score is better
        } else if (currentLeaderboardDisplay == LeaderboardDisplayType::FASTEST_TIME) {
            // Lower time is better, but 0 means not set, so 0 is worst
            if (a.fastestTime == 0) return false;
            if (b.fastestTime == 0) return true;
            return a.fastestTime < b.fastestTime;
        } else { // MOST_COINS
            return a.mostCoins > b.mostCoins; // More coins is better
        }
    });

    // Display top users
    for (size_t i = 0; i < std::min((size_t)10, sortedUsers.size()); ++i) {
        sf::Text userEntryText;
        userEntryText.setFont(font);
        std::string entryString = std::to_string(i + 1) + ". " + sortedUsers[i].username + ": ";
        switch (currentLeaderboardDisplay) {
            case LeaderboardDisplayType::SCORE:
                entryString += std::to_string(sortedUsers[i].score);
                break;
            case LeaderboardDisplayType::FASTEST_TIME:
                entryString += std::to_string(static_cast<int>(sortedUsers[i].fastestTime)) + "s";
                break;
            case LeaderboardDisplayType::MOST_COINS:
                entryString += std::to_string(sortedUsers[i].mostCoins);
                break;
        }
        userEntryText.setString(entryString);
        userEntryText.setCharacterSize(24);
        userEntryText.setFillColor(sf::Color::White);
        userEntryText.setOutlineColor(sf::Color::Black);
        userEntryText.setOutlineThickness(1.0f);
        userEntryText.setOrigin(userEntryText.getLocalBounds().width / 2.0f, userEntryText.getLocalBounds().height / 2.0f);
        userEntryText.setPosition(viewCenter.x, startY + i * lineHeight);
        window.draw(userEntryText);
    }

    // Render clickable rectangles for leaderboard categories
    scoreLeaderboardRect.setFillColor(currentLeaderboardDisplay == LeaderboardDisplayType::SCORE ? sf::Color(100, 100, 250, 150) : sf::Color(50, 50, 150, 150));
    window.draw(scoreLeaderboardRect);
    timeLeaderboardRect.setFillColor(currentLeaderboardDisplay == LeaderboardDisplayType::FASTEST_TIME ? sf::Color(100, 100, 250, 150) : sf::Color(50, 50, 150, 150));
    window.draw(timeLeaderboardRect);
    coinsLeaderboardRect.setFillColor(currentLeaderboardDisplay == LeaderboardDisplayType::MOST_COINS ? sf::Color(100, 100, 250, 150) : sf::Color(50, 50, 150, 150));
    window.draw(coinsLeaderboardRect);
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
    titleText.setString(GetLocalizedMenuText("welcomeTitle")); // Assuming a welcome title for auth
    titleText.setOrigin(titleText.getLocalBounds().width / 2.0f, titleText.getLocalBounds().height / 2.0f);
    titleText.setPosition(viewCenter.x, viewCenter.y - 300);
    window.draw(titleText);

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
    titleText.setString(GetLocalizedMenuText("pauseTitle"));
    titleText.setOrigin(titleText.getLocalBounds().width / 2.0f, titleText.getLocalBounds().height / 2.0f);
    titleText.setPosition(viewCenter.x, viewCenter.y - 150); // Higher position for the title
    window.draw(titleText);

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

    // Render victory title
    titleText.setString(GetLocalizedMenuText("victoryTitle"));
    titleText.setOrigin(titleText.getLocalBounds().width / 2.0f, titleText.getLocalBounds().height / 2.0f);
    titleText.setPosition(viewCenter.x, viewCenter.y - 300); // Higher position for the title
    window.draw(titleText);

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(40);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setString(GetLocalizedMenuText("scoreDisplay") + std::to_string(gameDataRef->score));
    scoreText.setOrigin(scoreText.getLocalBounds().width / 2.0f, scoreText.getLocalBounds().height / 2.0f);
    scoreText.setPosition(viewCenter.x, viewCenter.y - 100); // Below title
    window.draw(scoreText);

    sf::Text coinsText;
    coinsText.setFont(font);
    coinsText.setCharacterSize(40);
    coinsText.setFillColor(sf::Color::White);
    coinsText.setString(GetLocalizedMenuText("coinsDisplay") + std::to_string(gameDataRef->mario.coins));
    coinsText.setOrigin(coinsText.getLocalBounds().width / 2.0f, coinsText.getLocalBounds().height / 2.0f);
    coinsText.setPosition(viewCenter.x, viewCenter.y - 50); // Below score
    window.draw(coinsText);

    sf::Text timeText;
    timeText.setFont(font);
    timeText.setCharacterSize(40);
    timeText.setFillColor(sf::Color::White);
    timeText.setString(GetLocalizedMenuText("timeDisplay") + std::to_string(static_cast<int>(gameDataRef->timeElapsed)) + "s");
    timeText.setOrigin(timeText.getLocalBounds().width / 2.0f, timeText.getLocalBounds().height / 2.0f);
    timeText.setPosition(viewCenter.x, viewCenter.y); // Below coins
    window.draw(timeText);

    // Instructions text
    instructionsText.setString(GetLocalizedMenuText("pressEnterToContinue"));
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
    // std::cout << "GameMenu::initializePauseMenuActions called." << std::endl; // Отладочное сообщение
    if (!gameDataRef) {
        std::cerr << "Error: gameDataRef is null in initializePauseMenuActions." << std::endl;
        return;
    }
    items.clear();
    addMenuItem(GetLocalizedMenuText("continueGame"), [this]() {
        gameDataRef->paused = false;
        gameDataRef->music.play();
    });
    addMenuItem(GetLocalizedMenuText("optionsMenu"), [this]() {
        setState(GameMenuState::OPTIONS);
    });
    addMenuItem(GetLocalizedMenuText("exitToMainMenu"), [this]() {
        gameDataRef->paused = false;
        gameDataRef->isInMenu = true;
        gameDataRef->currentLevel = 0; // Reset level
        gameDataRef->music.stop();
        gameDataRef->gameMenu.setState(GameMenuState::MAIN_MENU); // Set menu state to main menu
        gameDataRef->gameMenu.initializeActions(gameDataRef); // Reinitialize main menu items
    });
    addMenuItem(GetLocalizedMenuText("exit"), []() {
        exit(0);
    });
    selectedIndex = 0; // Reset selected index
}

void GameMenu::setupMenuForState(GameMenuState newState) {
    // std::cout << "GameMenu::setupMenuForState called with state: " << static_cast<int>(newState) << std::endl; // Отладочное сообщение
    currentState = newState;
    items.clear(); // Clear current items before setting up new ones for the new state
    selectedIndex = 0; // Reset selected index for the new state

    switch (newState) {
        case GameMenuState::MAIN_MENU:
            initializeActions(gameDataRef); // Re-initialize main menu actions
            break;
        case GameMenuState::PAUSE:
            initializePauseMenuActions();
            break;
        case GameMenuState::OPTIONS:
            initializeOptionsActions(); // Инициализация пунктов меню опций
            break;
        default:
            // No specific menu items for other states (AUTH, CREDITS, EXIT)
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
    addMenuItem(GetLocalizedMenuText("musicToggle"), [this]() {
        // Toggle music here, update the label (volumeLabel)
        if (gameDataRef->music.getStatus() == sf::Music::Playing) {
            gameDataRef->music.pause();
            volumeLabel.setString(GetLocalizedMenuText("toggleMusicOff"));
        } else {
            gameDataRef->music.play();
            volumeLabel.setString(GetLocalizedMenuText("toggleMusicOn"));
        }
    });
    addMenuItem(GetLocalizedMenuText("changeVolume"), [this]() {
        activeSlider = OptionSliderType::VOLUME; // Change this to VOLUME slider, as it's the first option
    });
    // Remove the Brightness menu item, as the brightness slider is always active below
    // addMenuItem(GetLocalizedMenuText("brightnessOption"), [this]() {
    //     activeSlider = OptionSliderType::BRIGHTNESS;
    // });
    addMenuItem(GetLocalizedMenuText("backButton"), [this]() {
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