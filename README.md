Multi-Threaded Pac-Man Game in C using OpenGL
Overview

This project is a modern, multi-threaded version of the classic Pac-Man game developed in C using OpenGL. It leverages core concepts of operating systems, such as threading and synchronization, to create an engaging and responsive gameplay experience. The game includes a game engine thread, a user interface thread, and individual threads for each ghost controller.
Features

    Game Engine Thread: Manages game flow, input handling, game state updates, and rendering.
    User Interface Thread: Handles UI components, menus, and player input events.
    Ghost Controller Threads: Each ghost is controlled by a separate thread to enable independent behavior.
    Synchronization: Utilizes mutexes and semaphores to manage concurrent access to shared resources.

How to Run

To compile and run the game, follow these steps:
Prerequisites

    Ensure you have gcc, OpenGL, SDL2, SDL2_mixer, and SOIL installed on your system.

Installation

    Install SDL2 and SDL2_mixer:

    sh

    sudo apt-get update
    sudo apt-get install libsdl2-dev
    sudo apt-get install libsdl2-mixer-dev

    Install SOIL:

    Download and install the SOIL library from GitHub: SOIL GitHub Repository.

Compilation

Use the following command to compile the game:

sh

gcc -o pacman_game game.c main.c texture.c menu.c -lGL -lGLU -lglut -lm -lSOIL -lpthread -lm `sdl2-config --cflags --libs` -lSDL2_mixer

Running the Game

After compiling, you can run the game with:

sh

./pacman_game

Project Structure

    game.c: Contains the game engine logic.
    main.c: Manages the main game loop and threads.
    texture.c: Handles texture loading and rendering.
    menu.c: Manages the user interface and menus.
    menu.h: Header file for menu-related functions.

Known Issues

    The game may contain bugs.
    Some features, such as ghost tracking, are not fully implemented but have function definitions available.

Future Work

    Enhancing ghost AI for more challenging gameplay.
    Adding more levels and power-ups.
    Implementing multiplayer support.

Contribution

Contributions are welcome! Please fork the repository and submit a pull request for any improvements or bug fixes.
License

Acknowledgements

    OpenGL for providing the graphics API.
    SDL2 and SDL2_mixer for handling input and audio.
    SOIL for texture loading capabilities.
