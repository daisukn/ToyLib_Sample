#pragma once

#include "VisualComponent.h"

class BillboardComponent : public VisualComponent
{
public:
    BillboardComponent(class Actor* a, int drawOrder);
    ~BillboardComponent();

    void Draw(class Shader* shader) override;
    
private:
    float mScale;
};
