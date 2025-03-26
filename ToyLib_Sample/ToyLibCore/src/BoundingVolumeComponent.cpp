#include "BoundingVolumeComponent.h"
#include "Actor.h"
#include "Polygon.h"
#include "VertexArray.h"
#include "Shader.h"

#include "Application.h"
#include "Renderer.h"
#include "Texture.h"
#include "Polygon.h"

#include <vector>
#include <algorithm>

const int NUM_VERTEX = 12;

// コンストラクタ
BoundingVolumeComponent::BoundingVolumeComponent(Actor* a)
//    : Component(a)
: DebuggerComponent(a)
, mRadius(0.0f)
, mIsVisible(false)
{
    mBoundingBox = std::make_unique<Cube>();
    mObb = std::make_unique<OBB>();
    mPolygons.reset(new Polygon[NUM_VERTEX]);
    
}

// デストラクタ
BoundingVolumeComponent::~BoundingVolumeComponent()
{

}


void BoundingVolumeComponent::OnUpdateWorldTransform()
{
    // バウンディングボックスパラメータをセット
    mObb->pos = mOwner->GetPosition();
    float sc1 = mOwner->GetScale();
    mObb->max = mBoundingBox->max * sc1;
    mObb->min = mBoundingBox->min * sc1;
    
    Quaternion q1 = mOwner->GetRotation();
    Matrix4 mRot1 = Matrix4::CreateFromQuaternion(q1);
      
    mObb->axisX = mRot1.GetXAxis();
    mObb->axisY = mRot1.GetYAxis();
    mObb->axisZ = mRot1.GetZAxis();

    // 半径を設定
    mObb->radius = //Vector3::Dot(comp1->GetBoundingBox()->max, comp1->GetBoundingBox()->min);
              Vector3((fabsf(mObb->max.x) + fabsf(mObb->min.x)) / 2,
                        (fabsf(mObb->max.y) + fabsf(mObb->min.y)) / 2,
                        (fabsf(mObb->max.z) + fabsf(mObb->min.z)) / 2);
    

    // 角度、AXIS、ちょっと怪しいかも。。。
    mObb->rot = Vector3(mObb->axisX.x, mObb->axisY.y, mObb->axisZ.z);
    
    
    // バウンディングスフィア
    mRadius = mObb->radius.Length();
    
    
}

// バウンディングボリュームを計算
void BoundingVolumeComponent::ComputeBoundingVolume(const std::vector<VertexArray *> va)
{
    
    // 複数VertexArrayをまとめて計算
    for (const auto& v : va)
    {
        const auto& polygons = v->GetPolygons();
        for (const auto& p : polygons)
        {
            const auto* poly = p.get();

            mBoundingBox->min.x = std::min({mBoundingBox->min.x, poly->a.x, poly->b.x, poly->c.x});
            mBoundingBox->max.x = std::max({mBoundingBox->max.x, poly->a.x, poly->b.x, poly->c.x});

            mBoundingBox->min.y = std::min({mBoundingBox->min.y, poly->a.y, poly->b.y, poly->c.y});
            mBoundingBox->max.y = std::max({mBoundingBox->max.y, poly->a.y, poly->b.y, poly->c.y});

            mBoundingBox->min.z = std::min({mBoundingBox->min.z, poly->a.z, poly->b.z, poly->c.z});
            mBoundingBox->max.z = std::max({mBoundingBox->max.z, poly->a.z, poly->b.z, poly->c.z});
        }
    }
    
    CreateVArray();
    CreatePolygons();
    
}

// ポリゴンデータを生成
void BoundingVolumeComponent::CreatePolygons()
{


    
    // 0, 1, 3
    mPolygons[0].a = Vector3(mBoundingBox->min.x, mBoundingBox->min.y, mBoundingBox->min.z);
    mPolygons[0].b = Vector3(mBoundingBox->min.x, mBoundingBox->max.y, mBoundingBox->min.z);
    mPolygons[0].c = Vector3(mBoundingBox->max.x, mBoundingBox->max.y, mBoundingBox->min.z);
    
    // 3, 2, 0
    mPolygons[1].a = Vector3(mBoundingBox->max.x, mBoundingBox->max.y, mBoundingBox->min.z);
    mPolygons[1].b = Vector3(mBoundingBox->max.x, mBoundingBox->min.y, mBoundingBox->min.z);
    mPolygons[1].c = Vector3(mBoundingBox->min.x, mBoundingBox->min.y, mBoundingBox->min.z);
    
    // 4, 2, 3
    mPolygons[2].a = Vector3(mBoundingBox->max.x, mBoundingBox->min.y, mBoundingBox->max.z);
    mPolygons[2].b = Vector3(mBoundingBox->max.x, mBoundingBox->min.y, mBoundingBox->min.z);
    mPolygons[2].c = Vector3(mBoundingBox->max.x, mBoundingBox->max.y, mBoundingBox->min.z);
    
    // 3, 5, 4
    mPolygons[3].a = Vector3(mBoundingBox->max.x, mBoundingBox->max.y, mBoundingBox->min.z);
    mPolygons[3].b = Vector3(mBoundingBox->max.x, mBoundingBox->max.y, mBoundingBox->max.z);
    mPolygons[3].c = Vector3(mBoundingBox->max.x, mBoundingBox->min.y, mBoundingBox->max.z);
    
    // 5, 4, 6
    mPolygons[4].a = Vector3(mBoundingBox->max.x, mBoundingBox->max.y, mBoundingBox->max.z);
    mPolygons[4].b = Vector3(mBoundingBox->max.x, mBoundingBox->min.y, mBoundingBox->max.z);
    mPolygons[4].c = Vector3(mBoundingBox->min.x, mBoundingBox->min.y, mBoundingBox->max.z);
    
    // 6, 7, 5
    mPolygons[5].a = Vector3(mBoundingBox->min.x, mBoundingBox->min.y, mBoundingBox->max.z);
    mPolygons[5].b = Vector3(mBoundingBox->min.x, mBoundingBox->max.y, mBoundingBox->max.z);
    mPolygons[5].c = Vector3(mBoundingBox->max.x, mBoundingBox->max.y, mBoundingBox->max.z);
    
    // 0, 6, 7
    mPolygons[6].a = Vector3(mBoundingBox->min.x, mBoundingBox->min.y, mBoundingBox->min.z);
    mPolygons[6].b = Vector3(mBoundingBox->min.x, mBoundingBox->min.y, mBoundingBox->max.z);
    mPolygons[6].c = Vector3(mBoundingBox->max.x, mBoundingBox->max.y, mBoundingBox->max.z);
    
    // 7. 1. 0
    mPolygons[7].a = Vector3(mBoundingBox->max.x, mBoundingBox->max.y, mBoundingBox->max.z);
    mPolygons[7].b = Vector3(mBoundingBox->min.x, mBoundingBox->max.y, mBoundingBox->min.z);
    mPolygons[7].c = Vector3(mBoundingBox->min.x, mBoundingBox->min.y, mBoundingBox->min.z);
    
    // 3, 1, 7
    mPolygons[8].a = Vector3(mBoundingBox->max.x, mBoundingBox->max.y, mBoundingBox->min.z);
    mPolygons[8].b = Vector3(mBoundingBox->min.x, mBoundingBox->max.y, mBoundingBox->min.z);
    mPolygons[8].c = Vector3(mBoundingBox->max.x, mBoundingBox->max.y, mBoundingBox->max.z);
    
    // 7, 5, 3
    mPolygons[9].a = Vector3(mBoundingBox->min.x, mBoundingBox->max.y, mBoundingBox->max.z);
    mPolygons[9].b = Vector3(mBoundingBox->max.x, mBoundingBox->max.y, mBoundingBox->max.z);
    mPolygons[9].c = Vector3(mBoundingBox->max.x, mBoundingBox->max.y, mBoundingBox->min.z);

    // 2, 4, 6
    mPolygons[10].a = Vector3(mBoundingBox->max.x, mBoundingBox->min.y, mBoundingBox->min.z);
    mPolygons[10].b = Vector3(mBoundingBox->max.x, mBoundingBox->min.y, mBoundingBox->max.z);
    mPolygons[10].c = Vector3(mBoundingBox->min.x, mBoundingBox->min.y, mBoundingBox->max.z);
    
    // 6, 0, 2
    mPolygons[11].a = Vector3(mBoundingBox->min.x, mBoundingBox->min.y, mBoundingBox->max.z);
    mPolygons[11].b = Vector3(mBoundingBox->min.x, mBoundingBox->min.y, mBoundingBox->min.z);
    mPolygons[11].c = Vector3(mBoundingBox->max.x, mBoundingBox->min.y, mBoundingBox->min.z);
}


// Min Maxを直接指定
void BoundingVolumeComponent::ComputeBoundingVolume(const Vector3 min, const Vector3 max)
{
    mBoundingBox->min = min;
    mBoundingBox->max = max;
}

// バウンディングボックスを手動調整する（場所とサイズ（倍率））
void BoundingVolumeComponent::AdjustBoundingBox(const Vector3 pos, const Vector3 sc)
{
    
    mBoundingBox->max += pos;
    mBoundingBox->min += pos;
    
    mBoundingBox->max.x *= sc.x;
    mBoundingBox->min.x *= sc.x;
    mBoundingBox->max.y *= sc.y;
    mBoundingBox->min.y *= sc.y;
    mBoundingBox->max.z *= sc.z;
    mBoundingBox->min.z *= sc.z;

}




// デバッグ用
void BoundingVolumeComponent::CreateVArray()
{
    // ボックス用頂点バッファ（座標、法線、UV）
    float BBverts[] =
    {
        mBoundingBox->min.x, mBoundingBox->min.y, mBoundingBox->min.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 0
        mBoundingBox->min.x, mBoundingBox->max.y, mBoundingBox->min.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // 1
        mBoundingBox->max.x, mBoundingBox->min.y, mBoundingBox->min.z, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, // 2
        mBoundingBox->max.x, mBoundingBox->max.y, mBoundingBox->min.z, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // 3
        mBoundingBox->max.x, mBoundingBox->min.y, mBoundingBox->max.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 4
        mBoundingBox->max.x, mBoundingBox->max.y, mBoundingBox->max.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // 5
        mBoundingBox->min.x, mBoundingBox->min.y, mBoundingBox->max.z, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, // 6
        mBoundingBox->min.x, mBoundingBox->max.y, mBoundingBox->max.z, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f  // 7
        
    };
    
    unsigned int BBindex[] = {
        0, 1, 3,
        3, 2, 0,
        
        4, 2, 3,
        3, 5, 4,
        
        5, 4, 6,
        6, 7, 5,
        
        0, 6, 7,
        7, 1, 0,
        
        3, 1, 7,
        7, 5, 3,
        
        2, 4, 6,
        6, 0, 2
        

    };
    
    
    //mVertexArray = new VertexArray(BBverts, 8, (unsigned int*)BBindex, (unsigned int)36);
    mVertexArray = std::make_unique<VertexArray>(BBverts, 8, (unsigned int*)BBindex, (unsigned int)36);
    // マテリアル非対応なのでTextureで代用
    //texture = owner->GetApp()->GetRenderer()->GetTexture("Assets/BoundingVolume.png");
    
    //isVisible = true;
}


// バウンディングボリューム表示（ワイヤフレーム）
void BoundingVolumeComponent::Draw(Shader* shader)
{
    if (mIsVisible)
    {
        // WorldマトリックスをShaderに送る
        shader->SetMatrixUniform("uWorldTransform", mOwner->GetWorldTransform());

        // SpecPowerを送る
        shader->SetFloatUniform("uSpecPower", 1);


        //texture->SetActive();
            
        mVertexArray->SetActive();
        glDrawElements(GL_LINE_STRIP, 36, GL_UNSIGNED_INT, nullptr);
    }
}
