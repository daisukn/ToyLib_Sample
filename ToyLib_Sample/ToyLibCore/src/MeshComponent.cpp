
#include "MeshComponent.h"
#include "Shader.h"
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
: Component(a)
, mMesh(nullptr)
, mTextureIndex(0)
, mIsVisible(true)
, mIsSkeletal(isSkeletal)
, mIsToon(false)
, mContourFactor(1.1014f)
, mIsBlendAdd(false)
, mMeshType(type)
{

    switch(mMeshType)
    {
        case MESH_EFFECT:
            mOwnerActor->GetApp()->GetRenderer()->AddEffectMeshComp(this);
            break;
        case MESH_BG:
            mOwnerActor->GetApp()->GetRenderer()->AddBackGroudMeshComp(this);
            break;
        case MESH_NORMAL:
            mOwnerActor->GetApp()->GetRenderer()->AddMeshComp(this);
            break;
    }
}

// デストラクタ
MeshComponent::~MeshComponent()
{
    if (mOwnerActor && mOwnerActor->GetApp() && mOwnerActor->GetApp()->GetRenderer())
    {
        switch(mMeshType)
        {
            case MESH_EFFECT:
                mOwnerActor->GetApp()->GetRenderer()->RemoveEffectMeshComp(this);
                break;
            case MESH_BG:
                mOwnerActor->GetApp()->GetRenderer()->RemoveBackGroudMeshComp(this);
                break;
            case MESH_NORMAL:
                mOwnerActor->GetApp()->GetRenderer()->RemoveMeshComp(this);
                break;
        }
    }
}

// 描画
void MeshComponent::Draw(Shader* shader)
{
	if (mMesh)
	{
        if (mIsBlendAdd)
        {
            glBlendFunc(GL_ONE, GL_ONE);
        }
        
        // WorldマトリックスをShaderに送る
        shader->SetMatrixUniform("uWorldTransform", mOwnerActor->GetWorldTransform());

		// Vertex Array
        std::vector<VertexArray*> va = mMesh->GetVertexArray();
        for (auto v : va)
        {
            auto mat = mMesh->GetMaterial(v->GetTextureID());
            if (mat)
            {
                mat->BindToShader(shader, 0);
            }
            v->SetActive();
            glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
        }
        
        // 輪郭強調用
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
        
        
        if (mIsBlendAdd)
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        
	}
}


VertexArray* MeshComponent::GetVertexArray(int id) const
{
    return mMesh->GetVertexArray()[id];
}


void MeshComponent::DrawShadow(Shader* shader, const Matrix4& lightSpaceMatrix)
{
    if (!mMesh) return;
    
    
    // ワールドマトリックスを送る
    shader->SetMatrixUniform("uWorldTransform", mOwnerActor->GetWorldTransform());
    

    //shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());
    shader->SetMatrixUniform("uLightSpaceMatrix", lightSpaceMatrix);
    
    // Vertex Arrayを描画
    std::vector<VertexArray*> va = mMesh->GetVertexArray();
    for (auto v : va)
    {

        v->SetActive();
        glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
}
