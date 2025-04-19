#pragma once
#include "SkyDomeComponent.h"

enum class WeatherType
{
    CLEAR = 0,
    CLOUDY,
    RAIN,
    STORM,
    SNOW
};



class WeatherDomeComponent : public SkyDomeComponent
{
public:
    WeatherDomeComponent(class Actor* a);
    
    void Draw() override;
    void Update(float deltaTime) override;
    
    void SetTime(float t);
    void SetSunDirection(const Vector3& dir);

    WeatherType GetWeatherType() const { return mWeatherType; }
    void SetWeatherType(WeatherType weather) { mWeatherType = weather; }
    
    void SetAutoTimeTick(const bool b) { mIsAutoTimeTick = b; }
    
private:
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
