#include "pet.h"
#include "player.h"

#include <cstdlib>
#include <iostream>
#include <vector>

// What it does: Creates a pet with a name, health points, and attack value.
// What the inputs are: The pet name, health points, and attack value.
// What the outputs are: Initializes the pet object with the given values.
Pet::Pet(const std::string& petName, int petHp, int petAttack) {
    name = petName;
    hp = petHp;
    attack = petAttack;
    level = 1;
    experience = 0;
}

// What it does: Destroys the pet object safely through a base class pointer.
// What the inputs are: None.
// What the outputs are: No return value.
Pet::~Pet() {
}

// What it does: Gets the pet name.
// What the inputs are: None.
// What the outputs are: Returns the pet name.
std::string Pet::getName() const {
    return name;
}

// What it does: Gets the pet health points.
// What the inputs are: None.
// What the outputs are: Returns the pet health points.
int Pet::getHp() const {
    return hp;
}

// What it does: Gets the pet attack value.
// What the inputs are: None.
// What the outputs are: Returns the pet attack value.
int Pet::getAttack() const {
    return attack;
}

// What it does: Gets the pet level.
// What the inputs are: None.
// What the outputs are: Returns the pet level.
int Pet::getLevel() const {
    return level;
}

// What it does: Gets the pet experience count.
// What the inputs are: None.
// What the outputs are: Returns the pet experience count.
int Pet::getExperience() const {
    return experience;
}

// What it does: Checks whether the pet is still alive.
// What the inputs are: None.
// What the outputs are: Returns true if the pet has more than zero health points.
bool Pet::isAlive() const {
    return hp > 0;
}

// What it does: Changes the pet name.
// What the inputs are: The new pet name.
// What the outputs are: Updates the pet name.
void Pet::setName(const std::string& petName) {
    name = petName;
}

// What it does: Changes the pet health points.
// What the inputs are: The new health points.
// What the outputs are: Updates the pet health points.
void Pet::setHp(int petHp) {
    hp = petHp;
}

// What it does: Changes the pet attack value.
// What the inputs are: The new attack value.
// What the outputs are: Updates the pet attack value.
void Pet::setAttack(int petAttack) {
    attack = petAttack;
}

// What it does: Changes the pet level.
// What the inputs are: The new pet level.
// What the outputs are: Updates the pet level between one and three.
void Pet::setLevel(int petLevel) {
    if (petLevel < 1) {
        level = 1;
    } else if (petLevel > 3) {
        level = 3;
    } else {
        level = petLevel;
    }
}

// What it does: Changes the current level experience dots.
// What the inputs are: The new current level experience dots.
// What the outputs are: Updates experience dots within the current level limit.
void Pet::setExperience(int petExperience) {
    if (petExperience < 1) {
        experience = 0;
    } else if (level == 1 && petExperience > 1) {
        experience = 1;
    } else if (level == 2 && petExperience > 2) {
        experience = 2;
    } else if (level >= 3) {
        experience = 0;
    } else {
        experience = petExperience;
    }
}

// What it does: Reduces the pet health points by a damage amount.
// What the inputs are: The damage amount.
// What the outputs are: Updates the pet health points without going below zero.
void Pet::takeDamage(int damage) {
    hp = hp - damage;

    if (hp < 0) {
        hp = 0;
    }
}

// What it does: Increases the pet attack and health points.
// What the inputs are: The attack bonus and health bonus.
// What the outputs are: Updates the pet attack and health points.
void Pet::buffStats(int attackBonus, int hpBonus) {
    attack = attack + attackBonus;
    hp = hp + hpBonus;
}

// What it does: Raises the pet level by one until it reaches level three.
// What the inputs are: None.
// What the outputs are: Updates the pet level if it is below three.
void Pet::increaseLevel() {
    if (level < 3) {
        level++;
    }

    experience = 0;
}

// What it does: Adds current level experience dots to the pet.
// What the inputs are: The experience dot amount to add.
// What the outputs are: Updates experience dots and levels up if a threshold is reached.
void Pet::addExperience(int amount) {
    experience = experience + amount;

    if (level == 1 && experience >= 2) {
        level = 2;
        experience = 0;
    } else if (level == 2 && experience >= 3) {
        level = 3;
        experience = 0;
    } else if (level >= 3) {
        experience = 0;
    }
}

// What it does: Converts the current level and progress into total pet copies.
// What the inputs are: None.
// What the outputs are: Returns the total copy count between one and six.
int Pet::getTotalCopies() const {
    if (level <= 1) {
        return 1 + experience;
    }

    if (level == 2) {
        return 3 + experience;
    }

    return 6;
}

// What it does: Sets level and progress based on a total pet copy count.
// What the inputs are: The total copy count.
// What the outputs are: Updates this pet to the matching level and progress state.
void Pet::setFromTotalCopies(int totalCopies) {
    if (totalCopies <= 1) {
        setLevel(1);
        setExperience(0);
        return;
    }

    if (totalCopies == 2) {
        setLevel(1);
        setExperience(1);
        return;
    }

    if (totalCopies == 3) {
        setLevel(2);
        setExperience(0);
        return;
    }

    if (totalCopies == 4) {
        setLevel(2);
        setExperience(1);
        return;
    }

    if (totalCopies == 5) {
        setLevel(2);
        setExperience(2);
        return;
    }

    setLevel(3);
    setExperience(0);
}

// What it does: Creates a dynamic copy of the base pet.
// What the inputs are: None.
// What the outputs are: Returns a new pet pointer with copied stats and level.
Pet* Pet::clone() const {
    Pet* copiedPet = new Pet(getName(), getHp(), getAttack());
    copiedPet->setExperience(getExperience());
    return copiedPet;
}

// What it does: Runs the pet skill for base pets.
// What the inputs are: None.
// What the outputs are: No return value.
void Pet::triggerSkill() {
}

// What it does: Runs the shop start effect for base pets.
// What the inputs are: The player who owns this pet.
// What the outputs are: No return value.
void Pet::onShopStart(Player*) {
}

// What it does: Runs the faint effect for base pets.
// What the inputs are: The ally team pointer array and the team size.
// What the outputs are: No return value.
void Pet::onFaint(Pet**, int) {
}

// What it does: Runs the battle start effect for base pets.
// What the inputs are: The enemy team pointer array and the team size.
// What the outputs are: No return value.
void Pet::onBattleStart(Pet**, int) {
}

// What it does: Runs the hurt effect for base pets.
// What the inputs are: The ally team, own index, ally team size, enemy team, and enemy team size.
// What the outputs are: No return value.
void Pet::onHurt(Pet**, int, int, Pet**, int) {
}

// What it does: Runs the attack effect for base pets.
// What the inputs are: The ally team pointer array, own index, and team size.
// What the outputs are: No return value.
void Pet::onAttack(Pet**, int, int) {
}

// What it does: Runs the knockout effect for base pets.
// What the inputs are: The enemy pet that was knocked out.
// What the outputs are: No return value.
void Pet::onKnockOut(Pet*) {
}

// What it does: Resets battle-only state for base pets.
// What the inputs are: None.
// What the outputs are: No return value.
void Pet::resetBattleState() {
}

// What it does: Runs the shop end effect for base pets.
// What the inputs are: The ally team pointer array and the team size.
// What the outputs are: No return value.
void Pet::onShopEnd(Pet**, int) {
}

// What it does: Creates a Swan with its starting stats.
// What the inputs are: None.
// What the outputs are: Initializes the Swan object.
Swan::Swan() : Pet("Swan", 2, 1) {
}

// What it does: Creates a dynamic copy of this Swan.
// What the inputs are: None.
// What the outputs are: Returns a new Swan pointer with copied stats and level.
Pet* Swan::clone() const {
    Pet* copiedPet = new Swan();
    copiedPet->setHp(getHp());
    copiedPet->setAttack(getAttack());
    copiedPet->setExperience(getExperience());
    return copiedPet;
}

// What it does: Runs the Swan shop start skill.
// What the inputs are: The player who owns this Swan.
// What the outputs are: Adds one gold to the owner if the owner exists.
void Swan::onShopStart(Player* owner) {
    if (owner == nullptr) {
        return;
    }

    owner->addGold(getLevel());
}

// What it does: Creates an Ant with its starting stats.
// What the inputs are: None.
// What the outputs are: Initializes the Ant object.
Ant::Ant() : Pet("Ant", 2, 2) {
}

// What it does: Creates a dynamic copy of this Ant.
// What the inputs are: None.
// What the outputs are: Returns a new Ant pointer with copied stats and level.
Pet* Ant::clone() const {
    Pet* copiedPet = new Ant();
    copiedPet->setHp(getHp());
    copiedPet->setAttack(getAttack());
    copiedPet->setExperience(getExperience());
    return copiedPet;
}

// What it does: Runs the Ant faint skill.
// What the inputs are: The ally team pointer array and the team size.
// What the outputs are: Gives one living ally plus one attack and plus one health.
void Ant::onFaint(Pet** allyTeam, int teamSize) {
    std::vector<Pet*> livingAllies;

    if (allyTeam == nullptr || teamSize <= 0) {
        return;
    }

    for (int i = 0; i < teamSize; i++) {
        if (allyTeam[i] != nullptr && allyTeam[i] != this && allyTeam[i]->isAlive()) {
            livingAllies.push_back(allyTeam[i]);
        }
    }

    if (livingAllies.size() == 0) {
        return;
    }

    int targetIndex = std::rand() % livingAllies.size();
    livingAllies[targetIndex]->buffStats(getLevel(), getLevel());

}

// What it does: Creates a Mosquito with its starting stats.
// What the inputs are: None.
// What the outputs are: Initializes the Mosquito object.
Mosquito::Mosquito() : Pet("Mosquito", 2, 2) {
}

// What it does: Creates a dynamic copy of this Mosquito.
// What the inputs are: None.
// What the outputs are: Returns a new Mosquito pointer with copied stats and level.
Pet* Mosquito::clone() const {
    Pet* copiedPet = new Mosquito();
    copiedPet->setHp(getHp());
    copiedPet->setAttack(getAttack());
    copiedPet->setExperience(getExperience());
    return copiedPet;
}

// What it does: Runs the Mosquito battle start skill.
// What the inputs are: The enemy team pointer array and the team size.
// What the outputs are: Deals one damage to one random living enemy.
void Mosquito::onBattleStart(Pet** enemyTeam, int teamSize) {
    std::vector<Pet*> livingEnemies;

    if (enemyTeam == nullptr || teamSize <= 0) {
        return;
    }

    for (int i = 0; i < teamSize; i++) {
        if (enemyTeam[i] != nullptr && enemyTeam[i]->isAlive()) {
            livingEnemies.push_back(enemyTeam[i]);
        }
    }

    if (livingEnemies.size() == 0) {
        return;
    }

    for (int hitCount = 0; hitCount < getLevel() && livingEnemies.size() > 0; hitCount++) {
        int targetIndex = std::rand() % livingEnemies.size();
        livingEnemies[targetIndex]->takeDamage(1);

        livingEnemies.erase(livingEnemies.begin() + targetIndex);
    }
}

// What it does: Creates a Camel with its starting stats.
// What the inputs are: None.
// What the outputs are: Initializes the Camel object.
Camel::Camel() : Pet("Camel", 3, 3) {
}

// What it does: Creates a dynamic copy of this Camel.
// What the inputs are: None.
// What the outputs are: Returns a new Camel pointer with copied stats and level.
Pet* Camel::clone() const {
    Pet* copiedPet = new Camel();
    copiedPet->setHp(getHp());
    copiedPet->setAttack(getAttack());
    copiedPet->setExperience(getExperience());
    return copiedPet;
}

// What it does: Runs the Camel hurt skill.
// What the inputs are: The ally team, own index, ally team size, enemy team, and enemy team size.
// What the outputs are: Gives the nearest living ally behind it plus one attack and plus two health.
void Camel::onHurt(Pet** allyTeam, int myIndex, int allyTeamSize, Pet**, int) {
    if (allyTeam == nullptr || allyTeamSize <= 0 || myIndex < 0 || myIndex >= allyTeamSize) {
        return;
    }

    for (int i = myIndex - 1; i >= 0; i--) {
        if (allyTeam[i] != nullptr && allyTeam[i]->isAlive()) {
            allyTeam[i]->buffStats(getLevel(), 2 * getLevel());
            return;
        }
    }
}

// What it does: Creates a Skunk with its starting stats.
// What the inputs are: None.
// What the outputs are: Initializes the Skunk object.
Skunk::Skunk() : Pet("Skunk", 5, 3) {
}

// What it does: Creates a dynamic copy of this Skunk.
// What the inputs are: None.
// What the outputs are: Returns a new Skunk pointer with copied stats and level.
Pet* Skunk::clone() const {
    Pet* copiedPet = new Skunk();
    copiedPet->setHp(getHp());
    copiedPet->setAttack(getAttack());
    copiedPet->setExperience(getExperience());
    return copiedPet;
}

// What it does: Runs the Skunk battle start skill.
// What the inputs are: The enemy team pointer array and the team size.
// What the outputs are: Reduces the highest health living enemy by one third using integer math.
void Skunk::onBattleStart(Pet** enemyTeam, int teamSize) {
    Pet* target = nullptr;

    if (enemyTeam == nullptr || teamSize <= 0) {
        return;
    }

    for (int i = 0; i < teamSize; i++) {
        if (enemyTeam[i] != nullptr && enemyTeam[i]->isAlive()) {
            if (target == nullptr || enemyTeam[i]->getHp() > target->getHp()) {
                target = enemyTeam[i];
            }
        }
    }

    if (target == nullptr) {
        return;
    }

    int damage = target->getHp() / 3;

    if (getLevel() == 2) {
        damage = (target->getHp() * 2) / 3;
    } else if (getLevel() >= 3) {
        damage = (target->getHp() * 99) / 100;
    }

    target->takeDamage(damage);

}

// What it does: Creates an Elephant with its starting stats.
// What the inputs are: None.
// What the outputs are: Initializes the Elephant object.
Elephant::Elephant() : Pet("Elephant", 7, 3) {
}

// What it does: Creates a dynamic copy of this Elephant.
// What the inputs are: None.
// What the outputs are: Returns a new Elephant pointer with copied stats and level.
Pet* Elephant::clone() const {
    Pet* copiedPet = new Elephant();
    copiedPet->setHp(getHp());
    copiedPet->setAttack(getAttack());
    copiedPet->setExperience(getExperience());
    return copiedPet;
}

// What it does: Runs the Elephant attack skill.
// What the inputs are: The ally team pointer array, own index, and team size.
// What the outputs are: Deals one damage to the nearest living ally behind it.
void Elephant::onAttack(Pet** allyTeam, int myIndex, int teamSize) {
    if (allyTeam == nullptr || teamSize <= 0 || myIndex < 0 || myIndex >= teamSize) {
        return;
    }

    for (int hitCount = 0; hitCount < getLevel(); hitCount++) {
        for (int i = myIndex - 1; i >= 0; i--) {
            if (allyTeam[i] != nullptr && allyTeam[i]->isAlive()) {
                allyTeam[i]->takeDamage(1);
                break;
            }
        }
    }
}

// What it does: Creates a Hippo with its starting stats.
// What the inputs are: None.
// What the outputs are: Initializes the Hippo object and its battle trigger count.
Hippo::Hippo() : Pet("Hippo", 6, 3) {
    triggerCount = 0;
}

// What it does: Creates a dynamic copy of this Hippo.
// What the inputs are: None.
// What the outputs are: Returns a new Hippo pointer with copied stats, level, and trigger count.
Pet* Hippo::clone() const {
    Hippo* copiedPet = new Hippo();
    copiedPet->setHp(getHp());
    copiedPet->setAttack(getAttack());
    copiedPet->setExperience(getExperience());
    copiedPet->triggerCount = triggerCount;
    return copiedPet;
}

// What it does: Runs the Hippo knockout skill.
// What the inputs are: The enemy pet that was knocked out.
// What the outputs are: Gives Hippo plus three attack and plus three health up to three times.
void Hippo::onKnockOut(Pet* knockedOutEnemy) {
    if (knockedOutEnemy == nullptr || knockedOutEnemy->isAlive()) {
        return;
    }

    if (triggerCount >= 3) {
        return;
    }

    buffStats(3 * getLevel(), 3 * getLevel());
    triggerCount++;

}

// What it does: Resets the Hippo battle trigger count.
// What the inputs are: None.
// What the outputs are: Sets the Hippo trigger count to zero.
void Hippo::resetBattleState() {
    triggerCount = 0;
}

// What it does: Creates a Blowfish with its starting stats.
// What the inputs are: None.
// What the outputs are: Initializes the Blowfish object.
Blowfish::Blowfish() : Pet("Blowfish", 6, 3) {
}

// What it does: Creates a dynamic copy of this Blowfish.
// What the inputs are: None.
// What the outputs are: Returns a new Blowfish pointer with copied stats and level.
Pet* Blowfish::clone() const {
    Pet* copiedPet = new Blowfish();
    copiedPet->setHp(getHp());
    copiedPet->setAttack(getAttack());
    copiedPet->setExperience(getExperience());
    return copiedPet;
}

// What it does: Runs the Blowfish hurt skill.
// What the inputs are: The ally team, own index, ally team size, enemy team, and enemy team size.
// What the outputs are: Deals three damage to one random living enemy.
void Blowfish::onHurt(Pet**, int, int, Pet** enemyTeam, int enemyTeamSize) {
    std::vector<Pet*> livingEnemies;

    if (enemyTeam == nullptr || enemyTeamSize <= 0) {
        return;
    }

    for (int i = 0; i < enemyTeamSize; i++) {
        if (enemyTeam[i] != nullptr && enemyTeam[i]->isAlive()) {
            livingEnemies.push_back(enemyTeam[i]);
        }
    }

    if (livingEnemies.size() == 0) {
        return;
    }

    int targetIndex = std::rand() % livingEnemies.size();
    livingEnemies[targetIndex]->takeDamage(3 * getLevel());

}

// What it does: Creates a Monkey with its starting stats.
// What the inputs are: None.
// What the outputs are: Initializes the Monkey object.
Monkey::Monkey() : Pet("Monkey", 2, 1) {
}

// What it does: Creates a dynamic copy of this Monkey.
// What the inputs are: None.
// What the outputs are: Returns a new Monkey pointer with copied stats and level.
Pet* Monkey::clone() const {
    Pet* copiedPet = new Monkey();
    copiedPet->setHp(getHp());
    copiedPet->setAttack(getAttack());
    copiedPet->setExperience(getExperience());
    return copiedPet;
}

// What it does: Runs the Monkey shop end skill.
// What the inputs are: The ally team pointer array and the team size.
// What the outputs are: Gives the front-most living ally plus two attack and plus two health.
void Monkey::onShopEnd(Pet** allyTeam, int teamSize) {
    if (allyTeam == nullptr || teamSize <= 0) {
        return;
    }

    for (int i = teamSize - 1; i >= 0; i--) {
        if (allyTeam[i] != nullptr && allyTeam[i] != this && allyTeam[i]->isAlive()) {
            allyTeam[i]->buffStats(2 * getLevel(), 2 * getLevel());
            return;
        }
    }
}
