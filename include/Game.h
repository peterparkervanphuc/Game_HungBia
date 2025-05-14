#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>

// Constants
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
const int BEER_DELAY = 300;
const int BUTTON_SIZE = 40;
const int BUTTON_MARGIN = 10;
const int X2_SPAWN_INTERVAL = 25000;
const int X2_EFFECT_DURATION = 8000;
const int X2_MIN_BEERS = 37;
const int SHIELD_SPAWN_INTERVAL = 35000;
const int SHIELD_EFFECT_DURATION = 12000;
const int SHIELD_MIN_BEERS = 100;
const int SHIELD_MAX_MISSED = 2;
const int SLOW_SPAWN_INTERVAL = 35000;
const int SLOW_EFFECT_DURATION = 8000;
const int SLOW_MIN_BEERS = 76;
const float SLOW_EFFECT_RATE = 0.6f;

// Structs
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

struct ShieldIcon {
    int x, y;
    bool active;
    Uint32 spawnTime;
    static const int WIDTH = 40;
    static const int HEIGHT = 40;
    static const int FALL_SPEED = 9;
};

struct SlowIcon {
    int x, y;
    bool active;
    Uint32 spawnTime;
    static const int WIDTH = 40;
    static const int HEIGHT = 40;
    static const int FALL_SPEED = 7;
};

// Game state
struct GameState {
    std::vector<Beer> beers;
    int score;
    int beer_speed;
    int max_beers;
    int playerX;
    bool gameStarted;
    bool onFrameScreen;
    bool onVipScreen;
    bool paused;
    bool speedBoostUnlocked;
    bool showSpeedBoostMessage;
    bool speedBoostActive;
    bool showInfoPopup;
    X2Icon x2Icon;
    bool x2EffectActive;
    Uint32 x2EffectStartTime;
    ShieldIcon shieldIcon;
    bool shieldEffectActive;
    Uint32 shieldEffectStartTime;
    int missedBeers;
    SlowIcon slowIcon;
    bool slowEffectActive;
    Uint32 slowEffectStartTime;
    Uint32 pausedTime;
    Uint32 x2PausedTime;
    Uint32 shieldPausedTime;
    Uint32 slowPausedTime;
};

// Resources
struct GameResources {
    SDL_Texture* playerTexture;
    SDL_Texture* beerTexture;
    SDL_Texture* infoTexture;
    SDL_Texture* backButton;
    SDL_Texture* playPauseButton;
    SDL_Texture* infoButton;
    SDL_Texture* playButton;
    SDL_Texture* backgroundTexture;
    SDL_Texture* background2Texture;
    SDL_Texture* frameTexture;
    SDL_Texture* greenBackgroundTexture;
    SDL_Texture* homeButtonTexture;
    SDL_Texture* girl1Texture;
    SDL_Texture* girl2Texture;
    SDL_Texture* girl3Texture;
    SDL_Texture* selectedGirlTexture;
    SDL_Texture* infoPopupTexture;
    SDL_Texture* hopThoaiTexture;
    SDL_Texture* x2IconTexture;
    SDL_Texture* shieldIconTexture;
    SDL_Texture* slowIconTexture;
    SDL_Texture* startTexture;
    SDL_Texture* titleTexture;
    SDL_Texture* chooseTextTexture;
    TTF_Font* scoreFont;
    TTF_Font* startFont;
    TTF_Font* titleFont;
    TTF_Font* loseFont;
    TTF_Font* boostFont;
    Mix_Chunk* catchSound;
    Mix_Chunk* failSound;
    Mix_Music* backgroundMusic;
};

// Function declarations
void resetToHomeScreen(GameState& state, SDL_Rect& infoRect);
void playBackgroundMusicIfNotPlaying(GameResources& resources, const GameState& state);
void updateGame(GameState& state, GameResources& resources, SDL_Renderer* renderer);
void renderGame(SDL_Renderer* renderer, const GameState& state, const GameResources& resources, const SDL_Rect& backRect, const SDL_Rect& playPauseRect, const SDL_Rect& infoRect, const SDL_Rect& playButtonRect, const SDL_Rect& startRect, const SDL_Rect& titleRect, const SDL_Rect& chooseTextRect, const SDL_Rect& homeButtonRect);
void handleInput(SDL_Event& event, GameState& state, SDL_Rect& infoRect, const SDL_Rect& backRect, const SDL_Rect& playPauseRect, const SDL_Rect& playButtonRect, const SDL_Rect& homeButtonRect, GameResources& resources);

#endif // GAME_H