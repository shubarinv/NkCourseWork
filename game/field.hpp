//
// Created by vhund on 02.02.2020.
//

#ifndef VL_COURSEWORK_FIELD_HPP

#include <list>
#include <vector>

#include "Object.hpp"
#include "game.hpp"
#include "screenManager.hpp"

class Field {
  struct twoInt {
	int a{0}, b{0};
  };

 private:
  ScreenManager *screenManager{};
  std::list<Object> objectsOnField{};
  SDL_Texture *weight_plate_left{};
  SDL_Texture *weight_plate_right{};
  SDL_Texture *weight_base{};
  int left_Weight{0};
  int right_Weight{0};

 public:
  explicit Field(ScreenManager *pScreenManager = nullptr) {
	if (pScreenManager == nullptr) {
	  SDL_LogCritical(SDL_LOG_CATEGORY_ERROR,
					  "Field::Field() got empty screenManager");
	  throw(std::runtime_error("Field::Field() pScreenManager is NULL"));
	}
	screenManager = pScreenManager;
	/// todo update sprites
	objectsOnField.emplace_back(10, "Sprites/Weights.png", screenManager, 5, screenManager->getWindowResolutionY() - 45, -1, -1, false, false);
	objectsOnField.emplace_back(100, "Sprites/Weights.png", screenManager, 5 + 40, screenManager->getWindowResolutionY() - 50, 45, 45, false, false);
	objectsOnField.emplace_back(1000, "Sprites/Weights.png", screenManager, 5 + 85, screenManager->getWindowResolutionY() - 55, 50, 50, false, false);
	objectsOnField.emplace_back(10000, "Sprites/Weights.png", screenManager, 5 + 135, screenManager->getWindowResolutionY() - 60, 55, 55, false, false);
	objectsOnField.emplace_back(100000, "Sprites/Weights.png", screenManager, 5 + 190, screenManager->getWindowResolutionY() - 65, 60, 60, false, false);
	objectsOnField.emplace_back(1000000, "Sprites/Weights.png", screenManager, 5 + 250, screenManager->getWindowResolutionY() - 70, 65, 65, false, false);
	objectsOnField.emplace_back(10000000, "Sprites/Weights.png", screenManager, 5 + 315, screenManager->getWindowResolutionY() - 75, 70, 70, false, false);
	objectsOnField.emplace_back(100000000, "Sprites/Weights.png", screenManager, 5 + 385, screenManager->getWindowResolutionY() - 80, 75, 75, false, false);
	objectsOnField.emplace_back(3500, "Sprites/cat.png", screenManager, screenManager->getWindowResolutionX() / 2 + 170, screenManager->getWindowResolutionY() - 75, 70, 70, false, false);
	objectsOnField.emplace_back(200, "Sprites/rat.png", screenManager, screenManager->getWindowResolutionX() / 2 + 275, screenManager->getWindowResolutionY() - 75, 70, 70, false, false);
	objectsOnField.emplace_back(2000000, "Sprites/car.png", screenManager, screenManager->getWindowResolutionX() / 2 + 380, screenManager->getWindowResolutionY() - 55, 150, 50, false, false);
	objectsOnField.emplace_back(200000000, "Sprites/F9.png", screenManager, screenManager->getWindowResolutionX() - 35, screenManager->getWindowResolutionY() - 245, 30, 240, true, false);
	weight_base = screenManager->loadTexture("Sprites/Weight_base.png");
	weight_plate_left = screenManager->loadTexture("Sprites/Weight_Plate.png");
	weight_plate_right = screenManager->loadTexture("Sprites/Weight_Plate.png");
  }

  char determinePosition(const Object *obj) {///< determines in which part of the screen is object located
	int l_offset{50};
	int r_offset{50};
	if (left_Weight > right_Weight)
	  l_offset = 100;
	if (right_Weight > left_Weight)
	  r_offset = 100;
	///@todo rewrite so that it will determine upper and lower part of the
	///screen instead if left/right
	if ((obj->getLocation().a >= 60 && obj->getLocation().a + obj->getSize().a < 60 + ScreenManager::getTextureSize(weight_plate_left).a) && obj->getLocation().b <= screenManager->getWindowResolutionY() / 2 + l_offset) {
	  return 'l';
	} else if ((obj->getLocation().a >= screenManager->getWindowResolutionX() - ScreenManager::getTextureSize(weight_plate_right).a - 60 && obj->getLocation().a + obj->getSize().a < screenManager->getWindowResolutionX() - 60) && obj->getLocation().b <= screenManager->getWindowResolutionY() / 2 + r_offset) {
	  return 'r';
	}
	return 'u';
  }

  ///@todo rewrite so that it will count object size instead of weight
  twoInt countSidesWeight() {///@brief counts weight of each side
	int leftWeight{0};
	int rightWeight{0};
	for (const Object &obj : objectsOnField) {
	  switch (determinePosition(&obj)) {
		case 'l':
		  leftWeight += obj.getWeight();
		  break;
		case 'r':
		  rightWeight += obj.getWeight();
		  break;
	  }
	}
	return {leftWeight, rightWeight};
  }

  void redraw() {       ///<@brief redraws game field
	addObjectsToLists();/// Todo remove this deprecated function
	screenManager->renderTexture(weight_base, screenManager->getWindowResolutionX() / 2 - ScreenManager::getTextureSize(weight_base).a / 2, screenManager->getWindowResolutionY() - ScreenManager::getTextureSize(weight_base).b);
	if (left_Weight > right_Weight) {
	  screenManager->renderTexture(weight_plate_left, 60, 150);
	  screenManager->renderTexture(weight_plate_left, screenManager->getWindowResolutionX() - ScreenManager::getTextureSize(weight_plate_right).a - 60, 50);
	} else if (right_Weight > left_Weight) {
	  screenManager->renderTexture(weight_plate_left, 60, 50);
	  screenManager->renderTexture(weight_plate_left, screenManager->getWindowResolutionX() - ScreenManager::getTextureSize(weight_plate_right).a - 60, 150);
	} else {
	  screenManager->renderTexture(weight_plate_left, 60, 100);
	  screenManager->renderTexture(weight_plate_left, screenManager->getWindowResolutionX() - ScreenManager::getTextureSize(weight_plate_right).a - 60, 100);
	}
	for (auto &obj : objectsOnField) {
	  obj.redraw();
	}
  }

  void
  checkForGrab() {///<@brief grabs object that is mouse hover if LMB is pressed
	twoInt mouseLocation;
	mouseLocation.a = screenManager->getInputManager()->getMouseCoords().x;
	mouseLocation.b = screenManager->getInputManager()->getMouseCoords().y;
	objectsOnField.remove_if(Object::removalCheck);
	if (screenManager->getInputManager()->getMouseState() & SDL_BUTTON_LMASK) {
	  // std::cout << "\n--------------\nLMB Pressed (" << SDL_GetTicks() << ")"
	  // << std::endl;
	  for (auto &i : objectsOnField) {
		if (i.getGrabbed())
		  return;
	  }
	  //	std::cout << "There is no grabbed OBJ" << std::endl;
	  for (auto &i : objectsOnField) {
		if (i.checkCollision({mouseLocation.a, mouseLocation.b},
							 &objectsOnField)) {
		  return;
		}
	  }
	} else {
	  for (auto &i : objectsOnField) {
		if (i.getGrabbed()) {
		  i.setIsGrabbed(false);
		  return;
		}
	  }
	}
  }

  [[deprecated]] void addObjectsToLists() {///<@todo remove this
	left_Weight = countSidesWeight().a;
	right_Weight = countSidesWeight().b;
  }
  void snapObjects() {///@todo add object side snapping
  }
};

#endif// VL_COURSEWORK_FIELD_HPP
