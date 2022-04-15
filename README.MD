# Minima Text Editor

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
- g: go
- q: quit and save (no arguments)

You do not always need to specify both `quantity` and `unit`

`quantity` defaults to 1, and `unit` defaults to word

(Bonus: the negative sign evaluates to -1)

## Edit Mode:
Type to insert text


## Global Commands:
Global commands are single key shortcuts that can be used in edit mode 
by holding ctrl, and in command mode.

Use I,J,K,L to move cursor in single character/line increments.
Use U,O to move in Word increments, and H,N to move to start and end of line.

Use Z to undo, and Y to redo (this was very hard to code in a clean way)

Also on supported terminals, you can scroll with the mouse wheel.
