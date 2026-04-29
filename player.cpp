#include "player.h"

// What it does: Creates a player with five empty pet slots.
// What the inputs are: None.
// What the outputs are: Initializes health, gold, and empty team slots.
Player::Player() {
    hp = 5;
    gold = 10;

    for (int i = 0; i < 5; i++) {
        team[i] = nullptr;
    }
}

// What it does: Destroys the player and deletes all owned pets.
// What the inputs are: None.
// What the outputs are: Frees all pet pointers owned by the player.
Player::~Player() {
    clearTeam();
}

// What it does: Gets the player's current health points.
// What the inputs are: None.
// What the outputs are: Returns the current health points.
int Player::getHp() const {
    return hp;
}

// What it does: Adds gold to the player.
// What the inputs are: The amount of gold to add.
// What the outputs are: Updates the player's gold.
void Player::addGold(int amount) {
    gold = gold + amount;
}

// What it does: Resets the player gold to the normal shop phase amount.
// What the inputs are: None.
// What the outputs are: Sets the player's gold to ten.
void Player::resetGold() {
    gold = 10;
}

// What it does: Removes health points from the player.
// What the inputs are: The amount of health points to remove.
// What the outputs are: Updates player health without going below zero.
void Player::loseHp(int amount) {
    hp = hp - amount;

    if (hp < 0) {
        hp = 0;
    }
}

// What it does: Gets the player's current gold.
// What the inputs are: None.
// What the outputs are: Returns the current gold amount.
int Player::getGold() const {
    return gold;
}

// What it does: Gets a pet pointer from a team slot.
// What the inputs are: The slot index to read.
// What the outputs are: Returns the pet pointer or nullptr if the slot is invalid.
Pet* Player::getTeamPet(int slotIndex) const {
    if (slotIndex < 0 || slotIndex >= 5) {
        return nullptr;
    }

    return team[slotIndex];
}

// What it does: Gets the raw team array for skill functions.
// What the inputs are: None.
// What the outputs are: Returns the team pointer array without changing ownership.
Pet** Player::getTeamArray() {
    return team;
}

// What it does: Spends gold from the player if enough gold exists.
// What the inputs are: The amount of gold to spend.
// What the outputs are: Returns true if gold was spent, otherwise false.
bool Player::spendGold(int amount) {
    if (amount < 0 || gold < amount) {
        return false;
    }

    gold = gold - amount;
    return true;
}

// What it does: Buys or merges a shop pet into a chosen team slot.
// What the inputs are: The shop pet pointer and the target slot index.
// What the outputs are: Returns true if the buy or merge succeeds, otherwise false.
bool Player::buyPet(Pet* shopPet, int targetSlot) {
    if (shopPet == nullptr || targetSlot < 0 || targetSlot >= 5) {
        return false;
    }

    if (gold < 3) {
        return false;
    }

    if (team[targetSlot] == nullptr) {
        gold = gold - 3;
        team[targetSlot] = shopPet;
        return true;
    }

    if (team[targetSlot]->getName() == shopPet->getName()) {
        int oldLevel = team[targetSlot]->getLevel();

        gold = gold - 3;
        team[targetSlot]->addExperience(1);
        team[targetSlot]->buffStats(1, 1);

        if (team[targetSlot]->getLevel() > oldLevel) {
            team[targetSlot]->buffStats(1, 1);
        }

        delete shopPet;
        return true;
    }

    return false;
}

// What it does: Sells a pet from a team slot.
// What the inputs are: The slot index to sell from.
// What the outputs are: Adds gold from the pet level, deletes the pet, and clears the slot.
void Player::sellPet(int slotIndex) {
    if (slotIndex < 0 || slotIndex >= 5) {
        return;
    }

    if (team[slotIndex] != nullptr) {
        gold = gold + team[slotIndex]->getLevel();
        delete team[slotIndex];
        team[slotIndex] = nullptr;
    }
}

// What it does: Removes all pets from the player team.
// What the inputs are: None.
// What the outputs are: Deletes every pet pointer and clears all slots.
void Player::clearTeam() {
    for (int i = 0; i < 5; i++) {
        if (team[i] != nullptr) {
            delete team[i];
            team[i] = nullptr;
        }
    }
}
