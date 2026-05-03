#include "game.h"

#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <thread>

// What it does: Creates the game state and the first shop.
// What the inputs are: None.
// What the outputs are: Initializes player, shop pets, turn, wins, and message log.
Game::Game() {
    player = new Player();
    currentTurn = 1;
    wins = 0;
    difficultyMode = NORMAL_MODE;

    for (int i = 0; i < 3; i++) {
        shopPets[i] = nullptr;
    }

    for (int i = 0; i < 5; i++) {
        enemyTeam[i] = nullptr;
    }

    rollShop();
    addLog("Welcome to the pet shop.");
}

// What it does: Destroys the game and frees owned dynamic memory.
// What the inputs are: None.
// What the outputs are: Deletes the player and all remaining shop pet pointers.
Game::~Game() {
    delete player;
    player = nullptr;

    clearShop();
    clearEnemyTeam();
}

// What it does: Shows the title screen and lets the player choose a menu option.
// What the inputs are: None.
// What the outputs are: Returns true to start the game, or false to exit.
bool Game::selectDifficulty() {
    std::string choice;

    while (true) {
        std::cout << "\033[2J\033[1;1H";
        std::cout << R"(
   ____ _     ___      _         _          ____      _
  / ___| |   |_ _|    / \  _   _| |_ ___   |  _ \ ___| |_ ___
 | |   | |    | |    / _ \| | | | __/ _ \  | |_) / _ \ __/ __|
 | |___| |___ | |   / ___ \ |_| | || (_) | |  __/  __/ |_\__ \
  \____|_____|___| /_/   \_\__,_|\__\___/  |_|   \___|\__|___/


)";
        std::cout << YELLOW << "     Please maximize your terminal window to avoid UI misalignment!" << RESET << std::endl;
        std::cout << R"(
                    [1] Normal Mode
                    [2] Hard Mode
                    [3] Exit Game
)";
        std::cout << std::endl;
        std::cout << "Choice: ";

        if (!std::getline(std::cin, choice)) {
            difficultyMode = NORMAL_MODE;
            addLog("Normal mode selected.");
            return true;
        }

        choice = preprocessCommand(choice);

        if (choice == "1" || choice == "normal" || choice == "n") {
            difficultyMode = NORMAL_MODE;
            std::cout << "Starting CLI Auto Pets in Normal Mode." << std::endl;
            addLog("Normal mode selected.");
            return true;
        }

        if (choice == "2" || choice == "hard" || choice == "h") {
            difficultyMode = HARD_MODE;
            std::cout << "Hard Mode enabled. Good luck." << std::endl;
            addLog("Hard mode selected.");
            return true;
        }

        if (choice == "3" || choice == "exit" || choice == "x") {
            std::cout << "Goodbye!" << std::endl;
            return false;
        }

        std::cout << "Invalid difficulty choice. Press Enter to try again.";
        std::getline(std::cin, choice);
    }
}

// What it does: Gets the current difficulty name.
// What the inputs are: None.
// What the outputs are: Returns Normal or Hard as text.
std::string Game::getDifficultyName() const {
    if (difficultyMode == HARD_MODE) {
        return "Hard";
    }

    return "Normal";
}

// What it does: Adds a message to the game message log.
// What the inputs are: The message text, delay flag, and optional active battle teams.
// What the outputs are: Updates the message log and may redraw the UI with a short delay.
void Game::addLog(std::string msg, bool autoDelay, Pet** activePlayerTeam, Pet** activeEnemyTeam) {
    messageLog.push_back(msg);

    if (messageLog.size() > 5) {
        messageLog.erase(messageLog.begin());
    }

    if (autoDelay) {
        drawUI(activePlayerTeam, activeEnemyTeam);
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }
}

// What it does: Prints one fixed-width box cell with manual spacing.
// What the inputs are: The cell text and the inside cell width.
// What the outputs are: Prints one aligned cell content line.
void Game::printBoxCell(std::string text, int width) {
    int visualLength = static_cast<int>(text.size());
    std::string::size_type dotPosition;

    if (text.find("♥") != std::string::npos) {
        visualLength = visualLength - 2;
    }

    dotPosition = text.find("●");
    while (dotPosition != std::string::npos) {
        visualLength = visualLength - 2;
        dotPosition = text.find("●", dotPosition + 1);
    }

    dotPosition = text.find("○");
    while (dotPosition != std::string::npos) {
        visualLength = visualLength - 2;
        dotPosition = text.find("○", dotPosition + 1);
    }

    std::cout << "|" << text;

    for (int i = visualLength; i < width; i++) {
        std::cout << " ";
    }

    std::cout << "|";
}

// What it does: Builds the pet name and experience display line.
// What the inputs are: The pet pointer to display.
// What the outputs are: Returns a short name line with level and experience bar.
std::string Game::getPetNameLine(Pet* pet) {
    std::string petName;
    std::string expBar;
    std::string nameLine;

    if (pet == nullptr) {
        return "[   Empty   ]";
    }

    petName = pet->getName();

    if (pet->getLevel() == 1) {
        if (pet->getExperience() == 0) {
            expBar = "[○○]";
        } else {
            expBar = "[●○]";
        }
    } else if (pet->getLevel() == 2) {
        if (pet->getExperience() == 0) {
            expBar = "[○○○]";
        } else if (pet->getExperience() == 1) {
            expBar = "[●○○]";
        } else {
            expBar = "[●●○]";
        }
    } else {
        expBar = "[MAX]";
    }

    while (petName.size() > 3 && ("Lv" + std::to_string(pet->getLevel()) + " " + petName + " " + expBar).size() > 23) {
        petName.erase(petName.size() - 1);
    }

    nameLine = "Lv" + std::to_string(pet->getLevel()) + " " + petName + " " + expBar;
    return nameLine;
}

// What it does: Draws the full shop phase user interface.
// What the inputs are: Optional active player and enemy teams to display.
// What the outputs are: Prints the game stats, team, shop, menu, log, and input prompt.
void Game::drawUI(Pet** activePlayerTeam, Pet** activeEnemyTeam) {
    std::cout << "\033[2J\033[1;1H";

    std::cout << CYAN << "================================================================================" << RESET << std::endl;
    std::cout << YELLOW << "[ Gold: " << player->getGold() << " ]" << RESET << "  ";
    std::cout << RED << "[ HP: ♥x" << player->getHp() << " ]" << RESET << "  ";
    std::cout << "[ Turn: " << currentTurn << " ]  ";
    std::cout << "[ Wins: " << wins << "/10 ]  ";
    std::cout << "[ Mode: " << getDifficultyName() << " ]" << std::endl;
    std::cout << CYAN << "================================================================================" << RESET << std::endl;
    std::cout << std::endl;

    std::cout << GREEN << "TEAM SLOTS" << RESET << std::endl;
    for (int i = 0; i < 5; i++) {
        std::cout << "+-----------------+";
    }
    std::cout << std::endl;
    for (int i = 0; i < 5; i++) {
        std::string slotLine = "Slot " + std::to_string(i + 1);
        printBoxCell(slotLine, 17);
    }
    std::cout << std::endl;
    for (int i = 0; i < 5; i++) {
        Pet* pet = player->getTeamPet(i);
        std::string nameLine;

        if (activePlayerTeam != nullptr) {
            pet = activePlayerTeam[i];
        }

        nameLine = getPetNameLine(pet);

        printBoxCell(nameLine, 17);
    }
    std::cout << std::endl;
    for (int i = 0; i < 5; i++) {
        Pet* pet = player->getTeamPet(i);
        std::string statsLine = "";

        if (activePlayerTeam != nullptr) {
            pet = activePlayerTeam[i];
        }

        if (pet != nullptr) {
            statsLine = "ATK:" + std::to_string(pet->getAttack());
            statsLine = statsLine + " HP:♥" + std::to_string(pet->getHp());
        }

        printBoxCell(statsLine, 17);
    }
    std::cout << std::endl;
    for (int i = 0; i < 5; i++) {
        std::cout << "+-----------------+";
    }
    std::cout << std::endl;
    std::cout << CYAN << "--------------------------------------------------------------------------------" << RESET << std::endl;
    std::cout << std::endl;

    if (activeEnemyTeam == nullptr) {
        std::cout << GREEN << "SHOP SLOTS" << RESET << std::endl;
    } else {
        std::cout << GREEN << "ENEMY TEAM" << RESET << std::endl;
    }
    for (int i = 0; i < 5; i++) {
        if (activeEnemyTeam == nullptr && i >= 3) {
            break;
        }

        std::cout << "+-----------------+";
    }
    std::cout << std::endl;
    for (int i = 0; i < 5; i++) {
        std::string shopLine = "Enemy " + std::to_string(i + 1);

        if (activeEnemyTeam == nullptr && i >= 3) {
            break;
        }

        if (activeEnemyTeam == nullptr) {
            shopLine = "Shop ";
            shopLine = shopLine + static_cast<char>('A' + i);
        }

        printBoxCell(shopLine, 17);
    }
    std::cout << std::endl;
    for (int i = 0; i < 5; i++) {
        Pet* pet = nullptr;
        std::string nameLine;

        if (activeEnemyTeam == nullptr && i >= 3) {
            break;
        }

        if (activeEnemyTeam == nullptr) {
            pet = shopPets[i];
        } else {
            pet = activeEnemyTeam[i];
        }

        nameLine = getPetNameLine(pet);

        printBoxCell(nameLine, 17);
    }
    std::cout << std::endl;
    for (int i = 0; i < 5; i++) {
        Pet* pet = nullptr;
        std::string statsLine = "";

        if (activeEnemyTeam == nullptr && i >= 3) {
            break;
        }

        if (activeEnemyTeam == nullptr) {
            pet = shopPets[i];
        } else {
            pet = activeEnemyTeam[i];
        }

        if (pet != nullptr) {
            statsLine = "ATK:" + std::to_string(pet->getAttack());
            statsLine = statsLine + " HP:♥" + std::to_string(pet->getHp());
        }

        printBoxCell(statsLine, 17);
    }
    std::cout << std::endl;
    for (int i = 0; i < 5; i++) {
        if (activeEnemyTeam == nullptr && i >= 3) {
            break;
        }

        std::cout << "+-----------------+";
    }
    std::cout << std::endl;
    std::cout << CYAN << "--------------------------------------------------------------------------------" << RESET << std::endl;
    std::cout << std::endl;

    std::cout << "--- MESSAGE LOG ---" << std::endl;
    for (int i = 0; i < static_cast<int>(messageLog.size()); i++) {
        std::cout << messageLog[i] << std::endl;
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << CYAN << "==================== ACTIONS ====================" << RESET << std::endl;
    std::cout << "  " << GREEN << "[ r ]" << RESET << "     : Roll Shop (-1 Gold)" << std::endl;
    std::cout << "  " << GREEN << "[ b A 1 ]" << RESET << " : Buy Shop Pet 'A' -> Team Slot 1" << std::endl;
    std::cout << "  " << GREEN << "[ s 1 ]" << RESET << "   : Sell Team Pet in Slot 1" << std::endl;
    std::cout << "  " << GREEN << "[ m 1 4 ]" << RESET << " : Move/Swap Team Slot 1 and 4" << std::endl;
    std::cout << "  " << GREEN << "[ c 1 4 ]" << RESET << " : Combine Slot 1 into Slot 4" << std::endl;
    std::cout << "  " << GREEN << "[ e ]" << RESET << "     : End Shop Phase & Start Battle" << std::endl;
    std::cout << CYAN << "=================================================" << RESET << std::endl;
    std::cout << "Action Input: ";
}

// What it does: Creates a random new pet up to a chosen tier.
// What the inputs are: The highest allowed pet tier.
// What the outputs are: Returns a newly allocated pet pointer.
Pet* Game::createRandomPetByTier(int maxTier) {
    int choiceCount = 3;

    if (maxTier == 2) {
        choiceCount = 6;
    } else if (maxTier >= 3) {
        choiceCount = 9;
    }

    int choice = std::rand() % choiceCount;

    if (choice == 0) {
        return new Swan();
    } else if (choice == 1) {
        return new Ant();
    } else if (choice == 2) {
        return new Mosquito();
    } else if (choice == 3) {
        return new Camel();
    } else if (choice == 4) {
        return new Skunk();
    } else if (choice == 5) {
        return new Elephant();
    } else if (choice == 6) {
        return new Hippo();
    } else if (choice == 7) {
        return new Blowfish();
    }

    return new Monkey();
}

// What it does: Deletes all shop pets and clears the shop slots.
// What the inputs are: None.
// What the outputs are: Frees shop pet pointers and sets shop slots to nullptr.
void Game::clearShop() {
    for (int i = 0; i < 3; i++) {
        if (shopPets[i] != nullptr) {
            delete shopPets[i];
            shopPets[i] = nullptr;
        }
    }
}

// What it does: Deletes all enemy pets and clears the enemy team slots.
// What the inputs are: None.
// What the outputs are: Frees enemy pet pointers and sets enemy slots to nullptr.
void Game::clearEnemyTeam() {
    for (int i = 0; i < 5; i++) {
        if (enemyTeam[i] != nullptr) {
            delete enemyTeam[i];
            enemyTeam[i] = nullptr;
        }
    }
}

// What it does: Copies the player's team into a temporary team.
// What the inputs are: The target temporary team array.
// What the outputs are: Creates cloned pet pointers in the target team.
void Game::clonePlayerTeam(Pet* targetTeam[5]) {
    for (int i = 0; i < 5; i++) {
        targetTeam[i] = nullptr;

        if (player->getTeamPet(i) != nullptr) {
            targetTeam[i] = player->getTeamPet(i)->clone();
        }
    }
}

// What it does: Copies the enemy team into a temporary team.
// What the inputs are: The target temporary team array.
// What the outputs are: Creates cloned enemy pet pointers in the target team.
void Game::cloneEnemyTeam(Pet* targetTeam[5]) {
    for (int i = 0; i < 5; i++) {
        targetTeam[i] = nullptr;

        if (enemyTeam[i] != nullptr) {
            targetTeam[i] = enemyTeam[i]->clone();
        }
    }
}

// What it does: Deletes all pets in a temporary team.
// What the inputs are: The temporary team array to clear.
// What the outputs are: Frees all pet pointers and sets slots to nullptr.
void Game::clearTempTeam(Pet* team[5]) {
    for (int i = 0; i < 5; i++) {
        if (team[i] != nullptr) {
            delete team[i];
            team[i] = nullptr;
        }
    }
}

// What it does: Finds the right-most living pet in a team.
// What the inputs are: The team pointer array.
// What the outputs are: Returns the front pet index or -1 if no pet exists.
int Game::findFrontIndex(Pet* team[5]) {
    for (int i = 4; i >= 0; i--) {
        if (team[i] != nullptr && team[i]->isAlive()) {
            return i;
        }
    }

    return -1;
}

// What it does: Moves all remaining pets toward the right side.
// What the inputs are: The team pointer array to shift.
// What the outputs are: Keeps pet order while filling slots toward index four.
void Game::shiftTeamRight(Pet* team[5]) {
    Pet* shiftedTeam[5];
    int writeIndex = 4;

    for (int i = 0; i < 5; i++) {
        shiftedTeam[i] = nullptr;
    }

    for (int i = 4; i >= 0; i--) {
        if (team[i] != nullptr) {
            shiftedTeam[writeIndex] = team[i];
            writeIndex--;
        }
    }

    for (int i = 0; i < 5; i++) {
        team[i] = shiftedTeam[i];
    }
}

// What it does: Checks whether a team has any living pet.
// What the inputs are: The team pointer array.
// What the outputs are: Returns true if at least one living pet exists.
bool Game::hasAnyPet(Pet* team[5]) {
    return findFrontIndex(team) != -1;
}

// What it does: Removes all fainted pets from a temporary team.
// What the inputs are: The team to clean and the active teams for battle display.
// What the outputs are: Triggers faint skills, deletes fainted pets, and shifts the team right.
void Game::removeFaintedPets(Pet* team[5], Pet** activePlayerTeam, Pet** activeEnemyTeam) {
    bool removedPet = false;

    for (int i = 0; i < 5; i++) {
        if (team[i] != nullptr && team[i]->getHp() <= 0) {
            std::string faintedName = team[i]->getName();
            team[i]->onFaint(team, 5);

            if (faintedName == "Ant") {
                addLog(faintedName + " faint skill triggered.", true, activePlayerTeam, activeEnemyTeam);
            }

            delete team[i];
            team[i] = nullptr;
            addLog(faintedName + " fainted.", true, activePlayerTeam, activeEnemyTeam);
            removedPet = true;
        }
    }

    if (removedPet) {
        shiftTeamRight(team);
        drawUI(activePlayerTeam, activeEnemyTeam);
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }
}

// What it does: Replaces the whole shop with three turn-limited random pets.
// What the inputs are: None.
// What the outputs are: Deletes old shop pets and creates three new shop pets for the current turn.
void Game::rollShop() {
    int maxTier = 1;

    clearShop();

    if (currentTurn <= 2) {
        maxTier = 1;
    } else if (currentTurn <= 4) {
        maxTier = 2;
    } else {
        maxTier = 3;
    }

    for (int i = 0; i < 3; i++) {
        shopPets[i] = createRandomPetByTier(maxTier);
    }
}

// What it does: Converts a shop slot letter into an array index.
// What the inputs are: The shop slot letter.
// What the outputs are: Returns 0, 1, or 2 for valid letters, otherwise -1.
int Game::getShopIndex(char shopLetter) {
    if (shopLetter == 'A' || shopLetter == 'a') {
        return 0;
    }

    if (shopLetter == 'B' || shopLetter == 'b') {
        return 1;
    }

    if (shopLetter == 'C' || shopLetter == 'c') {
        return 2;
    }

    return -1;
}

// What it does: Removes spaces and changes command text to lowercase.
// What the inputs are: The raw input line from the player.
// What the outputs are: Returns the cleaned command string.
std::string Game::preprocessCommand(std::string inputLine) {
    std::string command;

    for (int i = 0; i < static_cast<int>(inputLine.size()); i++) {
        char currentChar = inputLine[i];

        if (currentChar == ' ') {
            continue;
        }

        if (currentChar >= 'A' && currentChar <= 'Z') {
            currentChar = currentChar - 'A' + 'a';
        }

        command = command + currentChar;
    }

    return command;
}

// What it does: Applies turn-based level and stat scaling to an enemy pet.
// What the inputs are: The enemy pet pointer and the current turn.
// What the outputs are: Updates the enemy pet level and stats.
void Game::applyEnemyScaling(Pet* pet, int turn) {
    applyNormalEnemyScaling(pet, turn);

    if (difficultyMode == HARD_MODE) {
        applyHardEnemyScaling(pet, turn);
        boostHardEnemyStats(pet);
    }
}

// What it does: Applies Normal mode level and stat scaling to an enemy pet.
// What the inputs are: The enemy pet pointer and the current turn.
// What the outputs are: Updates the enemy pet level and stats for Normal mode.
void Game::applyNormalEnemyScaling(Pet* pet, int turn) {
    int attackBonus = 0;
    int hpBonus = 0;
    int levelRoll = std::rand() % 100;

    if (pet == nullptr) {
        return;
    }

    if (turn == 1) {
        attackBonus = 0;
        hpBonus = 0;
    } else if (turn == 2) {
        attackBonus = std::rand() % 2;
        hpBonus = std::rand() % 2;
    } else if (turn == 3) {
        if (levelRoll < 10) {
            pet->setLevel(2);
        }

        attackBonus = std::rand() % 2;
        hpBonus = std::rand() % 2;
    } else if (turn == 4) {
        if (levelRoll < 30) {
            pet->setLevel(2);
        }

        attackBonus = 1 + (std::rand() % 2);
        hpBonus = 1 + (std::rand() % 2);
    } else if (turn == 5) {
        if (levelRoll < 50) {
            pet->setLevel(2);
        }

        attackBonus = 1 + (std::rand() % 3);
        hpBonus = 1 + (std::rand() % 3);
    } else if (turn == 6) {
        if (levelRoll < 75) {
            pet->setLevel(2);
        }

        attackBonus = 2 + (std::rand() % 3);
        hpBonus = 2 + (std::rand() % 3);
    } else if (turn == 7) {
        if (levelRoll < 10) {
            pet->setLevel(3);
        } else {
            pet->setLevel(2);
        }

        attackBonus = 3 + (std::rand() % 3);
        hpBonus = 3 + (std::rand() % 3);
    } else if (turn == 8) {
        if (levelRoll < 30) {
            pet->setLevel(3);
        } else {
            pet->setLevel(2);
        }

        attackBonus = 4 + (std::rand() % 3);
        hpBonus = 4 + (std::rand() % 3);
    } else if (turn == 9) {
        if (levelRoll < 50) {
            pet->setLevel(3);
        } else {
            pet->setLevel(2);
        }

        attackBonus = 5 + (std::rand() % 3);
        hpBonus = 5 + (std::rand() % 3);
    } else {
        if (levelRoll < 80) {
            pet->setLevel(3);
        } else {
            pet->setLevel(2);
        }

        attackBonus = 6 + (std::rand() % 4);
        hpBonus = 6 + (std::rand() % 4);
    }

    pet->buffStats(attackBonus, hpBonus);
}

// What it does: Applies Hard mode level scaling to an enemy pet.
// What the inputs are: The enemy pet pointer and the current turn.
// What the outputs are: Updates the enemy pet level for Hard mode.
void Game::applyHardEnemyScaling(Pet* pet, int turn) {
    int levelRoll = std::rand() % 100;

    if (pet == nullptr) {
        return;
    }

    if (turn == 1) {
        pet->setLevel(1);
    } else if (turn == 2) {
        if (levelRoll < 10) {
            pet->setLevel(2);
        } else {
            pet->setLevel(1);
        }
    } else if (turn == 3) {
        if (levelRoll < 20) {
            pet->setLevel(2);
        } else {
            pet->setLevel(1);
        }
    } else if (turn == 4) {
        if (levelRoll < 40) {
            pet->setLevel(2);
        } else {
            pet->setLevel(1);
        }
    } else if (turn == 5) {
        if (levelRoll < 60) {
            pet->setLevel(2);
        }
    } else if (turn == 6) {
        if (levelRoll < 10) {
            pet->setLevel(3);
        } else {
            pet->setLevel(2);
        }
    } else if (turn == 7) {
        if (levelRoll < 20) {
            pet->setLevel(3);
        } else {
            pet->setLevel(2);
        }
    } else if (turn == 8) {
        if (levelRoll < 40) {
            pet->setLevel(3);
        } else {
            pet->setLevel(2);
        }
    } else if (turn == 9) {
        if (levelRoll < 60) {
            pet->setLevel(3);
        } else {
            pet->setLevel(2);
        }
    } else {
        if (levelRoll < 90) {
            pet->setLevel(3);
        } else {
            pet->setLevel(2);
        }
    }
}

// What it does: Raises Hard mode enemy stats using integer scaling.
// What the inputs are: The enemy pet pointer.
// What the outputs are: Updates attack and health with a stronger Hard mode value.
void Game::boostHardEnemyStats(Pet* pet) {
    int boostedAttack = 0;
    int boostedHp = 0;

    if (pet == nullptr) {
        return;
    }

    boostedAttack = pet->getAttack() + (pet->getAttack() / 5) + 1;
    boostedHp = pet->getHp() + (pet->getHp() / 5) + 1;
    pet->setAttack(boostedAttack);
    pet->setHp(boostedHp);
}

// What it does: Creates a scaled enemy team for the current turn.
// What the inputs are: The current turn number.
// What the outputs are: Deletes the old enemy team and fills new enemy slots.
void Game::generateEnemyTeam(int turn) {
    int enemyCount = 3;
    int maxTier = 1;

    clearEnemyTeam();

    if (turn <= 2) {
        enemyCount = 3;
        maxTier = 1;
    } else if (turn <= 4) {
        enemyCount = 4;
        maxTier = 2;
    } else {
        enemyCount = 5;
        maxTier = 3;
    }

    for (int i = 0; i < enemyCount; i++) {
        int targetIndex = 5 - enemyCount + i;
        enemyTeam[targetIndex] = createRandomPetByTier(maxTier);
        applyEnemyScaling(enemyTeam[targetIndex], turn);
    }
}

// What it does: Runs the real auto battle using cloned temporary teams.
// What the inputs are: None.
// What the outputs are: Returns 1 for win, 0 for draw, and -1 for loss.
int Game::battlePhase() {
    Pet* tempPlayerTeam[5];
    Pet* tempEnemyTeam[5];
    int battleResult = 0;

    for (int i = 0; i < 5; i++) {
        tempPlayerTeam[i] = nullptr;
        tempEnemyTeam[i] = nullptr;
    }

    clonePlayerTeam(tempPlayerTeam);
    cloneEnemyTeam(tempEnemyTeam);

    for (int i = 0; i < 5; i++) {
        if (tempPlayerTeam[i] != nullptr) {
            tempPlayerTeam[i]->resetBattleState();
        }

        if (tempEnemyTeam[i] != nullptr) {
            tempEnemyTeam[i]->resetBattleState();
        }
    }

    addLog("Battle started.", true, tempPlayerTeam, tempEnemyTeam);

    for (int i = 0; i < 5; i++) {
        if (tempPlayerTeam[i] != nullptr && tempPlayerTeam[i]->isAlive()) {
            tempPlayerTeam[i]->onBattleStart(tempEnemyTeam, 5);

            if (tempPlayerTeam[i]->getName() == "Mosquito" || tempPlayerTeam[i]->getName() == "Skunk") {
                addLog(tempPlayerTeam[i]->getName() + " start battle skill triggered.", true, tempPlayerTeam, tempEnemyTeam);
            }

            removeFaintedPets(tempEnemyTeam, tempPlayerTeam, tempEnemyTeam);
        }

        if (tempEnemyTeam[i] != nullptr && tempEnemyTeam[i]->isAlive()) {
            tempEnemyTeam[i]->onBattleStart(tempPlayerTeam, 5);

            if (tempEnemyTeam[i]->getName() == "Mosquito" || tempEnemyTeam[i]->getName() == "Skunk") {
                addLog(tempEnemyTeam[i]->getName() + " start battle skill triggered.", true, tempPlayerTeam, tempEnemyTeam);
            }

            removeFaintedPets(tempPlayerTeam, tempPlayerTeam, tempEnemyTeam);
        }
    }

    while (true) {
        int playerFrontIndex = findFrontIndex(tempPlayerTeam);
        int enemyFrontIndex = findFrontIndex(tempEnemyTeam);

        if (playerFrontIndex == -1 && enemyFrontIndex == -1) {
            battleResult = 0;
            break;
        }

        if (playerFrontIndex == -1) {
            battleResult = -1;
            break;
        }

        if (enemyFrontIndex == -1) {
            battleResult = 1;
            break;
        }

        Pet* playerFront = tempPlayerTeam[playerFrontIndex];
        Pet* enemyFront = tempEnemyTeam[enemyFrontIndex];
        int playerAttack = playerFront->getAttack();
        int enemyAttack = enemyFront->getAttack();
        int playerHpBeforeDamage = playerFront->getHp();
        int enemyHpBeforeDamage = enemyFront->getHp();

        playerFront->onAttack(tempPlayerTeam, playerFrontIndex, 5);
        if (playerFront->getName() == "Elephant") {
            addLog(playerFront->getName() + " attack skill triggered.", true, tempPlayerTeam, tempEnemyTeam);
        }
        removeFaintedPets(tempPlayerTeam, tempPlayerTeam, tempEnemyTeam);
        removeFaintedPets(tempEnemyTeam, tempPlayerTeam, tempEnemyTeam);

        enemyFront->onAttack(tempEnemyTeam, enemyFrontIndex, 5);
        if (enemyFront->getName() == "Elephant") {
            addLog(enemyFront->getName() + " attack skill triggered.", true, tempPlayerTeam, tempEnemyTeam);
        }
        removeFaintedPets(tempPlayerTeam, tempPlayerTeam, tempEnemyTeam);
        removeFaintedPets(tempEnemyTeam, tempPlayerTeam, tempEnemyTeam);

        playerFrontIndex = findFrontIndex(tempPlayerTeam);
        enemyFrontIndex = findFrontIndex(tempEnemyTeam);

        if (playerFrontIndex == -1 || enemyFrontIndex == -1) {
            continue;
        }

        playerFront = tempPlayerTeam[playerFrontIndex];
        enemyFront = tempEnemyTeam[enemyFrontIndex];
        playerAttack = playerFront->getAttack();
        enemyAttack = enemyFront->getAttack();
        playerHpBeforeDamage = playerFront->getHp();
        enemyHpBeforeDamage = enemyFront->getHp();

        addLog(playerFront->getName() + " attacks " + enemyFront->getName() + " for " + std::to_string(playerAttack) + " damage.", true, tempPlayerTeam, tempEnemyTeam);
        addLog(enemyFront->getName() + " attacks " + playerFront->getName() + " for " + std::to_string(enemyAttack) + " damage.", true, tempPlayerTeam, tempEnemyTeam);

        enemyFront->takeDamage(playerAttack);
        playerFront->takeDamage(enemyAttack);

        drawUI(tempPlayerTeam, tempEnemyTeam);
        std::this_thread::sleep_for(std::chrono::milliseconds(800));

        if (playerFront->getHp() > 0 && playerFront->getHp() < playerHpBeforeDamage) {
            playerFront->onHurt(tempPlayerTeam, playerFrontIndex, 5, tempEnemyTeam, 5);

            if (playerFront->getName() == "Camel" || playerFront->getName() == "Blowfish") {
                addLog(playerFront->getName() + " hurt skill triggered.", true, tempPlayerTeam, tempEnemyTeam);
            }
        }

        if (enemyFront->getHp() > 0 && enemyFront->getHp() < enemyHpBeforeDamage) {
            enemyFront->onHurt(tempEnemyTeam, enemyFrontIndex, 5, tempPlayerTeam, 5);

            if (enemyFront->getName() == "Camel" || enemyFront->getName() == "Blowfish") {
                addLog(enemyFront->getName() + " hurt skill triggered.", true, tempPlayerTeam, tempEnemyTeam);
            }
        }

        if (enemyFront->getHp() <= 0 && playerFront->getHp() > 0) {
            playerFront->onKnockOut(enemyFront);

            if (playerFront->getName() == "Hippo") {
                addLog(playerFront->getName() + " knock out skill triggered.", true, tempPlayerTeam, tempEnemyTeam);
            }
        }

        if (playerFront->getHp() <= 0 && enemyFront->getHp() > 0) {
            enemyFront->onKnockOut(playerFront);

            if (enemyFront->getName() == "Hippo") {
                addLog(enemyFront->getName() + " knock out skill triggered.", true, tempPlayerTeam, tempEnemyTeam);
            }
        }

        removeFaintedPets(tempPlayerTeam, tempPlayerTeam, tempEnemyTeam);
        removeFaintedPets(tempEnemyTeam, tempPlayerTeam, tempEnemyTeam);
    }

    clearTempTeam(tempPlayerTeam);
    clearTempTeam(tempEnemyTeam);

    return battleResult;
}

// What it does: Runs the shop phase command loop.
// What the inputs are: None.
// What the outputs are: Processes player commands until the player ends the phase or the game ends.
void Game::shopPhase() {
    char command;
    std::string commandLine;
    std::string cleanCommand;

    player->resetGold();

    for (int i = 0; i < 5; i++) {
        Pet* pet = player->getTeamPet(i);

        if (pet != nullptr) {
            pet->onShopStart(player);

            if (pet->getName() == "Swan") {
                addLog("Swan shop start skill triggered.", true);
            }
        }
    }

    while (true) {
        if (player->getHp() <= 0) {
            addLog("Defeat. The player has no HP left.", true);
            return;
        }

        if (wins >= 10) {
            addLog("Victory. The player reached 10 wins.", true);
            return;
        }

        drawUI();

        if (!std::getline(std::cin, commandLine)) {
            return;
        }

        cleanCommand = preprocessCommand(commandLine);

        if (cleanCommand.size() == 0) {
            addLog("Invalid command format!", true);
            continue;
        }

        command = cleanCommand[0];

        if (command == 18) {
            command = 'r';
        }

        if (command == 'r') {
            if (cleanCommand.size() != 1) {
                addLog("Invalid command format!", true);
                continue;
            }

            if (player->spendGold(1)) {
                rollShop();
                addLog("Rolled a completely new shop.", true);
            } else {
                addLog("Not enough gold to roll.", true);
            }
        } else if (command == 'b') {
            int shopIndex = -1;
            int targetSlot = -1;

            if (cleanCommand.size() != 3) {
                addLog("Invalid command format!", true);
                continue;
            }

            shopIndex = getShopIndex(cleanCommand[1]);

            if (cleanCommand[2] >= '1' && cleanCommand[2] <= '5') {
                targetSlot = cleanCommand[2] - '1';
            }

            if (shopIndex < 0 || targetSlot < 0 || targetSlot >= 5) {
                addLog("Invalid command format!", true);
            } else if (shopPets[shopIndex] == nullptr) {
                addLog("That shop slot is empty.", true);
            } else if (player->buyPet(shopPets[shopIndex], targetSlot)) {
                shopPets[shopIndex] = nullptr;
                addLog("Bought or merged the shop pet.", true);
            } else {
                addLog("Buy failed. Check gold or target slot.", true);
            }
        } else if (command == 's') {
            int targetSlot = -1;
            Pet* soldPet = nullptr;

            if (cleanCommand.size() != 2) {
                addLog("Invalid command format!", true);
                continue;
            }

            if (cleanCommand[1] >= '1' && cleanCommand[1] <= '5') {
                targetSlot = cleanCommand[1] - '1';
            }

            soldPet = player->getTeamPet(targetSlot);

            if (soldPet == nullptr) {
                addLog("There is no pet in that slot.", true);
            } else {
                std::string soldName = soldPet->getName();
                int soldLevel = soldPet->getLevel();
                player->sellPet(targetSlot);
                addLog("Sold " + soldName + " for " + std::to_string(soldLevel) + " gold.", true);
            }
        } else if (command == 'm') {
            int fromSlot = -1;
            int toSlot = -1;

            if (cleanCommand.size() != 3) {
                addLog("Invalid command format!", true);
                continue;
            }

            if (cleanCommand[1] >= '1' && cleanCommand[1] <= '5') {
                fromSlot = cleanCommand[1] - '1';
            }

            if (cleanCommand[2] >= '1' && cleanCommand[2] <= '5') {
                toSlot = cleanCommand[2] - '1';
            }

            if (fromSlot < 0 || fromSlot >= 5 || toSlot < 0 || toSlot >= 5) {
                addLog("Invalid command format!", true);
            } else if (player->movePet(fromSlot, toSlot)) {
                addLog("Moved team slot " + std::to_string(fromSlot + 1) + " and slot " + std::to_string(toSlot + 1) + ".", true);
            } else {
                addLog("Move failed. Source slot is empty.", true);
            }
        } else if (command == 'c') {
            int fromSlot = -1;
            int toSlot = -1;
            Pet* sourcePet = nullptr;
            Pet* targetPet = nullptr;

            if (cleanCommand.size() != 3) {
                addLog("Invalid command format!", true);
                continue;
            }

            if (cleanCommand[1] >= '1' && cleanCommand[1] <= '5') {
                fromSlot = cleanCommand[1] - '1';
            }

            if (cleanCommand[2] >= '1' && cleanCommand[2] <= '5') {
                toSlot = cleanCommand[2] - '1';
            }

            if (fromSlot < 0 || fromSlot >= 5 || toSlot < 0 || toSlot >= 5 || fromSlot == toSlot) {
                addLog("Invalid command format!", true);
                continue;
            }

            sourcePet = player->getTeamPet(fromSlot);
            targetPet = player->getTeamPet(toSlot);

            if (sourcePet == nullptr || targetPet == nullptr) {
                addLog("Combine failed. Both slots need pets.", true);
            } else if (sourcePet->getName() != targetPet->getName()) {
                addLog("Combine failed. Pets must be the same type.", true);
            } else if (sourcePet->getLevel() >= 3 || targetPet->getLevel() >= 3) {
                addLog("Combine failed. Lv3 pets cannot combine.", true);
            } else if (player->combinePets(fromSlot, toSlot)) {
                addLog("Combined slot " + std::to_string(fromSlot + 1) + " into slot " + std::to_string(toSlot + 1) + ".", true);
            } else {
                addLog("Combine failed.", true);
            }
        } else if (command == 'e') {
            if (cleanCommand.size() != 1) {
                addLog("Invalid command format!", true);
                continue;
            }

            for (int i = 0; i < 5; i++) {
                Pet* pet = player->getTeamPet(i);

                if (pet != nullptr) {
                    pet->onShopEnd(player->getTeamArray(), 5);

                    if (pet->getName() == "Monkey") {
                        addLog("Monkey shop end skill triggered.", true);
                    }
                }
            }

            addLog("Ending shop phase.", true);
            return;
        } else {
            addLog("Invalid command format!", true);
        }
    }
}

// What it does: Runs the main game loop until victory or defeat.
// What the inputs are: None.
// What the outputs are: Runs shop, enemy generation, battle result, and final game over output.
void Game::start() {
    if (!selectDifficulty()) {
        return;
    }

    while (player->getHp() > 0 && wins < 10) {
        shopPhase();

        if (!std::cin) {
            break;
        }

        if (player->getHp() <= 0 || wins >= 10) {
            break;
        }

        generateEnemyTeam(currentTurn);
        int battleResult = battlePhase();

        std::cout << "\033[2J\033[1;1H";
        std::cout << "Turn " << currentTurn << " Result" << std::endl;
        std::cout << std::endl;

        if (battleResult == 1) {
            std::cout << "You won this round! o(>▽<)o" << std::endl;
            wins++;
        } else if (battleResult == -1) {
            std::cout << "You lost! (╥﹏╥)" << std::endl;
            player->loseHp(1);
        } else {
            std::cout << "It's a draw ╮(─▽─)╭" << std::endl;
            std::cout << "No HP lost." << std::endl;
        }

        std::cout << std::endl;
        std::cout << "Press Enter to continue.";
        std::cin.get();

        currentTurn++;
        rollShop();
    }

    std::cout << "\033[2J\033[1;1H";
    std::cout << "Game Over" << std::endl;
    std::cout << std::endl;

    if (wins >= 10) {
        std::cout << "Victory. You reached 10 wins." << std::endl;
    } else {
        std::cout << "Defeat. The player has no HP left." << std::endl;
    }
}
