
#include "MeshComponent.h"
#include "Shader.h"
#include "LightingManager.h"
#include "Mesh.h"
#include "Actor.h"
#include "Application.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Material.h"

#include <GL/glew.h>
#include <vector>

// コンストラクタ
MeshComponent::MeshComponent(Actor* a, bool isSkeletal, MeshType type)
: VisualComponent(a, 100, VisualLayer::Object3D)
, mMesh(nullptr)
, mTextureIndex(0)
//, mIsVisible(true)
, mIsSkeletal(isSkeletal)
, mIsToon(false)
, mContourFactor(1.1014f)
//, mIsBlendAdd(false)
, mMeshType(type)
{
    auto renderer = mOwnerActor->GetApp()->GetRenderer();
    mShader = renderer->GetShader("Mesh");
    mShadowShader = renderer->GetShader("ShadowMesh");
    mLightingManger = renderer->GetLightingManager();
    mIsVisible = true;
    mLayer = VisualLayer::Object3D;
    mEnableShadow = true;
}

// デストラクタ
MeshComponent::~MeshComponent()
{

}

// 描画
void MeshComponent::Draw()
{
    if (!mMesh) return;
    if (mIsBlendAdd)
    {
        glBlendFunc(GL_ONE, GL_ONE);
    }
    
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
    
    // WorldマトリックスをShaderに送る
    mShader->SetMatrixUniform("uWorldTransform", mOwnerActor->GetWorldTransform());

    // Vertex Array
    std::vector<VertexArray*> va = mMesh->GetVertexArray();
    for (auto v : va)
    {
        auto mat = mMesh->GetMaterial(v->GetTextureID());
        if (mat)
        {
            mat->BindToShader(mShader, 0);
        }
        v->SetActive();
        glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
        
    // 輪郭強調用
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


VertexArray* MeshComponent::GetVertexArray(int id) const
{
    return mMesh->GetVertexArray()[id];
}


void MeshComponent::DrawShadow()
{
    if (!mMesh) return;
    
    auto renderer = mOwnerActor->GetApp()->GetRenderer();
    Matrix4 light = renderer->GetLightSpaceMatrix();
    mShadowShader->SetActive();
    
    
    // ワールドマトリックスを送る
    mShadowShader->SetMatrixUniform("uWorldTransform", mOwnerActor->GetWorldTransform());
    mShadowShader->SetMatrixUniform("uLightSpaceMatrix", light);
    
    // Vertex Arrayを描画
    std::vector<VertexArray*> va = mMesh->GetVertexArray();
    for (auto v : va)
    {
        v->SetActive();
        glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
}
