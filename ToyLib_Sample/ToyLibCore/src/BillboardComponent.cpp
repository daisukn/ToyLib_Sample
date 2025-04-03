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
    mType = VisualType::Billboard;
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
    
    // カメラと位置取得
    Vector3 pos = mOwnerActor->GetPosition();
    Matrix4 view = mOwnerActor->GetApp()->GetRenderer()->GetViewMatrix();
    Matrix4 invView = view;
    invView.Invert();
    Vector3 cameraPos = invView.GetTranslation();

    // 回転角（Y軸）
    Vector3 toCamera = pos - cameraPos;
    toCamera.y = 0.0f;
    toCamera.Normalize();

    float angle = atan2f(toCamera.x, toCamera.z);
    Matrix4 rotY = Matrix4::CreateRotationY(angle);

    // スケール＋平行移動
    float scale = mScale * mOwnerActor->GetScale();
    Matrix4 scaleMat = Matrix4::CreateScale(mTexture->GetWidth() * scale,
                                            mTexture->GetHeight() * scale, 1.0f);
    Matrix4 translate = Matrix4::CreateTranslation(pos);

    // 最終行列
    Matrix4 world = scaleMat * rotY * translate;
    shader->SetMatrixUniform("uWorldTransform", world);

    mTexture->SetActive(2);
    shader->SetTextureUniform("uTexture", 2);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    if (mIsBlendAdd)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}
