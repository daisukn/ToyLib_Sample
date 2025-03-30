#include "Renderer.h"
#include "Shader.h"
#include "SpriteComponent.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "ParticleComponent.h"
#include "BillboardComponent.h"

#include "WireframeComponent.h"

#include <GL/glew.h>
#include <algorithm>
#include <string>


const std::string LIBRARY_PATH = "ToyLibCore/";

// コンストラクタ
Renderer::Renderer()
: mStrTitle("")
, mScreenWidth(0.f)
, mScreenHeight(0.f)
, mWindow(nullptr)
, mGLContext(nullptr)
, mPerspectiveFOV(45.f)
, mCameraPosition(Vector3(0.f, 0.f, 0.f))
, mIsDebugMode(false)
, mClearColor(Vector3(0.2f, 0.5f, 0.8f))
, mDirLightPosition(Vector3(20, 20, -5))
, mDirLightTarget(Vector3(0, 0, 0))
, mFogMaxDist(100.f)
, mFogMinDist(0.001f)
, mFogColor(Vector3(0.2f, 0.5f, 0.8f))
{
}

// デストラクタ
Renderer::~Renderer()
{
}


// ウィンドウ生成とGL初期化
bool Renderer::Initialize(std::string title, float scWidth, float scHeight, bool iSFullScreen)
{
    mStrTitle       = title;    // ウィンドウタイトル
    mScreenWidth    = scWidth;  // スクリーン幅
    mScreenHeight   = scHeight; // スクリーン高さ
    mIsFullScreen   = iSFullScreen;
    
    
    // OpenGL プロファイル, バージョン
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // ダブルバッファリング
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // RGBAチャンネル
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    // ハードウェアアクセラレーション有効化
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    
    unsigned int WINDOW_FLAGS = SDL_WINDOW_OPENGL;
    if (mIsFullScreen)
    {
        WINDOW_FLAGS =  WINDOW_FLAGS | SDL_WINDOW_FULLSCREEN;
    }

    //ウインドウ生成
    mWindow = SDL_CreateWindow(mStrTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, static_cast<int>(mScreenWidth), static_cast<int>(mScreenHeight), WINDOW_FLAGS);
    
    if (!mWindow )
    {
        std::cout << "Unable to create window" << std::endl;
        return false;
    }
    
    
    // OpenGL コンテキスト生成
    mGLContext = SDL_GL_CreateContext(mWindow);
    
    // GLEW初期化
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return false;
    }
   
    
    /*シェーダー のロードなどはここでやる*/
    LoadShaders();
    CreateSpriteVerts();

    
    // シャドウマッピング初期化
    InitializeShadowMapping(1024, 1024);
    // クリアカラーのデフォルト値
    SetClearColor(mClearColor);

    return true;
}

// リリース処理
void Renderer::Shutdown()
{
    SDL_GL_DeleteContext(mGLContext);
    SDL_DestroyWindow(mWindow);

}

void::Renderer::SetClearColor(const Vector3 color)
{
    mClearColor = color;
    glClearColor(mClearColor.x, mClearColor.y, mClearColor.z, 1.0f);
}

// 描画処理
void Renderer::Draw()
{
    
    // シャドウマップレンダー
    RenderShadowMap();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Culling設定
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW); // 左手座標系

    
    /* 描画処理 メッシュ、スプライト*/
    // アルファブレンディングを有効化
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
    // 各コンポーネント描画
    DrawBackGround();
    DrawMesh();
    DrawDebugger();
    DrawParticle();
    DrawBillboard();
    DrawSprite();


    SDL_GL_SwapWindow(mWindow);

}

// 背景オブジェクトの描画
void Renderer::DrawBackGround()
{
    // 背景スプライト
    glDisable(GL_DEPTH_TEST);
    mSpriteShader->SetActive();
    for (auto sprite : mBgSpriteComps)
    {
        sprite->Draw(mSpriteShader.get());
    }
    glEnable(GL_DEPTH_TEST);
 
    
    
    // 背景用メッシュ描画
    mBackGroundShader->SetActive();
    mBackGroundShader->SetMatrixUniform("uViewProj", mViewMatrix * mProjectionMatrix);
    // Update lighting uniforms
    SetLightUniforms(mBackGroundShader.get());
    for (auto bg : mBgMesh)
    {
        if (bg->GetVisible())
        {
            bg->Draw(mBackGroundShader.get());
        }
    }
    
}

// メッシュの描画
void Renderer::DrawMesh()
{
    // 共通シャドウマップ設定（ユニット1）
    glActiveTexture(GL_TEXTURE1);
    mShadowMapTexture->SetActive(); // ← Textureクラスの SetActive()
    glActiveTexture(GL_TEXTURE0);   // メインテクスチャ用に戻す

    // 通常メッシュ描画（スキンなし）
    for (auto mc : mMeshComps)
    {
        if (!mc->GetVisible()) continue;

        mMeshShader->SetActive();

        // ユニフォーム設定
        mMeshShader->SetMatrixUniform("uViewProj", mViewMatrix * mProjectionMatrix);
        mMeshShader->SetMatrixUniform("uLightSpaceMatrix", mLightSpaceMatrix);
        mMeshShader->SetTextureUniform("uShadowMap", 1);
        mMeshShader->SetFloatUniform("uShadowBias", 0.005);
        mMeshShader->SetBooleanUniform("uUseToon", mc->GetToon()); // トゥーンON/OFF

        SetLightUniforms(mMeshShader.get());

        mc->Draw(mMeshShader.get());
    }
    
    // スキンメッシュ描画
    for (auto sk : mSkeletalMeshes)
    {
        if (!sk->GetVisible()) continue;

        mSkinnedShader->SetActive();

        // ユニフォーム設定
        mSkinnedShader->SetMatrixUniform("uViewProj", mViewMatrix * mProjectionMatrix);
        mSkinnedShader->SetMatrixUniform("uLightSpaceMatrix", mLightSpaceMatrix);
        mSkinnedShader->SetTextureUniform("uShadowMap", 1);
        mSkinnedShader->SetFloatUniform("uShadowBias", 0.005);
        mSkinnedShader->SetBooleanUniform("uUseToon", sk->GetToon()); // トゥーンON/OFF
        SetLightUniforms(mSkinnedShader.get());

        sk->Draw(mSkinnedShader.get());
    }
}

// パーティクル
void Renderer::DrawParticle()
{
    // パーティクルの処理
    // Zバッファに書き込まない
    glDepthMask(GL_FALSE);
    //加算合成
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mSpriteVerts->SetActive();
    mParticleShader->SetActive();
    mParticleShader->SetMatrixUniform("uViewProj", mViewMatrix * mProjectionMatrix);

    for (auto parts : mParticleComps)
    {
        parts->Draw(mParticleShader.get());
    }
    glDepthMask(GL_TRUE);
}

void Renderer::DrawBillboard()
{
    // ビルボード
    // Zバッファに書き込まない
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mSpriteVerts->SetActive();
    mBillboardShader->SetActive();
    SetLightUniforms(mBillboardShader.get());
    mBillboardShader->SetMatrixUniform("uViewProj", mViewMatrix * mProjectionMatrix);

    for (auto bb : mBillboardComps)
    {
        
        bb->Draw(mBillboardShader.get());
    }
    glDepthMask(GL_TRUE);
    
}

void Renderer::DrawSprite()
{
    // スプライト処理
    glDisable(GL_DEPTH_TEST);
    // アルファブレンディング
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mSpriteShader->SetActive();
    for (auto sprite : mSpriteComps)
    {
        sprite->Draw(mSpriteShader.get());
    }
    glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawDebugger()
{
    // デバッグモードでない場合キャンセル
    if (!mIsDebugMode) return;
    
    // デバッガー用の描画
    mSolidShader->SetActive();
    mSolidShader->SetMatrixUniform("uViewProj", mViewMatrix * mProjectionMatrix);
    mSolidShader->SetVectorUniform("uSolColor", Vector3(1.f, 1.f, 1.f));
    // Update lighting uniforms
    SetLightUniforms(mSolidShader.get());
    for (auto wireframe : mWireframeComps)
    {
        wireframe->Draw(mSolidShader.get());
    }
}

void Renderer::DrawEffect()
{
    // エフェクトメッシュ描画
    mMeshShader->SetActive();
    mMeshShader->SetMatrixUniform("uViewProj", mViewMatrix * mProjectionMatrix);
    // Update lighting uniforms
    SetLightUniforms(mMeshShader.get());
    for (auto mesh : mEffectMesh)
    {
        if (mesh->GetVisible())
        {
            mesh->Draw(mMeshShader.get());
        }
    }
    
}

// シェーダー 読み込み
bool Renderer::LoadShaders()
{
    std::string vShaderName;
    std::string fShaderName;

    // スプライト用シェーダー生成
    mSpriteShader = std::make_unique<Shader>();
    vShaderName = LIBRARY_PATH + "Shaders/Sprite.vert";
    fShaderName = LIBRARY_PATH + "Shaders/Sprite.frag";
    if (!mSpriteShader->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }

    // ビューマトリックス、プロジェクションマトリックス
    Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
    mSpriteShader->SetMatrixUniform("uViewProj", viewProj);

    // Billboard用シェーダー
    mBillboardShader = std::make_unique<Shader>();
    vShaderName = LIBRARY_PATH + "Shaders/Billboard.vert";
    fShaderName = LIBRARY_PATH + "Shaders/Billboard.frag";
    if (!mBillboardShader->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    
    // Particle用シェーダー
    mParticleShader = std::make_unique<Shader>();
    vShaderName = LIBRARY_PATH + "Shaders/Billboard.vert";
    fShaderName = LIBRARY_PATH + "Shaders/Particle.frag";
    if (!mParticleShader->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    
    // メッシュ用シェーダー生成
    mMeshShader = std::make_unique<Shader>();
    vShaderName = LIBRARY_PATH + "Shaders/Phong.vert";
    fShaderName = LIBRARY_PATH + "Shaders/Toon.frag";
    if (!mMeshShader->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    
    // メッシュ用シェーダー(Toon)生成
    mMeshShaderToon = std::make_unique<Shader>();
    vShaderName = LIBRARY_PATH + "Shaders/Phong.vert";
    fShaderName = LIBRARY_PATH + "Shaders/Toon.frag";
    if (!mMeshShaderToon->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    
    // 背景用シェーダー生成
    mBackGroundShader = std::make_unique<Shader>();
    vShaderName = LIBRARY_PATH + "Shaders/BasicMesh.vert";
    fShaderName = LIBRARY_PATH + "Shaders/BasicMesh.frag";
    if (!mBackGroundShader->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    
    // スキンメッシュ用シェーダー
    mSkinnedShader = std::make_unique<Shader>();
    vShaderName = LIBRARY_PATH + "Shaders/Skinned.vert";
    fShaderName = LIBRARY_PATH + "Shaders/Toon.frag";
    if (!mSkinnedShader->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    
    // スキンメッシュ用シェーダー(Toon)
    mSkinnedShaderToon = std::make_unique<Shader>();
    vShaderName = LIBRARY_PATH + "Shaders/Skinned.vert";
    fShaderName = LIBRARY_PATH + "Shaders/Toon.frag";
    if (!mSkinnedShaderToon->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    
    // ワイヤフレーム用　単色シェーダー生成
    mSolidShader = std::make_unique<Shader>();
    vShaderName = LIBRARY_PATH + "Shaders/BasicMesh.vert";
    fShaderName = LIBRARY_PATH + "Shaders/SolidColor.frag";
    if (!mSolidShader->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    
    // シャドウマッピング透けるタルメッシュ用
    mShadowSkinnedShader = std::make_unique<Shader>();
    vShaderName = LIBRARY_PATH + "Shaders/ShadowMapping_Skinned.vert";
    fShaderName = LIBRARY_PATH + "Shaders/ShadowMapping.frag";
    if (!mShadowSkinnedShader->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    
    // シャドウマップメッシュ用
    mShadowMeshShader = std::make_unique<Shader>();
    vShaderName = LIBRARY_PATH + "Shaders/ShadowMapping_Mesh.vert";
    fShaderName = LIBRARY_PATH + "Shaders/ShadowMapping.frag";
    if (!mShadowMeshShader->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    
    // ビューマトリックス、プロジェクションマトリックス（デフォルト値）
    mViewMatrix = Matrix4::CreateLookAt(Vector3(0, 0.5f, -3), Vector3(0, 0, 10), Vector3::UnitY);
    mProjectionMatrix = Matrix4::CreatePerspectiveFOV(Math::ToRadians(mPerspectiveFOV), mScreenWidth, mScreenHeight, 1.0f, 2000.0f);

    return true;
}

// ライト設定
void Renderer::SetLightUniforms(Shader* shader)
{
    // カメラポジション
    Matrix4 invView = mViewMatrix;
    invView.Invert();
    shader->SetVectorUniform("uCameraPos", invView.GetTranslation());
    // Ambient light
    mAmbientLight = Vector3(1,1,1);
    shader->SetVectorUniform("uAmbientLight", mAmbientLight);
    // Directional light
    mDirLight.Direction = Vector3::Normalize(mDirLightTarget - mDirLightPosition);;
    mDirLight.DiffuseColor = Vector3(0.4f, 0.4f, 0.4f);
    mDirLight.SpecColor = Vector3(0.00f, 0.0f, 0.0f);
    shader->SetVectorUniform("uDirLight.mDirection", mDirLight.Direction);
    shader->SetVectorUniform("uDirLight.mDiffuseColor", mDirLight.DiffuseColor);
    shader->SetVectorUniform("uDirLight.mSpecColor", mDirLight.SpecColor);
    
    // フォグ
    shader->SetFloatUniform("uFoginfo.maxDist", mFogMaxDist);
    shader->SetFloatUniform("uFoginfo.minDist", mFogMinDist);
    shader->SetVectorUniform("uFoginfo.color", mFogColor);
}

// スプライトコンポーネントの登録
void Renderer::AddSprite(SpriteComponent* sprite)
{
    // DrawOrderを探して 自分より優先度の高いものの次を見つける
    int drawOrder = sprite->GetDrawOrder();
    auto iter = mSpriteComps.begin();
    for (;iter != mSpriteComps.end(); ++iter)
    {
        if (drawOrder < (*iter)->GetDrawOrder())
        {
            break;
        }
    }

    // 見つけた箇所に挿入
    mSpriteComps.insert(iter, sprite);
}

// スプライト削除
void Renderer::RemoveSprite(SpriteComponent* sprite)
{
    auto iter = std::find(mSpriteComps.begin(), mSpriteComps.end(), sprite);
    if (iter != mSpriteComps.end())
    { // 要素が見つかった場合のみ削除
        mSpriteComps.erase(iter);
    }

}

// 背景スプライトコンポーネントの登録
void Renderer::AddBackGroundSprite(SpriteComponent* sprite)
{
    // DrawOrderを探して 自分より優先度の高いものの次を見つける
    int drawOrder = sprite->GetDrawOrder();
    auto iter = mBgSpriteComps.begin();
    for (;iter != mBgSpriteComps.end(); ++iter)
    {
        if (drawOrder < (*iter)->GetDrawOrder())
        {
            break;
        }
    }
    // 見つけた箇所に挿入
    mBgSpriteComps.insert(iter, sprite);
}

// スプライト削除
void Renderer::RemoveBackGroundSprite(SpriteComponent* sprite)
{
    auto iter = std::find(mBgSpriteComps.begin(), mBgSpriteComps.end(), sprite);
    if (iter != mBgSpriteComps.end())
    { // 要素が見つかった場合のみ削除
        mBgSpriteComps.erase(iter);
    }
}

// テクスチャ取り出し
Texture* Renderer::GetTexture(const std::string &fileName){
 
    Texture* tex = nullptr;
    auto iter = mTextures.find(fileName);
    if (iter != mTextures.end())
    {
        tex = iter->second.get();
    }
    else
    {
        std::unique_ptr<Texture> t = std::make_unique<Texture>();
        if(t->Load(fileName))
        {
            tex = t.get();
            mTextures.emplace(fileName, std::move(t));
        }
        else
        {
            tex = nullptr;
        }
    }
    return tex;
}

//スプライト用ポリゴン
void Renderer::CreateSpriteVerts()
{
    const float vertices[] = {
        -0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
        0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
        0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
        -0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  // bottom left
    };
    

    const unsigned int indices[] = {
        2, 1, 0,
        0, 3, 2
    };
    mSpriteVerts = std::make_unique<VertexArray>((float*)vertices, (unsigned int)4, (unsigned int*)indices, (unsigned int)6);
}

// メッシュ取り出し
Mesh* Renderer::GetMesh(const std::string& fileName, bool isRightHanded)
{
    Mesh* m = nullptr;
    auto iter = mMeshes.find(fileName);
    if (iter != mMeshes.end())
    {
        m = iter->second.get();
    }
    else
    {
        std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
        
        if (mesh->Load(fileName, this, isRightHanded))
        {
            m = mesh.get();
            mMeshes.emplace(fileName, std::move(mesh));
        }
        else
        {
            m = nullptr;
        }
    }
    return m;
}

// メッシュコンポーネント登録
void Renderer::AddMeshComp(MeshComponent* mesh)
{
    if (mesh->GetIsSkeletal())
    {
        SkeletalMeshComponent* sk = static_cast<SkeletalMeshComponent*>(mesh);
        mSkeletalMeshes.emplace_back(sk);
    }
    else
    {

        mMeshComps.emplace_back(mesh);
    }
 }


// メッシュコンポーネント削除
void Renderer::RemoveMeshComp(MeshComponent* mesh)
{
    if (mesh->GetIsSkeletal())
    {
        SkeletalMeshComponent* sk = static_cast<SkeletalMeshComponent*>(mesh);
        auto iter = std::find(mSkeletalMeshes.begin(), mSkeletalMeshes.end(), sk);
        if (iter != mSkeletalMeshes.end())
        { // 要素が見つかった場合のみ削除
            mSkeletalMeshes.erase(iter);
        }
    }
    else
    {
        auto iter = std::find(mMeshComps.begin(), mMeshComps.end(), mesh);
        if (iter != mMeshComps.end())
        { // 要素が見つかった場合のみ削除
            mMeshComps.erase(iter);
        }
    }
}

// BGメッシュコンポーネント登録
void Renderer::AddBackGroudMeshComp(MeshComponent* mesh)
{
    mBgMesh.emplace_back(mesh);
}


// BGメッシュコンポーネント削除
void Renderer::RemoveBackGroudMeshComp(MeshComponent* mesh)
{
    auto iter = std::find(mBgMesh.begin(), mBgMesh.end(), mesh);
    if (iter != mBgMesh.end())
    { // 要素が見つかった場合のみ削除
        mBgMesh.erase(iter);
    }
 
}

// エフェクトメッシュコンポーネント登録
void Renderer::AddEffectMeshComp(MeshComponent* mesh)
{
    mEffectMesh.emplace_back(mesh);
}

// BGメッシュコンポーネント削除
void Renderer::RemoveEffectMeshComp(MeshComponent* mesh)
{
    auto iter = std::find(mEffectMesh.begin(), mEffectMesh.end(), mesh);
    if (iter != mEffectMesh.end())
    { // 要素が見つかった場合のみ削除
        mEffectMesh.erase(iter);
    }
}

// パーティクルコンポーネント登録
void Renderer::AddParticleComp(ParticleComponent* part)
{
    int myDrawOrder = part->GetDrawOrder();
    auto iter = mParticleComps.begin();
    for (;iter != mParticleComps.end(); ++iter)
    {
        if (myDrawOrder < (*iter)->GetDrawOrder())
        {
            break;
        }
    }
    mParticleComps.insert(iter, part);
}

// パーティクルコンポーネント削除
void Renderer::RemoveParticleComp(ParticleComponent* part)
{
    auto iter = std::find(mParticleComps.begin(), mParticleComps.end(), part);
    if (iter != mParticleComps.end())
    { // 要素が見つかった場合のみ削除
        mParticleComps.erase(iter);
    }
}


// ビルボードコンポーネント登録
void Renderer::AddBillboardComp(BillboardComponent* billboard)
{
    // DrawOrderを探して 自分より優先度の高いものの次を見つける
    int drawOrder = billboard->GetDrawOrder();
    auto iter = mBillboardComps.begin();
    for (;iter != mBillboardComps.end(); ++iter)
    {
        if (drawOrder < (*iter)->GetDrawOrder())
        {
            break;
        }
    }

    // 見つけた箇所に挿入
    mBillboardComps.insert(iter, billboard);
}

// パーティクルコンポーネント登録
void Renderer::RemoveBillboardComp(BillboardComponent* billboard)
{
    auto iter = std::find(mBillboardComps.begin(), mBillboardComps.end(), billboard);
    if (iter != mBillboardComps.end())
    { // 要素が見つかった場合のみ削除
        mBillboardComps.erase(iter);
    }
}

// デバッガーコンポーネント登録
void Renderer::AddWireframeComp(WireframeComponent* wf)
{
    mWireframeComps.emplace_back(wf);
}

// パーティクルコンポーネント登録
void Renderer::RemoveWireframeComp(WireframeComponent* wf)
{
    auto iter = std::find(mWireframeComps.begin(), mWireframeComps.end(), wf);
    if (iter != mWireframeComps.end())
    { // 要素が見つかった場合のみ削除
        mWireframeComps.erase(iter);
    }
}

// データ解放
void Renderer::UnloadData()
{
    // テクスチャ削除
    mTextures.clear();
    // メッシュ削除
    mMeshes.clear();
    mSkeletalMeshes.clear();
}

// シャドウマッピング
bool Renderer::InitializeShadowMapping(int width, int height)
{
    mShadowWidth = width;
    mShadowHeight = height;

    // シャドウマップ用のFBO作成
    glGenFramebuffers(1, &mShadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mShadowFBO);

    // シャドウ用テクスチャ生成
    mShadowMapTexture = std::make_unique<Texture>();
    mShadowMapTexture->CreateShadowMap(width, height);

    // FBOにアタッチ
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           mShadowMapTexture->GetTextureID(), 0);

    // 確認用
    GLint compareMode;
    glBindTexture(GL_TEXTURE_2D, mShadowMapTexture->GetTextureID());
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, &compareMode);
    SDL_Log("Compare mode: %d", compareMode); // GL_COMPARE_REF_TO_TEXTUREでなければNG（値は 34894）
    
    // カラーバッファなし（深度のみ）
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // 完成チェック
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Error: Shadow framebuffer is not complete!" << std::endl;
        return false;
    }

    // FBOのバインド解除
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

// シャドウマップのレンダリング
void Renderer::RenderShadowMap()
{
    // FBOバインドして深度バッファだけ描画
    glBindFramebuffer(GL_FRAMEBUFFER, mShadowFBO);
    glViewport(0, 0, static_cast<GLsizei>(mShadowWidth), static_cast<GLsizei>(mShadowHeight));
    
    glEnable(GL_DEPTH_TEST);  // ← これ必要！！
    glClear(GL_DEPTH_BUFFER_BIT);

    // ビュー・プロジェクション行列をライトから構築
    //Vector3 lightPos = Vector3(50, 50, -30);
    //Vector3 targetPos = Vector3(0, 0, 10);
    Matrix4 lightViewMatrix = Matrix4::CreateLookAt(mDirLightPosition, mDirLightTarget, Vector3::UnitY);
    Matrix4 lightProjMatrix = Matrix4::CreateOrtho(50.0f, 50.0f, 10.f, 150.0f);
    mLightSpaceMatrix =  lightViewMatrix * lightProjMatrix;
    // スキンメッシュのシャドウ描画
    mShadowSkinnedShader->SetActive();
    mShadowSkinnedShader->SetMatrixUniform("uLightSpaceMatrix", mLightSpaceMatrix);

    for (auto& mesh : mSkeletalMeshes)
    {
        if (mesh->GetVisible())
        {
            mesh->DrawShadow(mShadowSkinnedShader.get(), mLightSpaceMatrix);
        }
    }
    
    // 通常メッシュのシャドウ描画（他のメッシュも追加してOK）
    mShadowMeshShader->SetActive();
    mShadowMeshShader->SetMatrixUniform("uLightSpaceMatrix", mLightSpaceMatrix);
    
    for (auto& mesh : mMeshComps)
    {
        if (mesh->GetVisible())
        {
            mesh->DrawShadow(mShadowMeshShader.get(), mLightSpaceMatrix);
        }
    }

    // ビューポートを戻す（スクリーンサイズ）
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, static_cast<GLsizei>(mScreenWidth), static_cast<GLsizei>(mScreenHeight));
}

void Renderer::SetDirectionalLightPosition(const Vector3 &pos, const Vector3 &target)
{
    mDirLightPosition = pos;
    mDirLightTarget = target;
}

void Renderer::SetFogInfo(const float max, const float min, Vector3 color)
{
    mFogMaxDist = max;
    mFogMinDist = min;
    mFogColor = color;
}
