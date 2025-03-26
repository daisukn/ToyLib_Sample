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
    auto a = CreateActor<Actor>();
    auto mc = a->CreateComponent<SkeletalMeshComponent>();
    mc->SetMesh(GetRenderer()->GetMesh("Assets/wolf.fbx"));
    mc->SetAnimID(3, PLAY_CYCLIC);
    mc->SetScale(0.1);
    a->SetPosition(Vector3(0,-5,30));
    Quaternion q = Quaternion(Vector3::UnitY, Math::ToRadians(150));
    a->SetRotation(q);
    

}

void Game::UpdateGame(float deltaTime)
{
    
}

void Game::ShutdownGame()
{
    
}
