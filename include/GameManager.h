#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "SDL.h"
#include "SDL_mixer.h"
#include "Structures.h"
#include "Constants.h"
#include <vector>

class GameManager {
public:
    static void resetToHomeScreen(bool& gameStarted, bool& onFrameScreen, bool& onVipScreen, bool& paused, 
                                std::vector<Beer>& beers, int& score, int& beer_speed, int& playerX, 
                                SDL_Rect& infoRect, bool& speedBoostUnlocked, bool& showSpeedBoostMessage);
    
    static void playBackgroundMusicIfNotPlaying(Mix_Music* backgroundMusic, bool onFrameScreen, 
                                              bool onVipScreen, bool gameStarted);
    
    static void updateBeerCount(std::vector<Beer>& beers, int score, int beer_speed);
};

#endif // GAME_MANAGER_H
