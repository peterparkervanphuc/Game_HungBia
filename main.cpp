#include "include/SDL.h"
#include "include/SDL_image.h"
#include "include/SDL_ttf.h"
#include "include/SDL_mixer.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string> 

#undef main

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 800;
const int GAME_AREA_WIDTH = 600;
const int INFO_AREA_WIDTH = 300;
const int PLAYER_WIDTH = 150;
const int PLAYER_HEIGHT = 80;
const int BEER_WIDTH = 40;
const int BEER_HEIGHT = 120;
const int PLAYER_SPEED = 10;
const float INITIAL_BEER_SPEED = 3.7; 
int beer_speed = INITIAL_BEER_SPEED;
int max_beers = 1;
const int BEER_DELAY = 300;
const int BUTTON_SIZE = 40;
const int BUTTON_MARGIN = 10;

const int X2_SPAWN_INTERVAL = 50000; 
const int X2_EFFECT_DURATION = 8000; 
const int X2_MIN_BEERS = 35; 

struct Beer {
    int x, y, speed;
    Uint32 spawnTime;
};

struct X2Icon {
    int x, y;
    bool active;
    Uint32 spawnTime;
    static const int WIDTH = 40;
    static const int HEIGHT = 40;
    static const int FALL_SPEED = 8;
};

bool showSpeedBoostMessage = false; 

X2Icon x2Icon = {0, 0, false, 0};
bool x2EffectActive = false;
Uint32 x2EffectStartTime = 0;

void resetToHomeScreen(bool& gameStarted, bool& onFrameScreen, bool& onVipScreen, bool& paused, 
                       std::vector<Beer>& beers, int& score, int& beer_speed, int& playerX, SDL_Rect& infoRect, 
                       bool& speedBoostUnlocked, bool& showSpeedBoostMessage) {
    gameStarted = false;
    onFrameScreen = false;
    onVipScreen = false;
    paused = false;
    beers.clear();
    score = 0;
    beer_speed = INITIAL_BEER_SPEED;
    playerX = GAME_AREA_WIDTH / 2 - PLAYER_WIDTH / 2;
    infoRect.x = SCREEN_WIDTH - BUTTON_SIZE - BUTTON_MARGIN;
    max_beers = 1;
    speedBoostUnlocked = false; 
    showSpeedBoostMessage = false;
    x2Icon.active = false;
    x2EffectActive = false;
}

void playBackgroundMusicIfNotPlaying(Mix_Music* backgroundMusic, bool onFrameScreen, bool onVipScreen, bool gameStarted) {
    if (!Mix_PlayingMusic() && !onFrameScreen && !onVipScreen && !gameStarted) {
        Mix_PlayMusic(backgroundMusic, -1);
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init(); 
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    
    Mix_Chunk* catchSound = Mix_LoadWAV("assets/collect.mp3");
    Mix_Chunk* failSound = Mix_LoadWAV("assets/break.mp3");
    Mix_Music* backgroundMusic = Mix_LoadMUS("assets/music1.mp3");
    if (!backgroundMusic) {
        std::cerr << "Failed to load background music: " << Mix_GetError() << std::endl;
        return -1;
    }
    
    SDL_Window* window = SDL_CreateWindow("Beer Catcher", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    SDL_Surface* iconSurface = IMG_Load("assets/icon.ico"); 
    if (iconSurface) {
        SDL_SetWindowIcon(window, iconSurface); 
        SDL_FreeSurface(iconSurface);
    } else {
        std::cerr << "Failed to load icon: " << IMG_GetError() << std::endl;
    }
    
    SDL_Texture* playerTexture = IMG_LoadTexture(renderer, "assets/thungbia.png");
    SDL_Texture* beerTexture = IMG_LoadTexture(renderer, "assets/bia.png");
    SDL_Texture* infoTexture = IMG_LoadTexture(renderer, "assets/info.png");
    SDL_Texture* backButton = IMG_LoadTexture(renderer, "assets/back.png");
    SDL_Texture* playPauseButton = IMG_LoadTexture(renderer, "assets/playpause.png");
    SDL_Texture* infoButton = IMG_LoadTexture(renderer, "assets/info_button.png");
    SDL_Texture* playButton = IMG_LoadTexture(renderer, "assets/play.png");
    SDL_Texture* backgroundTexture = IMG_LoadTexture(renderer, "assets/background.png");
    SDL_Texture* background2Texture = IMG_LoadTexture(renderer, "assets/background2.png"); 
    SDL_Texture* frameTexture = IMG_LoadTexture(renderer, "assets/frame.png");
    if (!frameTexture) {
        std::cerr << "Failed to load frame: " << IMG_GetError() << std::endl;
        return -1;
    }
    if (!background2Texture) {
        std::cerr << "Failed to load background2: " << IMG_GetError() << std::endl;
        return -1;
    }
    SDL_Texture* greenBackgroundTexture = IMG_LoadTexture(renderer, "assets/greenbackground.jpg");
    if (!greenBackgroundTexture) {
        std::cerr << "Failed to load greenbackground: " << IMG_GetError() << std::endl;
        return -1;
    }
    SDL_Texture* homeButtonTexture = IMG_LoadTexture(renderer, "assets/home.png");
    if (!homeButtonTexture) {
        std::cerr << "Failed to load home button: " << IMG_GetError() << std::endl;
        return -1;
    }

    SDL_Texture* girl1Texture = IMG_LoadTexture(renderer, "assets/girl1.png");
    SDL_Texture* girl2Texture = IMG_LoadTexture(renderer, "assets/girl2.png");
    SDL_Texture* girl3Texture = IMG_LoadTexture(renderer, "assets/girl3.png");
    if (!girl1Texture || !girl2Texture || !girl3Texture) {
        std::cerr << "Failed to load girl images: " << IMG_GetError() << std::endl;
        return -1;
    }

    SDL_Texture* selectedGirlTexture = NULL; 
    TTF_Font* scoreFont = TTF_OpenFont("fonts/score.ttf", 24); 
    if (!scoreFont) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return -1;
    }
    SDL_Color whiteColor = {255, 255, 255, 255}; 
    SDL_Color blackColor = {0, 0, 0, 255}; 

    SDL_Rect homeButtonRect = {SCREEN_WIDTH - BUTTON_SIZE - BUTTON_MARGIN, BUTTON_MARGIN, BUTTON_SIZE, BUTTON_SIZE};
    bool onVipScreen = false;
    
    TTF_Font* startFont = TTF_OpenFont("fonts/start.ttf", 48); 
    if (!startFont) {
        std::cerr << "Failed to load font for START: " << TTF_GetError() << std::endl;
        return -1;
    }
    SDL_Surface* startSurface = TTF_RenderText_Solid(startFont, "START", whiteColor); 
    SDL_Texture* startTexture = SDL_CreateTextureFromSurface(renderer, startSurface); 
    SDL_Rect startRect = {SCREEN_WIDTH / 2 - startSurface->w / 2, SCREEN_HEIGHT / 2 + 60, startSurface->w, startSurface->h}; 
    SDL_FreeSurface(startSurface); 

    TTF_Font* titleFont = TTF_OpenFont("fonts/hungbia.ttf", 56); 
    if (!titleFont) {
        std::cerr << "Failed to load font for title: " << TTF_GetError() << std::endl;
        return -1;
    }
    SDL_Surface* titleSurface = TTF_RenderText_Solid(titleFont, "HUNG BIA", whiteColor); 
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface); 
    SDL_Rect titleRect = {SCREEN_WIDTH / 2 - titleSurface->w / 2, SCREEN_HEIGHT / 4 - titleSurface->h / 2, titleSurface->w, titleSurface->h}; 
    SDL_FreeSurface(titleSurface); 

    TTF_Font* loseFont = TTF_OpenFont("fonts/youlose.ttf", 70);
    if (!loseFont) {
        std::cerr << "Failed to load font for YOU LOSE: " << TTF_GetError() << std::endl;
        return -1;
    }

    SDL_Surface* chooseTextSurface = TTF_RenderText_Solid(scoreFont, "CHON 1 TRONG 3 EM", whiteColor);
    SDL_Texture* chooseTextTexture = SDL_CreateTextureFromSurface(renderer, chooseTextSurface);
    SDL_Rect chooseTextRect = {SCREEN_WIDTH / 2 - chooseTextSurface->w, 60, chooseTextSurface->w * 2, chooseTextSurface->h * 2};
    SDL_FreeSurface(chooseTextSurface);

    TTF_Font* boostFont = TTF_OpenFont("fonts/score.ttf", 36); 
    if (!boostFont) {
        std::cerr << "Failed to load boost font: " << TTF_GetError() << std::endl;
        return -1;
    }

    SDL_Texture* hopThoaiTexture = IMG_LoadTexture(renderer, "assets/hopthoai.png");
    if (!hopThoaiTexture) {
        std::cerr << "Failed to load hopthoai texture: " << IMG_GetError() << std::endl;
        return -1;
    }

    SDL_Texture* x2IconTexture = IMG_LoadTexture(renderer, "assets/x2.png");
    if (!x2IconTexture) {
        std::cerr << "Failed to load x2 icon: " << IMG_GetError() << std::endl;
        return -1;
    }

    int playerX = GAME_AREA_WIDTH / 2 - PLAYER_WIDTH / 2;
    std::vector<Beer> beers;
    int score = 0;
    bool running = true;
    bool moveLeft = false, moveRight = false;
    bool gameStarted = false;
    bool paused = false;
    bool speedBoostUnlocked = false; 
    bool speedBoostActive = false;  
    const int BOOSTED_PLAYER_SPEED = PLAYER_SPEED + 5;
    bool onFrameScreen = false;
    bool showInfoPopup = false;
    SDL_Texture* infoPopupTexture = IMG_LoadTexture(renderer, "assets/info_popup.jpg");
    if (!infoPopupTexture) {
        std::cerr << "Failed to load info popup: " << IMG_GetError() << std::endl;
        return -1;
    }
    SDL_Event event;
    
    SDL_Rect backRect = {SCREEN_WIDTH - INFO_AREA_WIDTH - 3 * (BUTTON_SIZE + BUTTON_MARGIN), BUTTON_MARGIN, BUTTON_SIZE, BUTTON_SIZE};
    SDL_Rect playPauseRect = {SCREEN_WIDTH - INFO_AREA_WIDTH - 2 * (BUTTON_SIZE + BUTTON_MARGIN), BUTTON_MARGIN, BUTTON_SIZE, BUTTON_SIZE};
    SDL_Rect infoRect = {SCREEN_WIDTH - BUTTON_SIZE - BUTTON_MARGIN, BUTTON_MARGIN, BUTTON_SIZE, BUTTON_SIZE};
    SDL_Rect playButtonRect = {SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50, 100, 100};
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x, y = event.button.y;
                if (!showInfoPopup && x >= infoRect.x && x <= infoRect.x + infoRect.w &&
                    y >= infoRect.y && y <= infoRect.y + infoRect.h) {
                    showInfoPopup = true;
                } else if (showInfoPopup) {
                    showInfoPopup = false;
                } else if (!gameStarted && !onFrameScreen && x >= playButtonRect.x && x <= playButtonRect.x + playButtonRect.w &&
                    y >= playButtonRect.y && y <= playButtonRect.y + playButtonRect.h) {
                    onFrameScreen = true;
                    if (Mix_PlayingMusic()) {
                        Mix_HaltMusic();
                    }
                } else if (onFrameScreen && !gameStarted) {
                    SDL_Rect frameRect = {SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 - 200, 600, 400};
                    SDL_Rect girl1Rect = {SCREEN_WIDTH / 2 - 310, SCREEN_HEIGHT / 2 - 150, 280, 375};
                    SDL_Rect girl2Rect = {SCREEN_WIDTH / 2 - 85, SCREEN_HEIGHT / 2 - 160, 170, 320};
                    SDL_Rect girl3Rect = {SCREEN_WIDTH / 2 + 150, SCREEN_HEIGHT / 2 - 150, 150, 300};

                    if (x >= girl1Rect.x && x <= girl1Rect.x + girl1Rect.w &&
                        y >= girl1Rect.y && y <= girl1Rect.y + girl1Rect.h) {
                        selectedGirlTexture = girl1Texture; 
                        gameStarted = true;
                    } else if (x >= girl2Rect.x && x <= girl2Rect.x + girl2Rect.w &&
                               y >= girl2Rect.y && y <= girl2Rect.y + girl2Rect.h) {
                        selectedGirlTexture = girl2Texture; 
                        gameStarted = true;
                    } else if (x >= girl3Rect.x && x <= girl3Rect.x + girl3Rect.w &&
                               y >= girl3Rect.y && y <= girl3Rect.y + girl3Rect.h) {
                        selectedGirlTexture = girl3Texture;
                        gameStarted = true;
                    }
                    if (gameStarted) {
                        beers.push_back({rand() % (GAME_AREA_WIDTH - BEER_WIDTH), 0, beer_speed, SDL_GetTicks()});
                        infoRect.x = GAME_AREA_WIDTH - BUTTON_SIZE - BUTTON_MARGIN;
                        if (Mix_PlayingMusic()) {
                            Mix_HaltMusic();
                        }
                    }
                } else if (gameStarted && x >= playPauseRect.x && x <= playPauseRect.x + playPauseRect.w &&
                           y >= playPauseRect.y && y <= playPauseRect.y + playPauseRect.h) {
                    paused = !paused;
                } else if (gameStarted && x >= backRect.x && x <= backRect.x + backRect.w &&
                           y >= backRect.y && y <= backRect.y + backRect.h) {
                    resetToHomeScreen(gameStarted, onFrameScreen, onVipScreen, paused, beers, score, beer_speed, playerX, infoRect, speedBoostUnlocked, showSpeedBoostMessage);
                    playBackgroundMusicIfNotPlaying(backgroundMusic, onFrameScreen, onVipScreen, gameStarted);
                } else if (onVipScreen && x >= homeButtonRect.x && x <= homeButtonRect.x + homeButtonRect.w &&
                           y >= homeButtonRect.y && y <= homeButtonRect.y + homeButtonRect.h) {
                    resetToHomeScreen(gameStarted, onFrameScreen, onVipScreen, paused, beers, score, beer_speed, playerX, infoRect, speedBoostUnlocked, showSpeedBoostMessage);
                    playBackgroundMusicIfNotPlaying(backgroundMusic, onFrameScreen, onVipScreen, gameStarted);
                }
            }
            else if (!paused && event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_LEFT) moveLeft = true;
                if (event.key.keysym.sym == SDLK_RIGHT) moveRight = true;
                if (event.key.keysym.sym == SDLK_SPACE) {
                    if (speedBoostUnlocked) {
                        speedBoostActive = true;
                    }
                }
            } else if (!paused && event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_LEFT) moveLeft = false;
                if (event.key.keysym.sym == SDLK_RIGHT) moveRight = false;
                if (event.key.keysym.sym == SDLK_SPACE) {
                    speedBoostActive = false;
                }
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        if (showInfoPopup) {
            SDL_Rect popupRect = {SCREEN_WIDTH / 2 - 200, 0, 400, SCREEN_HEIGHT}; 
            SDL_RenderCopy(renderer, infoPopupTexture, NULL, &popupRect);
        } else if (onVipScreen) {
            SDL_Rect greenBackgroundRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, greenBackgroundTexture, NULL, &greenBackgroundRect);

            SDL_Rect hopThoaiRect = {109, 73, 400, 200 };
            SDL_RenderCopy(renderer, hopThoaiTexture, NULL, &hopThoaiRect);
            SDL_RenderCopy(renderer, homeButtonTexture, NULL, &homeButtonRect);

            if (selectedGirlTexture) {
                SDL_Rect girlVipRect = {
                    SCREEN_WIDTH - 450, 
                    SCREEN_HEIGHT / 2 - 250, 
                    500, 
                    500  
                };
                SDL_RenderCopy(renderer, selectedGirlTexture, NULL, &girlVipRect);
            }

            if (Mix_PlayingMusic()) {
                Mix_HaltMusic();
            }
        } else if (!gameStarted) {
            if (!onFrameScreen) {
                playBackgroundMusicIfNotPlaying(backgroundMusic, onFrameScreen, onVipScreen, gameStarted);
                SDL_Rect background2Rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
                SDL_RenderCopy(renderer, background2Texture, NULL, &background2Rect);
                SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect); 
                SDL_RenderCopy(renderer, playButton, NULL, &playButtonRect);
                SDL_RenderCopy(renderer, infoButton, NULL, &infoRect);
                SDL_RenderCopy(renderer, startTexture, NULL, &startRect);
            } else if (onFrameScreen) {
                SDL_Rect greenBackgroundRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
                SDL_RenderCopy(renderer, greenBackgroundTexture, NULL, &greenBackgroundRect);
                SDL_Rect frameRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}; 
                SDL_RenderCopy(renderer, frameTexture, NULL, &frameRect);

                SDL_Rect girl1Rect = {SCREEN_WIDTH / 2 - 310, SCREEN_HEIGHT / 2 - 150, 280, 375}; 
                SDL_Rect girl2Rect = {SCREEN_WIDTH / 2 - 85, SCREEN_HEIGHT / 2 - 160, 170, 320}; 
                SDL_Rect girl3Rect = {SCREEN_WIDTH / 2 + 135, SCREEN_HEIGHT / 2 - 150, 150, 300};
                SDL_RenderCopy(renderer, girl1Texture, NULL, &girl1Rect);
                SDL_RenderCopy(renderer, girl2Texture, NULL, &girl2Rect);
                SDL_RenderCopy(renderer, girl3Texture, NULL, &girl3Rect);

                SDL_RenderCopy(renderer, chooseTextTexture, NULL, &chooseTextRect);
                SDL_RenderCopy(renderer, infoButton, NULL, &infoRect);
                if (Mix_PlayingMusic()) {
                    Mix_HaltMusic();
                }
            }
        } else {
            if (Mix_PlayingMusic()) {
                Mix_HaltMusic();
            }
            SDL_Rect backgroundRect = {0, 0, GAME_AREA_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);
            SDL_Rect infoAreaRect = {GAME_AREA_WIDTH, 0, INFO_AREA_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, infoTexture, NULL, &infoAreaRect);

            if (selectedGirlTexture) {
                SDL_Rect girlInfoRect;
                if (selectedGirlTexture == girl1Texture) {
                    girlInfoRect = {GAME_AREA_WIDTH - 100, 0, INFO_AREA_WIDTH + 200, SCREEN_HEIGHT}; 
                } else {
                    girlInfoRect = {GAME_AREA_WIDTH, 0, INFO_AREA_WIDTH, SCREEN_HEIGHT}; 
                }
                SDL_RenderCopy(renderer, selectedGirlTexture, NULL, &girlInfoRect);
            }

            if (!paused && gameStarted) {
                Uint32 currentTime = SDL_GetTicks();

                if (x2EffectActive && currentTime - x2EffectStartTime >= X2_EFFECT_DURATION) {
                    x2EffectActive = false;
                }

                if (score >= X2_MIN_BEERS && !x2Icon.active && 
                    currentTime - x2Icon.spawnTime >= X2_SPAWN_INTERVAL) {
                    x2Icon.active = true;
                    x2Icon.x = rand() % (GAME_AREA_WIDTH - X2Icon::WIDTH);
                    x2Icon.y = 0;
                    x2Icon.spawnTime = currentTime;
                }

                if (x2Icon.active) {
                    x2Icon.y += X2Icon::FALL_SPEED;

                    if (x2Icon.y + X2Icon::HEIGHT >= SCREEN_HEIGHT - PLAYER_HEIGHT &&
                        x2Icon.x + X2Icon::WIDTH > playerX && 
                        x2Icon.x < playerX + PLAYER_WIDTH) {
                        x2Icon.active = false;
                        x2EffectActive = true;
                        x2EffectStartTime = currentTime;
                        Mix_PlayChannel(-1, catchSound, 0);
                    }

                    if (x2Icon.y > SCREEN_HEIGHT) {
                        x2Icon.active = false;
                    }
                }

                if (score >= 60 && !speedBoostUnlocked) {
                    speedBoostUnlocked = true; 
                    showSpeedBoostMessage = true; 
                }

                if (speedBoostActive) {
                    showSpeedBoostMessage = false;
                }

                int currentSpeed = PLAYER_SPEED;
                if (speedBoostActive && speedBoostUnlocked) {
                    currentSpeed = BOOSTED_PLAYER_SPEED;
                }

                if (moveLeft && playerX > 0) playerX -= currentSpeed;
                if (moveRight && playerX < GAME_AREA_WIDTH - PLAYER_WIDTH) playerX += currentSpeed;

                for (auto& beer : beers) {
                    beer.y += beer.speed;
                    if (beer.y > SCREEN_HEIGHT) {
                        Mix_PlayChannel(-1, failSound, 0);
                        SDL_Surface* loseSurface = TTF_RenderText_Solid(loseFont, "YOU LOSE", whiteColor);
                        SDL_Texture* loseTexture = SDL_CreateTextureFromSurface(renderer, loseSurface);
                        SDL_Rect loseRect = {GAME_AREA_WIDTH / 2 - loseSurface->w / 2, SCREEN_HEIGHT / 2 - loseSurface->h / 2, loseSurface->w, loseSurface->h};
                        SDL_FreeSurface(loseSurface);
                        SDL_RenderCopy(renderer, loseTexture, NULL, &loseRect);
                        SDL_RenderPresent(renderer);
                        SDL_DestroyTexture(loseTexture);

                        SDL_Delay(5000); 
                        resetToHomeScreen(gameStarted, onFrameScreen, onVipScreen, paused, beers, score, beer_speed, playerX, infoRect, speedBoostUnlocked, showSpeedBoostMessage);
                        playBackgroundMusicIfNotPlaying(backgroundMusic, onFrameScreen, onVipScreen, gameStarted);
                        break;
                    }
                    if (beer.y + BEER_HEIGHT >= SCREEN_HEIGHT - PLAYER_HEIGHT && beer.x + BEER_WIDTH > playerX && beer.x < playerX + PLAYER_WIDTH) {
                        beer.y = 0;
                        beer.x = rand() % (GAME_AREA_WIDTH - BEER_WIDTH);
                        score++;
                        if (x2EffectActive) {
                            score++;
                        }
                        std::cout << score << std::endl; 
                        Mix_PlayChannel(-1, catchSound, 0);

                        if (score == 70) { 
                            gameStarted = false;
                            paused = false;
                            beers.clear();
                            onFrameScreen = false;
                            onVipScreen = true;

                            Uint32 startTime = SDL_GetTicks();
                            std::vector<int> heartPositions; 

                            SDL_Texture* heartTexture = IMG_LoadTexture(renderer, "assets/heart.png");
                            if (!heartTexture) {
                                std::cerr << "Failed to load heart texture: " << IMG_GetError() << std::endl;
                                return -1;
                            }

                            while (SDL_GetTicks() - startTime < 5000) {
                                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                                SDL_RenderClear(renderer);

                                SDL_Rect greenBackgroundRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
                                SDL_RenderCopy(renderer, greenBackgroundTexture, NULL, &greenBackgroundRect);

                                SDL_Rect hopThoaiRect = {109, 73, 400, 200 };
                                SDL_RenderCopy(renderer, hopThoaiTexture, NULL, &hopThoaiRect);

                                if (selectedGirlTexture) {
                                    SDL_Rect girlVipRect = {
                                        SCREEN_WIDTH - 450,
                                        SCREEN_HEIGHT / 2 - 250, 
                                        500, 
                                        500  
                                    };
                                    SDL_RenderCopy(renderer, selectedGirlTexture, NULL, &girlVipRect);
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

                                    SDL_Rect heartRect = {
                                        static_cast<int>(xPos),
                                        static_cast<int>(SCREEN_HEIGHT - (SDL_GetTicks() - startTime) / 10 - i * 50),
                                        50, 50
                                    };
                                    SDL_RenderCopy(renderer, heartTexture, NULL, &heartRect);
                                }

                                SDL_RenderPresent(renderer);
                                SDL_Delay(8);
                            }

                            SDL_DestroyTexture(heartTexture);
                            break;
                        }

                        if (score >= 7 && (score - 7) % 14 == 0) beer_speed++;
                        
                        if (score % 18 == 0 && max_beers < 5) {
                            max_beers++;
                            while (beers.size() < max_beers) {
                                beers.push_back({rand() % (GAME_AREA_WIDTH - BEER_WIDTH), 0, beer_speed, SDL_GetTicks()});
                            }
                        }
                    }
                }
            }
            SDL_Rect playerRect = {playerX, SCREEN_HEIGHT - PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT};
            SDL_RenderCopy(renderer, playerTexture, NULL, &playerRect);
            for (auto& beer : beers) {
                SDL_Rect beerRect = {beer.x, beer.y, BEER_WIDTH, BEER_HEIGHT};
                SDL_RenderCopy(renderer, beerTexture, NULL, &beerRect);
            }
            if (x2Icon.active) {
                SDL_Rect x2Rect = {x2Icon.x, x2Icon.y, X2Icon::WIDTH, X2Icon::HEIGHT};
                SDL_RenderCopy(renderer, x2IconTexture, NULL, &x2Rect);
            }
            SDL_RenderCopy(renderer, backButton, NULL, &backRect);
            SDL_RenderCopy(renderer, playPauseButton, NULL, &playPauseRect);
            SDL_RenderCopy(renderer, infoButton, NULL, &infoRect);
            int displayScore = score * 100;
            std::string scoreText = "Score: " + std::to_string(displayScore);
            SDL_Surface* scoreSurface = TTF_RenderText_Solid(scoreFont, scoreText.c_str(), whiteColor);
            if (scoreSurface) {
                SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
                SDL_Rect scoreRect = {10, 10, scoreSurface->w, scoreSurface->h}; 
                SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
                SDL_FreeSurface(scoreSurface); 
                SDL_DestroyTexture(scoreTexture);
            }

            if (showSpeedBoostMessage) {
                SDL_Surface* boostMessageSurface = TTF_RenderText_Solid(boostFont, "AN SPACE DE TANG TOC", whiteColor);
                SDL_Texture* boostMessageTexture = SDL_CreateTextureFromSurface(renderer, boostMessageSurface);
                SDL_Rect boostMessageRect = {GAME_AREA_WIDTH / 2 - boostMessageSurface->w / 2, SCREEN_HEIGHT / 2 - boostMessageSurface->h / 2, boostMessageSurface->w, boostMessageSurface->h};
                SDL_RenderCopy(renderer, boostMessageTexture, NULL, &boostMessageRect);
                SDL_FreeSurface(boostMessageSurface);
                SDL_DestroyTexture(boostMessageTexture);
            }

            if (x2EffectActive) {
                int remainingTime = (X2_EFFECT_DURATION - (SDL_GetTicks() - x2EffectStartTime)) / 1000;
                std::string x2Text = "X2 SCORE: " + std::to_string(remainingTime) + "s";
                SDL_Surface* x2Surface = TTF_RenderText_Solid(scoreFont, x2Text.c_str(), whiteColor);
                if (x2Surface) {
                    SDL_Texture* x2Texture = SDL_CreateTextureFromSurface(renderer, x2Surface);
                    SDL_Rect x2Rect = {10, 40, x2Surface->w, x2Surface->h};
                    SDL_RenderCopy(renderer, x2Texture, NULL, &x2Rect);
                    SDL_FreeSurface(x2Surface);
                    SDL_DestroyTexture(x2Texture);
                }
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    Mix_FreeMusic(backgroundMusic);
    SDL_DestroyTexture(startTexture); 
    SDL_DestroyTexture(titleTexture); 
    SDL_DestroyTexture(background2Texture); 
    SDL_DestroyTexture(frameTexture);
    SDL_DestroyTexture(greenBackgroundTexture);
    SDL_DestroyTexture(chooseTextTexture);
    TTF_CloseFont(loseFont);
    SDL_DestroyTexture(homeButtonTexture);
    TTF_CloseFont(titleFont);
    TTF_CloseFont(startFont); 
    TTF_CloseFont(scoreFont); 
    TTF_CloseFont(boostFont); 
    SDL_DestroyTexture(infoPopupTexture);
    SDL_DestroyTexture(girl1Texture);
    SDL_DestroyTexture(girl2Texture);
    SDL_DestroyTexture(girl3Texture);
    SDL_DestroyTexture(hopThoaiTexture);
    SDL_DestroyTexture(x2IconTexture);
    TTF_Quit(); 
    SDL_Quit();
    return 0;
}
