Minima Text Editor

Two modes: Command and edit. User Esc to toggle between them

Command Mode:
A command has the format [quantity][unit][action]
To delete the next 3 words, do 3wd
To delete the previous line, do -1rd

    Quantities:
    All integers, positive and negative

    Units:
    c: character
    w: word
    r: row
    l: line
    p: paragraph

    Actions:
    d: delete
    g: go
    q: quit (no arguments)

    You do not always need to specify [quantity] and [unit]
    [quantity] defaults to 1, and [unit] defaults to word


Edit Mode:
Type to insert text


Navigation:

In command mode, use I,J,K,L to move cursor in single character/line increments,
and use U,O to move in Word increments, and H,N to move to start and end of line.

You can do the same in edit mode with Ctrl held down.

Also on supported terminals, you can scroll with the mouse wheel.
