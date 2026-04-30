#ifndef PLAYER_H
#define PLAYER_H

#include "pet.h"

class Player
{
private:
    int hp;
    int gold;
    Pet *team[5];

public:
    Player();
    ~Player();

    int getHp() const;
    int getGold() const;
    Pet *getTeamPet(int slotIndex) const;
    Pet **getTeamArray();

    void addGold(int amount);
    void resetGold();
    void loseHp(int amount);
    bool spendGold(int amount);
    bool buyPet(Pet *shopPet, int targetSlot);
    void sellPet(int slotIndex);
    bool movePet(int fromSlot, int toSlot);
    bool combinePets(int fromSlot, int toSlot);
    void clearTeam();
};

#endif
