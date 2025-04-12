#include "Renderer.h"
#include "Shader.h"
#include "LightingManager.h"
#include "SpriteComponent.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "ParticleComponent.h"
#include "BillboardComponent.h"
#include "VisualComponent.h"
#include "SkyDomeComponent.h"
#include "WireframeComponent.h"
#include <GL/glew.h>
#include <algorithm>
#include <string>

// コンストラクタ
Renderer::Renderer()
: mStrTitle("ToyLib App")
, mScreenWidth(0.f)
, mScreenHeight(0.f)
, mIsFullScreen(false)
, mPerspectiveFOV(45.f)
, mIsDebugMode(false)
, mClearColor(Vector3(0.2f, 0.5f, 0.8f))
, mShadowNear(10.f)
, mShadowFar(100)
, mShadowOrthoWidth(100.f)
, mShadowOrthoHeight(100.f)
, mShadowFBOWidth(4096)
, mShadowFBOHeight(4096)
, mWindow(nullptr)
, mGLContext(nullptr)
, mShaderPath("ToyLibCore/Shaders/")
, mRainAmount(0.5f)
, mFogAmount(0.f)
, mSnowAmount(0.5f)
{
    LoadSettings("Settings/Renderer_Settings.json");
}

// デストラクタ
Renderer::~Renderer()
{
}

// ウィンドウ生成とGL初期化
bool Renderer::Initialize()
{
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
   
    // ライティングマネージャ生成
    mLightingManager = std::make_shared<LightingManager>();
    // シェーダー のロードなどはここでやる
    LoadShaders();
    // スプライト用頂点バッファ
    CreateSpriteVerts();
    // オーバーレイエフェクト用頂点バッファ
    CreateFullScreenQuad();
    // シャドウマッピング初期化
    InitializeShadowMapping();
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderShadowMap();
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    DrawSky();
    DrawBackGround();
    DrawDebugger();
    DrawVisualLayer(VisualLayer::Background2D);
    DrawMesh();
    DrawVisualLayer(VisualLayer::Object3D);
    DrawVisualLayer(VisualLayer::Effect3D);
    
    DrawWeatherOverlay();
    
    DrawVisualLayer(VisualLayer::UI);

    SDL_GL_SwapWindow(mWindow);
}

// 背景オブジェクトの描画
void Renderer::DrawBackGround()
{
   
    
    // 背景用メッシュ描画
    
    auto shader = mShaders["Mesh"];
    
    shader->SetActive();
    mLightingManager->ApplyToShader(shader.get(), mViewMatrix);

    for (auto bg : mBgMesh)
    {
        if (bg->GetVisible())
        {
            shader->SetMatrixUniform("uViewProj", mViewMatrix * mProjectionMatrix);
            shader->SetMatrixUniform("uLightSpaceMatrix", mLightSpaceMatrix);
            shader->SetTextureUniform("uShadowMap", 1);
            shader->SetFloatUniform("uShadowBias", 0.005);
            shader->SetBooleanUniform("uUseToon", bg->GetToon()); // トゥーンON/OFF

            bg->Draw(shader.get());
        }
    }
}

void Renderer::DrawSky()
{
    if (!mSkyDomeComp) return;
    mSkyDomeComp->Draw(mShaders["SkyDome"].get());

}

// メッシュの描画
void Renderer::DrawMesh()
{

    mShadowMapTexture->SetActive(1); // 共通シャドウマップ用テクスチャ


    // 通常メッシュ描画（スキンなし）
    for (auto mc : mMeshComps)
    {
        if (!mc->GetVisible()) continue;
        
        auto shader = mShaders["Mesh"];
        shader->SetActive();
        mLightingManager->ApplyToShader(shader.get(), mViewMatrix);

        // ユニフォーム設定
        shader->SetMatrixUniform("uViewProj", mViewMatrix * mProjectionMatrix);
        shader->SetMatrixUniform("uLightSpaceMatrix", mLightSpaceMatrix);
        shader->SetTextureUniform("uShadowMap", 1);
        shader->SetFloatUniform("uShadowBias", 0.005);
        shader->SetBooleanUniform("uUseToon", mc->GetToon()); // トゥーンON/OFF
        mc->Draw(shader.get());
    }
    
    // スキンメッシュ描画
    for (auto sk : mSkeletalMeshes)
    {
        if (!sk->GetVisible()) continue;

        auto shader = mShaders["Skinned"];
        shader->SetActive();
        mLightingManager->ApplyToShader(shader.get(), mViewMatrix);

        // ユニフォーム設定
        shader->SetMatrixUniform("uViewProj", mViewMatrix * mProjectionMatrix);
        shader->SetMatrixUniform("uLightSpaceMatrix", mLightSpaceMatrix);
        shader->SetTextureUniform("uShadowMap", 1);
        shader->SetFloatUniform("uShadowBias", 0.005);
        shader->SetBooleanUniform("uUseToon", sk->GetToon()); // トゥーンON/OFF

        sk->Draw(shader.get());
    }
    
    glActiveTexture(GL_TEXTURE0);
}

void Renderer::DrawDebugger()
{
    // デバッグモードでない場合キャンセル
    if (!mIsDebugMode) return;
    
    // デバッガー用の描画
    mShaders["Solid"]->SetActive();
    mLightingManager->ApplyToShader(mShaders["Solid"].get(), mViewMatrix);
    mShaders["Solid"]->SetMatrixUniform("uViewProj", mViewMatrix * mProjectionMatrix);
    mShaders["Solid"]->SetVectorUniform("uSolColor", Vector3(1.f, 1.f, 1.f));
    // Update lighting uniforms
    for (auto wireframe : mWireframeComps)
    {
        wireframe->Draw(mShaders["Solid"].get());
    }
}

void Renderer::DrawEffect()
{
    // エフェクトメッシュ描画
    auto shader = mShaders["Mesh"];
    shader->SetActive();
    mLightingManager->ApplyToShader(shader.get(), mViewMatrix);
    shader->SetMatrixUniform("uViewProj", mViewMatrix * mProjectionMatrix);
    for (auto mesh : mEffectMesh)
    {
        if (mesh->GetVisible())
        {
            mesh->Draw(shader.get());
        }
    }
    
}

// シェーダー 読み込み
bool Renderer::LoadShaders()
{
    std::string vShaderName;
    std::string fShaderName;

    
    // 天気シェーダー（統合版）
    vShaderName = mShaderPath + "WeatherScreen.vert";
    fShaderName = mShaderPath + "WeatherScreen.frag";
    mShaders["WeatherOverlay"] = std::make_shared<Shader>();
    if (!mShaders["WeatherOverlay"]->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    // メッシュ用シェーダー
    vShaderName = mShaderPath + "Phong.vert";
    fShaderName = mShaderPath + "Toon.frag";
    mShaders["Mesh"] = std::make_shared<Shader>();
    if (!mShaders["Mesh"]->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    // スキンメッシュ用シェーダー
    vShaderName = mShaderPath + "Skinned.vert";
    fShaderName = mShaderPath + "Toon.frag";
    mShaders["Skinned"] = std::make_shared<Shader>();
    if (!mShaders["Skinned"]->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    // スプライト用シェーダー
    vShaderName = mShaderPath +"Sprite.vert";
    fShaderName = mShaderPath + "Sprite.frag";
    mShaders["Sprite"] = std::make_shared<Shader>();
    if (!mShaders["Sprite"]->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
    mShaders["Sprite"]->SetMatrixUniform("uViewProj", viewProj);
    // ビルボード用シェーダー
    vShaderName = mShaderPath + "Billboard.vert";
    fShaderName = mShaderPath + "Billboard.frag";
    mShaders["Billboard"] = std::make_shared<Shader>();
    if (!mShaders["Billboard"]->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    // パーティクル
    vShaderName = mShaderPath + "Billboard.vert";
    fShaderName = mShaderPath + "Particle.frag";
    mShaders["Particle"] = std::make_shared<Shader>();
    if (!mShaders["Particle"]->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    // ソリッドカラー
    vShaderName = mShaderPath + "BasicMesh.vert";
    fShaderName = mShaderPath + "SolidColor.frag";
    mShaders["Solid"] = std::make_shared<Shader>();
    if (!mShaders["Solid"]->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    // シャドウマップ用（Skinned）
    vShaderName = mShaderPath + "ShadowMapping_Skinned.vert";
    fShaderName = mShaderPath + "ShadowMapping.frag";
    mShaders["ShadowSkinned"] = std::make_shared<Shader>();
    if (!mShaders["ShadowSkinned"]->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    // シャドウマップ用（Mesh）
    vShaderName = mShaderPath + "ShadowMapping_Mesh.vert";
    fShaderName = mShaderPath + "ShadowMapping.frag";
    mShaders["ShadowMesh"] = std::make_shared<Shader>();
    if (!mShaders["ShadowMesh"]->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    // スカイドーム
    vShaderName = mShaderPath + "WeatherDome.vert";
    fShaderName = mShaderPath + "WeatherDome.frag";
    mShaders["SkyDome"] = std::make_shared<Shader>();
    if (!mShaders["SkyDome"]->Load(vShaderName.c_str(), fShaderName.c_str()))
    {
        return false;
    }
    // ビューマトリックス、プロジェクションマトリックス（デフォルト値）
    mViewMatrix = Matrix4::CreateLookAt(Vector3(0, 0.5f, -3), Vector3(0, 0, 10), Vector3::UnitY);
    mProjectionMatrix = Matrix4::CreatePerspectiveFOV(Math::ToRadians(mPerspectiveFOV), mScreenWidth, mScreenHeight, 1.0f, 2000.0f);

    return true;
}

void Renderer::AddVisualComp(VisualComponent* comp)
{
    auto iter = mVisualComps.begin();
    for (; iter != mVisualComps.end(); ++iter)
    {
        if (comp->GetDrawOrder() < (*iter)->GetDrawOrder())
            break;
    }
    mVisualComps.insert(iter, comp);
}

void Renderer::RemoveVisualComp(VisualComponent* comp)
{
    auto iter = std::find(mVisualComps.begin(), mVisualComps.end(), comp);
    if (iter != mVisualComps.end())
        mVisualComps.erase(iter);
}

void Renderer::DrawVisualLayer(VisualLayer layer)
{
    if (layer == VisualLayer::UI || layer == VisualLayer::Background2D)
    {
        glDisable(GL_DEPTH_TEST);     // Zテスト不要
        glDepthMask(GL_FALSE);        // 書き込みも不要（2D要素）
    }
    else if (layer == VisualLayer::Effect3D)
    {
        glEnable(GL_DEPTH_TEST);     // 粒同士のZ隠し合いを防ぐ
        glDepthMask(GL_FALSE);        // Zバッファ汚さない
    }
    else
    {
        glEnable(GL_DEPTH_TEST);      // 通常描画
        glDepthMask(GL_TRUE);         // 書き込みON
    }
    
    
    mSpriteVerts->SetActive();       // VAO

    for (auto comp : mVisualComps)
    {
        if (comp->IsVisible() && comp->GetLayer() == layer)
        {
            comp->Draw();
        }
    }

    glEnable(GL_DEPTH_TEST); // 念のため戻す
    glDepthMask(GL_TRUE);
}



// テクスチャ取り出し
Texture* Renderer::GetTexture(const std::string &fileName)
{
 
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

// 埋め込みテクスチャ
Texture* Renderer::GetEmbeddedTexture(const std::string& nameKey, const uint8_t* data, size_t dataSize)
{
    Texture* tex = nullptr;

    auto iter = mTextures.find(nameKey);
    if (iter != mTextures.end())
    {
        tex = iter->second.get();
    }
    else
    {
        std::unique_ptr<Texture> t = std::make_unique<Texture>();
        if (t->LoadFromMemory(data, static_cast<unsigned int>(dataSize)))
        {
            tex = t.get();
            mTextures.emplace(nameKey, std::move(t));
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
    const float vertices[] =
    {
        -0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
        0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
        0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
        -0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  // bottom left
    };
    

    const unsigned int indices[] =
    {
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

// エフェクトメッシュコンポーネント削除
void Renderer::RemoveEffectMeshComp(MeshComponent* mesh)
{
    auto iter = std::find(mEffectMesh.begin(), mEffectMesh.end(), mesh);
    if (iter != mEffectMesh.end())
    { // 要素が見つかった場合のみ削除
        mEffectMesh.erase(iter);
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
bool Renderer::InitializeShadowMapping()
{

    // シャドウマップ用のFBO作成
    glGenFramebuffers(1, &mShadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mShadowFBO);

    // シャドウ用テクスチャ生成
    mShadowMapTexture = std::make_unique<Texture>();
    mShadowMapTexture->CreateShadowMap(mShadowFBOWidth, mShadowFBOHeight);

    // FBOにアタッチ
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mShadowMapTexture->GetTextureID(), 0);

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
    glViewport(0, 0, static_cast<GLsizei>(mShadowFBOWidth), static_cast<GLsizei>(mShadowFBOHeight));
    
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    // ビュー・プロジェクション行列
    // カメラ追従
    Vector3 camCenter = mInvView.GetTranslation() + mInvView.GetZAxis() * 30.0f;
    //Vector3 lightDir = Vector3::Normalize(mDirLight.Direction);
    Vector3 lightDir = mLightingManager->GetLightDirection();
    Vector3 lightPos = camCenter - lightDir * 50.0f;
    
    Matrix4 lightViewMatrix = Matrix4::CreateLookAt(lightPos, camCenter, Vector3::UnitY);
    Matrix4 lightProjMatrix = Matrix4::CreateOrtho(mShadowOrthoWidth, mShadowOrthoHeight, mShadowNear, mShadowFar);
    mLightSpaceMatrix =  lightViewMatrix * lightProjMatrix;

    // スキンメッシュのシャドウ描画
    mShaders["ShadowSkinned"] ->SetActive();
    mShaders["ShadowSkinned"] ->SetMatrixUniform("uLightSpaceMatrix", mLightSpaceMatrix);

    for (auto& mesh : mSkeletalMeshes)
    {
        if (mesh->GetVisible())
        {
            mesh->DrawShadow(mShaders["ShadowSkinned"] .get(), mLightSpaceMatrix);
        }
    }
    
    // 通常メッシュのシャドウ描画
    mShaders["ShadowMesh"]->SetActive();
    mShaders["ShadowMesh"]->SetMatrixUniform("uLightSpaceMatrix", mLightSpaceMatrix);
    
    for (auto& mesh : mMeshComps)
    {
        if (mesh->GetVisible())
        {
            mesh->DrawShadow(mShaders["ShadowMesh"].get(), mLightSpaceMatrix);
        }
    }

    // ビューポートを戻す（スクリーンサイズ）
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, static_cast<GLsizei>(mScreenWidth), static_cast<GLsizei>(mScreenHeight));
}

// 雨エフェクトの初期化
void Renderer::CreateFullScreenQuad()
{
    // クアッド作成
    float quadVerts[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };
    unsigned int quadIndices[] = {
        0, 1, 2,
        2, 3, 0
    };
    mFullScreenQuad = std::make_unique<VertexArray>(quadVerts, 4, quadIndices, 6, true);

}

void Renderer::DrawWeatherOverlay()
{
    auto shader = mShaders["WeatherOverlay"];
    if (!shader || !mFullScreenQuad) return;

 
    // フルスクリーン用設定
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // シェーダーをアクティブ化
    shader->SetActive();

    // ユニフォーム設定
    shader->SetFloatUniform("uTime", SDL_GetTicks() / 1000.0f); // 経過時間（秒）
    shader->SetFloatUniform("uRainAmount", mRainAmount); // 0〜1で設定
    shader->SetFloatUniform("uFogAmount", mFogAmount); // 0〜1で設定
    shader->SetFloatUniform("uSnowAmount", mSnowAmount); // 0〜1で設定
    shader->SetVector2Uniform("uResolution", Vector2(mScreenWidth, mScreenHeight));

    // フルスクリーンポリゴンを描画
    mFullScreenQuad->SetActive();
    glDrawElements(GL_TRIANGLES, mFullScreenQuad->GetNumIndices(), GL_UNSIGNED_INT, nullptr);

    // 状態戻す
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::RegisterSkyDome(SkyDomeComponent* sky)
{
    mSkyDomeComp = sky;
    if (mSkyDomeComp)
    {
        mSkyDomeComp->SetLightingManager(mLightingManager);
    }
}

