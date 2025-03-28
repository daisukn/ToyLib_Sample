#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include <string>


// アプリ制御クラス
class Application
{
public:
    // コンストラクタ
    Application(std::string title, float scWidth = 1280.f, float scHeight = 720.f, bool isFullScreen = false);
    // デストラクタ
    virtual ~Application();

    // 初期化
    bool Initialize();
    // メインループ
    void RunLoop();
    // 終了処理
    void Shutdown();
    
    // アクターを登録
    void AddActor(std::unique_ptr<class Actor> a);
    
    // アクター生成
    template <typename T, typename... Args>
    T* CreateActor(Args&&... args)
    {
        auto actor = std::make_unique<T>(this, std::forward<Args>(args)...);
        T* rawPtr = actor.get();
        AddActor(std::move(actor));
        return rawPtr;
    }
    
    // 描画エンジンを取得
    class Renderer* GetRenderer() const { return mRenderer.get(); }
    // 物理エンジンを取得
    class PhysWorld* GetPhysWorld() const { return mPhysWorld.get(); }
    
protected:
    virtual void UpdateGame(float deltaTime) { };
    virtual void InitGame() {};
    virtual void ShutdownGame() {};
    
    
private:
    
    // アプリの名前、Windowのタイトルバーに出る
    std::string mApplicationTitle;
    
    // ゲームデータ読み込み/開放
    void UnloadData();
    void LoadData();
    
    // フルスクリーンかどうか
    bool mIsFullScreen;
    // 解像度
    float mScreenW;
    float mScreenH;
    
    // false 終了、true 実行中
    bool mIsActive;
    // 描画エンジン
    std::unique_ptr<class Renderer> mRenderer;
    // 入力エンジン
    std::unique_ptr<class InputSystem> mInputSys;
    // 物理エンジン
    std::unique_ptr<class PhysWorld> mPhysWorld;
    // 時間管理
    unsigned int mTicksCount;

    
    // アクティブなアクター
    std::vector<std::unique_ptr<class Actor>> mActors;
    // ペンディング中のアクター
    std::vector<std::unique_ptr<class Actor>> mPendingActors;
    // true の場合Pendingsにまわす
    bool mIsUpdatingActors;

    // 入力受付
    void ProcessInput();
    // ゲームメイン
    void UpdateFrame();
    // 描画
    void Draw();

    // pause
    bool mIsPause;

};

