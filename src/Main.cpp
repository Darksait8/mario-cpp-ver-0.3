#include "Game.h"
#include "Renderer.h"
#include "Resources.h"
#include <SFML/Graphics.hpp>

int main() {
  sf::RenderWindow window(sf::VideoMode(1200, 900), "BETTER Mario");
  sf::Clock deltaClock;
  Renderer renderer(window);
  window.setFramerateLimit(60);

  // Инициализируем ресурсы после создания окна
  Resources::initialize();

  bool quit = false;
  Begin(window);
  while (!quit) {
    float deltaTime = deltaClock.restart().asSeconds();

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        quit = true;
      HandleEvent(event);
    }

    Update(deltaTime);

    window.clear();

    window.setView(GetCamera().GetView(window.getSize()));
    Render(renderer, window);

    window.setView(GetCamera().GetUIView());
    RenderUI(renderer);

    window.display();
  }

  End();
  return 0;
}
