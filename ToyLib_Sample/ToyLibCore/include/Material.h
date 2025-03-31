#pragma once
#include "MathUtil.h"
#include

class Material {
public:
    Material();

    // セッター
    void SetAmbientColor(const Vector3& color);
    void SetDiffuseColor(const Vector3& color);
    void SetSpecularColor(const Vector3& color);
    void SetShininess(float s);

    void SetDiffuseMap(class Texture* tex);
    void SetSpecularMap(class Texture* tex);
    void SetNormalMap(class Texture* tex);

    // ゲッター
    const Vector3& GetAmbientColor() const;
    const Vector3& GetDiffuseColor() const;
    const Vector3& GetSpecularColor() const;
    float GetShininess() const;

    Texture* GetDiffuseMap() const;
    Texture* GetSpecularMap() const;
    Texture* GetNormalMap() const;

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
