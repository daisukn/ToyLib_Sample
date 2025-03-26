#include "game.h"
#include "Actor.h"
#include "SkeletalMeshComponent.h"
#include "ColliderComponent.h"
#include "BoundingVolumeComponent.h"
#include "Mesh.h"
#include "Renderer.h"

Game::Game(std::string name)
: Application(name)
{
    
}

Game::~Game()
{
    
}

void Game::InitGame()
{
    // ウルフ
    auto a = CreateActor<Actor>();
    auto mc = a->CreateComponent<SkeletalMeshComponent>();
    auto cc = a->CreateComponent<ColliderComponent>();
    mc->SetMesh(GetRenderer()->GetMesh("Assets/stan.gltf"));
    mc->SetAnimID(5, PLAY_CYCLIC);
    
    a->SetPosition(Vector3(0,-5,30));
    a->SetScale(1.f);
    Quaternion q = Quaternion(Vector3::UnitY, Math::ToRadians(0));
    a->SetRotation(q);

    cc->GetBoundingVolume()->ComputeBoundingVolume(GetRenderer()->GetMesh("Assets/stan.gltf")->GetVertexArray());
    //cc->GetBoundingVolume()->AdjustBoundingBox(Vector3(0.0f, 35, 30), Vector3(0.9, 0.9, 0.6));
    cc->GetBoundingVolume()->CreateVArray();
    cc->GetBoundingVolume()->SetVisible(true);
}

void Game::UpdateGame(float deltaTime)
{
    
}

void Game::ShutdownGame()
{
    
}
