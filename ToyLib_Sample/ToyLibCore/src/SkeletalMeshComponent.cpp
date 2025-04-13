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


SkeletalMeshComponent::SkeletalMeshComponent(Actor* owner)
: MeshComponent(owner, true)
, mAnimTime(0.0f)
{
    auto renderer = mOwnerActor->GetApp()->GetRenderer();
    mShader = renderer->GetShader("Skinned");
    mShadowShader = renderer->GetShader("ShadowSkinned");
}

void SkeletalMeshComponent::SetAnimID(const unsigned int animID, const PlayMode mode)
{
    mMesh->SetAnimID(animID, mode);
}

bool SkeletalMeshComponent::GetIsPlaing() const
{
    return mMesh->GetIsPlaying();
}

void SkeletalMeshComponent::Draw()
{
    if (!mMesh) return;

    auto renderer = mOwnerActor->GetApp()->GetRenderer();
    Matrix4 view = renderer->GetViewMatrix();
    Matrix4 proj = renderer->GetProjectionMatrix();
    Matrix4 light = renderer->GetLightSpaceMatrix();
 
    mShader->SetActive();
    mLightingManger->ApplyToShader(mShader.get(), view);
    mShader->SetMatrixUniform("uViewProj", view * proj);
    mShader->SetMatrixUniform("uLightSpaceMatrix", light);
    mShader->SetTextureUniform("uShadowMap", 1);
    mShader->SetFloatUniform("uShadowBias", 0.005);
    mShader->SetBooleanUniform("uUseToon", mIsToon);
    // ワールドマトリックスを送る
    mShader->SetMatrixUniform("uWorldTransform", mOwnerActor->GetWorldTransform());

    std::vector<Matrix4> transform;
    mMesh->BoneTransform(mAnimTime, transform);

    mShader->SetMatrixUniforms("uMatrixPalette", transform.data(), (unsigned int)transform.size());
    mShader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());

    // Vertex Arrayを描画
    std::vector<VertexArray*> va = mMesh->GetVertexArray();
    for (auto v : va)
    {
        {
            auto mat = mMesh->GetMaterial(v->GetTextureID());
            if (mat)
            {
                mat->BindToShader(mShader.get());
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
                mat->BindToShader(mShader.get(), 0);
            }
            v->SetActive();
            glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
            mat->SetOverrideColor(false, Vector3(0.f, 0.f, 0.f));

        }
        glFrontFace(GL_CCW);
    }
}

void SkeletalMeshComponent::DrawShadow(Shader* shader, const Matrix4& lightSpaceMatrix)
{
    if (!mMesh) return;
    
    
    // ワールドマトリックスを送る
    shader->SetMatrixUniform("uWorldTransform", mOwnerActor->GetWorldTransform());

    std::vector<Matrix4> transform;
    mMesh->BoneTransform(0, transform);

    shader->SetMatrixUniforms("uMatrixPalette", transform.data(), (unsigned int)transform.size());
    shader->SetMatrixUniform("uLightSpaceMatrix", lightSpaceMatrix);

    // Vertex Arrayを描画
    std::vector<VertexArray*> va = mMesh->GetVertexArray();
    for (auto v : va)
    {
        v->SetActive();
        glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
    
}

void SkeletalMeshComponent::Update(float deltaTime)
{
    mAnimTime = deltaTime;
    
}

