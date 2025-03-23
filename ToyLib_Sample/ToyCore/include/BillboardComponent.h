#pragma once

#include "Component.h"
#include "MathUtils.h"

// ビルボードコンポーネント
class BillboardComponent : public Component
{
public:
    BillboardComponent(class Actor* a, int order = 100);
    ~BillboardComponent();
    
    // Rendererから呼ばれる
    virtual void Draw(class Shader* shader);

    // テクスチャセット
    virtual void SetTexture(class Texture* tex);
    
    // アップデート処理
    void Update(float deltaTime) override;
    

    void SetVisible(bool visible) { mIsVisible = visible; }
    bool GetVisible() const { return mIsVisible; }
    
    void SetScale(float f){ mScale = f;}

    int GetDrawOrder() const { return mDrawOrder; }
    
    
    void SetBlendAdd(bool b) { mIsBlendAdd = b; }
    bool GetBlendAdd() const { return mIsBlendAdd; }
    
private:
    int             mDrawOrder;
    bool            mIsVisible;
    float           mScale;
    class Texture*  mTexture;
    
    // 加算合成するか
    bool            mIsBlendAdd;

};

