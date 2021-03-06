# My Game Engine
My Game Engine is a 2d game engine that enables developers to build a game using interfaces provided by the game engine.

## v1.0 features
- Configure characters with JSON files.
- Various controllers (AI and player input) and components.
- Simulate physics.
- Box collision.
- Allocate, free and coalesce memory.
- Smart pointers and weak pointers.

## Example Game
The example game is a bullethell-ish game. W,A,S,D to move. The longer you hold W,A,S,D, the faster you are.
Q to quit. Player can rebound off the wall to escape from enemies.

There are four rounds and three kinds of enemies in total.
More and more enemies are created and it's getting harder.
Turtle moves from left to right.
Ghost spawns at both top and bottom and move to the other end.
Bomb spawns at a random place on the map and can ricochet around the map.

When all four rounds are cleared, player wins. Once player collides with an enemy, game is over.
