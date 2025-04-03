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
    LoadData();

    auto spActor = CreateActor<Actor>();
    spActor->SetPosition(Vector3(-460.0f, -330.0f, 0.0f));
    spActor->SetScale(1);
    auto spSprite = spActor->CreateComponent<SpriteComponent>(100, VisualLayer::UI);
    spSprite->SetTexture(GetRenderer()->GetTexture("Assets/HealthBar.png"));
    spSprite->SetVisible(true);

    
    auto treeActor = CreateActor<Actor>();
    treeActor->SetPosition(Vector3(0.0f, 2.5f, 0.0f));
    treeActor->SetScale(0.02);
    auto treeBillboard = treeActor->CreateComponent<BillboardComponent>(100);
    treeBillboard->SetTexture(GetRenderer()->GetTexture("Assets/tree.png"));
    treeBillboard->SetVisible(true);

    auto shadow = treeActor->CreateComponent<ShadowSpriteComponent>(10);
    shadow->SetTexture(GetRenderer()->GetTexture("Assets/shadowcircle.png"));
    shadow->SetVisible(true);
    shadow->SetOffsetPosition(Vector3(0.0f, -4.3f, 0.0f));
    shadow->SetOffsetScale(0.03f);
    
    
    auto particleActor = CreateActor<Actor>();
    particleActor->SetPosition(Vector3(-15, -2, 15));
    auto particleComp = particleActor->CreateComponent<ParticleComponent>();
    particleComp->SetTexture(GetRenderer()->GetTexture("Assets/fire.png"));
    particleComp->CreateParticles(Vector3(0, 1, 0),
                                  10,
                                  1000,
                                  0.3,
                                  1.5,
                                  ParticleComponent::P_SMOKE);
    particleComp->SetAddBlend(true);
}

void Game::LoadData()
{
    
    auto hero = CreateActor<HeroActor>();
    
    // stan
    auto stanActor = CreateActor<Actor>();
    auto stanMesh = stanActor->CreateComponent<SkeletalMeshComponent>();
    auto stanCllider = stanActor->CreateComponent<ColliderComponent>();
    stanMesh->SetMesh(GetRenderer()->GetMesh("Assets/stan.gltf", true));
    stanMesh->SetAnimID(6, PLAY_CYCLIC);
    stanMesh->SetToonRender(true, 1.015f);
    
    stanActor->SetPosition(Vector3(-3,-2,10));
    stanActor->SetScale(0.5f);
    Quaternion q = Quaternion(Vector3::UnitY, Math::ToRadians(-30));
    stanActor->SetRotation(q);
    
    stanCllider->GetBoundingVolume()->ComputeBoundingVolume(GetRenderer()->GetMesh("Assets/stan.gltf")->GetVertexArray());
    stanCllider->GetBoundingVolume()->AdjustBoundingBox(Vector3(0.0f, 0, 0), Vector3(0.5, 1.f, 0.6));
    stanCllider->GetBoundingVolume()->CreateVArray();
    stanCllider->SetDisp(true);
    stanCllider->SetColliderType(C_WALL);
    
    auto stanCllider2 = stanActor->CreateComponent<ColliderComponent>();
    stanCllider2->GetBoundingVolume()->ComputeBoundingVolume(GetRenderer()->GetMesh("Assets/stan.gltf")->GetVertexArray());
    stanCllider2->GetBoundingVolume()->AdjustBoundingBox(Vector3(0.0f, 0, 0), Vector3(0.5, 1.f, 0.6));
    stanCllider2->SetDisp(true);
    stanCllider2->SetColliderType(C_ENEMY);

    
    auto stanMove = stanActor->CreateComponent<FollowMoveComponent>();
    stanMove->SetTarget(hero);
    stanMove->SetFollowSpeed(1);

    
    
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
    
    towerActor->SetPosition(Vector3(15, -2, 15));
    towerActor->SetScale(0.003f);
    q = Quaternion(Vector3::UnitY, Math::ToRadians(150));
    towerActor->SetRotation(q);
    
    
    // 焚き火
    auto fireActor = CreateActor<Actor>();
    auto fireMesh = fireActor->CreateComponent<MeshComponent>();
    fireMesh->SetMesh(GetRenderer()->GetMesh("Assets/Campfire.fbx"));
    
    fireActor->SetPosition(Vector3(-15, -2, 15));
    fireActor->SetScale(0.03f);
    auto fireCollider = fireActor->CreateComponent<ColliderComponent>();
    fireCollider->GetBoundingVolume()->ComputeBoundingVolume(GetRenderer()->GetMesh("Assets/Campfire.fbx")->GetVertexArray());
    fireCollider->SetDisp(true);
    fireCollider->SetColliderType(C_WALL);
    
    // 地面
    auto b = CreateActor<Actor>();
    auto g = b->CreateComponent<MeshComponent>();
    g->SetMesh(GetRenderer()->GetMesh("Assets/ground.x"));
    b->SetPosition(Vector3(0,-2,20));
    b->SetScale(0.5);
    g->SetToonRender(false);
    

}

void Game::UpdateGame(float deltaTime)
{
}

void Game::ShutdownGame()
{
    
}
