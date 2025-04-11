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

//float gTimeOfDay = 0.f;

void SkyDomeComponent::Draw(Shader* shader)
{
    if (!mSkyVAO || !shader) return;

    Matrix4 invView = mOwnerActor->GetApp()->GetRenderer()->GetInvViewMatrix();
    
    Vector3 camPos = invView.GetTranslation() + Vector3(0, -20, 0);
    Matrix4 model = Matrix4::CreateScale(100.0f) * Matrix4::CreateTranslation(camPos);
    Matrix4 view = mOwnerActor->GetApp()->GetRenderer()->GetViewMatrix();
    Matrix4 proj = mOwnerActor->GetApp()->GetRenderer()->GetProjectionMatrix();
    Matrix4 mvp = model * view * proj;


    shader->SetActive();
    shader->SetMatrixUniform("uMVP", mvp);
    
    float t = fmod(SDL_GetTicks() / 1000.0f, 40.0f) / 60.0f; // 0〜1で60秒周期
    shader->SetFloatUniform("uTime", t);
    shader->SetIntUniform("uWeatherType", 1);
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

void SkyDomeComponent::Update(float deltaTime)
{
    mTime += mTimeSpeed;
    
    // 時間帯 (0.0〜1.0) に基づいて太陽のベクトルを算出
    // 夜: 0.0, 朝: 0.25, 昼: 0.5, 夕: 0.75, 夜: 1.0
    /*float angle = Math::TwoPi * mTime - Math::PiOver2; // -90° から 270° 回転
    // 半球上の円弧に沿って太陽を動かす（Z前方、Y上下）
    mSunDir = Vector3(0.0f, -sinf(angle), cosf(angle));
    mSunDir.Normalize();
    
    mOwnerActor->GetApp()->GetRenderer()->SetDirectionalLightPosition(Vector3(-mSunDir.x, -mSunDir.y, -mSunDir.z), Vector3::Zero);

     */
     
     // ゲーム時間 0.0〜1.0 → 0〜180度（π）を回す
     float angle = Math::Pi * fmod(mTime, 1.0f); // 0.0〜π（180°）

     // 軌道の定義：XZ平面で +X からスタート、+Z に向かって傾く円弧
     // 例えば、XY平面ではなく XZ平面に投影しながら、Yも上下に
    mSunDir = Vector3(
         -cosf(angle),        // +X方向から -X方向へ移動
         -sinf(angle),       // 太陽が昇って沈む（Y軸）
         0.2f * cosf(angle)  // 南方向に傾ける（+Z成分）
     );
    mSunDir.Normalize();

     // セット（ディレクショナルライトとシェーダー両方に）
     mOwnerActor->GetApp()->GetRenderer()->SetDirectionalLightPosition(Vector3(-mSunDir.x, -mSunDir.y, -mSunDir.z), Vector3::Zero);
    
    std::cout << "time = " << fmod(mTime, 1.0f) << std::endl;
    
    float time = fmod(mTime, 1.0f); // 0.0 ~ 1.0

    Vector3 color;

    if (time < 0.2f) {
        // 🌃 夜明け前
        color = Vector3(0.1f, 0.1f, 0.15f);
    }
    else if (time < 0.4f) {
        // 🌅 朝〜昼
        float t = (time - 0.2f) / 0.2f;
        color = Vector3::Lerp(Vector3(1.0f, 0.6f, 0.3f), Vector3(1.0f, 1.0f, 1.0f), t); // 暖→白
    }
    else if (time < 0.6f) {
        // ☀ 昼
        color = Vector3(1.0f, 1.0f, 1.0f);
    }
    else if (time < 0.8f) {
        // 🌇 夕方〜夜
        float t = (time - 0.6f) / 0.2f;
        color = Vector3::Lerp(Vector3(1.0f, 1.0f, 1.0f), Vector3(1.0f, 0.5f, 0.3f), t); // 白→赤
    }
    else {
        // 🌌 夜
        color = Vector3(0.05f, 0.05f, 0.1f);
    }

    mOwnerActor->GetApp()->GetRenderer()->SetDirectionalLightColor(color);
    
}
