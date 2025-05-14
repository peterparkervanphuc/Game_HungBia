#include "game.h"

void handleInput(SDL_Event& event, GameState& state, SDL_Rect& infoRect, const SDL_Rect& backRect, const SDL_Rect& playPauseRect, const SDL_Rect& playButtonRect, const SDL_Rect& homeButtonRect, GameResources& resources) {
    static bool moveLeft = false;
    static bool moveRight = false;

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x = event.button.x, y = event.button.y;
        if (!state.showInfoPopup && x >= infoRect.x && x <= infoRect.x + infoRect.w &&
            y >= infoRect.y && y <= infoRect.y + infoRect.h) {
            state.showInfoPopup = true;
        } else if (state.showInfoPopup) {
            state.showInfoPopup = false;
        } else if (!state.gameStarted && !state.onFrameScreen && x >= playButtonRect.x && x <= playButtonRect.x + playButtonRect.w &&
                   y >= playButtonRect.y && y <= playButtonRect.y + playButtonRect.h) {
            state.onFrameScreen = true;
            if (Mix_PlayingMusic()) {
                Mix_HaltMusic();
            }
        } else if (state.onFrameScreen && !state.gameStarted) {
            SDL_Rect girl1Rect = {SCREEN_WIDTH / 2 - 310, SCREEN_HEIGHT / 2 - 150, 280, 375};
            SDL_Rect girl2Rect = {SCREEN_WIDTH / 2 - 85, SCREEN_HEIGHT / 2 - 160, 170, 320};
            SDL_Rect girl3Rect = {SCREEN_WIDTH / 2 + 150, SCREEN_HEIGHT / 2 - 150, 150, 300};

            if (x >= girl1Rect.x && x <= girl1Rect.x + girl1Rect.w &&
                y >= girl1Rect.y && y <= girl1Rect.y + girl1Rect.h) {
                resources.selectedGirlTexture = resources.girl1Texture;
                state.gameStarted = true;
            } else if (x >= girl2Rect.x && x <= girl2Rect.x + girl2Rect.w &&
                       y >= girl2Rect.y && y <= girl2Rect.y + girl2Rect.h) {
                resources.selectedGirlTexture = resources.girl2Texture;
                state.gameStarted = true;
            } else if (x >= girl3Rect.x && x <= girl3Rect.x + girl3Rect.w &&
                       y >= girl3Rect.y && y <= girl3Rect.y + girl3Rect.h) {
                resources.selectedGirlTexture = resources.girl3Texture;
                state.gameStarted = true;
            }
            if (state.gameStarted) {
                state.beers.push_back({rand() % (GAME_AREA_WIDTH - BEER_WIDTH), 0, state.beer_speed, SDL_GetTicks()});
                infoRect.x = GAME_AREA_WIDTH - BUTTON_SIZE - BUTTON_MARGIN;
                if (Mix_PlayingMusic()) {
                    Mix_HaltMusic();
                }
            }
        } else if (state.gameStarted && x >= playPauseRect.x && x <= playPauseRect.x + playPauseRect.w &&
                   y >= playPauseRect.y && y <= playPauseRect.y + playPauseRect.h) {
            if (!state.paused) {
                state.pausedTime = SDL_GetTicks();
                if (state.x2EffectActive) {
                    state.x2PausedTime = state.pausedTime - state.x2EffectStartTime;
                }
                if (state.shieldEffectActive) {
                    state.shieldPausedTime = state.pausedTime - state.shieldEffectStartTime;
                }
                if (state.slowEffectActive) {
                    state.slowPausedTime = state.pausedTime - state.slowEffectStartTime;
                }
            } else {
                if (state.x2EffectActive) {
                    state.x2EffectStartTime = SDL_GetTicks() - state.x2PausedTime;
                }
                if (state.shieldEffectActive) {
                    state.shieldEffectStartTime = SDL_GetTicks() - state.shieldPausedTime;
                }
                if (state.slowEffectActive) {
                    state.slowEffectStartTime = SDL_GetTicks() - state.slowPausedTime;
                }
            }
            state.paused = !state.paused;
        } else if (state.gameStarted && x >= backRect.x && x <= backRect.x + backRect.w &&
                   y >= backRect.y && y <= backRect.y + backRect.h) {
            resetToHomeScreen(state, infoRect);
            playBackgroundMusicIfNotPlaying(resources, state);
        } else if (state.onVipScreen && x >= homeButtonRect.x && x <= homeButtonRect.x + homeButtonRect.w &&
                   y >= homeButtonRect.y && y <= homeButtonRect.y + homeButtonRect.h) {
            resetToHomeScreen(state, infoRect);
            playBackgroundMusicIfNotPlaying(resources, state);
        }
    } else if (!state.paused && event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) moveLeft = true;
        if (event.key.keysym.sym == SDLK_RIGHT) moveRight = true;
        if (event.key.keysym.sym == SDLK_SPACE) {
            if (state.speedBoostUnlocked) {
                state.speedBoostActive = true;
            }
        }
    } else if (!state.paused && event.type == SDL_KEYUP) {
        if (event.key.keysym.sym == SDLK_LEFT) moveLeft = false;
        if (event.key.keysym.sym == SDLK_RIGHT) moveRight = false;
        if (event.key.keysym.sym == SDLK_SPACE) {
            state.speedBoostActive = false;
        }
    }

    // Update player movement
    int currentSpeed = state.speedBoostActive && state.speedBoostUnlocked ? PLAYER_SPEED + 5 : PLAYER_SPEED;
    if (moveLeft && state.playerX > 0) state.playerX -= currentSpeed;
    if (moveRight && state.playerX < GAME_AREA_WIDTH - PLAYER_WIDTH) state.playerX += currentSpeed;
}