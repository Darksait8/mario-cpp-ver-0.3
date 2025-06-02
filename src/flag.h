#ifndef FLAG_H
#define FLAG_H

#include "Object.h" // Предполагается, что Object.h существует
#include <SFML/Graphics.hpp>

class Flag : public Object {
public:
    Flag();
    ~Flag() override;

    void Update(float deltaTime) override;
    void Draw(Renderer& renderer) override;

    // Добавьте специфичные для флага члены, например, для анимации
    bool isDescending = false;
    float descendSpeed = 100.0f; // Скорость спуска флага в пикселях в секунду

private:
    // Возможно, понадобится добавить специфичные текстуры или спрайты здесь
};

#endif // FLAG_H 