#ifndef GAME_H
#define GAME_H

#include "pet.h"
#include "player.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#define RESET "\033[0m"
#define RED "\033[31m"
#define BLUE "\033[34m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define GREEN "\033[32m"
#define WHITE "\033[37m"

class Game
{
private:
    enum DifficultyMode
    {
        NORMAL_MODE,
        HARD_MODE
    };

    static const int WIN_TARGET = 5;

    Player *player;
    Pet *shopPets[3];
    Pet *enemyTeam[5];
    int currentTurn;
    int wins;
    DifficultyMode difficultyMode;
    bool saveAndQuitRequested;
    bool resumeShopWithoutSetup;
    bool goldFlash;
    int playerNameFlash[5];
    int playerAttackFlash[5];
    int playerHpFlash[5];
    int enemyNameFlash[5];
    int enemyAttackFlash[5];
    int enemyHpFlash[5];
    std::vector<std::string> messageLog;

    bool showTitleMenu();
    std::string getDifficultyName() const;
    bool saveGameToFile(const std::string &fileName);
    bool loadGameFromFile(const std::string &fileName);
    void writePetSaveLine(std::ofstream &outputFile, const std::string &slotLabel, Pet *pet);
    bool readPetSaveLine(std::istringstream &lineStream, Pet *&pet);
    bool readExpectedLine(std::ifstream &inputFile, const std::string &expectedLine);
    bool readSaveIntLine(std::ifstream &inputFile, const std::string &expectedKey, int &value);
    void rollShop();
    Pet *createPetByName(const std::string &petName);
    Pet *createRandomPetByTier(int maxTier);
    void generateEnemyTeam(int turn);
    void applyEnemyScaling(Pet *pet, int turn);
    void applyNormalEnemyScaling(Pet *pet, int turn);
    void applyHardEnemyScaling(Pet *pet, int turn);
    void boostHardEnemyStats(Pet *pet);
    void clearShop();
    void clearEnemyTeam();
    void flashGoldChange();
    void clearStatFlash();
    void captureTeamStats(Pet *team[5], int attackValues[5], int hpValues[5]);
    bool markStatFlash(Pet *team[5], int beforeAttack[5], int beforeHp[5], bool isPlayerTeam);
    void flashStatChanges(Pet *playerTeam[5], Pet *enemyTeam[5], int beforePlayerAttack[5], int beforePlayerHp[5], int beforeEnemyAttack[5], int beforeEnemyHp[5]);
    void clonePlayerTeam(Pet *targetTeam[5]);
    void cloneEnemyTeam(Pet *targetTeam[5]);
    void clearTempTeam(Pet *team[5]);
    int findFrontIndex(Pet *team[5]);
    void shiftTeamRight(Pet *team[5]);
    bool hasAnyPet(Pet *team[5]);
    void removeFaintedPets(Pet *team[5], Pet **activePlayerTeam, Pet **activeEnemyTeam, const std::string &teamLabel, bool isPlayerTeam, int roundNumber);
    int getShopIndex(char shopLetter);
    std::string preprocessCommand(std::string inputLine);
    void printBoxCell(std::string text, int width);
    std::string applyStatColor(std::string valueText, int flashState);
    std::string getPetStatsLine(Pet *pet, int attackFlash, int hpFlash);
    std::string getPetNameLine(Pet *pet);
    bool showPetInfo(std::string petName);
    std::string getPetBaseStatsLine(std::string petName);
    std::string getPetSkillLine(std::string petName, int level);
    std::string colorPetName(const std::string &petName, bool isPlayerPet);
    std::string colorFactionLabel(bool isPlayerTeam);
    std::string formatBattleLog(int roundNumber, const std::string &tag, const std::string &detail);
    int battlePhase();

public:
    Game();
    ~Game();

    void addLog(std::string msg, bool autoDelay = false, Pet **activePlayerTeam = nullptr, Pet **activeEnemyTeam = nullptr);
    void drawUI(Pet **activePlayerTeam = nullptr, Pet **activeEnemyTeam = nullptr);
    void shopPhase();
    void start();
};

#endif
