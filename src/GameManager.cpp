#include "../include/GameManager.h"
#include "../include/Constants.h"
#include "SDL_mixer.h"

void GameManager::resetToHomeScreen(bool& gameStarted, bool& onFrameScreen, bool& onVipScreen, bool& paused, 
                                 std::vector<Beer>& beers, int& score, int& beer_speed, int& playerX, 
                                 SDL_Rect& infoRect, bool& speedBoostUnlocked, bool& showSpeedBoostMessage) {
    gameStarted = false;
    onFrameScreen = false;
    onVipScreen = false;
    paused = false;
    beers.clear();
    score = 0;
    beer_speed = INITIAL_BEER_SPEED;
    playerX = GAME_AREA_WIDTH / 2 - PLAYER_WIDTH / 2;
    infoRect.x = SCREEN_WIDTH - BUTTON_SIZE - BUTTON_MARGIN;
    speedBoostUnlocked = false;
    showSpeedBoostMessage = false;
    extern X2Icon x2Icon;
    x2Icon.active = false;
    extern bool x2EffectActive;
    x2EffectActive = false;
}

void GameManager::playBackgroundMusicIfNotPlaying(Mix_Music* backgroundMusic, bool onFrameScreen, 
                                                bool onVipScreen, bool gameStarted) {
    if (!Mix_PlayingMusic() && !onFrameScreen && !onVipScreen && !gameStarted) {
        Mix_PlayMusic(backgroundMusic, -1);
    }
}

void GameManager::updateBeerCount(std::vector<Beer>& beers, int score, int beer_speed) {
    if (score >= 18 && score < 36 && beers.size() < 2) {
        beers.push_back({rand() % (GAME_AREA_WIDTH - BEER_WIDTH), 0, beer_speed, SDL_GetTicks()});
    } 
    else if (score >= 36 && score < 54 && beers.size() < 3) {
        while (beers.size() < 3) {
            beers.push_back({rand() % (GAME_AREA_WIDTH - BEER_WIDTH), 0, beer_speed, SDL_GetTicks()});
        }
    }
    else if (score >= 54 && beers.size() < 4) {
        while (beers.size() < 4) {
            beers.push_back({rand() % (GAME_AREA_WIDTH - BEER_WIDTH), 0, beer_speed, SDL_GetTicks()});
        }
    }
}
