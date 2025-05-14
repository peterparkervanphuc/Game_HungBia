#include "game.h"
#include <iostream>

void resetToHomeScreen(GameState& state, SDL_Rect& infoRect) {
    state.gameStarted = false;
    state.onFrameScreen = false;
    state.onVipScreen = false;
    state.paused = false;
    state.beers.clear();
    state.score = 0;
    state.beer_speed = INITIAL_BEER_SPEED;
    state.playerX = GAME_AREA_WIDTH / 2 - PLAYER_WIDTH / 2;
    infoRect.x = SCREEN_WIDTH - BUTTON_SIZE - BUTTON_MARGIN;
    state.max_beers = 1;
    state.speedBoostUnlocked = false;
    state.showSpeedBoostMessage = false;
    state.x2Icon.active = false;
    state.x2EffectActive = false;
    state.shieldIcon.active = false;
    state.shieldEffectActive = false;
    state.missedBeers = 0;
    state.slowIcon.active = false;
    state.slowEffectActive = false;
}

void playBackgroundMusicIfNotPlaying(GameResources& resources, const GameState& state) {
    if (!Mix_PlayingMusic() && !state.onFrameScreen && !state.onVipScreen && !state.gameStarted) {
        Mix_PlayMusic(resources.backgroundMusic, -1);
    }
}

void updateGame(GameState& state, GameResources& resources, SDL_Renderer* renderer) {
    if (state.paused || !state.gameStarted) return;

    Uint32 currentTime = SDL_GetTicks();

    // Update x2 effect
    if (state.x2EffectActive && currentTime - state.x2EffectStartTime >= X2_EFFECT_DURATION) {
        state.x2EffectActive = false;
    }

    if (state.score >= X2_MIN_BEERS && !state.x2Icon.active &&
        currentTime - state.x2Icon.spawnTime >= X2_SPAWN_INTERVAL) {
        state.x2Icon.active = true;
        state.x2Icon.x = rand() % (GAME_AREA_WIDTH - X2Icon::WIDTH);
        state.x2Icon.y = 0;
        state.x2Icon.spawnTime = currentTime;
    }

    if (state.x2Icon.active) {
        state.x2Icon.y += X2Icon::FALL_SPEED;
        if (state.x2Icon.y + X2Icon::HEIGHT >= SCREEN_HEIGHT - PLAYER_HEIGHT &&
            state.x2Icon.x + X2Icon::WIDTH > state.playerX &&
            state.x2Icon.x < state.playerX + PLAYER_WIDTH) {
            state.x2Icon.active = false;
            state.x2EffectActive = true;
            state.x2EffectStartTime = currentTime;
            Mix_PlayChannel(-1, resources.catchSound, 0);
        }
        if (state.x2Icon.y > SCREEN_HEIGHT) {
            state.x2Icon.active = false;
        }
    }

    // Update shield effect
    if (state.shieldEffectActive && currentTime - state.shieldEffectStartTime >= SHIELD_EFFECT_DURATION) {
        state.shieldEffectActive = false;
        state.missedBeers = 0;
    }

    if (state.score >= SHIELD_MIN_BEERS && !state.shieldIcon.active &&
        currentTime - state.shieldIcon.spawnTime >= SHIELD_SPAWN_INTERVAL) {
        state.shieldIcon.active = true;
        state.shieldIcon.x = rand() % (GAME_AREA_WIDTH - ShieldIcon::WIDTH);
        state.shieldIcon.y = 0;
        state.shieldIcon.spawnTime = currentTime;
    }

    if (state.shieldIcon.active) {
        state.shieldIcon.y += ShieldIcon::FALL_SPEED;
        if (state.shieldIcon.y + ShieldIcon::HEIGHT >= SCREEN_HEIGHT - PLAYER_HEIGHT &&
            state.shieldIcon.x + ShieldIcon::WIDTH > state.playerX &&
            state.shieldIcon.x < state.playerX + PLAYER_WIDTH) {
            state.shieldIcon.active = false;
            state.shieldEffectActive = true;
            state.shieldEffectStartTime = currentTime;
            state.missedBeers = 0;
            Mix_PlayChannel(-1, resources.catchSound, 0);
        }
        if (state.shieldIcon.y > SCREEN_HEIGHT) {
            state.shieldIcon.active = false;
        }
    }

    // Update slow effect
    if (state.slowEffectActive && currentTime - state.slowEffectStartTime >= SLOW_EFFECT_DURATION) {
        state.slowEffectActive = false;
        for (auto& beer : state.beers) {
            beer.speed = state.beer_speed;
        }
    }

    if (state.score >= SLOW_MIN_BEERS && !state.slowIcon.active &&
        currentTime - state.slowIcon.spawnTime >= SLOW_SPAWN_INTERVAL) {
        state.slowIcon.active = true;
        state.slowIcon.x = rand() % (GAME_AREA_WIDTH - SlowIcon::WIDTH);
        state.slowIcon.y = 0;
        state.slowIcon.spawnTime = currentTime;
    }

    if (state.slowIcon.active) {
        state.slowIcon.y += SlowIcon::FALL_SPEED;
        if (state.slowIcon.y + SlowIcon::HEIGHT >= SCREEN_HEIGHT - PLAYER_HEIGHT &&
            state.slowIcon.x + SlowIcon::WIDTH > state.playerX &&
            state.slowIcon.x < state.playerX + PLAYER_WIDTH) {
            state.slowIcon.active = false;
            state.slowEffectActive = true;
            state.slowEffectStartTime = currentTime;
            for (auto& beer : state.beers) {
                beer.speed = state.beer_speed * SLOW_EFFECT_RATE;
            }
            Mix_PlayChannel(-1, resources.catchSound, 0);
        }
        if (state.slowIcon.y > SCREEN_HEIGHT) {
            state.slowIcon.active = false;
        }
    }

    // Update speed boost
    if (state.score >= 60 && !state.speedBoostUnlocked) {
        state.speedBoostUnlocked = true;
        state.showSpeedBoostMessage = true;
    }

    if (state.speedBoostActive) {
        state.showSpeedBoostMessage = false;
    }

    // Update beers
    for (auto& beer : state.beers) {
        beer.y += beer.speed;
        if (beer.y > SCREEN_HEIGHT) {
            if (state.shieldEffectActive) {
                state.missedBeers++;
                if (state.missedBeers > SHIELD_MAX_MISSED) {
                    Mix_PlayChannel(-1, resources.failSound, 0);
                    SDL_Surface* loseSurface = TTF_RenderText_Solid(resources.loseFont, "YOU LOSE", {255, 255, 255, 255});
                    SDL_Texture* loseTexture = SDL_CreateTextureFromSurface(renderer, loseSurface);
                    SDL_Rect loseRect = {GAME_AREA_WIDTH / 2 - loseSurface->w / 2, SCREEN_HEIGHT / 2 - loseSurface->h / 2, loseSurface->w, loseSurface->h};
                    SDL_RenderCopy(renderer, loseTexture, NULL, &loseRect);
                    SDL_RenderPresent(renderer);
                    SDL_DestroyTexture(loseTexture);
                    SDL_FreeSurface(loseSurface);
                    SDL_Delay(5000);
                    resetToHomeScreen(state, {SCREEN_WIDTH - BUTTON_SIZE - BUTTON_MARGIN, BUTTON_MARGIN, BUTTON_SIZE, BUTTON_SIZE});
                    playBackgroundMusicIfNotPlaying(resources, state);
                    break;
                }
                beer.y = 0;
                beer.x = rand() % (GAME_AREA_WIDTH - BEER_WIDTH);
            } else {
                Mix_PlayChannel(-1, resources.failSound, 0);
                SDL_Surface* loseSurface = TTF_RenderText_Solid(resources.loseFont, "YOU LOSE", {255, 255, 255, 255});
                SDL_Texture* loseTexture = SDL_CreateTextureFromSurface(renderer, loseSurface);
                SDL_Rect loseRect = {GAME_AREA_WIDTH / 2 - loseSurface->w / 2, SCREEN_HEIGHT / 2 - loseSurface->h / 2, loseSurface->w, loseSurface->h};
                SDL_RenderCopy(renderer, loseTexture, NULL, &loseRect);
                SDL_RenderPresent(renderer);
                SDL_DestroyTexture(loseTexture);
                SDL_FreeSurface(loseSurface);
                SDL_Delay(5000);
                resetToHomeScreen(state, {SCREEN_WIDTH - BUTTON_SIZE - BUTTON_MARGIN, BUTTON_MARGIN, BUTTON_SIZE, BUTTON_SIZE});
                playBackgroundMusicIfNotPlaying(resources, state);
                break;
            }
        }
        if (beer.y + BEER_HEIGHT >= SCREEN_HEIGHT - PLAYER_HEIGHT && beer.x + BEER_WIDTH > state.playerX && beer.x < state.playerX + PLAYER_WIDTH) {
            beer.y = 0;
            beer.x = rand() % (GAME_AREA_WIDTH - BEER_WIDTH);
            state.score++;
            if (state.x2EffectActive) {
                state.score++;
            }
            std::cout << state.score << std::endl;
            Mix_PlayChannel(-1, resources.catchSound, 0);

            if (state.score >= 110) {
                state.gameStarted = false;
                state.paused = false;
                state.beers.clear();
                state.onFrameScreen = false;
                state.onVipScreen = true;

                Uint32 startTime = SDL_GetTicks();
                std::vector<int> heartPositions;
                SDL_Texture* heartTexture = IMG_LoadTexture(renderer, "assets/heart.png");
                if (!heartTexture) {
                    std::cerr << "Failed to load heart texture: " << IMG_GetError() << std::endl;
                    return;
                }

                while (SDL_GetTicks() - startTime < 5000) {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderClear(renderer);

                    SDL_Rect greenBackgroundRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
                    SDL_RenderCopy(renderer, resources.greenBackgroundTexture, NULL, &greenBackgroundRect);

                    SDL_Rect hopThoaiRect = {109, 73, 400, 200};
                    SDL_RenderCopy(renderer, resources.hopThoaiTexture, NULL, &hopThoaiRect);

                    if (resources.selectedGirlTexture) {
                        SDL_Rect girlVipRect = {SCREEN_WIDTH - 450, SCREEN_HEIGHT / 2 - 250, 500, 500};
                        SDL_RenderCopy(renderer, resources.selectedGirlTexture, NULL, &girlVipRect);
                    }

                    for (int i = 0; i < 20; ++i) {
                        int xPos;
                        if (heartPositions.size() <= i) {
                            do {
                                xPos = rand() % (SCREEN_WIDTH - 50);
                            } while (!heartPositions.empty() && abs(xPos - heartPositions.back()) < 60);
                            heartPositions.push_back(xPos);
                        } else {
                            xPos = heartPositions[i];
                        }

                        SDL_Rect heartRect = {xPos, SCREEN_HEIGHT - (SDL_GetTicks() - startTime) / 10 - i * 50, 50, 50};
                        SDL_RenderCopy(renderer, heartTexture, NULL, &heartRect);
                    }

                    SDL_RenderPresent(renderer);
                    SDL_Delay(8);
                }

                SDL_DestroyTexture(heartTexture);
                return;
            }

            if (state.score >= 7 && (state.score - 7) % 14 == 0) state.beer_speed++;
            if (state.score % 18 == 0 && state.max_beers < 5) {
                state.max_beers++;
                while (state.beers.size() < state.max_beers) {
                    state.beers.push_back({rand() % (GAME_AREA_WIDTH - BEER_WIDTH), 0, state.beer_speed, SDL_GetTicks()});
                }
            }
        }
    }
}