#pragma once

#include "Component.h"
#include "Renderer.h"

enum class VisualType
{
    NoAssigned,
    Sprite,
    Mesh,
    Billboard,
    Particle,
    ShadowSprite
};

class VisualComponent : public Component
{
public:
    VisualComponent(class Actor* owner, int drawOrder, VisualLayer layer = VisualLayer::Effect3D);
    virtual ~VisualComponent();

    virtual void Draw() = 0;
    virtual void DrawShadow() {}

    virtual void SetTexture(class Texture* tex) { mTexture = tex; }
    class Texture* GetTexture() const { return mTexture; }

    void SetVisible(bool v) { mIsVisible = v; }
    bool IsVisible() const { return mIsVisible; }

    void SetBlendAdd(bool b) { mIsBlendAdd = b; }
    bool IsBlendAdd() const { return mIsBlendAdd; }

    void SetLayer(VisualLayer layer) { mLayer = layer; }
    VisualLayer GetLayer() const { return mLayer; }
    
    VisualType GetVisualType() const { return mType; }

    int GetDrawOrder() const { return mDrawOrder; }
    void SetDrawOrder(int order) { mDrawOrder = order; }
    
    void SetShader(std::shared_ptr<class Shader> shader) { mShader = shader; }
    void SetLightingManager(std::shared_ptr<LightingManager> light) { mLightingManager = light; }

    bool IsEnableShadow() const { return mEnableShadow; }
protected:
    class Texture* mTexture;
    std::shared_ptr<class Shader> mShader;
    std::shared_ptr<class LightingManager> mLightingManager;
    bool mIsVisible;
    bool mIsBlendAdd;
    VisualLayer mLayer;
    VisualType mType;
    int mDrawOrder;
    bool mEnableShadow;
    std::shared_ptr<class VertexArray> mSpriteVerts;
    
};
