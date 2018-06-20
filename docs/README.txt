Game Idea
=========

Use the little runner sprite in a platformer. The unique point of
the game is that there's only one control key: probably SPACE. So
probably a crp game compo entry. :)

By default he runs left to right, never stopping. When the user
hits space he changes direction, right to left. He never stops.
Falling from height doesn't hurt him. Only spikes and fire do.

There are trampoline cells in the ground. If the user hits space
on one of those, he jumps forward. If the user hits space while
he's in mid air, he switches direction, back where he came from.

Ther are fire pits in the ground for him to jump over, each
needing a trampoline. Speed up pills help him jump further.
Pills will need to regenerate so the player can have another go.

If he hits a wall, the bounces off and goes back the way he came.
Same for closed doors. Some doors animate up and down.

Some walls are stairways, which he climbs. Some change colour
every couple of seconds - green he goes up, red he goes down.


Design
======

Each screen needs:

A Layout
========

Trigger Points
==============

The idea here is to check on each game loop to see if something
needs to happen. e.g. If the player has pressed space and the player
is on a trampoline, start the jump state.

Maybe a list of structures, each containing a type, a pointer to a
check function, and a pointer to a result function. Call the check
function and if it returns 1 call the result function. Both need
to take a void* for input parameter structure.

Player Structure
================

x
y
speed_delta  Normally +1 or -1. Inc for faster movement.

Player sprite code needs routine to find the contents of the byte
below the player's foot (i.e. y+9). If it's zero he falls. Otherwise
he's standing on something. Find the attribute of the 1 or 2 cells
he's on. One need priority. i.e. jump pad or spike takes precedent
over flat service.