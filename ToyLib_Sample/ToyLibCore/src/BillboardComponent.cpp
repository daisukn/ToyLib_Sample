#include "BillboardComponent.h"
#include "Texture.h"
#include "Shader.h"
#include "Application.h"
#include "Actor.h"
#include "Renderer.h"

#include <GL/glew.h>

BillboardComponent::BillboardComponent(class Actor* a, int order)
    : VisualComponent(a, VisualLayer::Effect3D)
    , mScale(1.0f)
{
    mDrawOrder = order;
    mOwnerActor->GetApp()->GetRenderer()->AddVisualComp(this);
}

BillboardComponent::~BillboardComponent()
{
    mOwnerActor->GetApp()->GetRenderer()->RemoveVisualComp(this);
}

void BillboardComponent::Draw(Shader* shader)
{
    if (!mIsVisible || !mTexture) return;

    if (mIsBlendAdd)
    {
        glBlendFunc(GL_ONE, GL_ONE);
    }

    Matrix4 mat = mOwnerActor->GetWorldTransform();
    Matrix4 invView = mOwnerActor->GetApp()->GetRenderer()->GetInvViewMatrix();
    invView.mat[3][0] = mat.mat[3][0];
    invView.mat[3][1] = mat.mat[3][1];
    invView.mat[3][2] = mat.mat[3][2];

    Matrix4 scaleMat = Matrix4::CreateScale(mTexture->GetWidth() * mScale, mTexture->GetHeight() * mScale, 1);
    Matrix4 world = scaleMat * Matrix4::CreateScale(mOwnerActor->GetScale()) * invView;

    shader->SetMatrixUniform("uWorldTransform", world);
    mTexture->SetActive(0);
    shader->SetVectorUniform("uPosition", Vector3(0, 0, 0));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    if (mIsBlendAdd)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}
