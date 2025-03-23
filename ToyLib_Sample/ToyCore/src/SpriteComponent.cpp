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
        mOwner->GetApp()->GetRenderer()->AddBackGroundSprite(this);
    }
    else
    {
        mOwner->GetApp()->GetRenderer()->AddSprite(this);
    }
}

// デストラクタ
SpriteComponent::~SpriteComponent()
{
    
    if(mIsBackGround)
    {
        mOwner->GetApp()->GetRenderer()->RemoveBackGroundSprite(this);
    }
    else
    {
        mOwner->GetApp()->GetRenderer()->RemoveSprite(this);
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
        Matrix4 world = scaleMat * mOwner->GetWorldTransform();
		
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
