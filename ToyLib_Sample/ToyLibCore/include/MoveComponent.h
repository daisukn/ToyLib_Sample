#pragma once

#include "Component.h"
#include "MathUtils.h"

class MoveComponent : public Component
{
    
public:
    MoveComponent(class Actor* owner, int updateOrder = 10);
    void Update(float deltaTime) override;
    
    // Getter/Setter
    float GetAngularSpeed() const { return mAngularSpeed; }
    float GetForwardSpeed() const { return mForwardSpeed; }
    float GetRightSpeed() const { return mRightSpeed; }
    float GetVerticalSpeed() const { return mVerticalSpeed; }

    void SetAngularSpeed(float speed) { mAngularSpeed = speed; }
    void SetForwardSpeed(float speed) { mForwardSpeed = speed; }
    void SetRightSpeed(float speed) { mRightSpeed = speed;}
    void SetVerticalSpeed(float speed) { mVerticalSpeed = speed; }

    
    void Reset();


protected:
    float mAngularSpeed;     // 回転
    float mForwardSpeed;     // 前後
    float mRightSpeed;       // 左右
    float mVerticalSpeed;    // 上下 （正＝上昇、負＝落下）

};
