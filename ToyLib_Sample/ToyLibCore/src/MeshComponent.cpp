
#include "MeshComponent.h"
#include "Shader.h"
#include "Mesh.h"
#include "Actor.h"
#include "Application.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"

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
, mIsGlory(false)
, mMeshType(type)
, mScale(1.0f)
{

    switch(mMeshType)
    {
        case MESH_EFFECT:
            mOwner->GetApp()->GetRenderer()->AddEffectMeshComp(this);
            break;
        case MESH_BG:
            mOwner->GetApp()->GetRenderer()->AddBackGroudMeshComp(this);
            break;
        case MESH_NORMAL:
            mOwner->GetApp()->GetRenderer()->AddMeshComp(this);
            break;
    }
}

// デストラクタ
MeshComponent::~MeshComponent()
{
    if (mOwner && mOwner->GetApp() && mOwner->GetApp()->GetRenderer())
    {
        switch(mMeshType)
        {
            case MESH_EFFECT:
                mOwner->GetApp()->GetRenderer()->RemoveEffectMeshComp(this);
                break;
            case MESH_BG:
                mOwner->GetApp()->GetRenderer()->RemoveBackGroudMeshComp(this);
                break;
            case MESH_NORMAL:
                mOwner->GetApp()->GetRenderer()->RemoveMeshComp(this);
                break;
        }
    }
}

// 描画
void MeshComponent::Draw(Shader* s)
{
	if (mMesh)
	{
        if (mIsBlendAdd)
        {
            glBlendFunc(GL_ONE, GL_ONE);
        }
        
        // WorldマトリックスをShaderに送る
        Matrix4 m = Matrix4::CreateScale(mScale);
        s->SetMatrixUniform("uWorldTransform", m * mOwner->GetWorldTransform());

		// SpecPowerを送る
        s->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());


		// Vertex Array
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
        }
        
        // 輪郭強調用
        if (mIsToon)
        {
            glFrontFace(GL_CW);
            Matrix4 m = Matrix4::CreateScale(mContourFactor * mScale);
            s->SetMatrixUniform("uWorldTransform", m * mOwner->GetWorldTransform());
            s->SetVectorUniform("uSolColor", Vector3(0.f, 0.f, 0.f));
            for (auto v : va)
            {/*
                Texture* t = mOwner->GetApp()->GetRenderer()->GetTexture("Assets/black.png");
                if (t)
                {
                    //t->SetActive();
                }
              */
                v->SetActive();
                glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
            }
            glFrontFace(GL_CCW);
        }
        
        if (mIsGlory)
        {
            glBlendFunc(GL_ONE, GL_ONE);
            glFrontFace(GL_CW);
            Matrix4 m = Matrix4::CreateScale(mContourFactor * mScale);
            s->SetMatrixUniform("uWorldTransform", m * mOwner->GetWorldTransform());
            for (auto v : va)
            {
                Texture* t = mMesh->GetTexture(v->GetTextureID());
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

