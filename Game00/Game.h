#pragma once
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include <fstream>
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
#include <string>
#include <iostream>
using namespace std;

struct Vector2
{
	float x;
	float y;
};

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	int getRandomNumber(int min, int max) {
		return min + std::rand() % (max - min + 1);
	}

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;

	// font
	SDL_Texture* newTexture;
	SDL_Texture* newTexture2;
	SDL_Texture* newTexture3;
	SDL_Texture* newTexture4;
	SDL_Texture* alienTexture;
	SDL_Texture* infTexture;
	TTF_Font* theFont;
	int startWidth, startHeight;
	int restartWidth, restartHeight;
	int recordWidth, recordHeight;
	int failWidth, failHeight;
	int infWidth, infHeight;
	int size;
	int alienSize;
	// sound
	Mix_Chunk* gStart;
	Mix_Chunk* gClick;
	Mix_Chunk* gFail;

	// etc
	int clickCount;
	bool mIsRunning;
	bool start;
	bool colorChange;
	bool fail;
	bool moving;
	bool first;
	bool isInter;
	bool infMode;

	Vector2 mRectPos;
	Vector2 mRectVel;
	Vector2 alienPos;
	Vector2 alienVel;
	Uint32 mTicksCount;

};