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
    SkyDomeComponent(Actor* a);

    void SetTime(float t);
    void SetSunDirection(const Vector3& dir);
    void Draw(class Shader* shader);
    void Update(float deltaTime);
    
    WeatherType GetWeatherType() const { return mWeatherType; }
    void SetWeatherType(WeatherType weather) { mWeatherType = weather; }

private:
    std::unique_ptr<class VertexArray> mSkyVAO;
    float mTime;
    float mTimeSpeed;
    Vector3 mSunDir;
    WeatherType mWeatherType;
};
