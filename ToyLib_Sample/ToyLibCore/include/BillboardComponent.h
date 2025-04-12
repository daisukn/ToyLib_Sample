#pragma once

#include "VisualComponent.h"

class BillboardComponent : public VisualComponent
{
public:
    BillboardComponent(class Actor* a, int drawOrder);
    ~BillboardComponent();

    void Draw() override;
    
private:
    float mScale;
};
