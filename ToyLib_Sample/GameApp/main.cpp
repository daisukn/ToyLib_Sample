#include "game.h"
#include "SingleInstance.h"
#include <memory>


int main(int argc, char** argv)
{

    SingleInstance instance;
    if (!instance.IsLocked())
    {
        return 1;
    }

    
    std::unique_ptr<Game> game = std::make_unique<Game>("Sample Game");
    
    if (game->Initialize())
    {
        game->RunLoop();
    }
    game->Shutdown();
    return 0;
}

