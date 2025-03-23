#include "ParticleComponent.h"
#include "Actor.h"
#include "Shader.h"
#include "Texture.h"
#include "Application.h"
#include "Renderer.h"
#include "VertexArray.h"
#include "MathUtils.h"

#include <vector>
#include <random>



// コンストラクタ
ParticleComponent::ParticleComponent(Actor* a, int drawOrder)
    : Component(a)
    , mIsVisible(false)
    , mPosition(Vector3(0.0f, 0.0f, 0.0f))
    , mNumParts(0)
    , mLifeTime(0.0f)
    , mTotalLife(0.0f)
    , mPartLifecycle(0.0f)
    , mPartSize(0.0f)
    , mParticleMode(P_SPARK)
    , mPartSpeed(2.0f)
    , mIsBlendAdd(false)
{
    mOwner->GetApp()->GetRenderer()->AddParticleComp(this);
}
// デストラクタ
ParticleComponent::~ParticleComponent()
{
    // レンダラーからRemoveする
    mParts.clear();
    mOwner->GetApp()->GetRenderer()->RemoveParticleComp(this);
}

// 描画（Rendererから呼ばれる）
void ParticleComponent::Draw(Shader *shader)
{
    if (!mIsVisible) return;
    
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
        Matrix4 scaleMat = Matrix4::CreateScale(mPartSize, mPartSize, 1);
        Matrix4 world = scaleMat * Matrix4::CreateScale(mOwner->GetScale()) * invVew;

        // シェーダー に送る
        shader->SetMatrixUniform("uWorldTransform", world);
        mTexture->SetActive();
        
        for (int i = 0; i < mNumParts; i++)
        {
            if (mParts[i].isVisible)
            {
                shader->SetVectorUniform("uPosition", mParts[i].pos);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            }
        }
        
        if (mIsBlendAdd)
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        
    }
    
}

// テクスチャ設定
void ParticleComponent::SetTexture(Texture *tex)
{
    mTexture = tex;
}

// パーツを生成
void ParticleComponent::GenerateParts()
{
    
    // 乱数初期化
    std::random_device rnd;

    if (mIsVisible)
    {
    
        for (int i = 0; i < mNumParts; i++)
        {
            if (!mParts[i].isVisible)
            {
                // スピードの調整がちょっと強引なので再考する

                
                float x, y ,z;
                if (!Math::NearZero(mPartSpeed))
                {
                    x  = (float)(rnd() % (int)mPartSpeed);//5) / 10;
                    y  = (float)(rnd() % (int)mPartSpeed);//5) / 10;
                    z  = (float)(rnd() % (int)mPartSpeed);//5) / 10;
                }
                else
                {
                    x = y = z = 0.0f;
                }
                
                if(rand()%2) x *= -1;
                if(rand()%2) y *= -1;
                if(rand()%2) z *= -1;

                // パーツのパラメータをセット
                mParts[i].pos = mPosition;
                mParts[i].dir = Vector3(x, y, z);
                mParts[i].isVisible = true;
                mParts[i].lifeTime = 0.0f;
                mParts[i].size = mPartSize;
                
                break;
            }
        }
    }
}

// Update（Actorから呼ばれる）
void ParticleComponent::Update(float deltaTime)
{
    mLifeTime += deltaTime;
    
    // 時間経過で消える
    if (mLifeTime > mTotalLife)
    {
        mIsVisible = false;
    }
    
    
    // パーツの位置を進める
    for (int i = 0; i < mNumParts; i++)
    {
         
        if(mParts[i].isVisible)
        {
            // 重力を反映
            if (mParticleMode == P_WATER)
            {
                mParts[i].dir.y -= 0.05f;
            }
            if (mParticleMode == P_SMOKE)
            {
                mParts[i].dir.y += 0.05f;
            }

            
            mParts[i].lifeTime += deltaTime;
            mParts[i].pos += mParts[i].dir * deltaTime;
        
            if(mParts[i].lifeTime > mPartLifecycle)
            {
                mParts[i].isVisible = false;
            }
        }
    }
    
    std::random_device rnd;
    if(rnd()%2 == 0)
    {
        // パーツを追加（乱数で回数を間引くのもあり）
        GenerateParts();
    }
}

// パーティクル初期化
//（アクターとの相対位置、粒の総数、総表示時間、粒の表示時間、粒の大きさ、重力モード（未実装））
void ParticleComponent::CreateParticles(Vector3 pos, unsigned int num, float life, float part_life, float size, ParticleMode mode)
{
    // すでに表示されていたらキャンセル
    if(!mIsVisible)
    {
        mPosition = pos;
        mIsVisible = true;
        mNumParts = num;
        mLifeTime = 0.0f;
        mTotalLife = life;
        mPartLifecycle =  part_life;
        mPartSize = size;
        
        
        mParts.resize(mNumParts);
        //GenerateParts();
        
        mParticleMode = mode;
    }

}
