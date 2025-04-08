![logo](docs/images/logo.png)

Try not to get lost in the Great Winding Halls of Diabaig

SYNOPSIS
========

**diabaig** \[ **-htvw** \] \[ **-L** *loadfile* \] \[ **-s** *seed* \]
\[ **-S** *scorefile* \]

DESCRIPTION
===========

Diabaig is a traditional roguelike heavily inspired by the original
rogue. The game places you in the role of an adventurer tasked with the
retrieving a tooth from the jaws of the dragon inhabiting the halls of
Diabaig. No reason was given for the purpose of this task, is it a
trophy, is it valuable, does it have medical properties? You are not
told.

The graphics are strictly ncurses based and follow a similar scheme to
most traditional roguelikes. Below is a typical game scene in which the
player **\'@\'** is in one room with a goblin **\'g\'** and a magical
scroll **\'?\'**. The room is connected to another through a door
**\'+\'** and a corridor **\'\#\'**, this room has a stairway **\'\>\'**
down to the next layer of the dungeon. All creatures are denoted with a
letter and items are symbols: **\'!\'** potions, **\'/\'** weapons,
**\'\]\'** armour, **\'\*\'** gold and **\':\'** food.

                           #     ------- 
             ---------     #     |.....|
             |......g|     #     |.>...|
             |..@....+###########+.....|
             |....?..|           |.....|
             ---------           -------

Move through the dungeon layers with the arrow keys, numpad or \'hjkl\'.
Diagonal movement is allowed and sometimes required, this can be done
with either the numpad or \'yubn\'. You descend a staircase with \'\>\'
and climb with \'\<\'. Attack creatures that are aggresive towards you
by running into them. To do a ranged attack, you must equip a bow, and
half at least one arrow in your inventory. Fire the biw with \'f\' and
pick the direction. Additionally, weapons can be thrown at a target,
some, like spears and daggers do much more damage when this is done.
Daggers and arrows are consumable and you dont get thwm back if they hit
the target.

Potions **\'!\'** are key to success in Diabaig. They can be drank
**\'d\'**, thrown **\'t\'** or applied **\'a\'** to a weapon and its
effect passed to the target until the coating wears off. Potions are
unidentified at first but performing any one of these actions will
reveal the effect of the potion.

Scrolls **\'?\'** can be read **\'r\'** once, and some magic effect will
be applied. THe scroll disappears after it is read. The effect is
unidentified at first but can be identified by reading a scroll of
identification.

Food **:** is required to sustain your character. You will likely find
only meager pickings in the dungeon. Beware eating random mushrooms,
they may cause adverse effects.

Gold **\*** is the scoring mechanism in Diabaig. Collect as much as you
can to beat the highscores. Spells **1,2,3 are learned from magic
scrolls. You can know three at any given time, but new ones can be
learnt by forgetting one other. They are cast with 1,2,3** respectively.
The strength and/or range of most spells increases with every use, so
your character will become more proficient at using them in time. Some
spells can be mastered after enough uses and they take on a much more
powerdul form.

OPTIONS
=======

**-h**

:   Display basic usage information for diabaig. Produce more detailed
    information by including **-vh** in arguments.

**-L *loadfile.save***

:   Load a previous save file. Caution, this file will be deleted if it
    initialises the game correctly. If option not given, the program
    will look for \"*diabaig.save* when continuing progress.

**-S *scorefile.scr***

:   Use a specific highscore file.

**-s *seed***

:   Set the random number generator *seed*. This needs to be a integar.
    If the same seed is given, the dungeon will generate the same way
    again.

**-t**

:   Run diabaig in the test arena, where the map generation is turned
    off.

**-v**

:   Produce verbose debug information during the runtimme of the
    program. This is mostly for development.

**-w**

:   Launch the game in wizard mode. This offers the user the ability to
    spawn creatures, items and move around the dungeon unimpeded. Your
    score will not be added to the highscores list. This mode is
    intended either for development, testing or experiencing deeper
    layers of the dungeon.

AUTHOR
======

Written by Conor Nally.

BUGS
====

Probably a bunch.

Version: 0.13.0
