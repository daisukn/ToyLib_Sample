#pragma once

#include "Util/json.hpp" // nlohmann json.hpp
#include <string>
#include "MathUtils.h"

namespace JsonHelper
{
    // 基本型
    bool GetInt(const nlohmann::json& obj, const char* key, int& out);
    bool GetFloat(const nlohmann::json& obj, const char* key, float& out);
    bool GetBool(const nlohmann::json& obj, const char* key, bool& out);
    bool GetString(const nlohmann::json& obj, const char* key, std::string& out);

    // 数学型（Vectorなど）
    Vector2 GetVector2(const nlohmann::json& obj, const char* key, const Vector2& defaultValue = Vector2::Zero);
    Vector3 GetVector3(const nlohmann::json& obj, const char* key, const Vector3& defaultValue = Vector3::Zero);
    Quaternion GetQuaternionFromEuler(const nlohmann::json& obj, const char* key, const Quaternion& defaultValue = Quaternion::Identity);
}
