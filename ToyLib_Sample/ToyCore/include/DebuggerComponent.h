#pragma once

#include "Component.h"


// デバッガー用の表示などを管理
class DebuggerComponent : public Component
{
public:
    DebuggerComponent(class Actor* a);
    ~DebuggerComponent();
    
    virtual void Draw(class Shader* s) { }
};
