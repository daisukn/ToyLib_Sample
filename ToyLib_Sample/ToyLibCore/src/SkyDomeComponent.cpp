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

float gTimeOfDay = 0.f;

void SkyDomeComponent::Draw(Shader* shader)
{
    if (!mSkyVAO || !shader) return;

    Matrix4 invView = mOwnerActor->GetApp()->GetRenderer()->GetInvViewMatrix();
    
    Vector3 camPos = invView.GetTranslation() + Vector3(0, -40, 0);
    Matrix4 model = Matrix4::CreateScale(100.0f) * Matrix4::CreateTranslation(camPos);
    Matrix4 view = mOwnerActor->GetApp()->GetRenderer()->GetViewMatrix();
    Matrix4 proj = mOwnerActor->GetApp()->GetRenderer()->GetProjectionMatrix();
    Matrix4 mvp = model * view * proj;

    gTimeOfDay += 0.00051f;
    

    shader->SetActive();
    shader->SetMatrixUniform("uMVP", mvp);
    
    float t = fmod(SDL_GetTicks() / 1000.0f, 60.0f) / 60.0f; // 0〜1で60秒周期
    shader->SetFloatUniform("uTimeOfDay", t);
    shader->SetIntUniform("uWeatherType", 1);
    shader->SetFloatUniform("uTimeOfDay", fmod(gTimeOfDay, 1.0f)); // 0.0〜1.0
    
    // 時間帯 (0.0〜1.0) に基づいて太陽のベクトルを算出
    // 夜: 0.0, 朝: 0.25, 昼: 0.5, 夕: 0.75, 夜: 1.0
    float angle = Math::TwoPi * gTimeOfDay - Math::PiOver2; // -90° から 270° 回転
    // 半球上の円弧に沿って太陽を動かす（Z前方、Y上下）
    Vector3 sunDir = Vector3(0.0f, -sinf(angle), cosf(angle));

    sunDir.Normalize();
    shader->SetVectorUniform("uSunDir", sunDir);
    //shader->SetVectorUniform("uSunDir", Vector3::UnitY); // -Z方向など
    
    
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE); // Z書き込みを無効
    mSkyVAO->SetActive();
    glDrawElements(GL_TRIANGLES, mSkyVAO->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
}
