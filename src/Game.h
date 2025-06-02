#pragma once
#include "Camera.h"
#include "Object.h"
#include "Renderer.h"
#include <SFML/Graphics.hpp>

Camera &GetCamera();
bool &GetPaused();

void Begin();
void Update(float deltaTime);
void End();
void Render(Renderer &renderer);
void RenderUI(Renderer &renderer);

void DeleteObject(Object *object);

void AddScore(int scoreToAdd);
