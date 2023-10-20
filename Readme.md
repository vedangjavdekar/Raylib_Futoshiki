# About

This project is built with an amazing game framework called
[raylib](https://github.com/raysan5/raylib). I have been meaning to recreate
this game in the past, but I got distracted and created a keyboard based Menu
Navigation demo instead(this side quest was done using SFML!).

This time I made sure that I didn't get distracted and finished the actual game.
And all this was build in less than 10 hours.

Also, I will be adding a level editor shortly, so that you guys can share your levels with your friends.

# Building

- This project uses premake5(included with the project) as its build system.
- Just run the `GenerateProjectFiles.bat` and it will create a visual studio solution.
- Then the project can be built from within the visual studio.

# Features

Currently the engine is specifically build for Futoshiki, a number based puzzle
game where player has to fit in all numbers in a way that it satisfies all the
constraints on the cells.

Here are some features:

- Event Handling(very basic message passing system, but works for this purpose).
- Key bindings through action maps
- Mapping Contexts (currently in use, but more useful for level editor)
- Window Resizing support for the game board
- Hot reloading of the level
- Currently has only one level
- A level parser that uses a custom format

Game the game is feature complete:

- User can register the final number in a cell
- User can register the candidate numbers in a cell
- Game checks for all constraints and highlights problematic rows/columns
- User controls are disabled when the user completes the puzzle.

Stay tuned. Have fun :)
