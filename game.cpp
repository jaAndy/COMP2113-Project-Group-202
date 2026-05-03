#include "game.h"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

// What it does: Creates the game state and the first shop.
// What the inputs are: None.
// What the outputs are: Initializes player, shop pets, turn, wins, and message log.
Game::Game()
{
    player = new Player();
    currentTurn = 1;
    wins = 0;
    difficultyMode = NORMAL_MODE;
    saveAndQuitRequested = false;
    resumeShopWithoutSetup = false;
    goldFlash = false;
    clearStatFlash();

    for (int i = 0; i < 3; i++)
    {
        shopPets[i] = nullptr;
    }

    for (int i = 0; i < 5; i++)
    {
        enemyTeam[i] = nullptr;
    }

    rollShop();
    addLog("Welcome to the pet shop.");
}

// What it does: Destroys the game and frees owned dynamic memory.
// What the inputs are: None.
// What the outputs are: Deletes the player and all remaining shop pet pointers.
Game::~Game()
{
    delete player;
    player = nullptr;

    clearShop();
    clearEnemyTeam();
}

// What it does: Shows the title screen and lets the player choose the start option.
// What the inputs are: None.
// What the outputs are: Returns true to enter the game loop, or false to exit.
bool Game::showTitleMenu() {
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
                    NEW GAME
                    [1] Normal Mode
                    [2] Hard Mode

                    LOAD GAME
                    load : Load Game

                    QUIT
                    [3] Quit Game
)";
        std::cout << std::endl;
        std::cout << "Choice: ";

        if (!std::getline(std::cin, choice)) {
            difficultyMode = NORMAL_MODE;
            resumeShopWithoutSetup = false;
            addLog("Normal mode selected.");
            return true;
        }

        choice = preprocessCommand(choice);

        if (choice == "1" || choice == "normal" || choice == "n") {
            difficultyMode = NORMAL_MODE;
            resumeShopWithoutSetup = false;
            std::cout << "Starting CLI Auto Pets in Normal Mode." << std::endl;
            addLog("Normal mode selected.");
            return true;
        }

        if (choice == "2" || choice == "hard" || choice == "h") {
            difficultyMode = HARD_MODE;
            resumeShopWithoutSetup = false;
            std::cout << "Hard Mode enabled. Good luck." << std::endl;
            addLog("Hard mode selected.");
            return true;
        }

        if (choice == "load") {
            if (loadGameFromFile("cli_auto_pets_save.txt")) {
                return true;
            }

            std::cout << "No valid save file found. Press Enter to return to menu.";
            std::getline(std::cin, choice);
            continue;
        }

        if (choice == "3" || choice == "quit" || choice == "exit" || choice == "q" || choice == "x") {
            std::cout << "Goodbye!" << std::endl;
            return false;
        }

        std::cout << "Invalid menu choice. Press Enter to try again.";
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

// What it does: Writes one pet slot to the save file.
// What the inputs are: The output file, slot label, and pet pointer.
// What the outputs are: Writes one EMPTY or pet data line to the file.
void Game::writePetSaveLine(std::ofstream &outputFile, const std::string &slotLabel, Pet *pet)
{
    outputFile << slotLabel;

    if (pet == nullptr)
    {
        outputFile << " EMPTY" << std::endl;
        return;
    }

    outputFile << " " << pet->getName();
    outputFile << " " << pet->getAttack();
    outputFile << " " << pet->getHp();
    outputFile << " " << pet->getLevel();
    outputFile << " " << pet->getExperience();
    outputFile << std::endl;
}

// What it does: Saves the current shop-phase game state to a text file.
// What the inputs are: The save file name.
// What the outputs are: Returns true if the save file is written successfully, otherwise false.
bool Game::saveGameToFile(const std::string &fileName)
{
    std::ofstream outputFile(fileName.c_str());

    if (!outputFile.good())
    {
        return false;
    }

    outputFile << "CLI_AUTO_PETS_SAVE_V1" << std::endl;
    outputFile << "PHASE SHOP" << std::endl;
    outputFile << "DIFFICULTY " << getDifficultyName() << std::endl;
    outputFile << "TURN " << currentTurn << std::endl;
    outputFile << "WINS " << wins << std::endl;
    outputFile << "PLAYER_HP " << player->getHp() << std::endl;
    outputFile << "PLAYER_GOLD " << player->getGold() << std::endl;

    outputFile << "PLAYER_TEAM_BEGIN" << std::endl;
    for (int i = 0; i < 5; i++)
    {
        writePetSaveLine(outputFile, "TEAM_SLOT " + std::to_string(i + 1), player->getTeamPet(i));
    }
    outputFile << "PLAYER_TEAM_END" << std::endl;

    outputFile << "SHOP_BEGIN" << std::endl;
    for (int i = 0; i < 3; i++)
    {
        std::string slotLabel = "SHOP_SLOT ";
        slotLabel = slotLabel + static_cast<char>('A' + i);
        writePetSaveLine(outputFile, slotLabel, shopPets[i]);
    }
    outputFile << "SHOP_END" << std::endl;

    outputFile << "MESSAGE_LOG_BEGIN" << std::endl;
    outputFile << "MESSAGE_COUNT " << messageLog.size() << std::endl;
    for (int i = 0; i < static_cast<int>(messageLog.size()); i++)
    {
        outputFile << "LOG " << messageLog[i] << std::endl;
    }
    outputFile << "MESSAGE_LOG_END" << std::endl;
    outputFile << "END_SAVE" << std::endl;

    outputFile.close();
    return !outputFile.fail();
}

// What it does: Reads one exact line from a save file.
// What the inputs are: The input file and the expected line text.
// What the outputs are: Returns true if the next line matches exactly, otherwise false.
bool Game::readExpectedLine(std::ifstream &inputFile, const std::string &expectedLine)
{
    std::string line;

    if (!std::getline(inputFile, line))
    {
        return false;
    }

    return line == expectedLine;
}

// What it does: Reads one integer value line from a save file.
// What the inputs are: The input file, expected key, and integer output variable.
// What the outputs are: Returns true if the key and value are read successfully.
bool Game::readSaveIntLine(std::ifstream &inputFile, const std::string &expectedKey, int &value)
{
    std::string line;
    std::string key;
    std::istringstream lineStream;

    if (!std::getline(inputFile, line))
    {
        return false;
    }

    lineStream.str(line);

    if (!(lineStream >> key >> value))
    {
        return false;
    }

    return key == expectedKey;
}

// What it does: Creates a pet object from a saved pet name.
// What the inputs are: The saved pet name.
// What the outputs are: Returns a new pet pointer, or nullptr if the name is unknown.
Pet *Game::createPetByName(const std::string &petName)
{
    if (petName == "Swan")
    {
        return new Swan();
    }
    else if (petName == "Ant")
    {
        return new Ant();
    }
    else if (petName == "Mosquito")
    {
        return new Mosquito();
    }
    else if (petName == "Camel")
    {
        return new Camel();
    }
    else if (petName == "Skunk")
    {
        return new Skunk();
    }
    else if (petName == "Elephant")
    {
        return new Elephant();
    }
    else if (petName == "Hippo")
    {
        return new Hippo();
    }
    else if (petName == "Blowfish")
    {
        return new Blowfish();
    }
    else if (petName == "Monkey")
    {
        return new Monkey();
    }

    return nullptr;
}

// What it does: Reads one saved pet entry from a line stream.
// What the inputs are: The line stream after the slot label and the pet output pointer.
// What the outputs are: Returns true if an empty slot or valid pet is read.
bool Game::readPetSaveLine(std::istringstream &lineStream, Pet *&pet)
{
    std::string petName;
    int attack = 0;
    int hp = 0;
    int level = 0;
    int experience = 0;

    pet = nullptr;

    if (!(lineStream >> petName))
    {
        return false;
    }

    if (petName == "EMPTY")
    {
        return true;
    }

    if (!(lineStream >> attack >> hp >> level >> experience))
    {
        return false;
    }

    if (attack < 0 || hp < 0 || level < 1 || level > 3 || experience < 0)
    {
        return false;
    }

    if ((level == 1 && experience > 1) || (level == 2 && experience > 2) || (level == 3 && experience > 0))
    {
        return false;
    }

    pet = createPetByName(petName);

    if (pet == nullptr)
    {
        return false;
    }

    pet->setAttack(attack);
    pet->setHp(hp);
    pet->setLevel(level);
    pet->setExperience(experience);
    return true;
}

// What it does: Loads a saved shop-phase game state from a text file.
// What the inputs are: The save file name.
// What the outputs are: Returns true if the save is loaded and applied successfully.
bool Game::loadGameFromFile(const std::string &fileName)
{
    std::ifstream inputFile(fileName.c_str());
    Pet *loadedTeam[5];
    Pet *loadedShop[3];
    std::vector<std::string> loadedMessages;
    DifficultyMode loadedDifficulty = NORMAL_MODE;
    int loadedTurn = 0;
    int loadedWins = 0;
    int loadedHp = 0;
    int loadedGold = 0;
    int messageCount = 0;
    bool success = true;
    std::string line;
    std::string key;
    std::string difficultyName;

    for (int i = 0; i < 5; i++)
    {
        loadedTeam[i] = nullptr;
    }

    for (int i = 0; i < 3; i++)
    {
        loadedShop[i] = nullptr;
    }

    if (!inputFile.good())
    {
        return false;
    }

    if (!readExpectedLine(inputFile, "CLI_AUTO_PETS_SAVE_V1"))
    {
        success = false;
    }

    if (success && !readExpectedLine(inputFile, "PHASE SHOP"))
    {
        success = false;
    }

    if (success)
    {
        std::istringstream lineStream;

        if (!std::getline(inputFile, line))
        {
            success = false;
        }
        else
        {
            lineStream.str(line);

            if (!(lineStream >> key >> difficultyName) || key != "DIFFICULTY")
            {
                success = false;
            }
            else if (difficultyName == "Normal")
            {
                loadedDifficulty = NORMAL_MODE;
            }
            else if (difficultyName == "Hard")
            {
                loadedDifficulty = HARD_MODE;
            }
            else
            {
                success = false;
            }
        }
    }

    if (success && !readSaveIntLine(inputFile, "TURN", loadedTurn))
    {
        success = false;
    }

    if (success && !readSaveIntLine(inputFile, "WINS", loadedWins))
    {
        success = false;
    }

    if (success && !readSaveIntLine(inputFile, "PLAYER_HP", loadedHp))
    {
        success = false;
    }

    if (success && !readSaveIntLine(inputFile, "PLAYER_GOLD", loadedGold))
    {
        success = false;
    }

    if (success && (loadedTurn < 1 || loadedWins < 0 || loadedHp < 0 || loadedGold < 0))
    {
        success = false;
    }

    if (success && !readExpectedLine(inputFile, "PLAYER_TEAM_BEGIN"))
    {
        success = false;
    }

    for (int i = 0; success && i < 5; i++)
    {
        std::istringstream lineStream;
        std::string slotKey;
        int slotIndex = 0;

        if (!std::getline(inputFile, line))
        {
            success = false;
        }
        else
        {
            lineStream.str(line);

            if (!(lineStream >> slotKey >> slotIndex) || slotKey != "TEAM_SLOT" || slotIndex != i + 1)
            {
                success = false;
            }
            else if (!readPetSaveLine(lineStream, loadedTeam[i]))
            {
                success = false;
            }
        }
    }

    if (success && !readExpectedLine(inputFile, "PLAYER_TEAM_END"))
    {
        success = false;
    }

    if (success && !readExpectedLine(inputFile, "SHOP_BEGIN"))
    {
        success = false;
    }

    for (int i = 0; success && i < 3; i++)
    {
        std::istringstream lineStream;
        std::string slotKey;
        char slotLetter = 'A';

        if (!std::getline(inputFile, line))
        {
            success = false;
        }
        else
        {
            lineStream.str(line);

            if (!(lineStream >> slotKey >> slotLetter) || slotKey != "SHOP_SLOT" || slotLetter != static_cast<char>('A' + i))
            {
                success = false;
            }
            else if (!readPetSaveLine(lineStream, loadedShop[i]))
            {
                success = false;
            }
        }
    }

    if (success && !readExpectedLine(inputFile, "SHOP_END"))
    {
        success = false;
    }

    if (success && !readExpectedLine(inputFile, "MESSAGE_LOG_BEGIN"))
    {
        success = false;
    }

    if (success && !readSaveIntLine(inputFile, "MESSAGE_COUNT", messageCount))
    {
        success = false;
    }

    if (success && (messageCount < 0 || messageCount > 5))
    {
        success = false;
    }

    for (int i = 0; success && i < messageCount; i++)
    {
        if (!std::getline(inputFile, line))
        {
            success = false;
        }
        else if (line == "LOG")
        {
            loadedMessages.push_back("");
        }
        else if (line.size() >= 4 && line.substr(0, 4) == "LOG ")
        {
            loadedMessages.push_back(line.substr(4));
        }
        else
        {
            success = false;
        }
    }

    if (success && !readExpectedLine(inputFile, "MESSAGE_LOG_END"))
    {
        success = false;
    }

    if (success && !readExpectedLine(inputFile, "END_SAVE"))
    {
        success = false;
    }

    if (success)
    {
        clearShop();
        clearEnemyTeam();
        player->clearTeam();

        difficultyMode = loadedDifficulty;
        currentTurn = loadedTurn;
        wins = loadedWins;
        player->setHp(loadedHp);
        player->setGold(loadedGold);
        messageLog = loadedMessages;
        saveAndQuitRequested = false;
        resumeShopWithoutSetup = true;

        for (int i = 0; i < 5; i++)
        {
            player->setTeamPet(i, loadedTeam[i]);
            loadedTeam[i] = nullptr;
        }

        for (int i = 0; i < 3; i++)
        {
            shopPets[i] = loadedShop[i];
            loadedShop[i] = nullptr;
        }
    }

    for (int i = 0; i < 5; i++)
    {
        if (loadedTeam[i] != nullptr)
        {
            delete loadedTeam[i];
            loadedTeam[i] = nullptr;
        }
    }

    for (int i = 0; i < 3; i++)
    {
        if (loadedShop[i] != nullptr)
        {
            delete loadedShop[i];
            loadedShop[i] = nullptr;
        }
    }

    return success;
}

// What it does: Adds a message to the game message log.
// What the inputs are: The message text, delay flag, and optional active battle teams.
// What the outputs are: Updates the message log and may redraw the UI with a short delay.
void Game::addLog(std::string msg, bool autoDelay, Pet **activePlayerTeam, Pet **activeEnemyTeam)
{
    messageLog.push_back(msg);

    if (messageLog.size() > 5)
    {
        messageLog.erase(messageLog.begin());
    }

    if (autoDelay)
    {
        drawUI(activePlayerTeam, activeEnemyTeam);
        std::this_thread::sleep_for(std::chrono::milliseconds(900));
    }
}

// What it does: Prints one fixed-width box cell with manual spacing.
// What the inputs are: The cell text and the inside cell width.
// What the outputs are: Prints one aligned cell content line.
void Game::printBoxCell(std::string text, int width)
{
    std::string visibleText;
    int visualLength = 0;
    std::string::size_type dotPosition;

    for (int i = 0; i < static_cast<int>(text.size()); i++)
    {
        if (text[i] == '\033')
        {
            while (i < static_cast<int>(text.size()) && text[i] != 'm')
            {
                i++;
            }
        }
        else
        {
            visibleText = visibleText + text[i];
        }
    }

    visualLength = static_cast<int>(visibleText.size());

    if (visibleText.find("♥") != std::string::npos)
    {
        visualLength = visualLength - 2;
    }

    dotPosition = visibleText.find("●");
    while (dotPosition != std::string::npos)
    {
        visualLength = visualLength - 2;
        dotPosition = visibleText.find("●", dotPosition + 1);
    }

    dotPosition = visibleText.find("○");
    while (dotPosition != std::string::npos)
    {
        visualLength = visualLength - 2;
        dotPosition = visibleText.find("○", dotPosition + 1);
    }

    std::cout << "|" << text;

    for (int i = visualLength; i < width; i++)
    {
        std::cout << " ";
    }

    std::cout << "|";
}

// What it does: Applies a red or green flash color to stat text.
// What the inputs are: The value text and flash state.
// What the outputs are: Returns colored text when the flash state is active.
std::string Game::applyStatColor(std::string valueText, int flashState)
{
    if (flashState == 1)
    {
        return std::string(RED) + valueText + RESET;
    }

    if (flashState == 2)
    {
        return std::string(GREEN) + valueText + RESET;
    }

    return valueText;
}

// What it does: Builds the pet attack and health display line.
// What the inputs are: The pet pointer and flash states for attack and health.
// What the outputs are: Returns a stats line with colored changed values.
std::string Game::getPetStatsLine(Pet *pet, int attackFlash, int hpFlash)
{
    std::string statsLine = "";

    if (pet == nullptr)
    {
        return statsLine;
    }

    statsLine = "ATK:" + applyStatColor(std::to_string(pet->getAttack()), attackFlash);
    statsLine = statsLine + " HP:♥" + applyStatColor(std::to_string(pet->getHp()), hpFlash);
    return statsLine;
}

// What it does: Builds the pet name and experience display line.
// What the inputs are: The pet pointer to display.
// What the outputs are: Returns a short name line with level and experience bar.
std::string Game::getPetNameLine(Pet *pet)
{
    std::string petName;
    std::string expBar;
    std::string nameLine;

    if (pet == nullptr)
    {
        return "[   Empty   ]";
    }

    petName = pet->getName();

    if (pet->getLevel() == 1)
    {
        if (pet->getExperience() == 0)
        {
            expBar = "[○○]";
        }
        else
        {
            expBar = "[●○]";
        }
    }
    else if (pet->getLevel() == 2)
    {
        if (pet->getExperience() == 0)
        {
            expBar = "[○○○]";
        }
        else if (pet->getExperience() == 1)
        {
            expBar = "[●○○]";
        }
        else
        {
            expBar = "[●●○]";
        }
    }
    else
    {
        expBar = "[MAX]";
    }

    while (petName.size() > 3 && ("Lv" + std::to_string(pet->getLevel()) + " " + petName + " " + expBar).size() > 23)
    {
        petName.erase(petName.size() - 1);
    }

    nameLine = "Lv" + std::to_string(pet->getLevel()) + " " + petName + " " + expBar;
    return nameLine;
}

// What it does: Wraps a pet name with player or enemy color.
// What the inputs are: The pet name text and whether the pet belongs to the player.
// What the outputs are: Returns a colored pet name string.
std::string Game::colorPetName(const std::string &petName, bool isPlayerPet)
{
    if (isPlayerPet)
    {
        return std::string(BLUE) + petName + RESET;
    }

    return std::string(RED) + petName + RESET;
}

// What it does: Wraps a faction label with its team color.
// What the inputs are: Whether the faction is the player side.
// What the outputs are: Returns colored text "Player" or "Enemy".
std::string Game::colorFactionLabel(bool isPlayerTeam)
{
    if (isPlayerTeam)
    {
        return std::string(BLUE) + "Player" + RESET;
    }

    return std::string(RED) + "Enemy" + RESET;
}

// What it does: Formats one battle log line with a unified tag template.
// What the inputs are: The round number, event tag, and detail text.
// What the outputs are: Returns a formatted battle log string.
std::string Game::formatBattleLog(int roundNumber, const std::string &tag, const std::string &detail)
{
    if (roundNumber > 0)
    {
        return "[R" + std::to_string(roundNumber) + "][" + tag + "] " + detail;
    }

    return "[START][" + tag + "] " + detail;
}

// What it does: Gets the base stat description for one pet.
// What the inputs are: The lowercase pet name.
// What the outputs are: Returns the base stat line, or an empty string for an unknown pet.
std::string Game::getPetBaseStatsLine(std::string petName)
{
    if (petName == "swan")
    {
        return "Swan base stats: ATK 1, HP 2.";
    }
    else if (petName == "ant")
    {
        return "Ant base stats: ATK 2, HP 2.";
    }
    else if (petName == "mosquito")
    {
        return "Mosquito base stats: ATK 2, HP 2.";
    }
    else if (petName == "camel")
    {
        return "Camel base stats: ATK 3, HP 3.";
    }
    else if (petName == "skunk")
    {
        return "Skunk base stats: ATK 3, HP 5.";
    }
    else if (petName == "elephant")
    {
        return "Elephant base stats: ATK 3, HP 7.";
    }
    else if (petName == "hippo")
    {
        return "Hippo base stats: ATK 3, HP 6.";
    }
    else if (petName == "blowfish")
    {
        return "Blowfish base stats: ATK 3, HP 6.";
    }
    else if (petName == "monkey")
    {
        return "Monkey base stats: ATK 1, HP 2.";
    }

    return "";
}

// What it does: Gets one level skill description for one pet.
// What the inputs are: The lowercase pet name and skill level.
// What the outputs are: Returns the level skill line, or an empty string for an unknown pet.
std::string Game::getPetSkillLine(std::string petName, int level)
{
    std::string levelText = "Lv" + std::to_string(level) + ": ";

    if (petName == "swan")
    {
        return levelText + "At shop start, gain " + std::to_string(level) + " gold.";
    }
    else if (petName == "ant")
    {
        return levelText + "On faint, give one random living ally +" + std::to_string(level) + " ATK and +" + std::to_string(level) + " HP.";
    }
    else if (petName == "mosquito")
    {
        return levelText + "At battle start, deal 1 damage to " + std::to_string(level) + " random living enemy pet(s).";
    }
    else if (petName == "camel")
    {
        return levelText + "When hurt, give the nearest living ally behind +" + std::to_string(level) + " ATK and +" + std::to_string(2 * level) + " HP.";
    }
    else if (petName == "skunk")
    {
        if (level == 1)
        {
            return levelText + "At battle start, reduce the highest-health enemy by 1/3 of its current HP.";
        }
        else if (level == 2)
        {
            return levelText + "At battle start, reduce the highest-health enemy by 2/3 of its current HP.";
        }

        return levelText + "At battle start, reduce the highest-health enemy by 99% of its current HP.";
    }
    else if (petName == "elephant")
    {
        return levelText + "On attack, deal 1 damage to the nearest living ally behind " + std::to_string(level) + " time(s).";
    }
    else if (petName == "hippo")
    {
        return levelText + "On knockout, gain +" + std::to_string(3 * level) + " ATK and +" + std::to_string(3 * level) + " HP, up to 3 times per battle.";
    }
    else if (petName == "blowfish")
    {
        return levelText + "When hurt, deal " + std::to_string(3 * level) + " damage to one random living enemy.";
    }
    else if (petName == "monkey")
    {
        return levelText + "At shop end, give the front-most other living ally +" + std::to_string(2 * level) + " ATK and +" + std::to_string(2 * level) + " HP.";
    }

    return "";
}

// What it does: Shows one pet's base stats and level skills in the message log.
// What the inputs are: The lowercase pet name to view.
// What the outputs are: Adds pet info messages and returns true if the pet is known.
bool Game::showPetInfo(std::string petName)
{
    std::string baseStatsLine = getPetBaseStatsLine(petName);

    if (baseStatsLine == "")
    {
        addLog("Unknown pet. Try swan, ant, mosquito, camel, skunk, elephant, hippo, blowfish, or monkey.");
        return false;
    }

    addLog(baseStatsLine);
    addLog(getPetSkillLine(petName, 1));
    addLog(getPetSkillLine(petName, 2));
    addLog(getPetSkillLine(petName, 3));
    return true;
}

// What it does: Draws the full shop phase user interface.
// What the inputs are: Optional active player and enemy teams to display.
// What the outputs are: Prints the game stats, team, shop, menu, log, and input prompt.
void Game::drawUI(Pet **activePlayerTeam, Pet **activeEnemyTeam)
{
    int playerFrontIndex = -1;
    int enemyFrontIndex = -1;

    if (activePlayerTeam != nullptr)
    {
        playerFrontIndex = findFrontIndex(activePlayerTeam);
    }

    if (activeEnemyTeam != nullptr)
    {
        enemyFrontIndex = findFrontIndex(activeEnemyTeam);
    }

    std::cout << "\033[2J\033[1;1H";

    std::cout << CYAN << "================================================================================" << RESET << std::endl;
    if (goldFlash)
    {
        std::cout << WHITE << "[ Gold: " << player->getGold() << " ]" << RESET << "  ";
    }
    else
    {
        std::cout << YELLOW << "[ Gold: " << player->getGold() << " ]" << RESET << "  ";
    }
    std::cout << RED << "[ HP: ♥x" << player->getHp() << " ]" << RESET << "  ";
    std::cout << "[ Turn: " << currentTurn << " ]  ";
    std::cout << "[ Wins: " << wins << "/10 ]  ";
    std::cout << "[ Mode: " << getDifficultyName() << " ]" << std::endl;
    std::cout << CYAN << "================================================================================" << RESET << std::endl;
    std::cout << std::endl;

    std::cout << GREEN << "TEAM SLOTS" << RESET << std::endl;
    for (int i = 0; i < 5; i++)
    {
        std::cout << "+-----------------+";
    }
    std::cout << std::endl;
    for (int i = 0; i < 5; i++)
    {
        std::string slotLine = "Slot " + std::to_string(i + 1);

        if (activePlayerTeam != nullptr && i == playerFrontIndex)
        {
            slotLine = "Slot " + std::to_string(i + 1) + " FRONT";
        }
        printBoxCell(slotLine, 17);
    }
    std::cout << std::endl;
    for (int i = 0; i < 5; i++)
    {
        Pet *pet = player->getTeamPet(i);
        std::string nameLine;
        int nameFlash = 0;

        if (activePlayerTeam != nullptr)
        {
            pet = activePlayerTeam[i];
            nameFlash = playerNameFlash[i];
        }

        nameLine = getPetNameLine(pet);

        if (pet != nullptr)
        {
            nameLine = applyStatColor(nameLine, nameFlash);
        }

        printBoxCell(nameLine, 17);
    }
    std::cout << std::endl;
    for (int i = 0; i < 5; i++)
    {
        Pet *pet = player->getTeamPet(i);
        std::string statsLine = "";
        int attackFlash = 0;
        int hpFlash = 0;

        if (activePlayerTeam != nullptr)
        {
            pet = activePlayerTeam[i];
            attackFlash = playerAttackFlash[i];
            hpFlash = playerHpFlash[i];
        }

        statsLine = getPetStatsLine(pet, attackFlash, hpFlash);

        printBoxCell(statsLine, 17);
    }
    std::cout << std::endl;
    for (int i = 0; i < 5; i++)
    {
        std::cout << "+-----------------+";
    }
    std::cout << std::endl;
    std::cout << CYAN << "--------------------------------------------------------------------------------" << RESET << std::endl;
    std::cout << std::endl;

    if (activeEnemyTeam == nullptr)
    {
        std::cout << GREEN << "SHOP SLOTS" << RESET << std::endl;
    }
    else
    {
        std::cout << GREEN << "ENEMY TEAM" << RESET << std::endl;
    }
    for (int i = 0; i < 5; i++)
    {
        if (activeEnemyTeam == nullptr && i >= 3)
        {
            break;
        }

        std::cout << "+-----------------+";
    }
    std::cout << std::endl;
    for (int i = 0; i < 5; i++)
    {
        std::string shopLine = "Enemy " + std::to_string(i + 1);

        if (activeEnemyTeam == nullptr && i >= 3)
        {
            break;
        }

        if (activeEnemyTeam == nullptr)
        {
            shopLine = "Shop ";
            shopLine = shopLine + static_cast<char>('A' + i);
        }
        else if (i == enemyFrontIndex)
        {
            shopLine = "Enemy " + std::to_string(i + 1) + " FRONT";
        }

        printBoxCell(shopLine, 17);
    }
    std::cout << std::endl;
    for (int i = 0; i < 5; i++)
    {
        Pet *pet = nullptr;
        std::string nameLine;
        int nameFlash = 0;

        if (activeEnemyTeam == nullptr && i >= 3)
        {
            break;
        }

        if (activeEnemyTeam == nullptr)
        {
            pet = shopPets[i];
        }
        else
        {
            pet = activeEnemyTeam[i];
            nameFlash = enemyNameFlash[i];
        }

        nameLine = getPetNameLine(pet);

        if (pet != nullptr)
        {
            nameLine = applyStatColor(nameLine, nameFlash);
        }

        printBoxCell(nameLine, 17);
    }
    std::cout << std::endl;
    for (int i = 0; i < 5; i++)
    {
        Pet *pet = nullptr;
        std::string statsLine = "";
        int attackFlash = 0;
        int hpFlash = 0;

        if (activeEnemyTeam == nullptr && i >= 3)
        {
            break;
        }

        if (activeEnemyTeam == nullptr)
        {
            pet = shopPets[i];
        }
        else
        {
            pet = activeEnemyTeam[i];
            attackFlash = enemyAttackFlash[i];
            hpFlash = enemyHpFlash[i];
        }

        statsLine = getPetStatsLine(pet, attackFlash, hpFlash);

        printBoxCell(statsLine, 17);
    }
    std::cout << std::endl;
    for (int i = 0; i < 5; i++)
    {
        if (activeEnemyTeam == nullptr && i >= 3)
        {
            break;
        }

        std::cout << "+-----------------+";
    }
    std::cout << std::endl;
    std::cout << CYAN << "--------------------------------------------------------------------------------" << RESET << std::endl;
    std::cout << std::endl;

    std::cout << "--- MESSAGE LOG ---" << std::endl;
    for (int i = 0; i < static_cast<int>(messageLog.size()); i++)
    {
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
    std::cout << "  " << GREEN << "[ view swan ]" << RESET << " : View Pet Info" << std::endl;
    std::cout << "  " << GREEN << "[ saveq ]" << RESET << " : Save and Quit" << std::endl;
    std::cout << CYAN << "=================================================" << RESET << std::endl;
    std::cout << "Action Input: ";
}

// What it does: Creates a random new pet up to a chosen tier.
// What the inputs are: The highest allowed pet tier.
// What the outputs are: Returns a newly allocated pet pointer.
Pet *Game::createRandomPetByTier(int maxTier)
{
    int choiceCount = 3;

    if (maxTier == 2)
    {
        choiceCount = 6;
    }
    else if (maxTier >= 3)
    {
        choiceCount = 9;
    }

    int choice = std::rand() % choiceCount;

    if (choice == 0)
    {
        return new Swan();
    }
    else if (choice == 1)
    {
        return new Ant();
    }
    else if (choice == 2)
    {
        return new Mosquito();
    }
    else if (choice == 3)
    {
        return new Camel();
    }
    else if (choice == 4)
    {
        return new Skunk();
    }
    else if (choice == 5)
    {
        return new Elephant();
    }
    else if (choice == 6)
    {
        return new Hippo();
    }
    else if (choice == 7)
    {
        return new Blowfish();
    }

    return new Monkey();
}

// What it does: Deletes all shop pets and clears the shop slots.
// What the inputs are: None.
// What the outputs are: Frees shop pet pointers and sets shop slots to nullptr.
void Game::clearShop()
{
    for (int i = 0; i < 3; i++)
    {
        if (shopPets[i] != nullptr)
        {
            delete shopPets[i];
            shopPets[i] = nullptr;
        }
    }
}

// What it does: Deletes all enemy pets and clears the enemy team slots.
// What the inputs are: None.
// What the outputs are: Frees enemy pet pointers and sets enemy slots to nullptr.
void Game::clearEnemyTeam()
{
    for (int i = 0; i < 5; i++)
    {
        if (enemyTeam[i] != nullptr)
        {
            delete enemyTeam[i];
            enemyTeam[i] = nullptr;
        }
    }
}

// What it does: Briefly flashes the gold display in white.
// What the inputs are: None.
// What the outputs are: Draws a short white gold pulse in the shop interface.
void Game::flashGoldChange()
{
    const int flashMilliseconds = 300;

    goldFlash = true;
    drawUI();
    std::cout.flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(flashMilliseconds));
    goldFlash = false;
    drawUI();
    std::cout.flush();
}

// What it does: Clears all temporary battle stat flash markers.
// What the inputs are: None.
// What the outputs are: Resets player and enemy name, attack, and health flash states.
void Game::clearStatFlash()
{
    for (int i = 0; i < 5; i++)
    {
        playerNameFlash[i] = 0;
        playerAttackFlash[i] = 0;
        playerHpFlash[i] = 0;
        enemyNameFlash[i] = 0;
        enemyAttackFlash[i] = 0;
        enemyHpFlash[i] = 0;
    }
}

// What it does: Captures attack and health values from one team.
// What the inputs are: The team pointer array and output arrays for attack and health.
// What the outputs are: Stores current stats, or negative values for empty slots.
void Game::captureTeamStats(Pet *team[5], int attackValues[5], int hpValues[5])
{
    for (int i = 0; i < 5; i++)
    {
        attackValues[i] = -1;
        hpValues[i] = -1;

        if (team != nullptr && team[i] != nullptr)
        {
            attackValues[i] = team[i]->getAttack();
            hpValues[i] = team[i]->getHp();
        }
    }
}

// What it does: Marks flash states for stat changes after an event.
// What the inputs are: The team, before stats, and whether it is the player team.
// What the outputs are: Returns true if any attack or health value changed.
bool Game::markStatFlash(Pet *team[5], int beforeAttack[5], int beforeHp[5], bool isPlayerTeam)
{
    bool hasChange = false;

    if (team == nullptr)
    {
        return false;
    }

    for (int i = 0; i < 5; i++)
    {
        int attackFlash = 0;
        int hpFlash = 0;
        int nameFlash = 0;

        if (team[i] != nullptr && beforeAttack[i] >= 0 && beforeHp[i] >= 0)
        {
            if (team[i]->getAttack() < beforeAttack[i])
            {
                attackFlash = 1;
                nameFlash = 1;
                hasChange = true;
            }
            else if (team[i]->getAttack() > beforeAttack[i])
            {
                attackFlash = 2;
                nameFlash = 2;
                hasChange = true;
            }

            if (team[i]->getHp() < beforeHp[i])
            {
                hpFlash = 1;
                nameFlash = 1;
                hasChange = true;
            }
            else if (team[i]->getHp() > beforeHp[i])
            {
                hpFlash = 2;

                if (nameFlash == 0)
                {
                    nameFlash = 2;
                }

                hasChange = true;
            }
        }

        if (isPlayerTeam)
        {
            playerNameFlash[i] = nameFlash;
            playerAttackFlash[i] = attackFlash;
            playerHpFlash[i] = hpFlash;
        }
        else
        {
            enemyNameFlash[i] = nameFlash;
            enemyAttackFlash[i] = attackFlash;
            enemyHpFlash[i] = hpFlash;
        }
    }

    return hasChange;
}

// What it does: Briefly flashes changed battle stats in the user interface.
// What the inputs are: The active teams and their attack and health values before an event.
// What the outputs are: Draws a short colored stat pulse if any stat changed.
void Game::flashStatChanges(Pet *playerTeam[5], Pet *enemyTeam[5], int beforePlayerAttack[5], int beforePlayerHp[5], int beforeEnemyAttack[5], int beforeEnemyHp[5])
{
    bool hasPlayerChange = false;
    bool hasEnemyChange = false;
    const int flashMilliseconds = 300;
    const int resetMilliseconds = 80;

    clearStatFlash();
    hasPlayerChange = markStatFlash(playerTeam, beforePlayerAttack, beforePlayerHp, true);
    hasEnemyChange = markStatFlash(enemyTeam, beforeEnemyAttack, beforeEnemyHp, false);

    if (!hasPlayerChange && !hasEnemyChange)
    {
        clearStatFlash();
        return;
    }

    drawUI(playerTeam, enemyTeam);
    std::cout.flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(flashMilliseconds));
    clearStatFlash();
    drawUI(playerTeam, enemyTeam);
    std::cout.flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(resetMilliseconds));
}

// What it does: Copies the player's team into a temporary team.
// What the inputs are: The target temporary team array.
// What the outputs are: Creates cloned pet pointers in the target team.
void Game::clonePlayerTeam(Pet *targetTeam[5])
{
    for (int i = 0; i < 5; i++)
    {
        targetTeam[i] = nullptr;

        if (player->getTeamPet(i) != nullptr)
        {
            targetTeam[i] = player->getTeamPet(i)->clone();
        }
    }
}

// What it does: Copies the enemy team into a temporary team.
// What the inputs are: The target temporary team array.
// What the outputs are: Creates cloned enemy pet pointers in the target team.
void Game::cloneEnemyTeam(Pet *targetTeam[5])
{
    for (int i = 0; i < 5; i++)
    {
        targetTeam[i] = nullptr;

        if (enemyTeam[i] != nullptr)
        {
            targetTeam[i] = enemyTeam[i]->clone();
        }
    }
}

// What it does: Deletes all pets in a temporary team.
// What the inputs are: The temporary team array to clear.
// What the outputs are: Frees all pet pointers and sets slots to nullptr.
void Game::clearTempTeam(Pet *team[5])
{
    for (int i = 0; i < 5; i++)
    {
        if (team[i] != nullptr)
        {
            delete team[i];
            team[i] = nullptr;
        }
    }
}

// What it does: Finds the right-most living pet in a team.
// What the inputs are: The team pointer array.
// What the outputs are: Returns the front pet index or -1 if no pet exists.
int Game::findFrontIndex(Pet *team[5])
{
    for (int i = 4; i >= 0; i--)
    {
        if (team[i] != nullptr && team[i]->isAlive())
        {
            return i;
        }
    }

    return -1;
}

// What it does: Moves all remaining pets toward the right side.
// What the inputs are: The team pointer array to shift.
// What the outputs are: Keeps pet order while filling slots toward index four.
void Game::shiftTeamRight(Pet *team[5])
{
    Pet *shiftedTeam[5];
    int writeIndex = 4;

    for (int i = 0; i < 5; i++)
    {
        shiftedTeam[i] = nullptr;
    }

    for (int i = 4; i >= 0; i--)
    {
        if (team[i] != nullptr)
        {
            shiftedTeam[writeIndex] = team[i];
            writeIndex--;
        }
    }

    for (int i = 0; i < 5; i++)
    {
        team[i] = shiftedTeam[i];
    }
}

// What it does: Checks whether a team has any living pet.
// What the inputs are: The team pointer array.
// What the outputs are: Returns true if at least one living pet exists.
bool Game::hasAnyPet(Pet *team[5])
{
    return findFrontIndex(team) != -1;
}

// What it does: Removes all fainted pets from a temporary team.
// What the inputs are: The team to clean, active teams, team label, ownership flag, and round number.
// What the outputs are: Triggers faint skills, logs clear battle events, deletes fainted pets, and shifts right.
void Game::removeFaintedPets(Pet *team[5], Pet **activePlayerTeam, Pet **activeEnemyTeam, const std::string &teamLabel, bool isPlayerTeam, int roundNumber)
{
    bool removedPet = false;
    std::string factionLabel = teamLabel;

    if (teamLabel == "Player")
    {
        factionLabel = colorFactionLabel(true);
    }
    else if (teamLabel == "Enemy")
    {
        factionLabel = colorFactionLabel(false);
    }

    for (int i = 0; i < 5; i++)
    {
        if (team[i] != nullptr && team[i]->getHp() <= 0)
        {
            std::string faintedName = team[i]->getName();
            int faintedLevel = team[i]->getLevel();
            int allyAtkBefore[5];
            int allyHpBefore[5];
            int buffTargetIndex = -1;
            int beforePlayerAttack[5];
            int beforePlayerHp[5];
            int beforeEnemyAttack[5];
            int beforeEnemyHp[5];

            for (int j = 0; j < 5; j++)
            {
                allyAtkBefore[j] = -1;
                allyHpBefore[j] = -1;

                if (team[j] != nullptr && team[j] != team[i] && team[j]->isAlive())
                {
                    allyAtkBefore[j] = team[j]->getAttack();
                    allyHpBefore[j] = team[j]->getHp();
                }
            }

            captureTeamStats(activePlayerTeam, beforePlayerAttack, beforePlayerHp);
            captureTeamStats(activeEnemyTeam, beforeEnemyAttack, beforeEnemyHp);
            team[i]->onFaint(team, 5);
            flashStatChanges(activePlayerTeam, activeEnemyTeam, beforePlayerAttack, beforePlayerHp, beforeEnemyAttack, beforeEnemyHp);

            if (faintedName == "Ant")
            {
                for (int j = 0; j < 5; j++)
                {
                    if (allyAtkBefore[j] >= 0 && team[j] != nullptr && team[j]->isAlive())
                    {
                        if (team[j]->getAttack() > allyAtkBefore[j] || team[j]->getHp() > allyHpBefore[j])
                        {
                            buffTargetIndex = j;
                            break;
                        }
                    }
                }

                if (buffTargetIndex >= 0)
                {
                    std::string sourceName = colorPetName(faintedName, isPlayerTeam);
                    std::string targetName = colorPetName(team[buffTargetIndex]->getName(), isPlayerTeam);
                    addLog(formatBattleLog(roundNumber, "SKILL", sourceName + " faint -> " + targetName + " +" + std::to_string(faintedLevel) + "/+" + std::to_string(faintedLevel) + "."), true, activePlayerTeam, activeEnemyTeam);
                }
                else
                {
                    std::string sourceName = colorPetName(faintedName, isPlayerTeam);
                    addLog(formatBattleLog(roundNumber, "SKILL", sourceName + " faint -> no valid ally target."), true, activePlayerTeam, activeEnemyTeam);
                }
            }

            std::string faintedColored = colorPetName(faintedName, isPlayerTeam);
            delete team[i];
            team[i] = nullptr;
            addLog(formatBattleLog(roundNumber, "FAINT", factionLabel + " " + faintedColored + " faints."), true, activePlayerTeam, activeEnemyTeam);
            removedPet = true;
        }
    }

    if (removedPet)
    {
        shiftTeamRight(team);
        addLog(formatBattleLog(roundNumber, "SHIFT", factionLabel + " team shifts right."), true, activePlayerTeam, activeEnemyTeam);
    }
}

// What it does: Replaces the whole shop with three turn-limited random pets.
// What the inputs are: None.
// What the outputs are: Deletes old shop pets and creates three new shop pets for the current turn.
void Game::rollShop()
{
    int maxTier = 1;

    clearShop();

    if (currentTurn <= 2)
    {
        maxTier = 1;
    }
    else if (currentTurn <= 4)
    {
        maxTier = 2;
    }
    else
    {
        maxTier = 3;
    }

    for (int i = 0; i < 3; i++)
    {
        shopPets[i] = createRandomPetByTier(maxTier);
    }
}

// What it does: Converts a shop slot letter into an array index.
// What the inputs are: The shop slot letter.
// What the outputs are: Returns 0, 1, or 2 for valid letters, otherwise -1.
int Game::getShopIndex(char shopLetter)
{
    if (shopLetter == 'A' || shopLetter == 'a')
    {
        return 0;
    }

    if (shopLetter == 'B' || shopLetter == 'b')
    {
        return 1;
    }

    if (shopLetter == 'C' || shopLetter == 'c')
    {
        return 2;
    }

    return -1;
}

// What it does: Removes spaces and changes command text to lowercase.
// What the inputs are: The raw input line from the player.
// What the outputs are: Returns the cleaned command string.
std::string Game::preprocessCommand(std::string inputLine)
{
    std::string command;

    for (int i = 0; i < static_cast<int>(inputLine.size()); i++)
    {
        char currentChar = inputLine[i];

        if (currentChar == ' ')
        {
            continue;
        }

        if (currentChar >= 'A' && currentChar <= 'Z')
        {
            currentChar = currentChar - 'A' + 'a';
        }

        command = command + currentChar;
    }

    return command;
}

// What it does: Applies turn-based level and stat scaling to an enemy pet.
// What the inputs are: The enemy pet pointer and the current turn.
// What the outputs are: Updates the enemy pet level and stats.
void Game::applyEnemyScaling(Pet *pet, int turn)
{
    applyNormalEnemyScaling(pet, turn);

    if (difficultyMode == HARD_MODE)
    {
        applyHardEnemyScaling(pet, turn);
        boostHardEnemyStats(pet);
    }
}

// What it does: Applies Normal mode level and stat scaling to an enemy pet.
// What the inputs are: The enemy pet pointer and the current turn.
// What the outputs are: Updates the enemy pet level and stats for Normal mode.
void Game::applyNormalEnemyScaling(Pet *pet, int turn)
{
    int attackBonus = 0;
    int hpBonus = 0;
    int levelRoll = std::rand() % 100;

    if (pet == nullptr)
    {
        return;
    }

    if (turn == 1)
    {
        attackBonus = 0;
        hpBonus = 0;
    }
    else if (turn == 2)
    {
        attackBonus = std::rand() % 2;
        hpBonus = std::rand() % 2;
    }
    else if (turn == 3)
    {
        if (levelRoll < 10)
        {
            pet->setLevel(2);
        }

        attackBonus = std::rand() % 2;
        hpBonus = std::rand() % 2;
    }
    else if (turn == 4)
    {
        if (levelRoll < 30)
        {
            pet->setLevel(2);
        }

        attackBonus = 1 + (std::rand() % 2);
        hpBonus = 1 + (std::rand() % 2);
    }
    else if (turn == 5)
    {
        if (levelRoll < 50)
        {
            pet->setLevel(2);
        }

        attackBonus = 1 + (std::rand() % 3);
        hpBonus = 1 + (std::rand() % 3);
    }
    else if (turn == 6)
    {
        if (levelRoll < 75)
        {
            pet->setLevel(2);
        }

        attackBonus = 2 + (std::rand() % 3);
        hpBonus = 2 + (std::rand() % 3);
    }
    else if (turn == 7)
    {
        if (levelRoll < 10)
        {
            pet->setLevel(3);
        }
        else
        {
            pet->setLevel(2);
        }

        attackBonus = 3 + (std::rand() % 3);
        hpBonus = 3 + (std::rand() % 3);
    }
    else if (turn == 8)
    {
        if (levelRoll < 30)
        {
            pet->setLevel(3);
        }
        else
        {
            pet->setLevel(2);
        }

        attackBonus = 4 + (std::rand() % 3);
        hpBonus = 4 + (std::rand() % 3);
    }
    else if (turn == 9)
    {
        if (levelRoll < 50)
        {
            pet->setLevel(3);
        }
        else
        {
            pet->setLevel(2);
        }

        attackBonus = 5 + (std::rand() % 3);
        hpBonus = 5 + (std::rand() % 3);
    }
    else
    {
        if (levelRoll < 80)
        {
            pet->setLevel(3);
        }
        else
        {
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
void Game::applyHardEnemyScaling(Pet *pet, int turn)
{
    int levelRoll = std::rand() % 100;

    if (pet == nullptr)
    {
        return;
    }

    if (turn == 1)
    {
        pet->setLevel(1);
    }
    else if (turn == 2)
    {
        if (levelRoll < 10)
        {
            pet->setLevel(2);
        }
        else
        {
            pet->setLevel(1);
        }
    }
    else if (turn == 3)
    {
        if (levelRoll < 20)
        {
            pet->setLevel(2);
        }
        else
        {
            pet->setLevel(1);
        }
    }
    else if (turn == 4)
    {
        if (levelRoll < 40)
        {
            pet->setLevel(2);
        }
        else
        {
            pet->setLevel(1);
        }
    }
    else if (turn == 5)
    {
        if (levelRoll < 60)
        {
            pet->setLevel(2);
        }
    }
    else if (turn == 6)
    {
        if (levelRoll < 10)
        {
            pet->setLevel(3);
        }
        else
        {
            pet->setLevel(2);
        }
    }
    else if (turn == 7)
    {
        if (levelRoll < 20)
        {
            pet->setLevel(3);
        }
        else
        {
            pet->setLevel(2);
        }
    }
    else if (turn == 8)
    {
        if (levelRoll < 40)
        {
            pet->setLevel(3);
        }
        else
        {
            pet->setLevel(2);
        }
    }
    else if (turn == 9)
    {
        if (levelRoll < 60)
        {
            pet->setLevel(3);
        }
        else
        {
            pet->setLevel(2);
        }
    }
    else
    {
        if (levelRoll < 90)
        {
            pet->setLevel(3);
        }
        else
        {
            pet->setLevel(2);
        }
    }
}

// What it does: Raises Hard mode enemy stats using integer scaling.
// What the inputs are: The enemy pet pointer.
// What the outputs are: Updates attack and health with a stronger Hard mode value.
void Game::boostHardEnemyStats(Pet *pet)
{
    int boostedAttack = 0;
    int boostedHp = 0;

    if (pet == nullptr)
    {
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
void Game::generateEnemyTeam(int turn)
{
    int enemyCount = 3;
    int maxTier = 1;

    clearEnemyTeam();

    if (turn <= 2)
    {
        enemyCount = 3;
        maxTier = 1;
    }
    else if (turn <= 4)
    {
        enemyCount = 4;
        maxTier = 2;
    }
    else
    {
        enemyCount = 5;
        maxTier = 3;
    }

    for (int i = 0; i < enemyCount; i++)
    {
        int targetIndex = 5 - enemyCount + i;
        enemyTeam[targetIndex] = createRandomPetByTier(maxTier);
        applyEnemyScaling(enemyTeam[targetIndex], turn);
    }
}

// What it does: Runs the real auto battle using cloned temporary teams.
// What the inputs are: None.
// What the outputs are: Returns 1 for win, 0 for draw, and -1 for loss.
int Game::battlePhase()
{
    Pet *tempPlayerTeam[5];
    Pet *tempEnemyTeam[5];
    int battleResult = 0;
    int roundNumber = 1;
    int beforePlayerAttack[5];
    int beforePlayerHp[5];
    int beforeEnemyAttack[5];
    int beforeEnemyHp[5];

    for (int i = 0; i < 5; i++)
    {
        tempPlayerTeam[i] = nullptr;
        tempEnemyTeam[i] = nullptr;
    }

    clonePlayerTeam(tempPlayerTeam);
    cloneEnemyTeam(tempEnemyTeam);
    shiftTeamRight(tempPlayerTeam);
    shiftTeamRight(tempEnemyTeam);

    for (int i = 0; i < 5; i++)
    {
        if (tempPlayerTeam[i] != nullptr)
        {
            tempPlayerTeam[i]->resetBattleState();
        }

        if (tempEnemyTeam[i] != nullptr)
        {
            tempEnemyTeam[i]->resetBattleState();
        }
    }

    clearStatFlash();
    addLog(formatBattleLog(0, "PHASE", "Battle started. Frontline is on the right side."), true, tempPlayerTeam, tempEnemyTeam);

    for (int i = 0; i < 5; i++)
    {
        if (tempPlayerTeam[i] != nullptr && tempPlayerTeam[i]->isAlive())
        {
            std::string petName = tempPlayerTeam[i]->getName();
            int hpBefore[5];
            int targetCount = 0;
            std::string targetList = "";

            for (int j = 0; j < 5; j++)
            {
                hpBefore[j] = -1;

                if (tempEnemyTeam[j] != nullptr)
                {
                    hpBefore[j] = tempEnemyTeam[j]->getHp();
                }
            }

            captureTeamStats(tempPlayerTeam, beforePlayerAttack, beforePlayerHp);
            captureTeamStats(tempEnemyTeam, beforeEnemyAttack, beforeEnemyHp);
            tempPlayerTeam[i]->onBattleStart(tempEnemyTeam, 5);
            flashStatChanges(tempPlayerTeam, tempEnemyTeam, beforePlayerAttack, beforePlayerHp, beforeEnemyAttack, beforeEnemyHp);

            if (petName == "Mosquito")
            {
                for (int j = 0; j < 5; j++)
                {
                    if (tempEnemyTeam[j] != nullptr && hpBefore[j] > tempEnemyTeam[j]->getHp())
                    {
                        if (targetCount > 0)
                        {
                            targetList = targetList + ", ";
                        }

                        targetList = targetList + colorPetName(tempEnemyTeam[j]->getName(), false);
                        targetCount++;
                    }
                }

                if (targetCount == 0)
                {
                    addLog(formatBattleLog(0, "SKILL", colorPetName(petName, true) + " snipes no valid target."), true, tempPlayerTeam, tempEnemyTeam);
                }
                else
                {
                    addLog(formatBattleLog(0, "SKILL", colorPetName(petName, true) + " snipes " + targetList + " for 1 damage."), true, tempPlayerTeam, tempEnemyTeam);
                }
            }
            else if (petName == "Skunk")
            {
                for (int j = 0; j < 5; j++)
                {
                    if (tempEnemyTeam[j] != nullptr && hpBefore[j] > tempEnemyTeam[j]->getHp())
                    {
                        targetList = colorPetName(tempEnemyTeam[j]->getName(), false);
                        targetCount = 1;
                        break;
                    }
                }

                if (targetCount == 0)
                {
                    addLog(formatBattleLog(0, "SKILL", colorPetName(petName, true) + " finds no valid target."), true, tempPlayerTeam, tempEnemyTeam);
                }
                else
                {
                    addLog(formatBattleLog(0, "SKILL", colorPetName(petName, true) + " weakens " + targetList + "."), true, tempPlayerTeam, tempEnemyTeam);
                }
            }

            removeFaintedPets(tempEnemyTeam, tempPlayerTeam, tempEnemyTeam, "Enemy", false, 0);
        }

        if (tempEnemyTeam[i] != nullptr && tempEnemyTeam[i]->isAlive())
        {
            std::string petName = tempEnemyTeam[i]->getName();
            int hpBefore[5];
            int targetCount = 0;
            std::string targetList = "";

            for (int j = 0; j < 5; j++)
            {
                hpBefore[j] = -1;

                if (tempPlayerTeam[j] != nullptr)
                {
                    hpBefore[j] = tempPlayerTeam[j]->getHp();
                }
            }

            captureTeamStats(tempPlayerTeam, beforePlayerAttack, beforePlayerHp);
            captureTeamStats(tempEnemyTeam, beforeEnemyAttack, beforeEnemyHp);
            tempEnemyTeam[i]->onBattleStart(tempPlayerTeam, 5);
            flashStatChanges(tempPlayerTeam, tempEnemyTeam, beforePlayerAttack, beforePlayerHp, beforeEnemyAttack, beforeEnemyHp);

            if (petName == "Mosquito")
            {
                for (int j = 0; j < 5; j++)
                {
                    if (tempPlayerTeam[j] != nullptr && hpBefore[j] > tempPlayerTeam[j]->getHp())
                    {
                        if (targetCount > 0)
                        {
                            targetList = targetList + ", ";
                        }

                        targetList = targetList + colorPetName(tempPlayerTeam[j]->getName(), true);
                        targetCount++;
                    }
                }

                if (targetCount == 0)
                {
                    addLog(formatBattleLog(0, "SKILL", colorPetName(petName, false) + " snipes no valid target."), true, tempPlayerTeam, tempEnemyTeam);
                }
                else
                {
                    addLog(formatBattleLog(0, "SKILL", colorPetName(petName, false) + " snipes " + targetList + " for 1 damage."), true, tempPlayerTeam, tempEnemyTeam);
                }
            }
            else if (petName == "Skunk")
            {
                for (int j = 0; j < 5; j++)
                {
                    if (tempPlayerTeam[j] != nullptr && hpBefore[j] > tempPlayerTeam[j]->getHp())
                    {
                        targetList = colorPetName(tempPlayerTeam[j]->getName(), true);
                        targetCount = 1;
                        break;
                    }
                }

                if (targetCount == 0)
                {
                    addLog(formatBattleLog(0, "SKILL", colorPetName(petName, false) + " finds no valid target."), true, tempPlayerTeam, tempEnemyTeam);
                }
                else
                {
                    addLog(formatBattleLog(0, "SKILL", colorPetName(petName, false) + " weakens " + targetList + "."), true, tempPlayerTeam, tempEnemyTeam);
                }
            }

            removeFaintedPets(tempPlayerTeam, tempPlayerTeam, tempEnemyTeam, "Player", true, 0);
        }
    }

    while (true)
    {
        int playerFrontIndex = findFrontIndex(tempPlayerTeam);
        int enemyFrontIndex = findFrontIndex(tempEnemyTeam);

        if (playerFrontIndex == -1 && enemyFrontIndex == -1)
        {
            battleResult = 0;
            break;
        }

        if (playerFrontIndex == -1)
        {
            battleResult = -1;
            break;
        }

        if (enemyFrontIndex == -1)
        {
            battleResult = 1;
            break;
        }

        addLog(formatBattleLog(roundNumber, "ROUND", "Combat exchange begins."), true, tempPlayerTeam, tempEnemyTeam);

        Pet *playerFront = tempPlayerTeam[playerFrontIndex];
        Pet *enemyFront = tempEnemyTeam[enemyFrontIndex];
        int playerAttack = playerFront->getAttack();
        int enemyAttack = enemyFront->getAttack();
        int playerHpBeforeDamage = playerFront->getHp();
        int enemyHpBeforeDamage = enemyFront->getHp();

        std::string playerName = colorPetName(playerFront->getName(), true);
        std::string enemyName = colorPetName(enemyFront->getName(), false);

        captureTeamStats(tempPlayerTeam, beforePlayerAttack, beforePlayerHp);
        captureTeamStats(tempEnemyTeam, beforeEnemyAttack, beforeEnemyHp);
        playerFront->onAttack(tempPlayerTeam, playerFrontIndex, 5);
        flashStatChanges(tempPlayerTeam, tempEnemyTeam, beforePlayerAttack, beforePlayerHp, beforeEnemyAttack, beforeEnemyHp);
        if (playerFront->getName() == "Elephant")
        {
            int targetIndex = -1;

            for (int k = playerFrontIndex - 1; k >= 0; k--)
            {
                if (tempPlayerTeam[k] != nullptr && tempPlayerTeam[k]->isAlive())
                {
                    targetIndex = k;
                    break;
                }
            }

            if (targetIndex >= 0)
            {
                std::string targetName = colorPetName(tempPlayerTeam[targetIndex]->getName(), true);
                addLog(formatBattleLog(roundNumber, "SKILL", playerName + " tramples " + targetName + " for 1 damage x" + std::to_string(playerFront->getLevel()) + "."), true, tempPlayerTeam, tempEnemyTeam);
            }
            else
            {
                addLog(formatBattleLog(roundNumber, "SKILL", playerName + " has no ally behind to trample."), true, tempPlayerTeam, tempEnemyTeam);
            }
        }
        removeFaintedPets(tempPlayerTeam, tempPlayerTeam, tempEnemyTeam, "Player", true, roundNumber);
        removeFaintedPets(tempEnemyTeam, tempPlayerTeam, tempEnemyTeam, "Enemy", false, roundNumber);

        captureTeamStats(tempPlayerTeam, beforePlayerAttack, beforePlayerHp);
        captureTeamStats(tempEnemyTeam, beforeEnemyAttack, beforeEnemyHp);
        enemyFront->onAttack(tempEnemyTeam, enemyFrontIndex, 5);
        flashStatChanges(tempPlayerTeam, tempEnemyTeam, beforePlayerAttack, beforePlayerHp, beforeEnemyAttack, beforeEnemyHp);
        if (enemyFront->getName() == "Elephant")
        {
            int targetIndex = -1;

            for (int k = enemyFrontIndex - 1; k >= 0; k--)
            {
                if (tempEnemyTeam[k] != nullptr && tempEnemyTeam[k]->isAlive())
                {
                    targetIndex = k;
                    break;
                }
            }

            if (targetIndex >= 0)
            {
                std::string targetName = colorPetName(tempEnemyTeam[targetIndex]->getName(), false);
                addLog(formatBattleLog(roundNumber, "SKILL", enemyName + " tramples " + targetName + " for 1 damage x" + std::to_string(enemyFront->getLevel()) + "."), true, tempPlayerTeam, tempEnemyTeam);
            }
            else
            {
                addLog(formatBattleLog(roundNumber, "SKILL", enemyName + " has no ally behind to trample."), true, tempPlayerTeam, tempEnemyTeam);
            }
        }
        removeFaintedPets(tempPlayerTeam, tempPlayerTeam, tempEnemyTeam, "Player", true, roundNumber);
        removeFaintedPets(tempEnemyTeam, tempPlayerTeam, tempEnemyTeam, "Enemy", false, roundNumber);

        playerFrontIndex = findFrontIndex(tempPlayerTeam);
        enemyFrontIndex = findFrontIndex(tempEnemyTeam);

        if (playerFrontIndex == -1 || enemyFrontIndex == -1)
        {
            continue;
        }

        playerFront = tempPlayerTeam[playerFrontIndex];
        enemyFront = tempEnemyTeam[enemyFrontIndex];
        playerAttack = playerFront->getAttack();
        enemyAttack = enemyFront->getAttack();
        playerHpBeforeDamage = playerFront->getHp();
        enemyHpBeforeDamage = enemyFront->getHp();

        playerName = colorPetName(playerFront->getName(), true);
        enemyName = colorPetName(enemyFront->getName(), false);
        addLog(formatBattleLog(roundNumber, "ATTACK", playerName + " -> " + enemyName + ", DMG " + std::to_string(playerAttack) + "."), true, tempPlayerTeam, tempEnemyTeam);
        addLog(formatBattleLog(roundNumber, "ATTACK", enemyName + " -> " + playerName + ", DMG " + std::to_string(enemyAttack) + "."), true, tempPlayerTeam, tempEnemyTeam);

        captureTeamStats(tempPlayerTeam, beforePlayerAttack, beforePlayerHp);
        captureTeamStats(tempEnemyTeam, beforeEnemyAttack, beforeEnemyHp);
        enemyFront->takeDamage(playerAttack);
        playerFront->takeDamage(enemyAttack);
        flashStatChanges(tempPlayerTeam, tempEnemyTeam, beforePlayerAttack, beforePlayerHp, beforeEnemyAttack, beforeEnemyHp);
        addLog(formatBattleLog(roundNumber, "STATE", playerName + " HP " + std::to_string(playerHpBeforeDamage) + "->" + std::to_string(playerFront->getHp()) + ", " + enemyName + " HP " + std::to_string(enemyHpBeforeDamage) + "->" + std::to_string(enemyFront->getHp()) + "."), true, tempPlayerTeam, tempEnemyTeam);

        if (playerFront->getHp() > 0 && playerFront->getHp() < playerHpBeforeDamage)
        {
            if (playerFront->getName() == "Camel")
            {
                int targetIndex = -1;
                int atkBefore = 0;
                int hpBefore = 0;

                for (int k = playerFrontIndex - 1; k >= 0; k--)
                {
                    if (tempPlayerTeam[k] != nullptr && tempPlayerTeam[k]->isAlive())
                    {
                        targetIndex = k;
                        atkBefore = tempPlayerTeam[k]->getAttack();
                        hpBefore = tempPlayerTeam[k]->getHp();
                        break;
                    }
                }

                captureTeamStats(tempPlayerTeam, beforePlayerAttack, beforePlayerHp);
                captureTeamStats(tempEnemyTeam, beforeEnemyAttack, beforeEnemyHp);
                playerFront->onHurt(tempPlayerTeam, playerFrontIndex, 5, tempEnemyTeam, 5);
                flashStatChanges(tempPlayerTeam, tempEnemyTeam, beforePlayerAttack, beforePlayerHp, beforeEnemyAttack, beforeEnemyHp);

                if (targetIndex >= 0 && tempPlayerTeam[targetIndex] != nullptr)
                {
                    std::string targetName = colorPetName(tempPlayerTeam[targetIndex]->getName(), true);
                    addLog(formatBattleLog(roundNumber, "SKILL", playerName + " buffs " + targetName + " (" + std::to_string(atkBefore) + "/" + std::to_string(hpBefore) + " -> " + std::to_string(tempPlayerTeam[targetIndex]->getAttack()) + "/" + std::to_string(tempPlayerTeam[targetIndex]->getHp()) + ")."), true, tempPlayerTeam, tempEnemyTeam);
                }
                else
                {
                    addLog(formatBattleLog(roundNumber, "SKILL", playerName + " has no ally behind to buff."), true, tempPlayerTeam, tempEnemyTeam);
                }
            }
            else if (playerFront->getName() == "Blowfish")
            {
                int hpBeforeEnemy[5];
                int targetIndex = -1;

                for (int k = 0; k < 5; k++)
                {
                    hpBeforeEnemy[k] = -1;

                    if (tempEnemyTeam[k] != nullptr)
                    {
                        hpBeforeEnemy[k] = tempEnemyTeam[k]->getHp();
                    }
                }

                captureTeamStats(tempPlayerTeam, beforePlayerAttack, beforePlayerHp);
                captureTeamStats(tempEnemyTeam, beforeEnemyAttack, beforeEnemyHp);
                playerFront->onHurt(tempPlayerTeam, playerFrontIndex, 5, tempEnemyTeam, 5);
                flashStatChanges(tempPlayerTeam, tempEnemyTeam, beforePlayerAttack, beforePlayerHp, beforeEnemyAttack, beforeEnemyHp);

                for (int k = 0; k < 5; k++)
                {
                    if (tempEnemyTeam[k] != nullptr && hpBeforeEnemy[k] > tempEnemyTeam[k]->getHp())
                    {
                        targetIndex = k;
                        break;
                    }
                }

                if (targetIndex >= 0)
                {
                    std::string targetName = colorPetName(tempEnemyTeam[targetIndex]->getName(), false);
                    addLog(formatBattleLog(roundNumber, "SKILL", playerName + " splashes " + targetName + " for " + std::to_string(3 * playerFront->getLevel()) + "."), true, tempPlayerTeam, tempEnemyTeam);
                }
                else
                {
                    addLog(formatBattleLog(roundNumber, "SKILL", playerName + " finds no valid splash target."), true, tempPlayerTeam, tempEnemyTeam);
                }
            }
            else
            {
                captureTeamStats(tempPlayerTeam, beforePlayerAttack, beforePlayerHp);
                captureTeamStats(tempEnemyTeam, beforeEnemyAttack, beforeEnemyHp);
                playerFront->onHurt(tempPlayerTeam, playerFrontIndex, 5, tempEnemyTeam, 5);
                flashStatChanges(tempPlayerTeam, tempEnemyTeam, beforePlayerAttack, beforePlayerHp, beforeEnemyAttack, beforeEnemyHp);
            }
        }

        if (enemyFront->getHp() > 0 && enemyFront->getHp() < enemyHpBeforeDamage)
        {
            if (enemyFront->getName() == "Camel")
            {
                int targetIndex = -1;
                int atkBefore = 0;
                int hpBefore = 0;

                for (int k = enemyFrontIndex - 1; k >= 0; k--)
                {
                    if (tempEnemyTeam[k] != nullptr && tempEnemyTeam[k]->isAlive())
                    {
                        targetIndex = k;
                        atkBefore = tempEnemyTeam[k]->getAttack();
                        hpBefore = tempEnemyTeam[k]->getHp();
                        break;
                    }
                }

                captureTeamStats(tempPlayerTeam, beforePlayerAttack, beforePlayerHp);
                captureTeamStats(tempEnemyTeam, beforeEnemyAttack, beforeEnemyHp);
                enemyFront->onHurt(tempEnemyTeam, enemyFrontIndex, 5, tempPlayerTeam, 5);
                flashStatChanges(tempPlayerTeam, tempEnemyTeam, beforePlayerAttack, beforePlayerHp, beforeEnemyAttack, beforeEnemyHp);

                if (targetIndex >= 0 && tempEnemyTeam[targetIndex] != nullptr)
                {
                    std::string targetName = colorPetName(tempEnemyTeam[targetIndex]->getName(), false);
                    addLog(formatBattleLog(roundNumber, "SKILL", enemyName + " buffs " + targetName + " (" + std::to_string(atkBefore) + "/" + std::to_string(hpBefore) + " -> " + std::to_string(tempEnemyTeam[targetIndex]->getAttack()) + "/" + std::to_string(tempEnemyTeam[targetIndex]->getHp()) + ")."), true, tempPlayerTeam, tempEnemyTeam);
                }
                else
                {
                    addLog(formatBattleLog(roundNumber, "SKILL", enemyName + " has no ally behind to buff."), true, tempPlayerTeam, tempEnemyTeam);
                }
            }
            else if (enemyFront->getName() == "Blowfish")
            {
                int hpBeforePlayer[5];
                int targetIndex = -1;

                for (int k = 0; k < 5; k++)
                {
                    hpBeforePlayer[k] = -1;

                    if (tempPlayerTeam[k] != nullptr)
                    {
                        hpBeforePlayer[k] = tempPlayerTeam[k]->getHp();
                    }
                }

                captureTeamStats(tempPlayerTeam, beforePlayerAttack, beforePlayerHp);
                captureTeamStats(tempEnemyTeam, beforeEnemyAttack, beforeEnemyHp);
                enemyFront->onHurt(tempEnemyTeam, enemyFrontIndex, 5, tempPlayerTeam, 5);
                flashStatChanges(tempPlayerTeam, tempEnemyTeam, beforePlayerAttack, beforePlayerHp, beforeEnemyAttack, beforeEnemyHp);

                for (int k = 0; k < 5; k++)
                {
                    if (tempPlayerTeam[k] != nullptr && hpBeforePlayer[k] > tempPlayerTeam[k]->getHp())
                    {
                        targetIndex = k;
                        break;
                    }
                }

                if (targetIndex >= 0)
                {
                    std::string targetName = colorPetName(tempPlayerTeam[targetIndex]->getName(), true);
                    addLog(formatBattleLog(roundNumber, "SKILL", enemyName + " splashes " + targetName + " for " + std::to_string(3 * enemyFront->getLevel()) + "."), true, tempPlayerTeam, tempEnemyTeam);
                }
                else
                {
                    addLog(formatBattleLog(roundNumber, "SKILL", enemyName + " finds no valid splash target."), true, tempPlayerTeam, tempEnemyTeam);
                }
            }
            else
            {
                captureTeamStats(tempPlayerTeam, beforePlayerAttack, beforePlayerHp);
                captureTeamStats(tempEnemyTeam, beforeEnemyAttack, beforeEnemyHp);
                enemyFront->onHurt(tempEnemyTeam, enemyFrontIndex, 5, tempPlayerTeam, 5);
                flashStatChanges(tempPlayerTeam, tempEnemyTeam, beforePlayerAttack, beforePlayerHp, beforeEnemyAttack, beforeEnemyHp);
            }
        }

        if (enemyFront->getHp() <= 0 && playerFront->getHp() > 0)
        {
            captureTeamStats(tempPlayerTeam, beforePlayerAttack, beforePlayerHp);
            captureTeamStats(tempEnemyTeam, beforeEnemyAttack, beforeEnemyHp);
            playerFront->onKnockOut(enemyFront);
            flashStatChanges(tempPlayerTeam, tempEnemyTeam, beforePlayerAttack, beforePlayerHp, beforeEnemyAttack, beforeEnemyHp);

            if (playerFront->getName() == "Hippo")
            {
                addLog(formatBattleLog(roundNumber, "SKILL", playerName + " gains +" + std::to_string(3 * playerFront->getLevel()) + "/+" + std::to_string(3 * playerFront->getLevel()) + "."), true, tempPlayerTeam, tempEnemyTeam);
            }
        }

        if (playerFront->getHp() <= 0 && enemyFront->getHp() > 0)
        {
            captureTeamStats(tempPlayerTeam, beforePlayerAttack, beforePlayerHp);
            captureTeamStats(tempEnemyTeam, beforeEnemyAttack, beforeEnemyHp);
            enemyFront->onKnockOut(playerFront);
            flashStatChanges(tempPlayerTeam, tempEnemyTeam, beforePlayerAttack, beforePlayerHp, beforeEnemyAttack, beforeEnemyHp);

            if (enemyFront->getName() == "Hippo")
            {
                addLog(formatBattleLog(roundNumber, "SKILL", enemyName + " gains +" + std::to_string(3 * enemyFront->getLevel()) + "/+" + std::to_string(3 * enemyFront->getLevel()) + "."), true, tempPlayerTeam, tempEnemyTeam);
            }
        }

        removeFaintedPets(tempPlayerTeam, tempPlayerTeam, tempEnemyTeam, "Player", true, roundNumber);
        removeFaintedPets(tempEnemyTeam, tempPlayerTeam, tempEnemyTeam, "Enemy", false, roundNumber);
        roundNumber++;
    }

    clearStatFlash();
    clearTempTeam(tempPlayerTeam);
    clearTempTeam(tempEnemyTeam);

    return battleResult;
}

// What it does: Runs the shop phase command loop.
// What the inputs are: None.
// What the outputs are: Processes player commands until the player ends the phase or the game ends.
void Game::shopPhase()
{
    char command;
    std::string commandLine;
    std::string cleanCommand;

    if (resumeShopWithoutSetup)
    {
        resumeShopWithoutSetup = false;
    }
    else
    {
        player->resetGold();

        for (int i = 0; i < 5; i++)
        {
            Pet *pet = player->getTeamPet(i);

            if (pet != nullptr)
            {
                pet->onShopStart(player);

                if (pet->getName() == "Swan")
                {
                    addLog("Swan shop start skill triggered.", true);
                }
            }
        }
    }

    while (true)
    {
        if (player->getHp() <= 0)
        {
            addLog("Defeat. The player has no HP left.", true);
            return;
        }

        if (wins >= 10)
        {
            addLog("Victory. The player reached 10 wins.", true);
            return;
        }

        drawUI();

        if (!std::getline(std::cin, commandLine))
        {
            return;
        }

        cleanCommand = preprocessCommand(commandLine);

        if (cleanCommand.size() == 0)
        {
            addLog("Invalid command format!", true);
            continue;
        }

        if (cleanCommand == "saveq")
        {
            if (saveGameToFile("cli_auto_pets_save.txt"))
            {
                saveAndQuitRequested = true;
                std::cout << std::endl;
                std::cout << "Game saved to cli_auto_pets_save.txt. Goodbye!" << std::endl;
                return;
            }

            addLog("Save failed. Check file permissions and try again.", true);
            continue;
        }

        if (cleanCommand.size() >= 4 && cleanCommand.substr(0, 4) == "view")
        {
            showPetInfo(cleanCommand.substr(4));
            continue;
        }

        command = cleanCommand[0];

        if (command == 18)
        {
            command = 'r';
        }

        if (command == 'r')
        {
            if (cleanCommand.size() != 1)
            {
                addLog("Invalid command format!", true);
                continue;
            }

            if (player->spendGold(1))
            {
                rollShop();
                addLog("Rolled a completely new shop.", true);
            }
            else
            {
                addLog("Not enough gold to roll.", true);
            }
        }
        else if (command == 'b')
        {
            int shopIndex = -1;
            int targetSlot = -1;

            if (cleanCommand.size() != 3)
            {
                addLog("Invalid command format!", true);
                continue;
            }

            shopIndex = getShopIndex(cleanCommand[1]);

            if (cleanCommand[2] >= '1' && cleanCommand[2] <= '5')
            {
                targetSlot = cleanCommand[2] - '1';
            }

            if (shopIndex < 0 || targetSlot < 0 || targetSlot >= 5)
            {
                addLog("Invalid command format!", true);
            }
            else if (shopPets[shopIndex] == nullptr)
            {
                addLog("That shop slot is empty.", true);
            }
            else if (player->buyPet(shopPets[shopIndex], targetSlot))
            {
                shopPets[shopIndex] = nullptr;
                flashGoldChange();
                addLog("Bought or merged the shop pet.", true);
            }
            else
            {
                addLog("Buy failed. Check gold or target slot.", true);
            }
        }
        else if (command == 's')
        {
            int targetSlot = -1;
            Pet *soldPet = nullptr;

            if (cleanCommand.size() != 2)
            {
                addLog("Invalid command format!", true);
                continue;
            }

            if (cleanCommand[1] >= '1' && cleanCommand[1] <= '5')
            {
                targetSlot = cleanCommand[1] - '1';
            }

            soldPet = player->getTeamPet(targetSlot);

            if (soldPet == nullptr)
            {
                addLog("There is no pet in that slot.", true);
            }
            else
            {
                std::string soldName = soldPet->getName();
                int soldLevel = soldPet->getLevel();
                player->sellPet(targetSlot);
                addLog("Sold " + soldName + " for " + std::to_string(soldLevel) + " gold.", true);
            }
        }
        else if (command == 'm')
        {
            int fromSlot = -1;
            int toSlot = -1;

            if (cleanCommand.size() != 3)
            {
                addLog("Invalid command format!", true);
                continue;
            }

            if (cleanCommand[1] >= '1' && cleanCommand[1] <= '5')
            {
                fromSlot = cleanCommand[1] - '1';
            }

            if (cleanCommand[2] >= '1' && cleanCommand[2] <= '5')
            {
                toSlot = cleanCommand[2] - '1';
            }

            if (fromSlot < 0 || fromSlot >= 5 || toSlot < 0 || toSlot >= 5)
            {
                addLog("Invalid command format!", true);
            }
            else if (player->movePet(fromSlot, toSlot))
            {
                addLog("Moved team slot " + std::to_string(fromSlot + 1) + " and slot " + std::to_string(toSlot + 1) + ".", true);
            }
            else
            {
                addLog("Move failed. Source slot is empty.", true);
            }
        }
        else if (command == 'c')
        {
            int fromSlot = -1;
            int toSlot = -1;
            Pet *sourcePet = nullptr;
            Pet *targetPet = nullptr;

            if (cleanCommand.size() != 3)
            {
                addLog("Invalid command format!", true);
                continue;
            }

            if (cleanCommand[1] >= '1' && cleanCommand[1] <= '5')
            {
                fromSlot = cleanCommand[1] - '1';
            }

            if (cleanCommand[2] >= '1' && cleanCommand[2] <= '5')
            {
                toSlot = cleanCommand[2] - '1';
            }

            if (fromSlot < 0 || fromSlot >= 5 || toSlot < 0 || toSlot >= 5 || fromSlot == toSlot)
            {
                addLog("Invalid command format!", true);
                continue;
            }

            sourcePet = player->getTeamPet(fromSlot);
            targetPet = player->getTeamPet(toSlot);

            if (sourcePet == nullptr || targetPet == nullptr)
            {
                addLog("Combine failed. Both slots need pets.", true);
            }
            else if (sourcePet->getName() != targetPet->getName())
            {
                addLog("Combine failed. Pets must be the same type.", true);
            }
            else if (sourcePet->getLevel() >= 3 || targetPet->getLevel() >= 3)
            {
                addLog("Combine failed. Lv3 pets cannot combine.", true);
            }
            else if (player->combinePets(fromSlot, toSlot))
            {
                addLog("Combined slot " + std::to_string(fromSlot + 1) + " into slot " + std::to_string(toSlot + 1) + ".", true);
            }
            else
            {
                addLog("Combine failed.", true);
            }
        }
        else if (command == 'e')
        {
            if (cleanCommand.size() != 1)
            {
                addLog("Invalid command format!", true);
                continue;
            }

            for (int i = 0; i < 5; i++)
            {
                Pet *pet = player->getTeamPet(i);

                if (pet != nullptr)
                {
                    pet->onShopEnd(player->getTeamArray(), 5);

                    if (pet->getName() == "Monkey")
                    {
                        addLog("Monkey shop end skill triggered.", true);
                    }
                }
            }

            addLog("Ending shop phase.", true);
            return;
        }
        else
        {
            addLog("Invalid command format!", true);
        }
    }
}

// What it does: Runs the main game loop until victory or defeat.
// What the inputs are: None.
// What the outputs are: Runs shop, enemy generation, battle result, and final game over output.
void Game::start()
{
    if (!showTitleMenu())
    {
        return;
    }

    while (player->getHp() > 0 && wins < 10)
    {
        shopPhase();

        if (saveAndQuitRequested)
        {
            return;
        }

        if (!std::cin)
        {
            break;
        }

        if (player->getHp() <= 0 || wins >= 10)
        {
            break;
        }

        generateEnemyTeam(currentTurn);
        int battleResult = battlePhase();

        std::cout << "\033[2J\033[1;1H";
        std::cout << "Turn " << currentTurn << " Result" << std::endl;
        std::cout << std::endl;

        if (battleResult == 1)
        {
            std::cout << "You won this round! o(>▽<)o" << std::endl;
            wins++;
        }
        else if (battleResult == -1)
        {
            std::cout << "You lost! (╥﹏╥)" << std::endl;
            player->loseHp(1);
        }
        else
        {
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

    if (wins >= 10)
    {
        std::cout << "Victory. You reached 10 wins." << std::endl;
    }
    else
    {
        std::cout << "Defeat. The player has no HP left." << std::endl;
    }
}
