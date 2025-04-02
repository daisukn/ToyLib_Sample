#pragma once

#include "VisualComponent.h"
#include "MathUtils.h"
#include <vector>

class ParticleComponent : public VisualComponent
{
public:
    enum ParticleMode
    {
        P_SPARK,
        P_WATER,
        P_SMOKE
    };

    struct Particle
    {
        Vector3 pos;
        Vector3 dir;
        float lifeTime;
        float size;
        bool isVisible;
    };

    ParticleComponent(class Actor* a, int drawOrder);
    ~ParticleComponent();

    void Draw(class Shader* shader) override;
    void Update(float deltaTime) override;

    void SetTexture(class Texture* tex) override;
    void CreateParticles(Vector3 pos, unsigned int num, float life, float part_life, float size, ParticleMode mode);

private:
    void GenerateParts();

    Vector3 mPosition;
    unsigned int mNumParts;
    float mLifeTime;
    float mTotalLife;
    float mPartLifecycle;
    float mPartSize;
    float mPartSpeed;
    ParticleMode mParticleMode;
    std::vector<Particle> mParts;
};
