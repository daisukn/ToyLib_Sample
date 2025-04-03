#include "SkeletalMeshComponent.h"
#include "Shader.h"
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
}

void SkeletalMeshComponent::SetAnimID(const unsigned int animID, const PlayMode mode)
{
    mMesh->SetAnimID(animID, mode);
}

bool SkeletalMeshComponent::GetIsPlaing() const
{
    return mMesh->GetIsPlaying();
}

void SkeletalMeshComponent::Draw(Shader* shader)
{
    if (mMesh)
    {

        // ワールドマトリックスを送る
        shader->SetMatrixUniform("uWorldTransform", mOwnerActor->GetWorldTransform());

        std::vector<Matrix4> transform;
        mMesh->BoneTransform(mAnimTime, transform);

        shader->SetMatrixUniforms("uMatrixPalette", transform.data(), (unsigned int)transform.size());
        shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());


        // Vertex Arrayを描画
        std::vector<VertexArray*> va = mMesh->GetVertexArray();
        for (auto v : va)
        {
            {
                auto mat = mMesh->GetMaterial(v->GetTextureID());
                if (mat)
                {
                    mat->BindToShader(shader);
                }
                v->SetActive();
                glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
            }
        }
        
        if (mIsToon)
        {
            glFrontFace(GL_CW);
            Matrix4 m = Matrix4::CreateScale(mContourFactor);
            shader->SetMatrixUniform("uWorldTransform", m * mOwnerActor->GetWorldTransform());
            for (auto v : va)
            {
                auto mat = mMesh->GetMaterial(v->GetTextureID());
                if (mat)
                {
                    mat->SetOverrideColor(true, Vector3(0.f, 0.f, 0.f));
                    mat->BindToShader(shader, 0);
                }
                v->SetActive();
                glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
                mat->SetOverrideColor(false, Vector3(0.f, 0.f, 0.f));

            }
            glFrontFace(GL_CCW);
        }
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

