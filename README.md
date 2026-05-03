# CLI AUTO PETS

A text-based mini auto-battler game inspired by Super Auto Pets.

## Team Members
- Song Hengrui (UID: 3033113498)
- Zhu Jiean (UID: 3036484733)

## Application Description

### Game Name
CLI AUTO PETS

### What Is This Game?
CLI AUTO PETS is a text-based mini auto-battler where small pets can make big plays.
Build your team in the shop, arrange positions, and watch pets fight AI enemies automatically in fast and strategic rounds.

### How To Play
Each turn has two phases:

1. Shop Phase (Commands)
- `r` : Roll shop (`-1` gold)
- `b A 1` : Buy shop pet `A` into team slot `1`; each pet costs 3 gold
- `s 1` : Sell pet in slot `1` for 1 gold
- `m 1 4` : Move/swap slot `1` and slot `4`
- `c 1 4` : Combine same-type pet in slot `1` into slot `4`
- `view ant` : Check that pet's (`ant`) detailed information
- `e` : End shop phase and start battle

2. Battle Phase
- Your team fights an AI enemy team automatically.
- Pets attack from the front based on team order.
- Skills trigger at different timings (such as battle start, hurt, faint, and knockout).
- After battle, you get the round result (win/loss/draw), then continue to the next turn.

### Goal
Win a total of **10 rounds** before your HP reaches 0.

### Animal Introduction

Pets appear in the shop based on round progression:
- Round 1-2: Tier 1 pets only
- Round 3-4: Tier 1 and Tier 2 pets
- Round 5+: Tier 1, Tier 2, and Tier 3 pets

| Pet | Tier | Initial Stats | Lv1 Skill | Lv2 Skill | Lv3 Skill |
|---|---|---|---|---|---|
| Swan | Tier 1 | 1 ATK / 2 HP | At the start of each shop phase, gain +1 gold. | At the start of each shop phase, gain +2 gold. | At the start of each shop phase, gain +3 gold. |
| Ant | Tier 1 | 2 ATK / 2 HP | On faint, give a random living ally +1 ATK and +1 HP. | On faint, give a random living ally +2 ATK and +2 HP. | On faint, give a random living ally +3 ATK and +3 HP. |
| Mosquito | Tier 1 | 2 ATK / 2 HP | At battle start, deal 1 damage to 1 random living enemy. | At battle start, deal 1 damage to 2 random living enemies. | At battle start, deal 1 damage to 3 random living enemies. |
| Camel | Tier 2 | 3 ATK / 3 HP | On hurt, nearest living ally behind gets +1 ATK and +2 HP. | On hurt, nearest living ally behind gets +2 ATK and +4 HP. | On hurt, nearest living ally behind gets +3 ATK and +6 HP. |
| Skunk | Tier 2 | 3 ATK / 5 HP | At battle start, reduce highest-HP enemy by 33% HP. | At battle start, reduce highest-HP enemy by 66% HP. | At battle start, reduce highest-HP enemy by 99% HP. |
| Elephant | Tier 2 | 3 ATK / 7 HP | After attacking, deal 1 damage to nearest living ally behind once. | After attacking, deal 1 damage to nearest living ally behind twice. | After attacking, deal 1 damage to nearest living ally behind three times. |
| Hippo | Tier 3 | 3 ATK / 6 HP | On knockout, gain +3 ATK and +3 HP (max 3 triggers per battle). | On knockout, gain +6 ATK and +6 HP (max 3 triggers per battle). | On knockout, gain +9 ATK and +9 HP (max 3 triggers per battle). |
| Blowfish | Tier 3 | 3 ATK / 6 HP | On hurt, deal 3 damage to 1 random living enemy. | On hurt, deal 6 damage to 1 random living enemy. | On hurt, deal 9 damage to 1 random living enemy. |
| Monkey | Tier 3 | 1 ATK / 2 HP | At end of shop phase, front-most living ally gets +2 ATK and +2 HP. | At end of shop phase, front-most living ally gets +4 ATK and +4 HP. | At end of shop phase, front-most living ally gets +6 ATK and +6 HP. |

## Features Implemented

This section explains implemented features and how each required coding element supports the game.

### 1. Generation of random events
Randomness is a core part of gameplay and replayability in CLI AUTO PETS.

Implemented random-event features include:
- Random shop generation:
  - At the start of a turn (and after rolling), shop pets are generated randomly.
  - Shop pool is unlocked by round progression:
    - Round 1-2: Tier 1 only
    - Round 3-4: Tier 1 + Tier 2
    - Round 5+: Tier 1 + Tier 2 + Tier 3
- Random enemy team generation:
  - Enemy teams are generated randomly each battle, including pet types and stat scaling by turn.
- Random skill targeting:
  - Skills such as Ant, Mosquito, and Blowfish use random valid targets under battle conditions.

### 2. Data structures for storing data
(To be completed)

### 3. Dynamic memory management
(To be completed)

### 4. File input/output
(To be completed; save/load is currently in progress)

### 5. Program codes in multiple files
(To be completed)

### 6. Multiple Difficulty Levels
(To be completed)

## Non-standard Libraries

None.  
This project only uses standard C++ libraries available on the academy Linux server.

## Compilation and Execution Instructions
