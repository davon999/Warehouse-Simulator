# Port Warehouse Simulator

A day-by-day simulation of a port warehouse, written in C++. The program decides which cargo to order, where to place it and when to ship it, with one goal: **end the simulation with as much money as possible**.

```
FINAL_BALANCE = starting money + revenue − costs
```

## The problem

The warehouse is an **L × L grid of pallet slots**. Each cargo type has a name, a delivery time, a cost and a selling price. Every day, each slot can be used for two operations:

- **load**: reserve the slot and pay for the cargo. The cargo arrives after its delivery time and can be shipped from then on.
- **ship**: free the slot and collect the selling price.

What makes it interesting are the constraints:

- You can only load cargo you can afford with the money you have right now.
- Cargo must be shipped before the simulation ends; anything still on a pallet on the last day is lost money.
- After shipping from a slot, it needs **1 day** before it can be reused, or **3 days** if you load the same cargo type again.
- **Rotation rules** decide which cargo can follow which on the same slot. Some cargo types can follow anything (marked `N/A`), and an unused slot only accepts those.

## Build and run

```bash
make
./app tests/public/01.in out.txt
python3 judge_public.py tests/public/01.in out.txt
```

The Makefile compiles with `g++ -std=c++17 -O2 -Wall`. The last command validates the output and checks that every action respects the rules.

### Input format

```
L DAYS MONEY N
<name> <delivery_time> <cost> <price>      (N lines, one per cargo type)
ROTATIE R
<cargoA> <cargoB>                          (R lines: A can be loaded after B)
```

A rule like `<cargoA> N/A` means cargo A can follow anything.

### Output format

One line per action, then the final balance:

```
<day> incarca <row>x<col> <cargo>
<day> expediaza <row>x<col>
SUMA_FINALA: <money>
```

## Project structure

| File | Contents |
|---|---|
| `depozit.h` / `depozit.cpp` | `Marfa` (cargo) and `Depozit` (warehouse state) structs, input parsing and the day-by-day simulation loop |
| `Dispecer.h` / `Dispecer.cpp` | `Dispecer` (dispatcher) class: rule checks, the load and ship operations, and the decision logic |
| `main.cpp` | entry point |
| `Makefile` | build |

The warehouse state is stored flat: each slot has an index `p = row * L + col`, with arrays tracking whether it is occupied, which cargo it holds, when it was loaded and what was last shipped from it.

## How it works

### Reading the configuration

`citeste()` reads the grid size, number of days, starting money and the cargo list, then the rotation rules. Cargo names are mapped to indices with a small helper, `numetoindex()`, which returns:

- the cargo's index if the name is known,
- `-1` for an unknown name (the rule is skipped),
- `-2` for `N/A`, so it can't be mistaken for an unknown name.

The rules end up in two lookup tables: `isNA[a]` (cargo `a` can follow anything) and `dupaMarfa[a][b]` (cargo `a` can follow cargo `b`). This makes each rule check O(1) during the simulation.

### The simulation loop

`simuleaza()` runs one day at a time, in two passes over all slots:

1. **Ship** everything that is ready, to collect money first.
2. **Load** each free slot with the best cargo the dispatcher can find, using the money just collected.

### The dispatcher

`Dispecer` holds all the decision logic:

- `verifIncarcare()` checks whether a cargo can be loaded on a slot today: the slot is free, there is enough money, the cargo can be shipped before the end, the cooldown has passed and the rotation rule allows it.
- `verifExpediere()` checks whether a slot holds cargo that is ready to ship.
- `incarca()` and `expediaza()` apply the operations: they update the money and the slot state and write the action to the output.
- `alegereMarfa()` picks which cargo to load. This is where the optimisation lives.

## Optimisation: from 3M to 9M and beyond

### Version 1: first valid cargo (~3M)

The first version loaded the first cargo that passed all the checks. It was correct, but it made no attempt to choose well.

### Version 2: best profit per day (~9M)

Next, every cargo got a score: the profit it brings per day it occupies a slot.

```
score(c) = (price − cost) / (delivery_time + 1)
```

The `+1` accounts for the 1-day cooldown before the slot can be reused. Picking the highest-scoring valid cargo tripled the result to about 9M.

### Version 3: one-step lookahead

The greedy approach has a blind spot: it only looks at the current move. Suppose cargo **A** has the best score (10.33) but the rotation rules allow nothing after it, while cargo **C** scores lower (7.5) but can be followed by **B** (8.6). Greedy picks A and the slot sits idle afterwards; picking C and then B earns more over time.

So the dispatcher now scores **pairs** of consecutive loads on the same slot:

```
pairScore(a, b) = (profit(a) + profit(b)) / (time(a) + time(b) + cooldown + 1)
```

where `cooldown` is 1 day, or 3 days if `a` and `b` are the same cargo. For every cargo `a` that can be loaded now, `alegereMarfa()` tries every `b` the rules allow after it, skips pairs that would not finish before the last day, and keeps the best score. The cargo with the best pair score is loaded. A cargo with no valid follower still competes with its single score, so nothing is ruled out.

The cost is O(N²) per slot per day, where N is the number of cargo types (at most 20). That is negligible, even on a 99 × 99 grid.