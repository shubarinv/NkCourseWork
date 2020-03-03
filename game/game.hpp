//
// Created by vhundef on 24.12.2019.
//

#ifndef PROGONHLANG_GAME_HPP
#define PROGONHLANG_GAME_HPP

#include <SDL.h>
#include <SDL_image.h>

#include <iostream>
#include <stdexcept>

#include "UI/ui_main_menu.hpp"
#include "field.hpp"
#include "input_manager.hpp"
#include "screenManager.hpp"

using namespace std;

class Game {
 private:
  SDL_DisplayMode DM{};
  SDL_Window *win{};
  SDL_Renderer *ren{};
  InputManager *inputManager{};
  ScreenManager *uiManager;
  Field *field{};
  char state = 'm';///< r-playing game| p-pause| m-main_Menu| e-Editing Field
 public:
  Game() {
	std::cout << "Trying to init SDL2..." << std::endl;
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
	  std::string error = SDL_GetError();
	  SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "%s", error.c_str());
	  throw std::runtime_error("Unable to init SDL2");
	}
	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
	  std::string error = SDL_GetError();
	  SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "%s", error.c_str());
	  throw std::runtime_error("Unable to init SDL2_Image");
	  SDL_Quit();
	}SDL_GetCurrentDisplayMode(0, &DM);
	auto Width = DM.w;
	auto Height = DM.h;
	win = SDL_CreateWindow("Well, that's unfortunate", 0, 0, Width, Height, SDL_WINDOW_SHOWN);
	SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN);
	if (win == nullptr) {
	  std::string error = SDL_GetError();
	  SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "%s", error.c_str());
	  throw std::runtime_error("Unable to create window (SDL2)");
	}

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr) {
	  SDL_DestroyWindow(win);
	  std::string error = SDL_GetError();
	  SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "%s", error.c_str());
	  throw std::runtime_error("Unable to create render (SDL2)");
	}

	uiManager = new ScreenManager(SDL_GetWindowSurface(win), ren, win,
								  nullptr);//init ScreenManager and font related stuff
	inputManager = new InputManager();
	uiManager = new ScreenManager(SDL_GetWindowSurface(win), ren, win,
								  inputManager);//init ScreenManager and font related stuff
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "SDL2 init - Good\nGame Start");
	field = new Field(uiManager);
	//showRules();
	run();// Starts the game
  }

 private:
  void showRules() {
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
							 "Rules",
							 "You can edit Field by pressing E.\n\nMade by Vladimir Shubarin\n",
							 NULL);
  }

  int run() {
	Uint64 frameStart;
	Uint64 frameTime = 0;
	Uint64 curTime = 0;
	Uint64 endTime = 0;

	int frameDelay = 1;
	bool showDialog = true;
	UI_MainMenu uiMainMenu(uiManager, win);
	while (!inputManager->quitEventCheck() && state != 'q') {
	  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	  if (SDL_RenderClear(ren) < 0) {
		SDL_DestroyWindow(win);
		std::string error = SDL_GetError();
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "%s", error.c_str());
		throw std::runtime_error("Unable to clear render (SDL2)");
	  }

	  //cout << state << endl;

	  inputManager->updateEvents();
	  if (frameTime >= 2000) {
		state = 'q';
		throw runtime_error("Game took too much time to render: " + to_string(frameTime));
	  }

	  frameStart = SDL_GetTicks();
	  curTime = SDL_GetTicks();
	  //cout << "Frame delay: " << frameTime << endl;
	  inputManager->updateEvents();
	  if (state == 'm') {
		uiMainMenu.show();
		state = uiMainMenu.act();
	  }
	  if (state == 'r') {
		field->redraw();
		uiManager->printText("Upper length: "+std::to_string(field->GetUpperLength()),10,10,{0,0,0,255},16, false);
		uiManager->printText("Lower length: "+std::to_string(field->GetLowerLength()),uiManager->getWindowResolutionX()-uiManager->getTextSize("Lower length: "+std::to_string(field->GetLowerLength()),16).a,10,{0,0,0,255},16, false);
		field->checkForGrab();
	  }
	  SDL_RenderPresent(ren);
	}
	delete inputManager;
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Destroying render");
	SDL_DestroyRenderer(ren);
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Render Destroyed\nDestroying window");
	SDL_DestroyWindow(win);
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Destroyed window");
	free(uiManager);
	return 0;
  }
};

#endif//PROGONHLANG_GAME_HPP
