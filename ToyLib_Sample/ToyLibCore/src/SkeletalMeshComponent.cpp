#include "SkeletalMeshComponent.h"
#include "Shader.h"
#include "LightingManager.h"
#include "Mesh.h"
#include "Actor.h"
#include "Application.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Material.h"
#include "AnimationPlayer.h"

SkeletalMeshComponent::SkeletalMeshComponent(Actor* a, int drawOrder, VisualLayer layer)
: MeshComponent(a, drawOrder, layer,  true)
, mAnimTime(0.0f)
, mAnimPlayer(nullptr)
{
    auto renderer = mOwnerActor->GetApp()->GetRenderer();
    mShader = renderer->GetShader("Skinned");
    mShadowShader = renderer->GetShader("ShadowSkinned");
}

void SkeletalMeshComponent::SetAnimID(const unsigned int animID, const bool mode)
{
    if (mAnimPlayer)
    {
        mAnimPlayer->Play(animID, true);
    }
}
/*
bool SkeletalMeshComponent::GetIsPlaing() const
{
    return mMesh->GetIsPlaying();
}
*/
/*
void SkeletalMeshComponent::Draw()
{
    if (!mMesh) return;
    if (mIsBlendAdd)
    {
        glBlendFunc(GL_ONE, GL_ONE);
    }
    
    // シャドウマップテクスチャ有効化
    mShadowMapTexture->SetActive(1);
    auto renderer = mOwnerActor->GetApp()->GetRenderer();
    Matrix4 view = renderer->GetViewMatrix();
    Matrix4 proj = renderer->GetProjectionMatrix();
    Matrix4 light = renderer->GetLightSpaceMatrix();
 
    mShader->SetActive();
    mLightingManger->ApplyToShader(mShader, view);
    mShader->SetMatrixUniform("uViewProj", view * proj);
    mShader->SetMatrixUniform("uLightSpaceMatrix", light);
    mShader->SetTextureUniform("uShadowMap", 1);
    mShader->SetFloatUniform("uShadowBias", 0.005);
    mShader->SetBooleanUniform("uUseToon", mIsToon);
    // ワールドマトリックスを送る
    mShader->SetMatrixUniform("uWorldTransform", mOwnerActor->GetWorldTransform());

    std::vector<Matrix4> transform;
    if (mAnimPlayer)
    {
        transform = mAnimPlayer->GetFinalMatrices();
    }
    else
    {
        mMesh->BoneTransform(0.0f, transform); // Fallback用
    }

    mShader->SetMatrixUniforms("uMatrixPalette", transform.data(), (unsigned int)transform.size());
    mShader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());

    // Vertex Arrayを描画
    auto va = mMesh->GetVertexArray();
    for (auto v : va)
    {
        {
            auto mat = mMesh->GetMaterial(v->GetTextureID());
            if (mat)
            {
                mat->BindToShader(mShader);
            }
            v->SetActive();
            glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
        }
    }
    
    if (mIsToon)
    {
        glFrontFace(GL_CW);
        Matrix4 m = Matrix4::CreateScale(mContourFactor);
        mShader->SetMatrixUniform("uWorldTransform", m * mOwnerActor->GetWorldTransform());
        for (auto v : va)
        {
            auto mat = mMesh->GetMaterial(v->GetTextureID());
            if (mat)
            {
                mat->SetOverrideColor(true, Vector3(0.f, 0.f, 0.f));
                mat->BindToShader(mShader, 0);
            }
            v->SetActive();
            glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
            mat->SetOverrideColor(false, Vector3(0.f, 0.f, 0.f));

        }
        glFrontFace(GL_CCW);
    }
}
*/
void SkeletalMeshComponent::Draw()
{
    if (!mMesh) return;
    if (mIsBlendAdd)
    {
        glBlendFunc(GL_ONE, GL_ONE);
    }

    mShadowMapTexture->SetActive(1);
    auto renderer = mOwnerActor->GetApp()->GetRenderer();
    Matrix4 view = renderer->GetViewMatrix();
    Matrix4 proj = renderer->GetProjectionMatrix();
    Matrix4 light = renderer->GetLightSpaceMatrix();

    mShader->SetActive();
    mLightingManger->ApplyToShader(mShader, view);
    mShader->SetMatrixUniform("uViewProj", view * proj);
    mShader->SetMatrixUniform("uLightSpaceMatrix", light);
    mShader->SetTextureUniform("uShadowMap", 1);
    mShader->SetFloatUniform("uShadowBias", 0.005);
    mShader->SetBooleanUniform("uUseToon", mIsToon);
    mShader->SetMatrixUniform("uWorldTransform", mOwnerActor->GetWorldTransform());

    // アニメーション行列取得
    std::vector<Matrix4> transforms =
        mAnimPlayer ? mAnimPlayer->GetFinalMatrices() : std::vector<Matrix4>();

    mShader->SetMatrixUniforms("uMatrixPalette", transforms.data(), (unsigned int)transforms.size());
    mShader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());

    // VertexArray 描画
    auto va = mMesh->GetVertexArray();
    for (auto v : va)
    {
        auto mat = mMesh->GetMaterial(v->GetTextureID());
        if (mat)
        {
            mat->BindToShader(mShader);
        }
        v->SetActive();
        glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }

    // トゥーン輪郭描画（そのまま）
    if (mIsToon)
    {
        glFrontFace(GL_CW);
        Matrix4 m = Matrix4::CreateScale(mContourFactor);
        mShader->SetMatrixUniform("uWorldTransform", m * mOwnerActor->GetWorldTransform());
        for (auto v : va)
        {
            auto mat = mMesh->GetMaterial(v->GetTextureID());
            if (mat)
            {
                mat->SetOverrideColor(true, Vector3(0.f, 0.f, 0.f));
                mat->BindToShader(mShader, 0);
            }
            v->SetActive();
            glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
            mat->SetOverrideColor(false, Vector3(0.f, 0.f, 0.f));
        }
        glFrontFace(GL_CCW);
    }

    if (mIsBlendAdd)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}
/*
void SkeletalMeshComponent::DrawShadow()
{
    if (!mMesh) return;
    
    
    auto renderer = mOwnerActor->GetApp()->GetRenderer();
    Matrix4 light = renderer->GetLightSpaceMatrix();
    mShadowShader->SetActive();
    
    // ワールドマトリックスを送る
    mShadowShader->SetMatrixUniform("uWorldTransform", mOwnerActor->GetWorldTransform());

    std::vector<Matrix4> transform;
    if (mAnimPlayer)
    {
        transform = mAnimPlayer->GetFinalMatrices();
    }
    else
    {
        mMesh->BoneTransform(0.0f, transform); // Fallback用
    }

    mShadowShader->SetMatrixUniforms("uMatrixPalette", transform.data(), (unsigned int)transform.size());
    mShadowShader->SetMatrixUniform("uLightSpaceMatrix", light);

    // Vertex Arrayを描画
    auto va = mMesh->GetVertexArray();
    for (auto v : va)
    {
        v->SetActive();
        glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
    
}
*/
void SkeletalMeshComponent::DrawShadow()
{
    if (!mMesh) return;

    auto renderer = mOwnerActor->GetApp()->GetRenderer();
    Matrix4 light = renderer->GetLightSpaceMatrix();

    mShadowShader->SetActive();
    mShadowShader->SetMatrixUniform("uWorldTransform", mOwnerActor->GetWorldTransform());

    //
    std::vector<Matrix4> transforms =
        mAnimPlayer ? mAnimPlayer->GetFinalMatrices() : std::vector<Matrix4>();

    mShadowShader->SetMatrixUniforms("uMatrixPalette", transforms.data(), (unsigned int)transforms.size());
    mShadowShader->SetMatrixUniform("uLightSpaceMatrix", light);

    auto va = mMesh->GetVertexArray();
    for (auto v : va)
    {
        v->SetActive();
        glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
}



void SkeletalMeshComponent::Update(float deltaTime)
{
    if (mAnimPlayer)
    {
        mAnimPlayer->Update(deltaTime);

        // 例：再生終了していたらIdleに戻す
        if (mAnimPlayer->IsFinished())
        {
            int mIdleAnimID = 17;
            mAnimPlayer->Play(mIdleAnimID); // mIdleAnimID はあらかじめ決めておく
        }
    }
    
}

void SkeletalMeshComponent::SetMesh(std::shared_ptr<Mesh> mesh)
{
    MeshComponent::SetMesh(mesh);
    mAnimPlayer = std::make_unique<AnimationPlayer>(mesh);
}
