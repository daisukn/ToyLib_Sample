#include "SkeletalMeshComponent.h"
#include "Shader.h"
#include "Mesh.h"
#include "Actor.h"
#include "Application.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"


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
        Matrix4 m = Matrix4::CreateScale(mScale);
        shader->SetMatrixUniform("uWorldTransform", m * mOwner->GetWorldTransform());

        std::vector<Matrix4> transform;
        mMesh->BoneTransform(mAnimTime, transform);

        shader->SetMatrixUniforms("uMatrixPalette", transform.data(), (unsigned int)transform.size());
        shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());


        // Vertex Arrayを描画
        std::vector<VertexArray*> va = mMesh->GetVertexArray();
        for (auto v : va)
        {
            Texture* t = mMesh->GetTexture(v->GetTextureID());
            
            if (t)
            {
                t->SetActive();
            }
            
            v->SetActive();
            glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
//            glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
        }
        
        if (mIsToon)
        {
            glFrontFace(GL_CW);
            Matrix4 m = Matrix4::CreateScale(mContourFactor * mScale);
            shader->SetMatrixUniform("uWorldTransform", m * mOwner->GetWorldTransform());
            for (auto v : va)
            {
                Texture* t = mOwner->GetApp()->GetRenderer()->GetTexture("Assets/black.png");
                if (t)
                {
                    t->SetActive();
                }
                v->SetActive();
                glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
            }
            glFrontFace(GL_CCW);
        }
        
        if (mIsGlory)
        {
            glBlendFunc(GL_ONE, GL_ONE);
            glFrontFace(GL_CW);
            Matrix4 m = Matrix4::CreateScale(mContourFactor * 1.05);
            shader->SetMatrixUniform("uWorldTransform", m * mOwner->GetWorldTransform());
            for (auto v : va)
            {
                Texture* t = mOwner->GetApp()->GetRenderer()->GetTexture("Assets/glory.png");
                if (t)
                {
                    t->SetActive();
                }
                v->SetActive();
                glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
            }
            glFrontFace(GL_CCW);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        
        
    }
}

void SkeletalMeshComponent::Update(float deltaTime)
{
    mAnimTime = deltaTime;
    
}

