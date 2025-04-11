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
, mTimeSpeed(0.0005f)
, mSunDir(Vector3::UnitY)
, mWeatherType(WeatherType::STORM)
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

//float gTimeOfDay = 0.f;

void SkyDomeComponent::Draw(Shader* shader)
{
    if (!mSkyVAO || !shader) return;

    Matrix4 invView = mOwnerActor->GetApp()->GetRenderer()->GetInvViewMatrix();
    
    Vector3 camPos = invView.GetTranslation() + Vector3(0, -30, 0);
    Matrix4 model = Matrix4::CreateScale(100.0f) * Matrix4::CreateTranslation(camPos);
    Matrix4 view = mOwnerActor->GetApp()->GetRenderer()->GetViewMatrix();
    Matrix4 proj = mOwnerActor->GetApp()->GetRenderer()->GetProjectionMatrix();
    Matrix4 mvp = model * view * proj;


    shader->SetActive();
    shader->SetMatrixUniform("uMVP", mvp);
    
    float t = fmod(SDL_GetTicks() / 1000.0f, 40.0f) / 60.0f; // 0〜1で60秒周期
    shader->SetFloatUniform("uTime", t);
    shader->SetIntUniform("uWeatherType", static_cast<int>(mWeatherType));
    shader->SetFloatUniform("uTimeOfDay", fmod(mTime, 1.0f)); // 0.0〜1.0
    

    shader->SetVectorUniform("uSunDir", mSunDir);
    //shader->SetVectorUniform("uSunDir", Vector3::UnitY); // -Z方向など
    
    
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE); // Z書き込みを無効
    mSkyVAO->SetActive();
    glDrawElements(GL_TRIANGLES, mSkyVAO->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
}

float SkyDomeComponent::SmoothStep(float edge0, float edge1, float x)
{
    // Clamp x between edge0 and edge1
    float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}


void SkyDomeComponent::Update(float deltaTime)
{
    mTime += mTimeSpeed;
    mTime = 0.9f;
    
     // ゲーム時間 0.0〜1.0 → 0〜180度（π）を回す
     float angle = Math::Pi * fmod(mTime, 1.0f); // 0.0〜π（180°）

     // 軌道の定義：XZ平面で +X からスタート、+Z に向かって傾く円弧
     // 例えば、XY平面ではなく XZ平面に投影しながら、Yも上下に
    mSunDir = Vector3(
         -cosf(angle),        // +X方向から -X方向へ移動
         -sinf(angle),       // 太陽が昇って沈む（Y軸）
         0.5f * sin(angle)  // 南方向に傾ける（+Z成分）
     );
    mSunDir.Normalize();

     // セット（ディレクショナルライトとシェーダー両方に）
     mOwnerActor->GetApp()->GetRenderer()->SetDirectionalLightPosition(Vector3(-mSunDir.x, -mSunDir.y, -mSunDir.z), Vector3::Zero);
    
   // std::cout << "time = " << fmod(mTime, 1.0f) << std::endl;
    
    
    // ライティングを時間で変化
    float timeOfDay = fmod(mTime, 1.0f);

    float dayStrength = SmoothStep(0.15f, 0.25f, timeOfDay) *
                        (1.0f - SmoothStep(0.75f, 0.85f, timeOfDay));
    float nightStrength = 1.0f - dayStrength;

    // 太陽と月の Diffuse カラー補間
    Vector3 sunColor = Vector3(1.0f, 0.95f, 0.8f);
    Vector3 moonColor = Vector3(0.3f, 0.4f, 0.6f);
    Vector3 finalLightColor = sunColor * dayStrength + moonColor * nightStrength;
    mOwnerActor->GetApp()->GetRenderer()->SetDirectionalLightColor(finalLightColor);

    // Ambient（空気の明るさ）も同様に補間
    Vector3 dayAmbient = Vector3(0.7f, 0.7f, 0.7f);
    Vector3 nightAmbient = Vector3(0.1f, 0.15f, 0.2f);
    Vector3 finalAmbient = dayAmbient * dayStrength + nightAmbient * nightStrength;
    mOwnerActor->GetApp()->GetRenderer()->SetAmbientLightColor(finalAmbient);
}

