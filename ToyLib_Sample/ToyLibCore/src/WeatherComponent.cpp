/*
 #include "WeatherComponent.h"
#include "Actor.h"
#include "Application.h"
#include "Renderer.h"


WeatherComponent::WeatherComponent(Actor* a)
: SkyDomeComponent(a)
, mTimeSpeed(0.0005f)
{
    
}

void WeatherComponent::Update(float deltaTime)
{
    
    mTime += mTimeSpeed;
    
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

float WeatherComponent::SmoothStep(float edge0, float edge1, float x)
{
    // Clamp x between edge0 and edge1
    float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

*/
