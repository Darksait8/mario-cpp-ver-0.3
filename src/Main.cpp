#include "Game.h"
#include "Renderer.h"
#include <SFML/Graphics.hpp>

int main() {
  sf::RenderWindow window(sf::VideoMode(1200, 900), "BETTER Mario");
  sf::Clock deltaClock;
  Renderer renderer(window);
  window.setFramerateLimit(60);

  bool quit = false;
  Begin();
  while (!quit) {
    float deltaTime = deltaClock.restart().asSeconds();

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        quit = true;
      if (event.type == sf::Event::KeyPressed &&
          event.key.code == sf::Keyboard::Escape)
        GetPaused() = !GetPaused();
    }

    Update(deltaTime);

    window.clear();

    window.setView(GetCamera().GetView(window.getSize()));
    Render(renderer);

    window.setView(GetCamera().GetUIView());
    RenderUI(renderer);

    window.display();
  }

  End();
}
