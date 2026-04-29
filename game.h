#ifndef GAME_H
#define GAME_H

#include "pet.h"
#include "player.h"

#include <string>
#include <vector>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"

class Game {
private:
    Player* player;
    Pet* shopPets[3];
    Pet* enemyTeam[5];
    int currentTurn;
    int wins;
    std::vector<std::string> messageLog;

    void rollShop();
    Pet* createRandomPetByTier(int maxTier);
    void generateEnemyTeam(int turn);
    void applyEnemyScaling(Pet* pet, int turn);
    void clearShop();
    void clearEnemyTeam();
    void clonePlayerTeam(Pet* targetTeam[5]);
    void cloneEnemyTeam(Pet* targetTeam[5]);
    void clearTempTeam(Pet* team[5]);
    int findFrontIndex(Pet* team[5]);
    void shiftTeamRight(Pet* team[5]);
    bool hasAnyPet(Pet* team[5]);
    void removeFaintedPets(Pet* team[5], Pet** activePlayerTeam, Pet** activeEnemyTeam);
    int getShopIndex(char shopLetter);
    std::string preprocessCommand(std::string inputLine);
    void printBoxCell(std::string text, int width);
    std::string getPetNameLine(Pet* pet);
    int battlePhase();

public:
    Game();
    ~Game();

    void addLog(std::string msg, bool autoDelay = false, Pet** activePlayerTeam = nullptr, Pet** activeEnemyTeam = nullptr);
    void drawUI(Pet** activePlayerTeam = nullptr, Pet** activeEnemyTeam = nullptr);
    void shopPhase();
    void start();
};

#endif
