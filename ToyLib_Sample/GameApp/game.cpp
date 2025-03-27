#include "game.h"
#include "Actor.h"
#include "SkeletalMeshComponent.h"
#include "ColliderComponent.h"
#include "BoundingVolumeComponent.h"
#include "Mesh.h"
#include "Renderer.h"
#include "HeroActor.h"

Game::Game(std::string name)
: Application(name)
{
    
}

Game::~Game()
{
    
}

void Game::InitGame()
{
    
    // stan
    auto a = CreateActor<Actor>();
    auto mc = a->CreateComponent<SkeletalMeshComponent>();
    auto cc = a->CreateComponent<ColliderComponent>();
    mc->SetMesh(GetRenderer()->GetMesh("Assets/stan.gltf"));
    mc->SetAnimID(5, PLAY_CYCLIC);
    
    a->SetPosition(Vector3(-4,-2,10));
    a->SetScale(0.5f);
    Quaternion q = Quaternion(Vector3::UnitY, Math::ToRadians(30));
    a->SetRotation(q);

    cc->GetBoundingVolume()->ComputeBoundingVolume(GetRenderer()->GetMesh("Assets/stan.gltf")->GetVertexArray());
    cc->GetBoundingVolume()->AdjustBoundingBox(Vector3(0.0f, 0, 0), Vector3(0.5, 1.f, 0.6));
    cc->GetBoundingVolume()->CreateVArray();
    cc->GetBoundingVolume()->SetVisible(true);
    cc->SetDisp(true);
    cc->SetColliderType(C_WALL);
    
    auto hero = CreateActor<HeroActor>();
}

void Game::UpdateGame(float deltaTime)
{
    
}

void Game::ShutdownGame()
{
    
}
