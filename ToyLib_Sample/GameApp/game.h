#include "ToyLib.h"
#include <string>

class Game : public Application
{
public:
    Game();
    ~Game();
protected:
    void InitGame() override;
    void LoadData();
    void UpdateGame(float deltaTime) override;
    void ShutdownGame() override;
};
