# Screenshots

https://github.com/vedangjavdekar/Raylib_Futoshiki/assets/30022526/a5234c83-3571-43b2-b3cb-2b4dcd2df238

![Screenshot1](https://github.com/vedangjavdekar/Raylib_Futoshiki/blob/master/github/Screenshot1.png)
![Screenshot2](https://github.com/vedangjavdekar/Raylib_Futoshiki/blob/master/github/Screenshot2.png)

# About

The game is called **Futoshiki**.

Here's what [wikipedia](https://en.wikipedia.org/wiki/Futoshiki) says about it:

> Futoshiki (不等式, futōshiki), or More or Less, is a logic puzzle game from
> Japan. Its name means "inequality". It is also spelled hutosiki (using
> Kunrei-shiki romanization). Futoshiki was developed by Tamaki Seto in 2001.
>
> The puzzle is played on a square grid. The objective is to place the numbers
> such that each row and column contains only one of each digit. Some digits may
> be given at the start. Inequality constraints are initially specified between
> some of the squares, such that one must be higher or lower than its neighbor.
> These constraints must be honored in order to complete the puzzle.

The project is built with an amazing game framework called
[raylib](https://github.com/raysan5/raylib).

The game is complete with an in built level editor. The game engine is written in C++ and has some features I am proud of!

# Keybindings

### Global Bindings:

|        Key        |         Action         |
| :---------------: | :--------------------: |
| <kbd>Enter</kbd>  |         Commit         |
| <kbd>Escape</kbd> |         Cancel         |
|   <kbd>L</kbd>    | Open Level Select Menu |

### Game Mode:

|                  Key                   |             Action              |
| :------------------------------------: | :-----------------------------: |
|              <kbd>P</kbd>              |       Switch to Play Mode       |
|  <kbd>Up Arrow</kbd> or <kbd>W</kbd>   |            SELECT UP            |
| <kbd>Down Arrow</kbd> or <kbd>S</kbd>  |           SELECT DOWN           |
| <kbd>Left Arrow</kbd> or <kbd>A</kbd>  |           SELECT LEFT           |
| <kbd>Right Arrow</kbd> or <kbd>D</kbd> |          SELECT RIGHT           |
|  <kbd>Numpad 1</kbd> or <kbd>1</kbd>   |     Toggle Cell Value to 1      |
|  <kbd>Numpad 2</kbd> or <kbd>2</kbd>   |     Toggle Cell Value to 2      |
|  <kbd>Numpad 3</kbd> or <kbd>3</kbd>   |     Toggle Cell Value to 3      |
|  <kbd>Numpad 4</kbd> or <kbd>4</kbd>   |     Toggle Cell Value to 4      |
|  <kbd>Numpad 5</kbd> or <kbd>5</kbd>   |     Toggle Cell Value to 5      |
|  <kbd>Numpad 6</kbd> or <kbd>6</kbd>   |     Toggle Cell Value to 6      |
|  <kbd>Numpad 7</kbd> or <kbd>7</kbd>   |     Toggle Cell Value to 7      |
|  <kbd>Numpad 8</kbd> or <kbd>8</kbd>   |     Toggle Cell Value to 8      |
|  <kbd>Numpad 9</kbd> or <kbd>9</kbd>   |     Toggle Cell Value to 9      |
|       <kbd>Ctrl</kbd> + Numbers        | Toggle _Guess Mode_ cell values |

### Editor Mode:

|                  Key                   |                     Action                     |
| :------------------------------------: | :--------------------------------------------: |
|              <kbd>E</kbd>              |              Switch to Edit Mode               |
|  <kbd>Up Arrow</kbd> or <kbd>W</kbd>   |                   SELECT UP                    |
| <kbd>Down Arrow</kbd> or <kbd>S</kbd>  |                  SELECT DOWN                   |
| <kbd>Left Arrow</kbd> or <kbd>A</kbd>  |                  SELECT LEFT                   |
| <kbd>Right Arrow</kbd> or <kbd>D</kbd> |                  SELECT RIGHT                  |
|  <kbd>Numpad 1</kbd> or <kbd>1</kbd>   |             Toggle Lock Value to 1             |
|  <kbd>Numpad 2</kbd> or <kbd>2</kbd>   |             Toggle Lock Value to 2             |
|  <kbd>Numpad 3</kbd> or <kbd>3</kbd>   |             Toggle Lock Value to 3             |
|  <kbd>Numpad 4</kbd> or <kbd>4</kbd>   |             Toggle Lock Value to 4             |
|  <kbd>Numpad 5</kbd> or <kbd>5</kbd>   |             Toggle Lock Value to 5             |
|  <kbd>Numpad 6</kbd> or <kbd>6</kbd>   |             Toggle Lock Value to 6             |
|  <kbd>Numpad 7</kbd> or <kbd>7</kbd>   |             Toggle Lock Value to 7             |
|  <kbd>Numpad 8</kbd> or <kbd>8</kbd>   |             Toggle Lock Value to 8             |
|  <kbd>Numpad 9</kbd> or <kbd>9</kbd>   |             Toggle Lock Value to 9             |
|        <kbd>Ctrl</kbd> + Number        |  Set Board Size to _Number_ $\times$ _Number_  |
|              <kbd>R</kbd>              | Reload the original values from the Level File |
|     <kbd>Ctrl</kbd> + <kbd>R</kbd>     | Create a blank board with current/default size |
|              <kbd>F</kbd>              |               Save as New Level                |
|     <kbd>Ctrl</kbd> + <kbd>F</kbd>     |          Overwrite Current Level File          |
|           <kbd>Escape</kbd>            |              Switch to Play Mode               |

### Level Select:

Only `Navigational` controls and `Commit` and `Cancel` actions work.

# Building

- This project uses premake5(included with the project) as its build system.
- Just run the `GenerateProjectFiles.bat` and it will create a visual studio solution.
- Then the project can be built from within the visual studio.

# Features

The engine is specifically build for Futoshiki
Here are some notable features:

- Event Handling through Event Bus
- Level Selection UI
- Animated Scrollbox
- Notifications capabilities
- In-built Level Editor
- Support for Relative Positioning with respect to screen size for UI Elements.
- Fully Keyboard based navigation as well as playing

The game is feature complete:

- Level Selection Menu
- Each level is editable and overwritable
- Adding new levels is as easy as dropping a `*.data` file in the required
  format in the `data/` directory
- Game comes with 15 levels right now.

# Ideas to explore further

- Using the github repo as a central level database, and fetching all the levels
  at the start of the game.
- Getting the community created content through pull
  requests.
- Making this project cross platform:
  - Scripts for building on Linux and Mac.
  - Removing the `WinMain` from the code.
- Find better solutions Level Selection and how it tries to rename every single
  level if one of the in betweens are missing.
