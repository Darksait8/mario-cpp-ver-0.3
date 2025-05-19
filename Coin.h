#pragma once
#include "Object.h"
#include "Physics.h"
#include "Animation.h"
#include <Box2D/Dynamics/Body.hpp>

class Coin :
    public Object
{
public:
    ~Coin();

    virtual void Begin() override;
    virtual void Update(float deltaTime) override;
    virtual void Render(Renderer& renderer) override;
private:
    Animation animation;
    box2d::Body* body;
};
