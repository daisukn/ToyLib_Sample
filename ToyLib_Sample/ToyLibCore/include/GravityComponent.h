#pragma once
#include "Component.h"

class GravityComponent : public Component {
public:
    GravityComponent(class Actor* a);

    void Update(float deltaTime) override;
    void Jump();

    void SetGravityAccel(float g) { mGravityAccel = g; }
    void SetJumpSpeed(float s) { mJumpSpeed = s; }
    bool IsGrounded() const { return mIsGrounded; }

private:
    float mVelocityY = 0.0f;
    float mGravityAccel = -980.0f;
    float mJumpSpeed = 350.0f;
    bool mIsGrounded = false;
    
    
    class ColliderComponent* FindFootCollider();
};
