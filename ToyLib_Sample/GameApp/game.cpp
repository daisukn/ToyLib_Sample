#include "game.h"
#include "Actor.h"
#include "SkeletalMeshComponent.h"
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
    Actor* a = new Actor(this);
    SkeletalMeshComponent* mc = new SkeletalMeshComponent(a);
    mc->SetMesh(GetRenderer()->GetMesh("Assets/wolf.fbx"));
    mc->SetAnimID(1, PLAY_CYCLIC);
    mc->SetScale(0.1);
    a->SetPosition(Vector3(0,0,30));
    Quaternion q = Quaternion(Vector3::UnitY, 180);
    a->SetRotation(q);
    

}

void Game::UpdateGame(float deltaTime)
{
    
}

void Game::ShutdownGame()
{
    
}
