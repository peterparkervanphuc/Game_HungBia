#include "game.h"

void renderGame(SDL_Renderer* renderer, const GameState& state, const GameResources& resources, const SDL_Rect& backRect, const SDL_Rect& playPauseRect, const SDL_Rect& infoRect, const SDL_Rect& playButtonRect, const SDL_Rect& startRect, const SDL_Rect& titleRect, const SDL_Rect& chooseTextRect, const SDL_Rect& homeButtonRect) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (state.showInfoPopup) {
        SDL_Rect popupRect = {SCREEN_WIDTH / 2 - 250, 0, 500, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, resources.infoPopupTexture, NULL, &popupRect);
    } else if (state.onVipScreen) {
        SDL_Rect greenBackgroundRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, resources.greenBackgroundTexture, NULL, &greenBackgroundRect);
        SDL_Rect hopThoaiRect = {109, 73, 400, 200};
        SDL_RenderCopy(renderer, resources.hopThoaiTexture, NULL, &hopThoaiRect);
        SDL_RenderCopy(renderer, resources.homeButtonTexture, NULL, &homeButtonRect);
        if (resources.selectedGirlTexture) {
            SDL_Rect girlVipRect = {SCREEN_WIDTH - 450, SCREEN_HEIGHT / 2 - 250, 500, 500};
            SDL_RenderCopy(renderer, resources.selectedGirlTexture, NULL, &girlVipRect);
        }
        if (Mix_PlayingMusic()) {
            Mix_HaltMusic();
        }
    } else if (!state.gameStarted) {
        if (!state.onFrameScreen) {
            playBackgroundMusicIfNotPlaying(const_cast<GameResources&>(resources), state);
            SDL_Rect background2Rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, resources.background2Texture, NULL, &background2Rect);
            SDL_RenderCopy(renderer, resources.titleTexture, NULL, &titleRect);
            SDL_RenderCopy(renderer, resources.playButton, NULL, &playButtonRect);
            SDL_RenderCopy(renderer, resources.infoButton, NULL, &infoRect);
            SDL_RenderCopy(renderer, resources.startTexture, NULL, &startRect);
        } else {
            SDL_Rect greenBackgroundRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, resources.greenBackgroundTexture, NULL, &greenBackgroundRect);
            SDL_Rect frameRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, resources.frameTexture, NULL, &frameRect);
            SDL_Rect girl1Rect = {SCREEN_WIDTH / 2 - 310, SCREEN_HEIGHT / 2 - 150, 280, 375};
            SDL_Rect girl2Rect = {SCREEN_WIDTH / 2 - 85, SCREEN_HEIGHT / 2 - 160, 170, 320};
            SDL_Rect girl3Rect = {SCREEN_WIDTH / 2 + 135, SCREEN_HEIGHT / 2 - 150, 150, 300};
            SDL_RenderCopy(renderer, resources.girl1Texture, NULL, &girl1Rect);
            SDL_RenderCopy(renderer, resources.girl2Texture, NULL, &girl2Rect);
            SDL_RenderCopy(renderer, resources.girl3Texture, NULL, &girl3Rect);
            SDL_RenderCopy(renderer, resources.chooseTextTexture, NULL, &chooseTextRect);
            SDL_RenderCopy(renderer, resources.infoButton, NULL, &infoRect);
            if (Mix_PlayingMusic()) {
                Mix_HaltMusic();
            }
        }
    } else {
        if (Mix_PlayingMusic()) {
            Mix_HaltMusic();
        }
        SDL_Rect backgroundRect = {0, 0, GAME_AREA_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, resources.backgroundTexture, NULL, &backgroundRect);
        SDL_Rect infoAreaRect = {GAME_AREA_WIDTH, 0, INFO_AREA_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, resources.infoTexture, NULL, &infoAreaRect);

        if (resources.selectedGirlTexture) {
            SDL_Rect girlInfoRect = (resources.selectedGirlTexture == resources.girl1Texture) ?
                SDL_Rect{GAME_AREA_WIDTH - 100, 0, INFO_AREA_WIDTH + 200, SCREEN_HEIGHT} :
                SDL_Rect{GAME_AREA_WIDTH, 0, INFO_AREA_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, resources.selectedGirlTexture, NULL, &girlInfoRect);
        }

        SDL_Rect playerRect = {state.playerX, SCREEN_HEIGHT - PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT};
        SDL_RenderCopy(renderer, resources.playerTexture, NULL, &playerRect);
        for (const auto& beer : state.beers) {
            SDL_Rect beerRect = {beer.x, beer.y, BEER_WIDTH, BEER_HEIGHT};
            SDL_RenderCopy(renderer, resources.beerTexture, NULL, &beerRect);
        }
        if (state.x2Icon.active) {
            SDL_Rect x2Rect = {state.x2Icon.x, state.x2Icon.y, X2Icon::WIDTH, X2Icon::HEIGHT};
            SDL_RenderCopy(renderer, resources.x2IconTexture, NULL, &x2Rect);
        }
        if (state.shieldIcon.active) {
            SDL_Rect shieldRect = {state.shieldIcon.x, state.shieldIcon.y, ShieldIcon::WIDTH, ShieldIcon::HEIGHT};
            SDL_RenderCopy(renderer, resources.shieldIconTexture, NULL, &shieldRect);
        }
        if (state.slowIcon.active) {
            SDL_Rect slowRect = {state.slowIcon.x, state.slowIcon.y, SlowIcon::WIDTH, SlowIcon::HEIGHT};
            SDL_RenderCopy(renderer, resources.slowIconTexture, NULL, &slowRect);
        }
        SDL_RenderCopy(renderer, resources.backButton, NULL, &backRect);
        SDL_RenderCopy(renderer, resources.playPauseButton, NULL, &playPauseRect);
        SDL_RenderCopy(renderer, resources.infoButton, NULL, &infoRect);

        int displayScore = state.score * 100;
        std::string scoreText = "Score: " + std::to_string(displayScore);
        SDL_Surface* scoreSurface = TTF_RenderText_Solid(resources.scoreFont, scoreText.c_str(), {255, 255, 255, 255});
        if (scoreSurface) {
            SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
            SDL_Rect scoreRect = {10, 10, scoreSurface->w, scoreSurface	 scoreSurface->h};
            SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
            SDL_FreeSurface(scoreSurface);
            SDL_DestroyTexture(scoreTexture);
        }

        if (state.showSpeedBoostMessage) {
            SDL_Surface* boostMessageSurface = TTF_RenderText_Solid(resources.boostFont, "AN SPACE DE TANG TOC", {255, 255, 255, 255});
            SDL_Texture* boostMessageTexture = SDL_CreateTextureFromSurface(renderer, boostMessageSurface);
            SDL_Rect boostMessageRect = {GAME_AREA_WIDTH / 2 - boostMessageSurface->w / 2, SCREEN_HEIGHT / 2 - boostMessageSurface->h / 2, boostMessageSurface->w, boostMessageSurface->h};
            SDL_RenderCopy(renderer, boostMessageTexture, NULL, &boostMessageRect);
            SDL_FreeSurface(boostMessageSurface);
            SDL_DestroyTexture(boostMessageTexture);
        }

        if (state.x2EffectActive) {
            int remainingTime = (X2_EFFECT_DURATION - (SDL_GetTicks() - state.x2EffectStartTime)) / 1000;
            std::string x2Text = "X2 SCORE: " + std::to_string(remainingTime) + "s";
            SDL_Surface* x2Surface = TTF_RenderText_Solid(resources.scoreFont, x2Text.c_str(), {255, 255, 255, 255});
            if (x2Surface) {
                SDL_Texture* x2Texture = SDL_CreateTextureFromSurface(renderer, x2Surface);
                SDL_Rect x2Rect = {10, 40, x2Surface->w, x2Surface->h};
                SDL_RenderCopy(renderer, x2Texture, NULL, &x2Rect);
                SDL_FreeSurface(x2Surface);
                SDL_DestroyTexture(x2Texture);
            }
        }

        if (state.shieldEffectActive) {
            int remainingTime = (SHIELD_EFFECT_DURATION - (SDL_GetTicks() - state.shieldEffectStartTime)) / 1000;
            std::string shieldText = "SHIELD: " + std::to_string(remainingTime) + "s (Safe: " + std::to_string(SHIELD_MAX_MISSED - state.missedBeers) + ")";
            SDL_Surface* shieldSurface = TTF_RenderText_Solid(resources.scoreFont, shieldText.c_str(), {255, 255, 255, 255});
            if (shieldSurface) {
                SDL_Texture* shieldTexture = SDL_CreateTextureFromSurface(renderer, shieldSurface);
                SDL_Rect shieldTextRect = {10, 70, shieldSurface->w, shieldSurface->h};
                SDL_RenderCopy(renderer, shieldTexture, NULL, &shieldTextRect);
                SDL_FreeSurface(shieldSurface);
                SDL_DestroyTexture(shieldTexture);
            }
        }

        if (state.slowEffectActive) {
            int remainingTime = (SLOW_EFFECT_DURATION - (SDL_GetTicks() - state.slowEffectStartTime)) / 1000;
            std::string slowText = "SLOW: " + std::to_string(remainingTime) + "s";
            SDL_Surface* slowSurface = TTF_RenderText_Solid(resources.scoreFont, slowText.c_str(), {255, 255, 255, 255});
            if (slowSurface) {
                SDL_Texture* slowTexture = SDL_CreateTextureFromSurface(renderer, slowSurface);
                SDL_Rect slowTextRect = {10, 100, slowSurface->w, slowSurface->h};
                SDL_RenderCopy(renderer, slowTexture, NULL, &slowTextRect);
                SDL_FreeSurface(slowSurface);
                SDL_DestroyTexture(slowTexture);
            }
        }
    }
}