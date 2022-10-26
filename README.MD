# Minima Text Editor

I made Minima because I couldn't get used to Vim kebindings (even after
customizing them), and I needed an ssh-able text editor for
homework. Minima is designed to be efficient, portable,
and modular enough for me to edit the code directly in lieu of configs.

Requires ncurses/ncursesw, which can be found at your local 
package manager

Build as CMake project:
```
cd Minima
mkdir build && cd build
cmake ..
make
```

Run as `Minima [filename]`

# Usage
Two modes: Command and edit. Use Esc to toggle between them

## Command Mode:
A command has the format [quantity][unit][action]
To delete the next 3 words, do `3wd`
To delete the previous row, do `-1rd`

## Quantities:
All integers, positive and negative

### Units:
- c: character
- w: word
- r: row
- l: line
- p: paragraph

#### Actions:
- d: delete
- g: goto
- q: quit and save
- a: perform last command again
- f: find 
- s: select range

You do not always need to specify both `quantity` and `unit`

`quantity` defaults to 1, and `unit` defaults to word

The negative sign evaluates to -1. `-pg` brings the cursor to previous paragraph
You can also type `b` to flip the direction

To find a string, begin with `'` and type the string you want to find. 
Use `\` to type space and newline literals. Press `a` to find the same
string again.

Select can also be used as `ctrl + s` in edit mode, but in command
mode, it also takes parameters.

## Edit Mode:
Type to insert text


## Global Commands:
Global commands are single key shortcuts that can be used in edit mode
(by holding ctrl) and in command mode.

Use `I`,`J`,`K`,`L` to move cursor in single character/line increments.
Use `U`,`O` to move in word increments, and `H`,`N` to move to start and end of line.

Use `Z` to undo, and `Y` to redo (this was very hard to code in a clean way)

Tap `S` to toggle selection, can also hold `<shift>`

Also on supported terminals, you can scroll with the mouse wheel.
