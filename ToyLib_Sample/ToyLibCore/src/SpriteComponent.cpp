#include "SpriteComponent.h"
#include "Texture.h"
#include "Shader.h"
#include "Application.h"
#include "Renderer.h"
#include "Actor.h"
#include <GL/glew.h>

SpriteComponent::SpriteComponent(Actor* a, int drawOrder, VisualLayer layer)
    : VisualComponent(a, drawOrder, layer)
    , mScaleWidth(1.0f)
    , mScaleHeight(1.0f)
    , mTexWidth(0)
    , mTexHeight(0)
{
    mDrawOrder = drawOrder;
    mOwnerActor->GetApp()->GetRenderer()->AddVisualComp(this);
    mType = VisualType::Sprite;
}

SpriteComponent::~SpriteComponent()
{
    mOwnerActor->GetApp()->GetRenderer()->RemoveVisualComp(this);
}

void SpriteComponent::SetTexture(Texture* tex)
{
    VisualComponent::SetTexture(tex);
    mTexWidth = tex->GetWidth();
    mTexHeight = tex->GetHeight();
}

void SpriteComponent::Draw(Shader* shader)
{

    // 表示可能かチェック
    if (!mIsVisible || mTexture == nullptr) return;
    //SDL_Log("SpriteComponent TextureID: %u", mTexture->GetTextureID());
    //SDL_Log("Texture Size: %d x %d", mTexture->GetWidth(), mTexture->GetHeight());
    
    
    glDisable(GL_DEPTH_TEST);           // UIなどZ不要な場合
    glDepthMask(GL_FALSE);              // Zバッファ書き込みOFF
    glEnable(GL_BLEND);                 // アルファブレンドON
    glBlendFunc(mIsBlendAdd ? GL_ONE : GL_SRC_ALPHA,
                mIsBlendAdd ? GL_ONE : GL_ONE_MINUS_SRC_ALPHA);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // カラーバッファは有効

    // 座標計算
    float width = static_cast<float>(mTexWidth) * mScaleWidth;
    float height = static_cast<float>(mTexHeight) * mScaleHeight;

    Matrix4 scaleMat = Matrix4::CreateScale(width, height, 1.0f);
    Matrix4 world = scaleMat * mOwnerActor->GetWorldTransform();

    // シェーダー、テクスチャを設定
    mTexture->SetActive(2);
    shader->SetTextureUniform("uTexture", 2);
    shader->SetMatrixUniform("uWorldTransform", world);

    // 描画
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    
    // 戻す
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}
