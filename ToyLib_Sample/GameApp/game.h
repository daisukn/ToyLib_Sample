#include "Application.h"
#include <string>

class Game : public Application
{
public:
    Game(std::string name);
    ~Game();
protected:
    void InitGame() override;
    void UpdateGame(float deltaTime) override;
};
