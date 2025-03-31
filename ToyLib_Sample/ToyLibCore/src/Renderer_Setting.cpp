#include "Renderer.h"
#include "JsonHelper.h"
#include <fstream>


bool Renderer::LoadSettings(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        SDL_Log("Failed to open settings file: %s", filePath.c_str());
        return false;
    }

    nlohmann::json data;
    try
    {
        file >> data;
    }
    catch (const std::exception& e)
    {
        SDL_Log("JSON parse error: %s", e.what());
        return false;
    }

    // タイトル
    JsonHelper::GetString(data, "title", mStrTitle);

    // 画面サイズ
    if (data.contains("screen"))
    {
        JsonHelper::GetFloat(data["screen"], "width", mScreenWidth);
        JsonHelper::GetFloat(data["screen"], "height", mScreenHeight);
    }

    // FOV
    JsonHelper::GetFloat(data, "perspectiveFOV", mPerspectiveFOV);

    // カメラ位置
    mCameraPosition = JsonHelper::GetVector3(data["camera"], "position", Vector3::Zero);

    // デバッグモード
    if (data.contains("debug"))
    {
        JsonHelper::GetBool(data["debug"], "enabled", mIsDebugMode);
    }

    // クリアカラー
    mClearColor = JsonHelper::GetVector3(data, "clearColor", Vector3(0.2f, 0.5f, 0.8f));

    // ライト設定
    if (data.contains("directionalLight"))
    {
        mDirLightPosition = JsonHelper::GetVector3(data["directionalLight"], "position", Vector3(20, 20, -5));
        mDirLightTarget   = JsonHelper::GetVector3(data["directionalLight"], "target", Vector3::Zero);
    }

    // フォグ設定
    if (data.contains("fog"))
    {
        JsonHelper::GetFloat(data["fog"], "maxDist", mFogMaxDist);
        JsonHelper::GetFloat(data["fog"], "minDist", mFogMinDist);
        mFogColor = JsonHelper::GetVector3(data["fog"], "color", Vector3(0.2f, 0.5f, 0.8f));
    }

    SDL_Log("Loaded Renderer settings from %s", filePath.c_str());
    return true;
}
