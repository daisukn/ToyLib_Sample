#pragma once
#include "MathUtils.h"

class Material {
public:
    Material();

    // セッター
    void SetAmbientColor(const Vector3& color) { mAmbientColor = color; }
    void SetDiffuseColor(const Vector3& color) { mDiffuseColor = color; }
    void SetSpecularColor(const Vector3& color) { mSpecularColor = color; }
    void SetShininess(float s) { mShininess = s; };

    void SetDiffuseMap(class Texture* tex) { mDiffuseMap = tex; }
    void SetSpecularMap(class Texture* tex) { mSpecularMap = tex; }
    void SetNormalMap(class Texture* tex) { mNormalMap = tex; }

    // ゲッター
    const Vector3& GetAmbientColor() const { return mAmbientColor; }
    const Vector3& GetDiffuseColor() const { return mDiffuseColor; }
    const Vector3& GetSpecularColor() const { return mSpecularColor; }
    float GetShininess() const { return mShininess; }

    Texture* GetDiffuseMap() const { return mDiffuseMap; }
    Texture* GetSpecularMap() const { return mSpecularMap; }
    Texture* GetNormalMap() const { return mNormalMap; }

    // シェーダー転送
    void BindToShader(class Shader* shader) const;
    
private:
    Vector3 mAmbientColor{0.2f, 0.2f, 0.2f};
    Vector3 mDiffuseColor{0.8f, 0.8f, 0.8f};
    Vector3 mSpecularColor{1.0f, 1.0f, 1.0f};
    float mShininess = 32.0f;

    Texture* mDiffuseMap = nullptr;
    Texture* mSpecularMap = nullptr;
    Texture* mNormalMap = nullptr;
};
