#pragma once
#include "Component.h"
#include "Shader.h"
#include <memory>

enum class WeatherType
{
    CLEAR = 0,
    CLOUDY,
    RAIN,
    STORM,
    SNOW
};


class SkyDomeComponent : public Component {
public:
    SkyDomeComponent(class Actor* a);

    void SetTime(float t);
    void SetSunDirection(const Vector3& dir);
    void Draw(class Shader* shader);
    void Update(float deltaTime) override;
    
    WeatherType GetWeatherType() const { return mWeatherType; }
    void SetWeatherType(WeatherType weather) { mWeatherType = weather; }
    
    void SetAutoTimeTick(const bool b) { mIsAutoTimeTick = b; }
    
    void SetLightingManager(std::shared_ptr<class LightingManager> manager) { mLightingManager = std::move(manager); }

    
private:
    std::unique_ptr<class VertexArray> mSkyVAO;
    std::shared_ptr<class LightingManager> mLightingManager;
    float mTime;
    float mTimeSpeed;
    int mPastDay;
    Vector3 mSunDir;
    WeatherType mWeatherType;
    float SmoothStep(float edge0, float edge1, float x);
    void RandomizeWeather();
    void ApplyTime();
    bool mIsAutoTimeTick;
};
