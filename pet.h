#ifndef PET_H
#define PET_H

#include <string>

class Player;

class Pet {
private:
    std::string name;
    int hp;
    int attack;
    int level;
    int experience;

public:
    Pet(const std::string& petName, int petHp, int petAttack);
    virtual ~Pet();

    std::string getName() const;
    int getHp() const;
    int getAttack() const;
    int getLevel() const;
    int getExperience() const;
    bool isAlive() const;

    void setName(const std::string& petName);
    void setHp(int petHp);
    void setAttack(int petAttack);
    void setLevel(int petLevel);
    void setExperience(int petExperience);
    void takeDamage(int damage);
    void buffStats(int attackBonus, int hpBonus);
    void increaseLevel();
    void addExperience(int amount);
    int getTotalCopies() const;
    void setFromTotalCopies(int totalCopies);

    virtual Pet* clone() const;
    virtual void triggerSkill();
    virtual void onShopStart(Player* owner);
    virtual void onFaint(Pet** allyTeam, int teamSize);
    virtual void onBattleStart(Pet** enemyTeam, int teamSize);
    virtual void onHurt(Pet** allyTeam, int myIndex, int allyTeamSize, Pet** enemyTeam, int enemyTeamSize);
    virtual void onAttack(Pet** allyTeam, int myIndex, int teamSize);
    virtual void onKnockOut(Pet* knockedOutEnemy);
    virtual void resetBattleState();
    virtual void onShopEnd(Pet** allyTeam, int teamSize);
};

class Swan : public Pet {
public:
    Swan();

    virtual Pet* clone() const;
    virtual void onShopStart(Player* owner);
};

class Ant : public Pet {
public:
    Ant();

    virtual Pet* clone() const;
    virtual void onFaint(Pet** allyTeam, int teamSize);
};

class Mosquito : public Pet {
public:
    Mosquito();

    virtual Pet* clone() const;
    virtual void onBattleStart(Pet** enemyTeam, int teamSize);
};

class Camel : public Pet {
public:
    Camel();

    virtual Pet* clone() const;
    virtual void onHurt(Pet** allyTeam, int myIndex, int allyTeamSize, Pet** enemyTeam, int enemyTeamSize);
};

class Skunk : public Pet {
public:
    Skunk();

    virtual Pet* clone() const;
    virtual void onBattleStart(Pet** enemyTeam, int teamSize);
};

class Elephant : public Pet {
public:
    Elephant();

    virtual Pet* clone() const;
    virtual void onAttack(Pet** allyTeam, int myIndex, int teamSize);
};

class Hippo : public Pet {
private:
    int triggerCount;

public:
    Hippo();

    virtual Pet* clone() const;
    virtual void onKnockOut(Pet* knockedOutEnemy);
    virtual void resetBattleState();
};

class Blowfish : public Pet {
public:
    Blowfish();

    virtual Pet* clone() const;
    virtual void onHurt(Pet** allyTeam, int myIndex, int allyTeamSize, Pet** enemyTeam, int enemyTeamSize);
};

class Monkey : public Pet {
public:
    Monkey();

    virtual Pet* clone() const;
    virtual void onShopEnd(Pet** allyTeam, int teamSize);
};

#endif
