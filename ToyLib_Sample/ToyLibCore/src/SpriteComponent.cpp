#include "SpriteComponent.h"
#include "Texture.h"
#include "Shader.h"
#include "Application.h"
#include "Renderer.h"
#include "Actor.h"
#include <GL/glew.h>

SpriteComponent::SpriteComponent(Actor* a, int drawOrder, VisualLayer layer)
    : VisualComponent(a, layer)
    , mScaleWidth(1.0f)
    , mScaleHeight(1.0f)
    , mTexWidth(0)
    , mTexHeight(0)
{
    mDrawOrder = drawOrder;
    mOwnerActor->GetApp()->GetRenderer()->AddVisualComp(this);
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
    if (!mIsVisible || mTexture == nullptr)
        return;
    SDL_Log("SpriteComponent TextureID: %u", mTexture->GetTextureID());
    SDL_Log("Texture Size: %d x %d", mTexture->GetWidth(), mTexture->GetHeight());
    
    
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

/*
 #include "Application.h"
#include "Actor.h"
#include "Renderer.h"
#include "SpriteComponent.h"
#include "Texture.h"
#include "Shader.h"

#include <GL/glew.h>

// コンストラクタ
SpriteComponent::SpriteComponent(Actor* a, int order, bool isBG)
: Component(a)
, mTexture(nullptr)
, mDrawOrder(order)
, mTexWidth(0)
, mTexHeight(0)
, mIsBackGround(isBG)
, mIsVisible(true)
, mScaleWidth(1.0f)
, mScaleHeight(1.0f)
, mIsBlendAdd(false)
{
    if(isBG)
    {
        mOwnerActor->GetApp()->GetRenderer()->AddBackGroundSprite(this);
    }
    else
    {
        mOwnerActor->GetApp()->GetRenderer()->AddSprite(this);
    }
}

// デストラクタ
SpriteComponent::~SpriteComponent()
{
    
    if(mIsBackGround)
    {
        mOwnerActor->GetApp()->GetRenderer()->RemoveBackGroundSprite(this);
    }
    else
    {
        mOwnerActor->GetApp()->GetRenderer()->RemoveSprite(this);
    }
}

// 描画
void SpriteComponent::Draw(Shader* shader)
{
    if (!mIsVisible)
    {
        return;
    }
    
    if (mIsBlendAdd)
    {
        glBlendFunc(GL_ONE, GL_ONE);
    }
    else
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
	if (mTexture)
    {
		// Textureサイズからスケールマトリックスを作成
        
        float width = static_cast<float>(mTexWidth);
        float height = static_cast<float>(mTexHeight);
        
        if (mScaleWidth != 1.0f)
        {
            width = width * mScaleWidth;
        }
        if (mScaleHeight != 1.0f)
        {
            height = height * mScaleHeight;
        }
        
        Matrix4 scaleMat = Matrix4::CreateScale(width, height, 1.0f);
    
	//	Matrix4 scaleMat = Matrix4::CreateScale(static_cast<float>(texWidth), static_cast<float>(texHeight), 1.0f);
		// ワールド座標生成
        Matrix4 world = scaleMat * mOwnerActor->GetWorldTransform();
		
        // シェーダー に送る
		shader->SetMatrixUniform("uWorldTransform", world);
		// アクティブ化
        mTexture->SetActive();
        // 描画
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}
}

// テクスチャを設定
void SpriteComponent::SetTexture(Texture* tex)
{
    mTexture = tex;
    mTexWidth = mTexture->GetWidth();
	mTexHeight = mTexture->GetHeight();
}
*/
