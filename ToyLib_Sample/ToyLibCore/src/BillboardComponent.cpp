#include "BillboardComponent.h"
#include "Texture.h"
#include "Shader.h"
#include "Application.h"
#include "Actor.h"
#include "Renderer.h"

#include <GL/glew.h>

BillboardComponent::BillboardComponent(class Actor* a, int drawOrder)
: VisualComponent(a, drawOrder, VisualLayer::Object3D)
, mScale(1.0f)
{
    mDrawOrder = drawOrder;
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
/*
void BillboardComponent::DrawShadow(class Shader* shader)
{
    if (!mIsVisible || !mTexture) return;

    // ビルボードの位置を取得
    Vector3 pos = mOwnerActor->GetPosition();

    // ライト方向（逆方向を向かせる）
    Vector3 lightDir = mOwnerActor->GetApp()->GetRenderer()->GetDirLight().Direction;
    Vector3 look = Vector3(-lightDir.x, -lightDir.y, -lightDir.z);
    look.y = 0.0f; // Y軸は無視（水平に回転）
    if (Math::NearZero(look.LengthSq()))
    {
        look = Vector3(0, 0, 1); // デフォルト方向
    }
    look.Normalize();

    // Y軸回転角を取得
    float angle = atan2f(look.x, look.z);
    Matrix4 rotY = Matrix4::CreateRotationY(angle);

    // スケール（影の大きさを調整）
    float shadowScale = mTexture->GetWidth() * mScale * 0.5f;
    Matrix4 scaleMat = Matrix4::CreateScale(shadowScale, shadowScale, 1.0f);
    Matrix4 transMat = Matrix4::CreateTranslation(pos);

    Matrix4 world = scaleMat * rotY * transMat;

    // シェーダーに送信（uLightSpaceMatrix は呼び出し元でセット済み）
    shader->SetMatrixUniform("uWorldTransform", world);

    // ビルボードはSpriteVertsを使う
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
*/
