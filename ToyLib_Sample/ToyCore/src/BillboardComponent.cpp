#include "BillboardComponent.h"
#include "Texture.h"
#include "Shader.h"
#include "Application.h"
#include "Actor.h"
#include "Renderer.h"

BillboardComponent::BillboardComponent(class Actor* a, int order)
    : Component(a)
    , mDrawOrder(order)
    , mScale(1.0f)
    , mIsBlendAdd(false)
    , mIsVisible(false)
{
    mOwner->GetApp()->GetRenderer()->AddBillboardComp(this);
}

BillboardComponent::~BillboardComponent()
{
    mOwner->GetApp()->GetRenderer()->RemoveBillboardComp(this);
}


void BillboardComponent::Draw(Shader* shader)
{
    if(!mIsVisible) return;
    
    if (mTexture)
    {
        if (mIsBlendAdd)
        {
            glBlendFunc(GL_ONE, GL_ONE);
        }
        
        // Ownerのマトリックスを取得（Positionでも良いかもしれない。）
        Matrix4 mat = mOwner->GetWorldTransform();
        
        // ビルボーディング
        // ビューマトリックスの逆行列の座標を差し替えて流用
        Matrix4 invVew = mOwner->GetApp()->GetRenderer()->GetInvViewMatrix();
        // 座標差し替え
        invVew.mat[3][0] = mat.mat[3][0];
        invVew.mat[3][1] = mat.mat[3][1];
        invVew.mat[3][2] = mat.mat[3][2];
        
        // スケールを復元
        Matrix4 scaleMat = Matrix4::CreateScale(mTexture->GetWidth() * mScale, mTexture->GetHeight() * mScale, 1);
        Matrix4 world = scaleMat * Matrix4::CreateScale(mOwner->GetScale()) * invVew;

        // シェーダー に送る
        shader->SetMatrixUniform("uWorldTransform", world);
        mTexture->SetActive();
        

        shader->SetVectorUniform("uPosition", Vector3(0,0,0));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        if (mIsBlendAdd)
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }
}

void BillboardComponent::SetTexture(Texture* tex)
{
    mTexture = tex;
}

void BillboardComponent::Update(float deltaTime)
{
    
}
