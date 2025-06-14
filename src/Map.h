#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Renderer.h"
#include "Object.h"
#include "Physics.h"

class Map
{
public:
	Map(float cellSize = 32.0f);

	void CreateCheckerboard(size_t width, size_t height);
	sf::Vector2f CreateFromImage(const sf::Image& image, std::vector<Object*>& objects, class Mario* mario);

	void Draw(Renderer& renderer);

	// Метод для разрушения блока
	void BreakBlock(int x, int y, sf::Texture* newTexture);

	// Method to update map state, including flag animation
	void Update(float deltaTime);

	// Method to start the flag descent animation
	void StartFlagDescent();

	// Method to clear the map grid
	void ClearGrid();

	bool victorySequenceStarted = false; // Flag to indicate if the victory sequence has started

	// Удаляем объявление RemoveBlock, если оно было добавлено как статическое
	// static void RemoveBlock(int x, int y);

	// Убеждаемся, что grid не статическая
	std::vector<std::vector<sf::Texture*>> grid;
	float cellSize;

	// Перемещаем topFlagData в public
	FixtureData* topFlagData = nullptr; // Pointer to the FixtureData of the top flag block

	// sf::Sprite backgroundSprite; // Удаляю дублирующее объявление

private:
	sf::Sprite backgroundSprite; // Добавляем фоновый спрайт

};
