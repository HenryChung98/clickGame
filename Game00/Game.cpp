#include "SDL/SDL_mixer.h"
#include "Game.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <iomanip>
using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
int r = 255;
int g = 255;
int b = 255;
int backR = 0;
int backG = 0;
int backB = 0;
int objColor = 4;
int backColor = 0;
Uint32 startTime = 0;
float recordCheck = 0;

Game::Game()
{
	mWindow = nullptr;
	mRenderer = nullptr;

	// font
	newTexture = nullptr;
	newTexture2 = nullptr;
	newTexture3 = nullptr;
	newTexture4 = nullptr;
	alienTexture = nullptr;
	infTexture = nullptr;
	theFont = nullptr;

	// sound
	gStart = nullptr;
	gClick = nullptr;
	gFail = nullptr;

	// bools
	mIsRunning = true;
	start = false;
	colorChange = false;
	fail = false;
	moving = false;
	first = true;
	isInter = false;
	infMode = false;

	clickCount = 0;
	size = 30;
	alienSize = 30;
}


bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow("Click Game", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	// sound
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	gStart = Mix_LoadWAV("start.wav");
	if (!gStart) {
		SDL_Log("Failed to create startSound: %s", SDL_GetError());
		return false;
	}
	gClick = Mix_LoadWAV("click.wav");
	if (!gClick) {
		SDL_Log("Failed to create clickSound: %s", SDL_GetError());
		return false;
	}
	gFail = Mix_LoadWAV("fail.wav");
	if (!gFail) {
		SDL_Log("Failed to create failSound: %s", SDL_GetError());
		return false;
	}

	// font
	TTF_Init();
	theFont = TTF_OpenFont("Dubai-Regular.ttf", 28);
	if (theFont == nullptr)
	{
		SDL_Log("Failed to open font: %s", SDL_GetError());
	}
	// image
	IMG_Init(IMG_INIT_PNG);
	

	mRectVel.x = 100.0f;
	mRectVel.y = 100.0f;
	alienVel.x = 200.0f;
	alienVel.y = 200.0f;
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				if (start) {
					int mouseX = event.button.x;
					int mouseY = event.button.y;

					
					if (colorChange) {
						objColor = getRandomNumber(0, 4);
						backColor = getRandomNumber(0, 4);
						if (objColor == backColor) {
							objColor = 4;
							backColor = 0;
						}
					}
					// measure time
					
					Uint32 currentTime = SDL_GetTicks();

					Uint32 elapsedTime = currentTime - startTime;

					double seconds = static_cast<double>(elapsedTime) / 1000.0;

					recordCheck = seconds;
					// check collision
					if (mRectPos.x < mouseX && mRectPos.x + size > mouseX && mRectPos.y < mouseY && mRectPos.y + size > mouseY) {
						mRectPos.x = getRandomNumber(100, SCREEN_WIDTH - 100);
						mRectPos.y = getRandomNumber(100, SCREEN_HEIGHT - 100);
						Mix_PlayChannel(-1, gClick, 0);
						clickCount++;
					}
					else {
						fail = true;
						start = false;
						Mix_PlayChannel(-1, gFail, 0);
					}
				}
				if (!infMode) {
					if (clickCount == 10) {
						Mix_PlayChannel(-1, gStart, 0);
						start = false;
					}
				}
			}
			break;
		}
		if (event.type == SDL_KEYDOWN) {
			if (!start) {
				switch (event.key.keysym.scancode) {
				case SDL_SCANCODE_D:

					size++;
					if (size > 100) {
						size = 100;
					}
					break;
				case SDL_SCANCODE_A:
					size--;
					if (size < 5) {
						size = 5;
					}
					break;
				case SDL_SCANCODE_W:
					if (mRectVel.x >= 0) {
						mRectVel.x += 20;
					}
					else if (mRectVel.x <= 0) {
						mRectVel.x -= 20;
					}
					if (mRectVel.y >= 0) {
						mRectVel.y += 20;
					}
					else if (mRectVel.y <= 0) {
						mRectVel.y -= 20;
					}
					break;
				case SDL_SCANCODE_S:
					if (mRectVel.x <= 0) {
						mRectVel.x += 20;
					}
					else if (mRectVel.x >= 0) {
						mRectVel.x -= 20;
					}
					if (mRectVel.y <= 0) {
						mRectVel.y += 20;
					}
					else if (mRectVel.y >= 0) {
						mRectVel.y -= 20;
					}
					break;
				case SDL_SCANCODE_UP:
					if (alienVel.x >= 0) {
						alienVel.x += 20;
					}
					else if (alienVel.x <= 0) {
						alienVel.x -= 20;
					}
					if (alienVel.y >= 0) {
						alienVel.y += 20;
					}
					else if (alienVel.y <= 0) {
						alienVel.y -= 20;
					}
					break;
				case SDL_SCANCODE_DOWN:
					if (alienVel.x <= 0) {
						alienVel.x += 20;
					}
					else if (alienVel.x >= 0) {
						alienVel.x -= 20;
					}
					if (alienVel.y <= 0) {
						alienVel.y += 20;
					}
					else if (alienVel.y >= 0) {
						alienVel.y -= 20;
					}
					break;
				case SDL_SCANCODE_LEFT:
					alienSize--;
					if (alienSize <= 10) {
						alienSize = 10;
					}
					break;
				case SDL_SCANCODE_RIGHT:
					alienSize++;
					if (alienSize >= 100) {
						alienSize = 100;
					}
					break;
				}
			}
		}
		if (event.type == SDL_KEYUP) {
			switch (event.key.keysym.scancode) {
			case SDL_SCANCODE_ESCAPE:
				mIsRunning = false;
				break;
			case SDL_SCANCODE_Q:
				if (first) {
					startTime = SDL_GetTicks();
					first = false;
					start = true;
					Mix_PlayChannel(-1, gStart, 0);

					if (clickCount == 0) {
						mRectPos.x = getRandomNumber(100, SCREEN_WIDTH - 100);
						mRectPos.y = getRandomNumber(100, SCREEN_HEIGHT - 100);
					}
				}
				break;

			case SDL_SCANCODE_R:
				if (!start && !first) {
					clickCount = 0;
					mRectPos.x = getRandomNumber(100, SCREEN_WIDTH - 100);
					mRectPos.y = getRandomNumber(100, SCREEN_HEIGHT - 100);
					startTime = 0;
					startTime = SDL_GetTicks();
					start = true;
					fail = false;
					Mix_PlayChannel(-1, gStart, 0);
				}
				break;
			case SDL_SCANCODE_1:
				backColor++;
				if (backColor > 4) {
					backColor = 0;
				}
				if (backR == 0 && backG == 0 && backB == 0) {
					backR = 255;
					backG = 0;
					backB = 0;
				}
				else if (backR == 255 && backG == 0 && backB == 0) {
					backR = 0;
					backG = 255;
					backB = 0;
				}
				else if (backR == 0 && backG == 255 && backB == 0) {
					backR = 0;
					backG = 0;
					backB = 255;
				}
				else if (backR == 0 && backG == 0 && backB == 255) {
					backR = 255;
					backG = 255;
					backB = 255;
				}
				else if (backR == 255 && backG == 255 && backB == 255) {
					backR = 0;
					backG = 0;
					backB = 0;
				}
				break;
			case SDL_SCANCODE_2:
				objColor++;
				if (objColor > 4) {
					objColor = 0;
				}
				if (r == 0 && g == 0 && b == 0) {
					r = 255;
					r = 0;
					b = 0;
				}
				else if (r == 255 && g == 0 && b == 0) {
					r = 0;
					g = 255;
					b = 0;
				}
				else if (r == 0 && g == 255 && b == 0) {
					r = 0;
					g = 0;
					b = 255;
				}
				else if (r == 0 && g == 0 && b == 255) {
					r = 255;
					g = 255;
					b = 255;
				}
				else if (r == 255 && g == 255 && b == 255) {
					r = 0;
					g = 0;
					b = 0;
				}
				break;
			case SDL_SCANCODE_TAB:
				if (!start) {
					if (infMode) {
						infMode = false;
					}
					else {
						infMode = true;
					}
				}
				break;
			case SDL_SCANCODE_Z:
				if (!start) {
					if (colorChange) {
						colorChange = false;
					}
					else {
						colorChange = true;
					}
				}
				break;
			case SDL_SCANCODE_X:
				if (!start) {
					if (moving) {
						moving = false;
					}
					else {
						moving = true;
					}
				}
				break;
			case SDL_SCANCODE_C:
				if (!start) {
					if (isInter) {
						isInter = false;
					}
					else {
						isInter = true;
						alienPos.x = getRandomNumber(SCREEN_WIDTH - 50, SCREEN_WIDTH - 30);
						alienPos.y = getRandomNumber(10, SCREEN_HEIGHT - 100);
					}
				}
				break;
			
			}
		}
	
	}
}


void Game::UpdateGame(){
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();
	if (moving) {
		// Update ball position based on ball velocity
		mRectPos.x += mRectVel.x * deltaTime;
		mRectPos.y += mRectVel.y * deltaTime;


		if (mRectPos.x <= 0 && mRectVel.x < 0.0f)
		{
			mRectVel.x *= -1;
		}
		else if (mRectPos.x + size >= SCREEN_WIDTH && mRectVel.x > 0.0f)
		{
			mRectVel.x *= -1;
		}



		if (mRectPos.y <= 0 && mRectVel.y < 0.0f)
		{
			mRectVel.y *= -1;
		}
		else if (mRectPos.y + size >= SCREEN_HEIGHT && mRectVel.y > 0.0f)
		{
			mRectVel.y *= -1;
		}
	}
	// color auto change
	if (objColor == 0) {
		r = 0;
		g = 0;
		b = 0;
	}
	if (objColor == 1) {
		r = 255;
		g = 0;
		b = 0;
	}
	if (objColor == 2) {
		r = 0;
		g = 255;
		b = 0;
	}
	if (objColor == 3) {
		r = 0;
		g = 0;
		b = 255;
	}
	if (objColor == 4) {
		r = 255;
		g = 255;
		b = 255;
	}

	if (backColor == 0) {
		backR = 0;
		backG = 0;
		backB = 0;
	}
	if (backColor == 1) {
		backR = 255;
		backG = 0;
		backB = 0;
	}
	if (backColor == 2) {
		backR = 0;
		backG = 255;
		backB = 0;
	}
	if (backColor == 3) {
		backR = 0;
		backG = 0;
		backB = 255;
	}
	if (backColor == 4) {
		backR = 255;
		backG = 255;
		backB = 255;
	}

	// alien
	if (isInter) {
		alienPos.x += alienVel.x * deltaTime;
		alienPos.y += alienVel.y * deltaTime;


		if (alienPos.x <= 0 && alienVel.x < 0.0f)
		{
			alienVel.x *= -1;
		}
		else if (alienPos.x + alienSize >= SCREEN_WIDTH && alienVel.x > 0.0f)
		{
			alienVel.x *= -1;
		}



		if (alienPos.y <= 0 && alienVel.y < 0.0f)
		{
			alienVel.y *= -1;
		}
		else if (alienPos.y + alienSize >= SCREEN_HEIGHT && alienVel.y > 0.0f)
		{
			alienVel.y *= -1;
		}
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		if (start) {
			if (alienPos.x < mouseX && alienPos.x + alienSize > mouseX && alienPos.y < mouseY && alienPos.y + alienSize > mouseY) {
				fail = true;
				start = false;
				Mix_PlayChannel(-1, gFail, 0);
			}
		}

	}
}

void Game::GenerateOutput() {
	SDL_Color textColor = { r, g, b };
	string startText = "Press Q to start";
	string restartText = "Press R to restart";
	string failText = "Try again";
	string infText = to_string(clickCount);

	SDL_Surface* startSurface = TTF_RenderText_Solid(theFont, startText.c_str(), textColor);
	newTexture = SDL_CreateTextureFromSurface(mRenderer, startSurface);
	startWidth = startSurface->w;
	startHeight = startSurface->h;
	SDL_FreeSurface(startSurface);

	SDL_Surface* restartSurface = TTF_RenderText_Solid(theFont, restartText.c_str(), textColor);
	newTexture2 = SDL_CreateTextureFromSurface(mRenderer, restartSurface);
	restartWidth = restartSurface->w;
	restartHeight = restartSurface->h;
	SDL_FreeSurface(restartSurface);


	SDL_Surface* failSurface = TTF_RenderText_Solid(theFont, failText.c_str(), textColor);
	newTexture3 = SDL_CreateTextureFromSurface(mRenderer, failSurface);
	failWidth = failSurface->w;
	failHeight = failSurface->h;
	SDL_FreeSurface(failSurface);


	SDL_Surface* infSurface = TTF_RenderText_Solid(theFont, infText.c_str(), textColor);
	infTexture = SDL_CreateTextureFromSurface(mRenderer, infSurface);
	infWidth = infSurface->w;
	infHeight = infSurface->h;
	SDL_FreeSurface(infSurface);


	string recordText = to_string(recordCheck);
	size_t dotPosition = recordText.find('.');
	if (dotPosition != std::string::npos && dotPosition + 4 < recordText.size()) {
		recordText = recordText.substr(0, dotPosition + 4);

		recordText += " seconds";
	}
		SDL_Surface* recordSurface = TTF_RenderText_Solid(theFont, recordText.c_str(), textColor);
		newTexture4 = SDL_CreateTextureFromSurface(mRenderer, recordSurface);
		recordWidth = recordSurface->w;
		recordHeight = recordSurface->h;
		SDL_FreeSurface(recordSurface);


		SDL_SetRenderDrawColor(mRenderer, backR, backG, backB, 0x00);
		SDL_RenderClear(mRenderer);

		// first screen
		if (!start && first) {
			SDL_Rect renderQuad = { (SCREEN_WIDTH - startWidth) / 2, (SCREEN_HEIGHT - startHeight) / 2, startWidth, startHeight };
			SDL_RenderCopy(mRenderer, newTexture, NULL, &renderQuad);

			SDL_Rect fillRect = { mRectPos.x, mRectPos.y, size, size };
			SDL_SetRenderDrawColor(mRenderer, r, g, b, 0xFF);
			SDL_RenderFillRect(mRenderer, &fillRect);
		}

		// game running screen
		if (start) {
			SDL_Rect fillRect = { mRectPos.x, mRectPos.y, size, size };
			SDL_SetRenderDrawColor(mRenderer, r, g, b, 0xFF);
			SDL_RenderFillRect(mRenderer, &fillRect);
		}

		// game over screen
		if (!start && !first) {
			SDL_Rect fillRect = { mRectPos.x / 2, mRectPos.y / 2, size, size };
			SDL_SetRenderDrawColor(mRenderer, r, g, b, 0xFF);
			SDL_RenderFillRect(mRenderer, &fillRect);

			SDL_Rect renderQuad2 = { (SCREEN_WIDTH - restartWidth) / 2, (SCREEN_HEIGHT - restartHeight) / 2, restartWidth, restartHeight };
			SDL_RenderCopy(mRenderer, newTexture2, NULL, &renderQuad2);
			if (!fail) {
				SDL_Rect renderQuad3 = { (SCREEN_WIDTH - recordWidth) / 2, (SCREEN_HEIGHT - recordHeight) / 2 - 50, recordWidth, recordHeight };
				SDL_RenderCopy(mRenderer, newTexture4, NULL, &renderQuad3);
			}
			else if(fail && !infMode) {
				SDL_Rect renderQuad4 = { (SCREEN_WIDTH - failWidth) / 2, (SCREEN_HEIGHT - failHeight) / 2 - 50, failWidth, failHeight };
				SDL_RenderCopy(mRenderer, newTexture3, NULL, &renderQuad4);
			}

		}
		if (infMode) {
			if (start || first) {
				SDL_Rect infScore = { SCREEN_WIDTH - infWidth, 5, infWidth, infHeight };
				SDL_RenderCopy(mRenderer, infTexture, NULL, &infScore);
			}
			else if (fail) {
				SDL_Rect infScore = { SCREEN_WIDTH / 2, (SCREEN_HEIGHT - failHeight) / 2 - 50 , infWidth * 1.3, infHeight * 1.3};
				SDL_RenderCopy(mRenderer, infTexture, NULL, &infScore);
			}
		}
		if (isInter) {
			string alienImg = "alien.png";
			SDL_Surface* alienSurface = IMG_Load(alienImg.c_str());
			alienTexture = SDL_CreateTextureFromSurface(mRenderer, alienSurface);
			SDL_FreeSurface(alienSurface);

			SDL_Rect fillAlien = { alienPos.x, alienPos.y, alienSize, alienSize };
			SDL_RenderCopy(mRenderer, alienTexture, NULL, &fillAlien);
			//SDL_RenderCopyEx(mRenderer, alienTexture, NULL, &fillAlien, degrees, NULL, flipType);
		}
		SDL_RenderPresent(mRenderer);
	
}

void Game::Shutdown(){

	TTF_CloseFont(theFont);
	Mix_FreeChunk(gStart);
	gStart = nullptr;
	Mix_FreeChunk(gClick);
	gClick = nullptr;
	Mix_FreeChunk(gFail);
	Mix_CloseAudio();
	gFail = nullptr;
	SDL_DestroyTexture(newTexture);
	SDL_DestroyTexture(newTexture2);
	SDL_DestroyTexture(newTexture3);
	SDL_DestroyTexture(newTexture4);
	SDL_DestroyTexture(alienTexture);
	SDL_DestroyTexture(infTexture);
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	IMG_Quit();
	SDL_Quit();
}