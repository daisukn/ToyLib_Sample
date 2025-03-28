#pragma once

#include "Component.h"


// ワイヤーフレームの表示用
class WireframeComponent : public Component
{
public:
    WireframeComponent(class Actor* a);
    virtual ~WireframeComponent();
    
    virtual void Draw(class Shader* s) { }
};
