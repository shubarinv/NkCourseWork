//
// Created by vhundef on 24.12.2019.
//

#ifndef PROGONHLANG_GAME_HPP
#define PROGONHLANG_GAME_HPP


#include <SDL.h>
#include <iostream>
#include <stdexcept>
#include "screenManager.hpp"
#include "UI/ui_main_menu.hpp"
#include "input_manager.hpp"
#include "field.hpp"

using namespace std;

class Game {
private:
	SDL_DisplayMode DM{};
	SDL_Window *win{};
	SDL_Renderer *ren{};
	InputManager *inputManager{};
	ScreenManager *uiManager;
	Field*field{};
	Uint64 eButtonPress = 0;
	SDL_Surface *baseLayer;
	char state = 'm';///< r-playing game| p-pause| m-main_Menu| e-Editing Field
public:
	Game() {
		std::cout << "Trying to init SDL2..." << std::endl;
		if (SDL_Init(SDL_INIT_VIDEO) != 0) {
			std::string error = SDL_GetError();
			SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "%s", error.c_str());
			throw std::runtime_error("Unable to init SDL2");
		}
		SDL_GetCurrentDisplayMode(0, &DM);
		auto Width = DM.w;
		auto Height = DM.h;
		win = SDL_CreateWindow("WeightsComp", 0, 40, 1280, 720, SDL_WINDOW_SHOWN & SDL_WINDOW_OPENGL);

		if (win == nullptr) {
			std::string error = SDL_GetError();
			SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "%s", error.c_str());
			throw std::runtime_error("Unable to create window (SDL2)");
		}

		ren = SDL_CreateRenderer(win, -1, 0);
		if (ren == nullptr) {
			SDL_DestroyWindow(win);
			std::string error = SDL_GetError();
			SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "%s", error.c_str());
			throw std::runtime_error("Unable to create render (SDL2)");
		}

		uiManager = new ScreenManager(SDL_GetWindowSurface(win), ren, win,
		                              nullptr); //init ScreenManager and font related stuff
		inputManager = new InputManager();
		uiManager = new ScreenManager(SDL_GetWindowSurface(win), ren, win,
		                              inputManager); //init ScreenManager and font related stuff
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "SDL2 init - Good\nGame Start");
		field=new Field(uiManager);
//showRules();
		run(); // Starts the game
	}

	struct twoInt {
		int a{0}, b{0};
	};
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

		int frameDelay = 4;
		bool showDialog = true;
		UI_MainMenu uiMainMenu(uiManager, win);
		while (!inputManager->quitEventCheck() && state != 'q') {
			SDL_SetRenderDrawColor(ren, 86, 86, 86, 255);
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
			if(state=='r'){
				field->redraw();
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


#endif //PROGONHLANG_GAME_HPP
