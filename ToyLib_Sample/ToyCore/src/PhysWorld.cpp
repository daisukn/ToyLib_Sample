#include "PhysWorld.h"
#include "Application.h"
#include "VertexArray.h"
#include "Actor.h"
#include "CameraComponent.h"
#include "BoundingVolumeComponent.h"
#include "Polygon.h"
#include "ColliderComponent.h"

#include "MathUtils.h"

#include <iostream>
#include <algorithm>


PhysWorld::PhysWorld()
{
}

PhysWorld::~PhysWorld()
{
}

void PhysWorld::Test()
{
    for (auto c : mColliders)
    {
        c->ClearCollidBuffer();
    }
    
    // Player vs Enemy
    for (auto c1 : mCollPlayer)
    {
        if (c1->GetDisp())
        {
            for (auto c2 : mCollEnemy)
            {
                if (c2->GetDisp())
                {
                    if (JudgeWithRadius(c1, c2))
                    {
                        if (JudgeWithOBB(c1, c2) )
                        {
                            c1->Collided(c2);
                            c2->Collided(c1);
                        }
                    }
                }
            }
        }
    }
    
    // Player vs Bullet
    for (auto c1 : mCollPlayer)
    {
        if (c1->GetDisp())
        {
            for (auto c2 : mCollBullet)
            {
                if (c2->GetDisp())
                {
                    if (JudgeWithRadius(c1, c2))
                    {
                        if (JudgeWithOBB(c1, c2) )
                        {
                            c1->Collided(c2);
                            c2->Collided(c1);
                        }
                    }
                }
            }
        }
    }
    
    // Laser vs Enemy
    for (auto c1 : mCollLaser)
    {
        if (c1->GetDisp())
        {
            for (auto c2 : mCollEnemy)
            {
                if (c2->GetDisp())
                {
                    if (JudgeWithRadius(c1, c2) )
                    {
                        if (JudgeWithOBB(c1, c2))
                        {
                            c1->Collided(c2);
                            c2->Collided(c1);
                        }
                    }
                }
            }
        }
    }
}

// OBBの投影距離比較
bool PhysWorld::CompareLengthOBB(const OBB* cA, const OBB* cB, const Vector3 vSep, const Vector3 vDistance){
    // 分離軸上のAからBの距離
    float length = fabsf(Vector3::Dot(vSep, vDistance));


    // 分離軸上でAから最も遠いAの頂点までの距離
    float lenA =
          fabs(Vector3::Dot(cA->axisX, vSep) * cA->radius.x)
            + fabs(Vector3::Dot(cA->axisY, vSep) * cA->radius.y)
            + fabs(Vector3::Dot(cA->axisZ, vSep) * cA->radius.z);

    // 分離軸上でBから最も遠いBの頂点までの距離
    float lenB =
          fabs(Vector3::Dot(cB->axisX, vSep) * cB->radius.x)
          + fabs(Vector3::Dot(cB->axisY, vSep) * cB->radius.y)
          + fabs(Vector3::Dot(cB->axisZ, vSep) * cB->radius.z);

    // 距離による判定
    if(length > lenA + lenB)
    {
        return false;
    }
    return true;
}




// 回転に対応したOBBCollid
bool PhysWorld::JudgeWithOBB(ColliderComponent* col1, ColliderComponent* col2){
    
    auto obb1 = col1->GetBoundingVolume()->GetOBB();
    auto obb2 = col2->GetBoundingVolume()->GetOBB();
    
    
    // 判定する
    return  IsCollideBoxOBB(obb1, obb2);
}

// 衝突しているかどうか
bool PhysWorld::IsCollideBoxOBB(const OBB* cA, const OBB* cB){



    // 中心間の距離ベクトル算出
    Vector3 vDistance = cB->pos - cA->pos;


    // 分離軸を比較
    if(!CompareLengthOBB(cA, cB, cA->axisX, vDistance)) return false;
    if(!CompareLengthOBB(cA, cB, cA->axisY, vDistance)) return false;
    if(!CompareLengthOBB(cA, cB, cA->axisZ, vDistance)) return false;
    if(!CompareLengthOBB(cA, cB, cB->axisX, vDistance)) return false;
    if(!CompareLengthOBB(cA, cB, cB->axisY, vDistance)) return false;
    if(!CompareLengthOBB(cA, cB, cB->axisZ, vDistance)) return false;

    // 分離軸同士の外積を比較
    Vector3 vSep = Vector3::Cross(cA->axisX, cB->axisX);
    if(!CompareLengthOBB(cA, cB, vSep, vDistance)) return false;
    
    vSep = Vector3::Cross(cA->axisX, cB->axisY);
    if(!CompareLengthOBB(cA, cB, vSep, vDistance)) return false;
    
    vSep = Vector3::Cross(cA->axisX, cB->axisZ);
    if(!CompareLengthOBB(cA, cB, vSep, vDistance)) return false;
    
    vSep = Vector3::Cross(cA->axisY, cB->axisX);
    if(!CompareLengthOBB(cA, cB, vSep, vDistance)) return false;
    
    vSep = Vector3::Cross(cA->axisY, cB->axisY);
    if(!CompareLengthOBB(cA, cB, vSep, vDistance)) return false;
    
    vSep = Vector3::Cross(cA->axisY, cB->axisZ);
    if(!CompareLengthOBB(cA, cB, vSep, vDistance)) return false;
    
    vSep = Vector3::Cross(cA->axisZ, cB->axisX);
    if(!CompareLengthOBB(cA, cB, vSep, vDistance)) return false;
    
    vSep = Vector3::Cross(cA->axisZ, cB->axisY);
    if(!CompareLengthOBB(cA, cB, vSep, vDistance)) return false;
    
    vSep = Vector3::Cross(cA->axisZ, cB->axisZ);
    if(!CompareLengthOBB(cA, cB, vSep, vDistance)) return false;

    return true;
}

// バウンディングスフィアの距離で判定
bool PhysWorld::JudgeWithRadius(class ColliderComponent* col1, class ColliderComponent* col2)
{
    auto ditance = col1->GetPosition() - col2->GetPosition();
    float len = ditance.Length();
    float thleshold = col1->GetBoundingVolume()->GetRadius() + col2->GetBoundingVolume()->GetRadius();

    
    if(thleshold > len)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// コライダーの登録
void PhysWorld::AddColliderType(ColliderComponent* c, ColliderType t)
{
    switch(t)
    {
        case C_NONE:
            break;
        case C_PLAYER:
            mCollPlayer.emplace_back(c);
            break;
        case C_ENEMY:
            mCollEnemy.emplace_back(c);
            break;
        case C_BULLET:
            mCollBullet.emplace_back(c);
            break;
        case C_LASER:
            mCollLaser.emplace_back(c);
            break;
    }
}

// 高さを知らせるべきActorを登録
void PhysWorld::AddCollider(ColliderComponent* c)
{
    mColliders.emplace_back(c);
}
void PhysWorld::RemoveCollider(ColliderComponent* c)
{
    auto iter = std::find(mColliders.begin(), mColliders.end(), c);
    mColliders.erase(iter);
}



// XZ平面に投影し、点がポリゴン内に存在すればTrue
bool PhysWorld::IsInPolygon(const Polygon* pl, const Vector3 p)
{

    if(((pl->b.x - pl->a.x) * (p.z - pl->a.z) -
        (pl->b.z - pl->a.z) * (p.x - pl->a.x)) < 0)
    {
            return false;
    }
    if(((pl->c.x - pl->b.x) * (p.z - pl->b.z) -
        (pl->c.z - pl->b.z) * (p.x - pl->b.x)) < 0)
    {
            return false;
    }
    if(((pl->a.x - pl->c.x) * (p.z - pl->c.z) -
        (pl->a.z - pl->c.z) * (p.x - pl->c.x)) < 0)
    {
            return false;
    }

    return true;

}


// ポリゴン内のある座標における高さを返す
float PhysWorld::PolygonHeight(const Polygon* pl, const Vector3 p)
{

    float wa, wb, wc;    // 平明方程式の係数
    //float height;        // 高さが入る


    wa = (pl->c.z - pl->a.z) * (pl->b.y - pl->a.y) -
         (pl->c.y - pl->a.y) * (pl->b.z - pl->a.z);
    wb = (pl->c.y - pl->a.y) * (pl->b.x - pl->a.x) -
         (pl->c.x - pl->a.x) * (pl->b.y - pl->a.y);
    wc = (pl->c.x - pl->a.x) * (pl->b.z - pl->a.z) -
         (pl->c.z - pl->a.z) * (pl->b.x - pl->a.x);


    return -(wa * (p.x - pl->a.x) + wb * (p.z - pl->a.z)) / wc + pl->a.y;
}

