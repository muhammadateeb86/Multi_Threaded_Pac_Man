#ifndef STATE_H
#define STATE_H

enum AppState {
    MENU,
    GAME,
    PAUSE
};

extern enum AppState currentState;

#endif

