#include "game.h"

#include <cstdlib>
#include <ctime>

// What it does: Starts the text-based auto-battler game.
// What the inputs are: None.
// What the outputs are: Returns zero when the program finishes.
int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    Game game;
    game.start();

    return 0;
}
