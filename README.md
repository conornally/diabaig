![logo](docs/images/logo.png)

Try not to get lost in the Great Derelict Halls of Diabaig

## INSTALLATION

Disclaimer: I have no idea how to compile on windows, so the windows toolchain is purely for cross-compiling from linux.

Compile source code using:

```bash
$~ make
$~ make install
```

### Other compilation options

This is all still a bit messy and needs cleaning up.

If cross compiling, add `PLATFORM=<platform>` to each `make` command, where <platform> is chosen from `linux`,`macos`,`windows`. Alternatively add `TOOLCHAIN=/path/to/toolchain` to use a custom toolchain.
The executable can be linked statically on linux and macos with `make static`.
Package the executable to a .deb file on linux with `make all/static package`. On windows this will create a .zip folder with all the appropriate .dll files.

## KNOWN BUGS

#### [01] DAEMONS DYING

This has complex behaviour, and has not yet shown to be consistently reproducible.
Something kills the daemons, and might block some more from starting?
This can mean that once a status is applied, it never stops. And regen is killed.

#### [02] NULL tile creature

This is likely completely linked with [01]. It has been shown to happen with O and U and perhaps G. 
Perhaps in spawn_adds, creatures get placed on tiles but not processed properly. Therefore the moat ends up being null. For some reason it links onto the creature adjacent (or just left) of the empty tile.
If left, the null tile blocks movement, as if there is a creature there. But there is no combat shown.  If a potion is thrown at this tile, it might be applied to the player, or it might segfault on a getname strdup.

**POSSIBLE FIX** spawn_adds was not placing adds properly when in a tunnel. This should fix this issue but needs testing.
This had not been seen earlier because the witches and necromancers were never drawn out of their rooms. Whereas U and O could be drawn out

#### [03] LOCAL LIGHTING

Because local lighting order got changed a while ago, creatures are not visible during the update loop if they are in a corridor. Therefore they dont get any combat messages or appear in ?-7 menu. 
Im not completely sure that that is the cause now actually

#### [04] BLINDNESS IN PLAYER

The blindness effect seems to have a couple of unintended effects. Firstly, being hit when blind doesnt generate the hit message. I think because the local area is not visible. I just need to put a better check in place. 

Secondly, I think, although I was blind so i dont know, that the blindness might stop creatures from tracking you? I got the "shaken off trail" message after i left the room, but the creature surely still knew where I was. So it makes me think that the blindness tracking check is maybe checking the wrong thing.

## MINOR ISSUES

These I may resolve, or they may just stay for now.

- firestorm and blizzard can effect creatures through walls

## POLISHING ENHANCEMENTS

- pick direction, for spells and throws etc, you could click the target, that way you wouldnt need to line yourself up?
- do I want to try and fix the corner corridors? This would remove circular pathways from spawning. I quick not about that, do they only ever happen on the right and bottom of rooms? Im not sure Ive ever seen one on the left or top, so it might be just a single number to change. It is likely in the region_genroom function in map.c




            
  -----     
  |...|     
  |...|     
  |..x|     
  -----     
            
            
            

