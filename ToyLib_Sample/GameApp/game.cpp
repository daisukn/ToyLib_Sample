#include "game.h"
#include "ApplicationEntry.h"
#include "HeroActor.h"

TOYLIB_REGISTER_APP_WITH_ARGS(Game)


Game::Game()
: Application()
{
    
}

Game::~Game()
{
    
}

void Game::InitGame()
{
    // DebugMode
    GetRenderer()->SetDebugMode(false);
    
    
    auto hero = CreateActor<HeroActor>();
    
    // stan
    auto a = CreateActor<Actor>();
    auto mc = a->CreateComponent<SkeletalMeshComponent>();
    auto cc = a->CreateComponent<ColliderComponent>();
    mc->SetMesh(GetRenderer()->GetMesh("Assets/stan.gltf", true));
    mc->SetAnimID(6, PLAY_CYCLIC);
    mc->SetToonRender(true, 1.015f);
    
    a->SetPosition(Vector3(-3,-2,10));
    a->SetScale(0.5f);
    Quaternion q = Quaternion(Vector3::UnitY, Math::ToRadians(-30));
    a->SetRotation(q);
    
    cc->GetBoundingVolume()->ComputeBoundingVolume(GetRenderer()->GetMesh("Assets/stan.gltf")->GetVertexArray());
    cc->GetBoundingVolume()->AdjustBoundingBox(Vector3(0.0f, 0, 0), Vector3(0.5, 1.f, 0.6));
    cc->GetBoundingVolume()->CreateVArray();
    cc->SetDisp(true);
    cc->SetColliderType(C_WALL);
    
    auto cc2 = a->CreateComponent<ColliderComponent>();
    cc2->GetBoundingVolume()->ComputeBoundingVolume(GetRenderer()->GetMesh("Assets/stan.gltf")->GetVertexArray());
    cc2->GetBoundingVolume()->AdjustBoundingBox(Vector3(0.0f, 0, 0), Vector3(0.5, 1.f, 0.6));
    cc2->GetBoundingVolume()->CreateVArray();
    cc2->SetDisp(true);
    cc2->SetColliderType(C_ENEMY);

    
    auto move = a->CreateComponent<FollowMoveComponent>();
    move->SetTarget(hero);
    move->SetFollowSpeed(1);

    
    
    // 建物
    auto towerActor = CreateActor<Actor>();
    auto towerMesh = towerActor->CreateComponent<MeshComponent>();
    towerMesh->SetMesh(GetRenderer()->GetMesh("Assets/house.x"));
    //towerMesh->SetScale(0.001f);
    
    auto towerCollider = towerActor->CreateComponent<ColliderComponent>();
    towerCollider->GetBoundingVolume()->ComputeBoundingVolume(GetRenderer()->GetMesh("Assets/house.x")->GetVertexArray());
    towerCollider->GetBoundingVolume()->CreateVArray();
    towerCollider->SetDisp(true);
    towerCollider->SetColliderType(C_WALL);
    
    towerActor->SetPosition(Vector3(8, -2, 15));
    towerActor->SetScale(0.001f);
    q = Quaternion(Vector3::UnitY, Math::ToRadians(150));
    towerActor->SetRotation(q);
    
    // 地面
    auto b = CreateActor<Actor>();
    auto g = b->CreateComponent<MeshComponent>();
    g->SetMesh(GetRenderer()->GetMesh("Assets/ground.x"));
    b->SetPosition(Vector3(0,-2,20));
    b->SetScale(0.5);
    g->SetToonRender(true);
    

}

void Game::UpdateGame(float deltaTime)
{
    
}

void Game::ShutdownGame()
{
    
}
