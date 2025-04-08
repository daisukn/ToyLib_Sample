#include "SkyDomeComponent.h"
#include "SkyDomeMeshGenerator.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "Actor.h"
#include "Application.h"
#include "Renderer.h"

SkyDomeComponent::SkyDomeComponent(Actor* a)
: Component(a)
, mTime(0.0f)
, mSunDir(Vector3::UnitY)
, mWeatherType(WeatherType::CLEAR)
{
    mSkyVAO = SkyDomeMeshGenerator::CreateSkyDomeVAO(32, 16, 1.0f);
    mOwnerActor->GetApp()->GetRenderer()->SetSkyDome(this);
}

void SkyDomeComponent::SetTime(float t) {
    mTime = Math::Clamp(t, 0.0f, 1.0f);
}

void SkyDomeComponent::SetSunDirection(const Vector3& dir) {
    mSunDir = dir;
}

void SkyDomeComponent::Draw(Shader* shader)
{
    if (!mSkyVAO || !shader) return;

    Matrix4 invView = mOwnerActor->GetApp()->GetRenderer()->GetInvViewMatrix();
    Vector3 camPos = invView.GetTranslation();
    Matrix4 model = Matrix4::CreateScale(1.0f) * Matrix4::CreateTranslation(camPos);
    Matrix4 mvp = mOwnerActor->GetApp()->GetRenderer()->GetViewProjMatrix() * model;

    shader->SetActive();
    shader->SetMatrixUniform("uMVP", mvp);
    shader->SetFloatUniform("uTime", mTime);
    shader->SetVectorUniform("uSunDir", mSunDir);
    //glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE); // Z書き込みを無効
    mSkyVAO->SetActive();
    glDrawElements(GL_LINE_STRIP, mSkyVAO->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    glDepthMask(GL_TRUE);
    //glEnable(GL_CULL_FACE);
}
