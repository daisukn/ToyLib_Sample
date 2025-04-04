// ハイブリッド方式の GetNearestGroundY に対応した PhysWorld
#pragma once

#include "MathUtils.h"
#include "ColliderComponent.h"
#include <vector>

struct MTVResult
{
    Vector3 axis = Vector3::Zero;
    float depth = Math::Infinity;
    bool valid = false;
};

// 物理計算系のクラス
class PhysWorld
{
public:
    PhysWorld();
    ~PhysWorld();

    void ComputeGroundHeight();
    void Test();

    // コライダー管理
    void AddCollider(class ColliderComponent* c);
    void RemoveCollider(class ColliderComponent* c);
    void AddColliderType(class ColliderComponent* c, ColliderType t);

    // 地面情報インターフェイス
    float GetGroundHeightAt(const Vector3& pos) const;
    bool GetNearestGroundY(const class Actor* a, float& outY) const;
    void SetGroundPolygons(const std::vector<Polygon>& polys);

private:
    // 衝突判定
    bool CompareLengthOBB(const struct OBB* cA, const struct OBB* cB, const Vector3 vSep, const Vector3 vDistance);
    bool JudgeWithOBB(class ColliderComponent* col1, class ColliderComponent* col2);
    bool IsCollideBoxOBB(const OBB* cA, const OBB* cB);
    bool JudgeWithRadius(class ColliderComponent* col1, class ColliderComponent* col2);

    Vector3 ComputePushBackDirection(class ColliderComponent* a, class ColliderComponent* b);
    bool CompareLengthOBB_MTV(const OBB* cA, const OBB* cB, const Vector3& vSep, const Vector3& vDistance, MTVResult& mtv);
    bool IsCollideBoxOBB_MTV(const OBB* cA, const OBB* cB, MTVResult& mtv);

    // 地表計算
    bool IsInPolygon(const struct Polygon* pl, const struct Vector3 p) const;
    float PolygonHeight(const struct Polygon* pl, const struct Vector3 p) const;

    // コライダー保持
    std::vector<class ColliderComponent*> mColliders;
    std::vector<class ColliderComponent*> mCollPlayer;
    std::vector<class ColliderComponent*> mCollEnemy;
    std::vector<class ColliderComponent*> mCollLaser;
    std::vector<class ColliderComponent*> mCollBullet;
    std::vector<class ColliderComponent*> mCollWall;
    std::vector<class ColliderComponent*> mCollGround;
    std::vector<class ColliderComponent*> mCollFoot;

    // 地形（静的Mesh）
    std::vector<struct Polygon> mTerrainPolygons;
};

/*
#pragma once

#include "MathUtils.h"
#include "ColliderComponent.h"
#include <vector>


struct MTVResult
{
    Vector3 axis = Vector3::Zero;
    float depth = Math::Infinity;
    bool valid = false;
};

// 物理計算系のクラス
class PhysWorld
{
public:
    PhysWorld();
    ~PhysWorld();
    
    //void SetGroundVertex(class VertexArray* v) { mVertexGround = v;  }
    void ComputeGroundHeight();
    
    // 暫定
    void Test();
    
    // コライダーを追加
    void AddCollider(class ColliderComponent* c);
    void RemoveCollider(class ColliderComponent* c);

    // コライダータイプ別に登録
    void AddColliderType(class ColliderComponent* c, ColliderType t);
    
    // 重力コンポーネント向けのインターフェイス
    float GetGroundHeightAt(const Vector3& pos) const;
    bool GetNearestGroundY(const class Actor* a, float& outY) const;
    
    // 地形データ登録
    void SetGroundPolygons(const std::vector<Polygon>& polys);
private:
    
///衝突判定系
    bool CompareLengthOBB(const struct OBB* cA, const struct OBB* cB, const Vector3 vSep, const Vector3 vDistance);
    bool JudgeWithOBB(class ColliderComponent* col1, class ColliderComponent* col2);
    bool IsCollideBoxOBB(const OBB* cA, const OBB* cB);
    
    bool JudgeWithRadius(class ColliderComponent* col1, class ColliderComponent* col2);
    
    Vector3 ComputePushBackDirection(class ColliderComponent* a, class ColliderComponent* b);

    bool CompareLengthOBB_MTV(const OBB* cA, const OBB* cB, const Vector3& vSep, const Vector3& vDistance, MTVResult& mtv);
    bool IsCollideBoxOBB_MTV(const OBB* cA, const OBB* cB, MTVResult& mtv);
    
///地表計算系
    bool IsInPolygon(const struct Polygon* pl, const struct Vector3 p) const;
    float PolygonHeight(const struct Polygon* pl, const struct Vector3 p) const;

    // 物理計算をするコライダーを保持
    //std::vector<class Actor*> actors;
    std::vector<class ColliderComponent*> mColliders;
    std::vector<class ColliderComponent*> mCollPlayer;
    std::vector<class ColliderComponent*> mCollEnemy;
    std::vector<class ColliderComponent*> mCollLaser;
    std::vector<class ColliderComponent*> mCollBullet;
    std::vector<class ColliderComponent*> mCollWall;
    std::vector<class ColliderComponent*> mCollGround;
    std::vector<class ColliderComponent*> mCollFoot;


    // 地表面をVarrayで持つ（WorldTransform未対応）
    //class VertexArray* mVertexGround;
    
    std::vector<struct Polygon> mTerrainPolygons;


    
};

*/
