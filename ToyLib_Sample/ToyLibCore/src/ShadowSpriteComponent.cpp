#include "ShadowSpriteComponent.h"
#include "Actor.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "Application.h"
#include "Renderer.h"
#include "LightingManager.h"

ShadowSpriteComponent::ShadowSpriteComponent(Actor* owner, int drawOrder)
    : VisualComponent(owner, drawOrder)
    , mTexture(nullptr)
    , mScaleWidth(1.0f)
    , mScaleHeight(1.0f)
{
    mLayer = VisualLayer::Object3D; // 足元に描く
    mOwnerActor->GetApp()->GetRenderer()->AddVisualComp(this);
    mType = VisualType::ShadowSprite;
}

ShadowSpriteComponent::~ShadowSpriteComponent()
{
    mOwnerActor->GetApp()->GetRenderer()->RemoveVisualComp(this);
}

void ShadowSpriteComponent::SetTexture(Texture* tex)
{
    mTexture = tex;
}

void ShadowSpriteComponent::Draw(Shader* shader)
{
    if (!mIsVisible || mTexture == nullptr) return;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float width = static_cast<float>(mTexture->GetWidth()) * mScaleWidth;
    float height = static_cast<float>(mTexture->GetHeight()) * mScaleHeight;
    Matrix4 scale = Matrix4::CreateScale(width * mOffsetScale/2, height * mOffsetScale*2, 1.0f);

    // 光の方向から回転角を計算（XZ平面）
    Vector3 lightDir = mOwnerActor->GetApp()->GetRenderer()->GetLightingManager()->GetLightDirection();
    //mOwnerActor->GetApp()->GetRenderer()->GetDirLight().Direction;
    lightDir.y = 0.0f;
    if (lightDir.LengthSq() < 0.0001f) lightDir = Vector3(0, 0, 1);
    lightDir.Normalize();
    float angle = atan2f(lightDir.x, lightDir.z);
    Matrix4 rotY = Matrix4::CreateRotationY(angle);

    Matrix4 rotX = Matrix4::CreateRotationX(Math::ToRadians(90.0f));
    Matrix4 trans = Matrix4::CreateTranslation(mOwnerActor->GetPosition() + mOffsetPosition);
    Matrix4 world = scale * rotX * rotY * trans;

    shader->SetMatrixUniform("uWorldTransform", world);
    mTexture->SetActive(2); // ShadowSprite用ユニット
    shader->SetTextureUniform("uTexture", 2);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
