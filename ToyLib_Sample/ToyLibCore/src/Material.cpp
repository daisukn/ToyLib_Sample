#include "Material.h"
#include "Shader.h"

Material::Material()
    : mAmbientColor(0.2f, 0.2f, 0.2f)
    , mDiffuseColor(0.8f, 0.8f, 0.8f)
    , mSpecularColor(1.0f, 1.0f, 1.0f)
    , mShininess(32.0f)
    , mDiffuseMap(nullptr)
    , mSpecularMap(nullptr)
    , mNormalMap(nullptr)
{
}

void Material::BindToShader(Shader* shader) const
{
    if (!shader) return;

    // カラーや数値系を送る
    shader->SetVectorUniform("uMaterial.ambientColor", mAmbientColor);
    shader->SetVectorUniform("uMaterial.diffuseColor", mDiffuseColor);
    shader->SetVectorUniform("uMaterial.specularColor", mSpecularColor);
    shader->SetFloatUniform("uMaterial.shininess", mShininess);
/*
    // テクスチャ有無フラグを送る
    shader->SetUniform("uMaterial.useDiffuseMap", mDiffuseMap != nullptr);
    shader->SetUniform("uMaterial.useSpecularMap", mSpecularMap != nullptr);
    shader->SetUniform("uMaterial.useNormalMap", mNormalMap != nullptr);

    // 実際にバインド（使うテクスチャはユニット割り当て）
    int textureUnit = 0;

    if (mDiffuseMap)
    {
        mDiffuseMap->Bind(textureUnit);
        shader->SetUniform("uDiffuseMap", textureUnit);
        ++textureUnit;
    }

    if (mSpecularMap)
    {
        mSpecularMap->Bind(textureUnit);
        shader->SetUniform("uSpecularMap", textureUnit);
        ++textureUnit;
    }

    if (mNormalMap)
    {
        mNormalMap->Bind(textureUnit);
        shader->SetUniform("uNormalMap", textureUnit);
        ++textureUnit;
    }
 */
}
