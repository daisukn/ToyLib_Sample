#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include <string>

extern const float SCREEN_W;
extern const float SCREEN_H;

enum class StageTransitionID
{
    STAGE_TITLE,
    STAGE_CLOUD,
    STAGE_END
};


// アプリ制御クラス
class Application
{
public:
    // コンストラクタ
    Application(std::string title);
    // デストラクタ
    virtual ~Application();

    // 初期化
    bool Initialize();
    // メインループ
    void RunLoop();
    // 終了処理
    void Shutdown();
    
    // アクターを登録/削除
    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);
    
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
    std::vector<class Actor*> mActors;
    // ペンディング中のアクター
    std::vector<class Actor*> mPendingActors;
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

