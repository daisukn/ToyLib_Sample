#include "Application.h"
#include "Actor.h"
#include "Component.h"

#include <algorithm>
#include <iostream>

//コンストラクタ
Actor::Actor(Application* a)
    : mStatus(EActive)
    , mPosition(Vector3::Zero)
    , mRotation(Quaternion::Identity)
    , mScale(1.0f)
    , mApp(a)
    , mIsRecomputeWorldTransform(true)
{
    mApp->AddActor(this);
}


#include <iostream>
// デストラクタ
Actor::~Actor()
{
    // Applicationから削除
    mApp->RemoveActor(this);

    // 保持しているComponentはここで削除
    mComponents.clear();
    

}

// メインルーチン
void Actor::Update(float deltaTime)
{
    // EActiveの場合にComponentもUpdate
	if (mStatus == EActive)
    {
        // 派生先の処理を呼ぶ
        UpdateActor(deltaTime);
        // コンポーネントを更新
		UpdateComponents(deltaTime);
        // 座標系更新
        ComputeWorldTransform();

	}
}

// コンポーネントのUpdateを呼ぶ
void Actor::UpdateComponents(float deltaTime)
{
	for (auto comp : mComponents)
    {
		comp->Update(deltaTime);
	}
}


void Actor::UpdateActor(float deltaTime)
{
    // 実体は派生先でOverride
}

// 入力を受け取る
void Actor::ProcessInput(const struct InputState& state)
{
	if (mStatus == EActive)
    {
        // コンポーネントの入力処理
		for (auto comp : mComponents)
        {
			comp->ProcessInput(state);
		}
        // 派生先の入力処理に渡す
		ActorInput(state);
	}
}

// 入力処理　実装は派生先
void Actor::ActorInput(const struct InputState& state)
{
}

// ワールドマトリックス
void Actor::ComputeWorldTransform()
{
	if (mIsRecomputeWorldTransform)
    {
		mIsRecomputeWorldTransform = false;
		// Scale → rotate → translate
		mWorldTransform = Matrix4::CreateScale(mScale);
		mWorldTransform *= Matrix4::CreateFromQuaternion(mRotation);
		mWorldTransform *= Matrix4::CreateTranslation(mPosition);


		// 各Componentも計算する
		for (auto comp : mComponents)
		{
			comp->OnUpdateWorldTransform();
		}
	}
}

// コンポーネントを追加
void Actor::AddComponent(Component* component)
{
    // 優先度の高いものを探しす
	int order = component->GetUpdateOrder();
	auto iter = mComponents.begin();
	for (;iter != mComponents.end(); ++iter)
    {
		if (order < (*iter)->GetUpdateOrder())
        {
			break;
		}
	}
	// その次に挿入
	mComponents.insert(iter, component);
}

// コンポーネントを削除
void Actor::RemoveComponent(Component* component)
{	
    auto iter = std::find(mComponents.begin(), mComponents.end(), component);
	if (iter != mComponents.end())
    {
        mComponents.erase(iter);
	}
}


