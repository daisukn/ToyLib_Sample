#pragma once

#include "Component.h"
#include "Animation.h"
#include "MathUtils.h"
//#include <cstddef>

enum MeshType
{
    MESH_NORMAL,
    MESH_BG,
    MESH_EFFECT
};

// Meshを管理するComponent（Rendererから呼ばれる）
class MeshComponent : public Component
{
public:
    MeshComponent(class Actor* a, bool isSkeletal = false, MeshType type = MESH_NORMAL);
    virtual ~MeshComponent();
        
    // 描画 override
    virtual void Draw(class Shader* s);
    virtual void SetMesh(class Mesh* m) { mMesh = m; }              // メッシュセット
    void SetTextureIndex(unsigned int index) { mTextureIndex = index; }    // テクスチャGetter

    void SetVisible(bool visible) { mIsVisible = visible; }
    bool GetVisible() const { return mIsVisible; }
    
    bool GetIsSkeletal() const { return mIsSkeletal; }
    class VertexArray* GetVertexArray(int id) const;
    
    void SetToonRender(const bool t, const float f = 1.05f) { mIsToon = t; mContourFactor = f; }
    void SetContourFactor(const float f) { mContourFactor = f; }
    
    bool GetToon() const { return mIsToon; }
    
    void SetBlendAdd(bool b) { mIsBlendAdd = b; }
    bool GetBlendAdd() const { return mIsBlendAdd; }
    
    void SetGlory(bool b) { mIsGlory = b; }
    bool GetGlory() const { return mIsGlory; }
    
    //void SetScale(float f) { mScale = f; }
    //float GetScale() const { return mScale; }
    
    // 再生するモーションのID（SkerltalMeshでオーバーライドする。インターフェース確保のため）
    virtual void SetAnimID(const unsigned int animID, const PlayMode mode){}
    
    void DrawShadow(class Shader* shader, const Matrix4& lightSpaceMatrix);
    
protected:
    class Mesh* mMesh;      // メッシュ
    unsigned int mTextureIndex;    // TextureID
    
    //float mScale;
    
    bool mIsVisible;
    bool mIsSkeletal;
    
    MeshType mMeshType;
    
    
    // 輪郭強調
    bool mIsToon;
    float mContourFactor;
    
    // 後光
    bool mIsGlory;
    
    // 加算合成するか
    bool mIsBlendAdd;
};

