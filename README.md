# Terbombo

  **Terbombo** is a terminal based minesweeper clone for Linux. It supports every classic minesweeper feature such us toggling flags, revaeling tiles around flagged tiles, a scoreboard, a timer and much more! This was a personal project i created in the first year of university!

## Compile Project
  To compile the project in release mode just run **_make_** in the linux terminal. To play the game just navigate to the **_bin_** folder that was created and run the game. 

```console
foo@bar: make
foo@bar: cd bin
foo@bar: ./bosnia_sim
```

To compile the project in debug mode, run the **_make_** command with the **DEBUG** flag enbaled, then navigate to the **_bin_** folder and run the executable as such:

```console
foo@bar: make DEBUG=1
foo@bar: cd bin
foo@bar: ./bosnia_sim
```

### DEPENDENCIES:
  linux, ncurses

