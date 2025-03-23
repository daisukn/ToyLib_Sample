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
    float GetDropSpeed() const { return mDropSpeed; }
    void SetAngularSpeed(float speed) { mAngularSpeed = speed; }
    void SetForwardSpeed(float speed) { mForwardSpeed = speed; }
    void SetRightSpeed(float speed) { mRightSpeed = speed;}
    void SetDropSpeed(float speed) { mDropSpeed = speed; }
    void SetUpSpeed(float speed) {mUpperSpeed = speed; }
    
    void Reset();


private:
    float mAngularSpeed;     // 回転スピード
    float mForwardSpeed;     // 移動スピード
    float mRightSpeed;       // 横移動スピード
    float mDropSpeed;        // 落下スピード
    float mUpperSpeed;

};
