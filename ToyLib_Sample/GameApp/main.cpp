#include "Application.h"
#include "SingleInstance.h"
#include <memory>


int main(int argc, char** argv)
{

    SingleInstance instance;
    if (!instance.IsLocked())
    {
        return 1;
    }

    
    std::unique_ptr<Application> app = std::make_unique<Application>();
    
    if (app->Initialize())
    {
        app->RunLoop();
    }
    app->Shutdown();
    return 0;
}

