#include "game.h"
#include "ApplicationEntry.h"
#include "HeroActor.h"

TOYLIB_REGISTER_APP_WITH_ARGS(Game, "sample game", 1600, 900, false)


Game::Game(std::string name)
: Application(name)
{
    
}
Game::Game(std::string name, float w, float h, bool b)
: Application(name, w, h, b)
{
    
}


Game::~Game()
{
    
}

void Game::InitGame()
{
    // DebugMode
    GetRenderer()->SetDebugMode(true);
    
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
    cc->SetDisp(true);
    cc->SetColliderType(C_WALL);
    
    
    CreateActor<HeroActor>();
}

void Game::UpdateGame(float deltaTime)
{
    
}

void Game::ShutdownGame()
{
    
}
