#include "ToyLib.h"
#include <string>

class Game : public Application
{
public:
    Game(std::string name);
    Game(std::string name, float w, float h, bool b);
    ~Game();
protected:
    void InitGame() override;
    void UpdateGame(float deltaTime) override;
    void ShutdownGame() override;
};
