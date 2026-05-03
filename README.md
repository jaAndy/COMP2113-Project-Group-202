# CLI Auto Pets

```text
   ____ _     ___      _         _          ____      _
  / ___| |   |_ _|    / \  _   _| |_ ___   |  _ \ ___| |_ ___
 | |   | |    | |    / _ \| | | | __/ _ \  | |_) / _ \ __/ __|
 | |___| |___ | |   / ___ \ |_| | || (_) | |  __/  __/ |_\__ \
  \____|_____|___| /_/   \_\__,_|\__\___/  |_|   \___|\__|___/
```

A text-based mini auto-battler game inspired by Super Auto Pets.

## Team Members

- Song Hengrui (UID: 3033113498)
- Zhu Jiean (UID: 3036484733)

## Video Demo

[Video Demo](https://drive.google.com/file/d/1YX86DkuffcAT4XvVlBKdxOknsdlIDKzE/view?usp=sharing)

## How to use

1. Go to the project directory in your terminal.
2. Compile the game using Makefile:

   ```bash
   make
   ```

3. Run the executable:

   ```bash
   ./comp2113_game
   ```

## About the game

### Introduction

CLI AUTO PETS is a text-based mini auto-battler where the player chooses Pets with special abilities to fight against AI enemies. Build your team in the shop, arrange positions, and watch pets fight AI enemies automatically in fast and strategic rounds.

It is easy to pick up, but hard to win!

### How To Play

When you start the game, you will be greeted by the **Title Menu**.

#### Title Menu Selection

| Command | Action                       |
| ------- | ---------------------------- |
| `1`     | Start **Normal Mode**        |
| `2`     | Start **Hard Mode**          |
| `load`  | **Load Game** from save file |
| `3`     | **Quit** Game                |

Once a game is started, each turn has two phases:

#### 1. Shop Phase (Commands)

| Command    | Action               | Description                                                    |
| ---------- | -------------------- | -------------------------------------------------------------- |
| `r`        | **Roll shop**        | Refresh the shop pets (`-1` gold).                             |
| `b A 1`    | **Buy pet**          | Buy shop pet `A` into team slot `1` (each pet costs `3` gold). |
| `s 1`      | **Sell pet**         | Sell the pet in slot `1` for gold equal to its level (lv3=3coins). |
| `m 1 4`    | **Move/Swap**        | Move or swap the pet in slot `1` with slot `4`.                |
| `c 1 4`    | **Combine pets**     | Combine a same-type pet in slot `1` into slot `4`.             |
| `view ant` | **View pet info**    | Check detailed information about a specific pet (e.g., `ant`). |
| `saveq`    | **Save & Quit**      | Save the current game progress and quit the application.       |
| `e`        | **Start the battle** | End the shop phase and start the battle.                       |

#### 2. Battle Phase

- Your team fights an AI enemy team **automatically**.
- Pets attack from the front based on their team order.
- Skills trigger at different timings (such as `Battle Start`, `Hurt`, `Faint`, and `Knockout`).
- After the battle, you receive the round result (Win / Loss / Draw) and then continue to the next turn.

### Goal

Win a total of 5 rounds before your HP reaches 0.

### Pet Encyclopedia

Pets appear in the shop based on round progression:

- Round 1-2: Tier 1 only
- Round 3-4: Tier 1 / Tier 2
- Round 5+: Tier 1 / Tier 2 / Tier 3

| Pet      | Tier   | Initial Stats | Lv1 Skill                                                           | Lv2 Skill                                                           | Lv3 Skill                                                                 |
| -------- | ------ | ------------- | ------------------------------------------------------------------- | ------------------------------------------------------------------- | ------------------------------------------------------------------------- |
| Swan     | Tier 1 | 1 ATK / 2 HP  | At the start of each shop phase, gain +1 gold.                      | At the start of each shop phase, gain +2 gold.                      | At the start of each shop phase, gain +3 gold.                            |
| Ant      | Tier 1 | 2 ATK / 2 HP  | On faint, give a random living ally +1 ATK and +1 HP.               | On faint, give a random living ally +2 ATK and +2 HP.               | On faint, give a random living ally +3 ATK and +3 HP.                     |
| Mosquito | Tier 1 | 2 ATK / 2 HP  | At battle start, deal 1 damage to 1 random living enemy.            | At battle start, deal 1 damage to 2 random living enemies.          | At battle start, deal 1 damage to 3 random living enemies.                |
| Camel    | Tier 2 | 3 ATK / 3 HP  | On hurt, nearest living ally behind gets +1 ATK and +2 HP.          | On hurt, nearest living ally behind gets +2 ATK and +4 HP.          | On hurt, nearest living ally behind gets +3 ATK and +6 HP.                |
| Skunk    | Tier 2 | 3 ATK / 5 HP  | At battle start, reduce highest-HP enemy by 33% HP.                 | At battle start, reduce highest-HP enemy by 66% HP.                 | At battle start, reduce highest-HP enemy by 99% HP.                       |
| Elephant | Tier 2 | 3 ATK / 7 HP  | After attacking, deal 1 damage to nearest living ally behind once.  | After attacking, deal 1 damage to nearest living ally behind twice. | After attacking, deal 1 damage to nearest living ally behind three times. |
| Hippo    | Tier 3 | 3 ATK / 6 HP  | On knockout, gain +3 ATK and +3 HP (max 3 triggers per battle).     | On knockout, gain +6 ATK and +6 HP (max 3 triggers per battle).     | On knockout, gain +9 ATK and +9 HP (max 3 triggers per battle).           |
| Blowfish | Tier 3 | 3 ATK / 6 HP  | On hurt, deal 3 damage to 1 random living enemy.                    | On hurt, deal 6 damage to 1 random living enemy.                    | On hurt, deal 9 damage to 1 random living enemy.                          |
| Monkey   | Tier 3 | 1 ATK / 2 HP  | At end of shop phase, front-most living ally gets +2 ATK and +2 HP. | At end of shop phase, front-most living ally gets +4 ATK and +4 HP. | At end of shop phase, front-most living ally gets +6 ATK and +6 HP.       |

## Features Implemented & Coding Requirements

This section explains how each required coding element supports the game.

### 1. Generation of random events

Randomness (`std::srand` and `std::rand`) is a core part of the gameplay in CLI AUTO PETS. We implemented random events in three main features:

#### 1.1 Random Shop Generation

At the start of a turn or when the player uses the `roll` command, the shop pets are randomly generated from a tier pool based on the current round progression:

- **Round 1-2**: Tier 1 pets only.
- **Round 3-4**: Tier 1 and Tier 2 pets.
- **Round 5+**: Tier 1, Tier 2, and Tier 3 pets.

*Code snippet from* *`game.cpp`* *(Tier unlock & Random Selection):*

```cpp
int maxTier = 1;
if (currentTurn >= 3) { maxTier = 2; }
if (currentTurn >= 5) { maxTier = 3; }

// Inside createRandomPetByTier(int maxTier):
int choiceCount = 3;
if (maxTier == 2) { choiceCount = 6; }
else if (maxTier >= 3) { choiceCount = 9; }

int choice = std::rand() % choiceCount;
if (choice == 0) { return new Swan(); }
else if (choice == 1) { return new Ant(); }
// ...
```

#### 1.2 Random Enemy Team Generation

Enemy teams are generated randomly for each battle. The enemy's team size, pet tiers, levels, and bonus stats all scale dynamically based on the current turn and the chosen difficulty (Normal or Hard).

**Normal Mode AI Scaling:**

In Normal Mode, the AI follows this scaling table:

| Turn    | Enemy Count | Pet Tiers | Level Probability | Stat Bonus Range |
| ------- | ----------- | --------- | ----------------- | ---------------- |
| **1**   | 3           | Tier 1    | 100% Lv1          | +0               |
| **2**   | 3           | Tier 1    | 100% Lv1          | +0 \~ +1         |
| **3**   | 4           | Tier 1-2  | 10% Lv2           | +0 \~ +1         |
| **4**   | 4           | Tier 1-2  | 30% Lv2           | +1 \~ +2         |
| **5**   | 5           | Tier 1-3  | 50% Lv2           | +1 \~ +3         |
| **6**   | 5           | Tier 1-3  | 75% Lv2           | +2 \~ +4         |
| **7**   | 5           | Tier 1-3  | 10% Lv3, else Lv2 | +3 \~ +5         |
| **8**   | 5           | Tier 1-3  | 30% Lv3, else Lv2 | +4 \~ +6         |
| **9**   | 5           | Tier 1-3  | 50% Lv3, else Lv2 | +5 \~ +7         |
| **10+** | 5           | Tier 1-3  | 80% Lv3, else Lv2 | +6 \~ +9         |

**Hard Mode AI Enhancement:**

Hard Mode uses the Normal Mode table as a base, but applies two major enhancements:

| Enhancement                  | Description                                                                                                       | Formula / Example                                                 |
| ---------------------------- | ----------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------- |
| **Higher Level Probability** | The chance of facing Lv2 and Lv3 enemies is increased by 10%.                                                     | Lv3 appears at Turn 6. At Turn 10+, Lv3 chance is 90%.            |
| **Stat Inflation**           | Enemy stats are inflated by \~20%. We use integer math to avoid C++ truncation (where `+20%` on `2` becomes `0`). | `stat = stat + stat / 5 + 1`*(e.g., Turn 1: 2/2 pet becomes 3/3)* |

*Code snippet from* *`game.cpp`* *(Random Enemy Base Generation):*

```cpp
void Game::generateEnemyTeam(int turn) {
    int enemyCount = 3;
    int maxTier = 1;
    clearEnemyTeam();

    if (turn <= 2) { enemyCount = 3; maxTier = 1; }
    else if (turn <= 4) { enemyCount = 4; maxTier = 2; }
    else { enemyCount = 5; maxTier = 3; }

    // Right-align enemies in the array
    for (int i = 0; i < enemyCount; i++) {
        int targetIndex = 5 - enemyCount + i;
        enemyTeam[targetIndex] = createRandomPetByTier(maxTier);
        applyEnemyScaling(enemyTeam[targetIndex], turn); // Applies Normal and Hard mode buffs
    }
}
```

#### 1.3 Random Skill Targeting

Certain pets rely on randomness to trigger their skills during battle. We use random number generation (`std::rand() % livingTargets.size()`) to pick valid targets dynamically, ensuring skills never hit already fainted pets.

| Pet          | Skill Timing | Random Target (Lv1)                             |
| ------------ | ------------ | ----------------------------------------------- |
| **Ant**      | On Faint     | Give 1 **random** living ally +1 ATK and +1 HP. |
| **Mosquito** | Battle Start | Deal 1 damage to 1 **random** living enemy.     |
| **Blowfish** | On Hurt      | Deal 3 damage to 1 **random** living enemy.     |

*Code snippet from* *`pet.cpp`* *(Using Blowfish as an example):*

```cpp
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
```

### 2. Data structures for storing data

We use various data structures to effectively track and manage game states during runtime:

- **Arrays**: Pointer arrays are used in the `Player` and `Game` classes to securely store the fixed-size roster of pets on the team and in the shop.

  ```cpp
  // From game.h: Fixed-size arrays for game state
  Pet *shopPets[3];
  Pet *enemyTeam[5];
  ```

- **Vectors (`std::vector`)**: Dynamic arrays are used frequently during battles for resolving skills with varying targets. They filter and collect all currently living enemies, allowing random skills to dynamically pick valid targets.

  ```cpp
  // From pet.cpp (Mosquito skill): Dynamic array to store valid targets
  std::vector<Pet*> livingEnemies;
  livingEnemies.push_back(enemyTeam[i]); 
  ```

- **Strings (`std::string`)**: Used extensively for parsing user commands and storing message logs.

  ```cpp
  // From game.h: Dynamic message log system
  std::vector<std::string> messageLog;
  void addLog(std::string msg);
  ```

### 3. Dynamic memory management

The game uses heap memory allocation (`new` and `delete`) to create, move, and destroy pets dynamically.

- **Dynamic Creation (`new`)**: When a shop generates pets or when a pet is cloned for battle, we allocate memory for specific pet types.

  ```cpp
  return new Ant();
  ```

- **Memory Transfer**: Instead of copying whole objects, we move pointers. For example, when you buy a pet, the pointer simply moves from the shop array to the player's team array.

- **Safe Deallocation (`delete`)**: To prevent memory leaks, we explicitly delete pets when they are sold, combined, or when the game ends.

  ```cpp
  // Example: Selling a pet frees its memory
  gold = gold + team[slotIndex]->getLevel();
  delete team[slotIndex]; 
  team[slotIndex] = nullptr;
  ```

### 4. File input/output

The game features a robust save and load system using standard C++ file streams (`std::ifstream` and `std::ofstream`):

- When the player enters `saveq` during the shop phase, the program creates a `.txt` file to store the current game state and safely quits.

*Example of data stored in* *`cli_auto_pets_save.txt`:*

```text
CLI_AUTO_PETS_SAVE_V1
PHASE SHOP
DIFFICULTY Normal
TURN 5
WINS 3
PLAYER_HP 4
PLAYER_GOLD 10
PLAYER_TEAM_BEGIN
TEAM_SLOT 1 Elephant 5 9 2 0
TEAM_SLOT 2 Camel 4 4 1 1
TEAM_SLOT 3 Elephant 3 7 1 0
TEAM_SLOT 4 EMPTY
TEAM_SLOT 5 Ant 5 5 2 1
PLAYER_TEAM_END
SHOP_BEGIN
SHOP_SLOT A Hippo 3 6 1 0
SHOP_SLOT B Ant 2 2 1 0
SHOP_SLOT C Hippo 3 6 1 0
SHOP_END
MESSAGE_LOG_BEGIN
MESSAGE_COUNT 5
LOG [R5][ATTACK] Elephant -> Elephant, DMG 7.
LOG [R5][ATTACK] Elephant -> Elephant, DMG 5.
LOG [R5][STATE] Elephant HP 1->0, Elephant HP 9->2.
LOG [R5][FAINT] Player Elephant faints.
LOG [R5][SHIFT] Player team shifts right.
MESSAGE_LOG_END
END_SAVE
```

- When the player enters `load` in the Title Menu, the program reads this `.txt` file to reconstruct the exact game state.

**Save/Load Functions Breakdown:**

| Function                              | File Stream          | Description                                                                                                                                                                                                 |
| ------------------------------------- | -------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `saveGameToFile`                      | `std::ofstream`      | Serializes the current game state line-by-line into `cli_auto_pets_save.txt`. It saves integers (turn, wins, HP, gold) and loops through the arrays to save the player's team, shop pets, and message logs. |
| `writePetSaveLine`                    | `std::ofstream`      | Formats a single pet's data. If the slot is empty, it writes `EMPTY`. Otherwise, it saves the pet's specific stats: `[Name] [ATK] [HP] [Level] [EXP]`.                                                      |
| `loadGameFromFile`                    | `std::ifstream`      | Reads the `.txt` file line-by-line. It validates the save version (`CLI_AUTO_PETS_SAVE_V1`) and dynamically reconstructs the exact `Pet` instances (with current stats) based on the text strings.          |
| `readSaveIntLine` / `readPetSaveLine` | `std::istringstream` | Uses string streams to safely parse variables from a loaded text line, avoiding crashes if the save file is corrupted or manually modified.                                                                 |

### 5. Program codes in multiple files

To ensure maintainability, our project separates logic into 4 main modules.

| File Module               | Primary Responsibility                                                                                 |
| ------------------------- | ------------------------------------------------------------------------------------------------------ |
| `main.cpp`                | The entry point of the program. Initializes the random seed and starts the `Game` instance.            |
| `game.cpp` / `game.h`     | The core engine. Manages the game loop, UI rendering, file I/O, and AI enemy generation.               |
| `player.cpp` / `player.h` | Manages the player's state, including gold/HP and team roster (buying, selling, swapping pets).        |
| `pet.cpp` / `pet.h`       | Contains the `Pet` base class and implements all 9 specific animal subclasses and their unique skills. |

This game is highly suitable to be implemented using OOP principles. By utilizing the architecture above, we achieved a highly modular codebase.

**OOP Principles in our Architecture:**

| OOP Concept       | Implementation in Game                                                                                                                                             | File Separation                                                                                       |
| ----------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------ | ----------------------------------------------------------------------------------------------------- |
| **Inheritance**   | Specific pets (e.g., `Swan`, `Ant`) inherit from the base `Pet` class to share common attributes like HP, ATK, and Level.                                          | Base class and subclasses are declared in `pet.h` and implemented in `pet.cpp`.                       |
| **Polymorphism**  | We use `virtual` functions for skills (`onHurt`, `onFaint`). The battle engine can trigger skills using a generic `Pet*` pointer without knowing the exact animal. | The generic battle logic in `game.cpp` calls the specific overridden skills defined in `pet.cpp`.     |
| **Encapsulation** | Player resources (Gold, HP, Team Array) are kept `private`. They can only be modified through controlled public interfaces like `spendGold()` or `takeDamage()`.   | `player.h` exposes only safe public interfaces, hiding the internal memory logic inside `player.cpp`. |

*Example of Inheritance and Polymorphism (`pet.h`):*

```cpp
// Base Class
class Pet {
private:
    std::string name;
    int hp, attack, level, experience;
public:
    virtual ~Pet();
    virtual Pet* clone() const;
    // Polymorphic skill interfaces
    virtual void onHurt(Pet** allyTeam, int myIndex, int allyTeamSize, Pet** enemyTeam, int enemyTeamSize);
    virtual void onFaint(Pet** allyTeam, int teamSize);
};

// Derived Class (Inheritance)
class Blowfish : public Pet {
public:
    Blowfish();
    virtual Pet* clone() const;
    // Overriding the virtual function for specific skill
    virtual void onHurt(Pet** allyTeam, int myIndex, int allyTeamSize, Pet** enemyTeam, int enemyTeamSize);
};
```

### 6. Multiple Difficulty Levels

The game features two distinct difficulty modes, which can be selected from the Title Menu when starting a new game.

As detailed in [Section 1.2 (Random Enemy Team Generation)](#12-random-enemy-team-generation), the core difference lies in how the AI generates enemy teams to challenge the player's strategy:

**Normal Mode**: This is the standard game mode. Enemy strength increases slowly, perfect for beginners to learn how to play and combine different pets.

**Hard Mode**: This mode is much more challenging for experienced players. Enemies will have higher levels (Lv2/Lv3) much earlier in the game, and all enemy stats are increased by \~20%.

*The player's HP, Gold and the battle engine mechanics remain identical across both modes. Hard Mode's difficulty only comes from fighting stronger AI opponents.*

## Non-standard Libraries

None.

This project only uses standard C++ libraries available on the academy Linux server.
