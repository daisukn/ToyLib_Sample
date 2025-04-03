#pragma once

#include "MathUtils.h"

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <GL/glew.h>

// ディレクショナルライト
struct DirectionalLight
{
    Vector3 Direction;      // 向き
    Vector3 DiffuseColor;   // 色
    Vector3 SpecColor;      // 反射色
};

enum class VisualLayer
{
    Background2D,
    Effect3D,
    UI
};

// 描画エンジン
class Renderer
{
public:
    // コンストラクタ
    Renderer();
    // デストラクタ
    virtual ~Renderer();
      
    // 初期化
    bool Initialize();
    // 描画（Applicationから呼ばれる）
    void Draw();

    // 終了
    void Shutdown();

    // クリア色を設定
    void SetClearColor(const Vector3 color);


    // ビューマトリックスを設定する
    void SetViewMatrix(const Matrix4& view) { mInvView = mViewMatrix = view; mInvView.Invert();}
    Matrix4 GetViewMatrix() const { return mViewMatrix; }
    Matrix4 GetInvViewMatrix() const { return mInvView; }
    Matrix4 GetProjectionMatrix() const { return mProjectionMatrix; }
    
    // FOV取得（度）
    float GetPerspectiveFov() const { return mPerspectiveFOV; }
    void SetPoerspectiveFov(float f) { mPerspectiveFOV = f; }
    
    // カメラポジション
    Vector3 GetCameraPosition() const { return mCameraPosition; }
    void SetCameraPosition(Vector3 v) { mCameraPosition = v; }
    

    // スクリーンサイズのGtter
    float GetScreenWidth() const { return mScreenWidth; }
    float GetScreenHeight() const { return mScreenHeight; }
      
    
    // Visualコンポーネント
    void AddVisualComp(class VisualComponent* comp);
    void RemoveVisualComp(class VisualComponent* comp);
    void DrawVisualLayer(VisualLayer layer);
    
    // スプライトコンポーネント登録・削除
    //void AddSprite(class SpriteComponent* sprite);
    //void RemoveSprite(class SpriteComponent* sprite);
    // 背景スプライトコンポーネント登録・削除
    //void AddBackGroundSprite(class SpriteComponent* sprite);
    //void RemoveBackGroundSprite(class SpriteComponent* sprite);
    // メッシュコンポーネント登録・削除
    void AddMeshComp(class MeshComponent* mesh);
    void RemoveMeshComp(class MeshComponent* mesh);
    // 背景メッシュコンポーネント登録・削除
    void AddBackGroudMeshComp(class MeshComponent* mesh);
    void RemoveBackGroudMeshComp(class MeshComponent* mesh);
    // エフェクトメッシュコンポーネント登録・削除
    void AddEffectMeshComp(class MeshComponent* mesh);
    void RemoveEffectMeshComp(class MeshComponent* mesh);
    // パーティクルコンポーネント登録・削除
    void AddParticleComp(class ParticleComponent* part);
    void RemoveParticleComp(class ParticleComponent* part);
    // ビルボード登録・削除
    void AddBillboardComp(class BillboardComponent* billboard);
    void RemoveBillboardComp(class BillboardComponent* billboard);
    // ワイヤーフレームコンポーネント登録・削除
    void AddWireframeComp(class WireframeComponent* wireframe);
    void RemoveWireframeComp(class WireframeComponent* wireframe);

    // デバッグモード設定
    void SetDebugMode(const bool b) { mIsDebugMode = b; }
    bool GetDebugMode() const { return mIsDebugMode; }
    
    
    // メッシュ登録
    class Mesh* GetMesh(const std::string& fileName, bool isRightHanded = false);
    // テクスチャ登録
    class Texture* GetTexture(const std::string& fileName);
    class Texture* GetEmbeddedTexture(const std::string& nameKey, const uint8_t* data, size_t dataSize);

    // データ解放
    void UnloadData();
    
    // アンビエントライト設定
    void SetAmbientColor(const Vector3& ambient) { mAmbientColor = ambient; }
    DirectionalLight& GetDirectionalLight() { return mDirLight; }
    
    // 単色シェーダー取得
    class Shader* GetSolidShader() { return mSolidShader.get(); }
    
    // ディレクショナルライト設定
    void SetDirectionalLightPosition(const Vector3& pos, const Vector3& target);
    const DirectionalLight GetDirLight() const { return mDirLight; }
    // フォグ情報設定
    void SetFogInfo(const float max, const float min, Vector3 color);
    
private:
    // セッティング読み込み
    bool LoadSettings(const std::string& filePath);
    
    // レンダラーパラメーター //
    // ウィンドウタイトル
    std::string mStrTitle;
    //スクリーンサイズ
    float mScreenWidth;
    float mScreenHeight;
    bool mIsFullScreen;
    // 視野角(度)
    float mPerspectiveFOV;
    // カメラポジション
    Vector3 mCameraPosition;
    // デバッグモード
    bool mIsDebugMode;
    // クリア色
    Vector3 mClearColor;
    // ライト
    Vector3 mAmbientColor;
    Vector3 mDiffuseColor;
    Vector3 mSpecColor;
    Vector3 mDirLightPosition;
    Vector3 mDirLightTarget;
    // フォグ
    float mFogMaxDist;
    float mFogMinDist;
    Vector3 mFogColor;
    // シャドウマップ
    float mShadowNear;
    float mShadowFar;
    float mShadowOrthoWidth;
    float mShadowOrthoHeight;
    int mShadowFBOWidth;
    int mShadowFBOHeight;
    

    // ビューマトリックス
    Matrix4 mViewMatrix;
    Matrix4 mInvView;
    // プロジェクションマトリックス
    Matrix4 mProjectionMatrix;

    // DirectionalLight管理
    DirectionalLight mDirLight;

    
    // Windowハンドラ
    SDL_Window* mWindow;
    // GLコンテキスト
    SDL_GLContext mGLContext;

    // メッシュ用シェーダー
    std::unique_ptr<class Shader> mMeshShader;
    std::unique_ptr<class Shader> mMeshShaderToon;
    std::unique_ptr<class Shader> mBackGroundShader;
    // スキンメッシュ用シェーダー
    std::unique_ptr<class Shader> mSkinnedShader;
    std::unique_ptr<class Shader> mSkinnedShaderToon;
    // スプライト用シェーダー
    std::unique_ptr<class Shader> mSpriteShader;
    // ビルボード用シェーダー
    std::unique_ptr<class Shader> mBillboardShader;
    // パーティクル用シェーダー
    std::unique_ptr<class Shader> mParticleShader;
    // ワイヤーフレーム用　単色シェーダー
    std::unique_ptr<class Shader> mSolidShader;
    // シャドウマップ用シェーダー
    std::unique_ptr<class Shader> mShadowSkinnedShader;
    // シャドウマップ用シェーダー
    std::unique_ptr<class Shader> mShadowMeshShader;

    // シェーダー一括ロード
    bool LoadShaders();

    // スプライト用ポリゴン（Billboardでも使う）
    std::unique_ptr<class VertexArray> mSpriteVerts;
    // スプライト用ポリゴンの生成
    void CreateSpriteVerts();
    
    // Lightsingをシェーダーに送る
    void SetLightUniforms(class Shader* shader);
    
    
    // シャドウマッピング関連処理とパラメータ
    GLuint mShadowFBO;
    bool InitializeShadowMapping();
    void RenderShadowMap();
    Matrix4 mLightSpaceMatrix;
    std::unique_ptr<class Texture> mShadowMapTexture;

    
    // アセット
    std::unordered_map<std::string, std::unique_ptr<class Texture>> mTextures;
    std::unordered_map<std::string, std::unique_ptr<class Mesh>> mMeshes;

    // コンポーネント
    std::vector<class VisualComponent*> mVisualComps;
    //std::vector<class SpriteComponent*> mSpriteComps;
    //std::vector<class SpriteComponent*> mBgSpriteComps;
    std::vector<class MeshComponent*> mMeshComps;
    std::vector<class MeshComponent*> mBgMesh;
    std::vector<class MeshComponent*> mEffectMesh;
    std::vector<class SkeletalMeshComponent*> mSkeletalMeshes;
    std::vector<class ParticleComponent*> mParticleComps;
    std::vector<class BillboardComponent*> mBillboardComps;
    std::vector<class WireframeComponent*> mWireframeComps;
    
    

    void DrawBackGround();
    void DrawMesh();
    void DrawParticle();
    void DrawBillboard();
    void DrawSprite();
    void DrawDebugger();
    void DrawEffect();
    
    class Shader* GetVisualShader(const class VisualComponent* visual);
};

