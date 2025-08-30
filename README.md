<img src="docs/images/logo.png" align="left" width="350px"/>

Back when tigers used to smoke, the Great Halls of Diabaig were home to a thriving civilisation. However, few records survived the ensuing fires when a dragon moved in. Descend to the depths of Diabaig, carefully navigate the twisting corridors and dangerous creatures and return with a tooth from the dragon's jaws.

## Overview 

Diabaig is a traditional turn-based ASCII roguelike where you are never more than a few mistakes away from death. Choose a starting class, find powerful items, and learn new spells and techniques to face increasingly dangerous adversity. You will discover many floors with rooms filled with unique creatures, magic scrolls, unknown potions and mighty weapons. Use everything at your disposal to survive as long as possible and defeat the dragon, but remember to keep enough spare for the return journey.

## Basic Controls

* Move and attack with arrows/NUMPAD/hjkl (diagonals "yubn")
* (e) Eat food
* (r) Read scroll
* (d) Drink potion
* (t) Throw item
* (f) Fire bow
* (w) Weild weapon/armour/ring
* (a) Apply a potion to your weapon
* (s) Search your environment
* (1,2,3) Cast spell
* (i) Show inventory
* (>,<) Descend or Ascend stairway

For a full explanation of all commands, tips and stats, press (?)

## Installation

Disclaimer: The compilation of this code is not well streamlined. I have built it to work for my needs and not much more. 

The basic compilation of this code takes the following form:

```bash
$~ make
$~ make install
```

### Other compilation options

* Alter installation path of the final executable with `INSTALL_PATH=/path/to/folder`
* The executable can be linked statically on linux and macos with `make static`.
* If cross compiling, add `PLATFORM=<platform>` to each `make` command, where <platform> is chosen from `linux`,`macos`,`windows`. Alternatively add `TOOLCHAIN=/path/to/toolchain` to use a custom toolchain.
* A shareable zip 'package' can be put together with `make pkg`

