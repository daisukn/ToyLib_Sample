#include "Application.h"
#include "Actor.h"
#include "Renderer.h"
#include "InputSystem.h"
#include "PhysWorld.h"

#include <algorithm>



//#define __HIGH_RESOLUTION__

#ifdef __HIGH_RESOLUTION__
const float SCREEN_W = 1600.f;
const float SCREEN_H = 900.f;
#else
const float SCREEN_W = 1280.f;
const float SCREEN_H = 720.f;
#endif // __HIGH_RESOLUTION__


// コンストラクタ
Application::Application(std::string title)
    : mIsActive(false)
    , mIsUpdatingActors(false)
    , mIsPause(false)
    , mApplicationTitle(title)
{
    mRenderer = std::make_unique<class Renderer>();
    mInputSys = std::make_unique<class InputSystem>();
    mPhysWorld = std::make_unique<class PhysWorld>();
}

// デストラクタ
Application::~Application()
{
    //delete activeStage;
}


// アプリ初期化
bool Application::Initialize()
{
    
    // SDL初期化
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cout << "Failed to init SDL\n";
        return false;
    }
    
    // Renderer初期化
    mRenderer->Initialize(mApplicationTitle, SCREEN_W, SCREEN_H);

    // 入力システム初期化
    mInputSys->Initialize();
    
    // データ ロード、主にRendererに登録されるもの。
    LoadData();
    
    // 派生先のInitを呼ぶ
    InitGame();
    
    mIsActive = true;
    mTicksCount = SDL_GetTicks();
    return true;
}


// メインループ
void Application::RunLoop()
{
    while (mIsActive)
    {
        ProcessInput();
        UpdateFrame();
        Draw();
    }
}

// 描画処理 Rendererが描画する
void Application::Draw()
{
    mRenderer->Draw();
}


// 終了処理
void Application::Shutdown()
{
    ShutdownGame();
    UnloadData();
    mInputSys->Shutdown();
    mRenderer->Shutdown();
    SDL_Quit();
}

// 入力受付
void Application::ProcessInput(){
    
    // 前の状態を保存
    mInputSys->PrepareForUpdate();
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
                
        case SDL_QUIT:
            mIsActive = false;
            break;
        }
    }

    mInputSys->Update();
    const InputState& state = mInputSys->GetState();
        
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_ESCAPE) == EReleased)
    {
        mIsActive = false;
    }

    if (state.Keyboard.GetKeyState(SDL_SCANCODE_SPACE) == EHeld)
    {
        mIsPause = true;
    }
    else
    {
        mIsPause = false;
    }
    
    // 配下のActorsに渡す
    for (auto actor : mActors)
    {
        actor->ProcessInput(state);
    }
}


// Actor追加
void Application::AddActor(Actor* actor)
{
    // メインのActorsがUpdate中はPendingに追加
    if (mIsUpdatingActors)
    {
        mPendingActors.emplace_back(actor);
    }
    else
    {
        mActors.emplace_back(actor);
    }
}

// Actor削除
void Application::RemoveActor(Actor* actor)
{
    // Pendingにないか確認
    if (!mPendingActors.empty())
    {
        auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
        if (iter != mPendingActors.end())
        {
            std::iter_swap(iter, mPendingActors.end() - 1);
            mPendingActors.pop_back();
        }
    }


    // actorsにある場合は削除
    if (!mActors.empty())
    {
        auto iter = std::find(mActors.begin(), mActors.end(), actor);
        if (iter != mActors.end())
        {
            std::iter_swap(iter, mActors.end() - 1);
            mActors.pop_back();
        }
    }
}


// データ解放
void Application::UnloadData()
{
    mActors.clear();
    /*
    while (!actors.empty())
    {
        delete actors.back();
        actors.pop_back();
    }
    */
    if (mRenderer)
    {
        mRenderer->UnloadData();
    }
}

// Actors, Renderer関連
void Application::LoadData()
{
    // ライト
    mRenderer->SetAmbientLight(Vector3(0.9f, 0.9f, 0.9f));
    DirectionalLight& dir = mRenderer->GetDirectionalLight();
    dir.Direction = Vector3(0.f, 0.f, 1.f);
    dir.DiffuseColor = Vector3(0.4f, 0.4f, 0.4f);
    dir.SpecColor = Vector3(0.1f, 0.1f, 0.1f);
    

}

// ゲームメインルーチン
void Application::UpdateFrame()
{
    
    // FPS60固定
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
        ;

    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }
    mTicksCount = SDL_GetTicks();

    // ポーズ中以降の処理キャンセル
    if(mIsPause) return;
    
    // 派生先のUpdateを呼ぶ
    UpdateGame(deltaTime);
    
    // 物理計算系
    //physWorld->ComputeGroundHeight();
    mPhysWorld->Test();
    

    // Actorsメイン呼び出し
    mIsUpdatingActors = true;
    for (auto a : mActors)
    {
        a->Update(deltaTime);
    }
    mIsUpdatingActors = false;

    // Pendingがある場合はActorsに移動
    for (auto p : mPendingActors)
    {
        p->ComputeWorldTransform();
        mActors.emplace_back(p);
    }
    mPendingActors.clear();

    // EDeadフラグのアクターは削除
    std::vector<Actor*> deadActors;
    for (auto a : mActors)
    {
        if (a->GetState() == Actor::EDead)
        {
            deadActors.emplace_back(a);
        }
    }
    
    for (auto a : deadActors)
    {
        delete a;
    }

}

