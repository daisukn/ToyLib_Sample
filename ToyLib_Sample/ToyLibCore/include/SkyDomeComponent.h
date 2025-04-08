#pragma once
#include "VisualComponent.h"
#include "Shader.h"
#include <memory>

class SkyDomeComponent : public VisualComponent {
public:
    SkyDomeComponent(Actor* owner);

    void SetTime(float t);
    void SetSunDirection(const Vector3& dir);
    void Draw(class Shader* shader) override;

private:
    std::unique_ptr<VertexArray> mSkyVAO;
    float mTime;
    Vector3 mSunDir;
};
