#pragma once

#include "MathUtils.h"

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <GL/glew.h>

enum class VisualLayer
{
    Background2D,
    Effect3D,
    Object3D,
    UI,
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
    SDL_Window* GetSDLWindow() const { return mWindow; }
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
    Matrix4 GetViewProjMatrix() const { return mViewMatrix * mProjectionMatrix; }
    
    // FOV取得（度）
    float GetPerspectiveFov() const { return mPerspectiveFOV; }
    void SetPerspectiveFov(float f) { mPerspectiveFOV = f; }
    
    // スクリーンサイズのGtter
    float GetScreenWidth() const { return mScreenWidth; }
    float GetScreenHeight() const { return mScreenHeight; }
      
    
    // Visualコンポーネント
    void AddVisualComp(class VisualComponent* comp);
    void RemoveVisualComp(class VisualComponent* comp);

    // メッシュコンポーネント登録・削除
    void AddMeshComp(class MeshComponent* mesh);
    void RemoveMeshComp(class MeshComponent* mesh);
    // 背景メッシュコンポーネント登録・削除
    void AddBackGroudMeshComp(class MeshComponent* mesh);
    void RemoveBackGroudMeshComp(class MeshComponent* mesh);
    // エフェクトメッシュコンポーネント登録・削除
    void AddEffectMeshComp(class MeshComponent* mesh);
    void RemoveEffectMeshComp(class MeshComponent* mesh);
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
    
    // スカイドーム登録
    void RegisterSkyDome(class SkyDomeComponent* sky);
    class SkyDomeComponent* GetSkyDome() const { return mSkyDomeComp; }
    
    // 雨エフェクトのセット
    void SetRainAmount(const float amt) { mRainAmount = amt; }
    // 霧エフェクトのセット
    void SetFogAmount(const float amt) { mFogAmount = amt; }
    // 雪エフェクトのセット
    void SetSnowAmout(const float amt) { mSnowAmount = amt; }
    
    // ライトマネージャー
    void SetLightingManager(std::shared_ptr<class LightingManager> manager) { mLightingManager = manager; }
    std::shared_ptr<class LightingManager> GetLightingManager() const { return mLightingManager; }
    
    
    std::shared_ptr<class Shader> GetShader(const std::string& name) { return mShaders[name]; }
    
    // 光源マトリックス
    Matrix4 GetLightSpaceMatrix() const { return mLightSpaceMatrix; }

private:
    // セッティング読み込み
    bool LoadSettings(const std::string& filePath);
    // ライティング管理
    std::shared_ptr<class LightingManager> mLightingManager;
    
    // レンダラーパラメーター //
    std::string mShaderPath;
    // ウィンドウタイトル
    std::string mStrTitle;
    //スクリーンサイズ
    float mScreenWidth;
    float mScreenHeight;
    bool mIsFullScreen;
    // 視野角(度)
    float mPerspectiveFOV;
    // デバッグモード
    bool mIsDebugMode;
    // クリア色
    Vector3 mClearColor;

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

    // Windowハンドラ
    SDL_Window* mWindow;
    // GLコンテキスト
    SDL_GLContext mGLContext;
    
    // 雨の強さ
    float mRainAmount;
    // 霧の強さ
    float mFogAmount;
    // 雪の強さ
    float mSnowAmount;
    std::unique_ptr<class VertexArray> mFullScreenQuad;
    void CreateFullScreenQuad();
    
    // 天気エフェクトのオーバーレイ
    void DrawWeatherOverlay();
   
    // シェーダー格納
    std::unordered_map<std::string, std::shared_ptr<class Shader>> mShaders;
    // シェーダー一括ロード
    bool LoadShaders();

    // スプライト用ポリゴン（Billboardでも使う）
    std::unique_ptr<class VertexArray> mSpriteVerts;
    // スプライト用ポリゴンの生成
    void CreateSpriteVerts();
    
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
    std::vector<class MeshComponent*> mMeshComps;
    std::vector<class MeshComponent*> mBgMesh;
    std::vector<class MeshComponent*> mEffectMesh;
    std::vector<class SkeletalMeshComponent*> mSkeletalMeshes;
    std::vector<class WireframeComponent*> mWireframeComps;
    
    
    class SkyDomeComponent* mSkyDomeComp; // Gameアプリ側で生成、生ポインタを保持
    
    void DrawSky();
    void DrawBackGround();
    void DrawMesh();
    void DrawDebugger();
    void DrawEffect();
    void DrawVisualLayer(VisualLayer layer);
    class Shader* GetVisualShader(const class VisualComponent* visual);
};

