#pragma once
#include "Component.h"
#include "Shader.h"
#include <memory>

enum class WeatherType
{
    CLEAR,
    RAIN,
    STORM,
    CLOUDY
};


class SkyDomeComponent : public Component {
public:
    SkyDomeComponent(Actor* a);

    void SetTime(float t);
    void SetSunDirection(const Vector3& dir);
    void Draw(class Shader* shader);
    
    WeatherType GetWeatherType() const { return mWeatherType; }
    void SetWeatherType(WeatherType weather) { mWeatherType = weather; }

private:
    std::unique_ptr<class VertexArray> mSkyVAO;
    float mTime;
    Vector3 mSunDir;
    WeatherType mWeatherType;
};
