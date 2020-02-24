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
  int upperLength{0};
  int lowerLength{0};

 public:
  [[nodiscard]] int GetUpperLength() const {
	return upperLength;
  }
  [[nodiscard]] int GetLowerLength() const {
	return lowerLength;
  }

 public:
  explicit Field(ScreenManager *pScreenManager = nullptr) {
	if (pScreenManager == nullptr) {
	  SDL_LogCritical(SDL_LOG_CATEGORY_ERROR,
					  "Field::Field() got empty screenManager");
	  throw(std::runtime_error("Field::Field() pScreenManager is NULL"));
	}
	screenManager = pScreenManager;
	objectsOnField.emplace_back("Sprites/parrot.png", screenManager, 10, screenManager->getWindowResolutionY() - 32, 16, false);
	objectsOnField.emplace_back("Sprites/snake.png", screenManager, 64, screenManager->getWindowResolutionY() - 32, 64, false, 16 * 38);
	objectsOnField.emplace_back("Sprites/car.png", screenManager, 64 + 84, screenManager->getWindowResolutionY() - 32, 64, false, 460);
	objectsOnField.emplace_back("Sprites/Cat.png", screenManager, 64 + 84+84, screenManager->getWindowResolutionY() - 42, 64, false, 50);
	objectsOnField.emplace_back("Sprites/phone.jpg", screenManager, 64 + 84+84+84, screenManager->getWindowResolutionY() - 64, 32, false, 8);
	objectsOnField.emplace_back("Sprites/wheel.jpg", screenManager, 64 + 84+84+84+84, screenManager->getWindowResolutionY() - 64, 64, false, 64);
  }

  char determinePosition(const Object *obj) {///< determines in which part of the screen is object located
	if (obj->getLocation().b <= screenManager->getWindowResolutionY() / 2 - screenManager->getWindowResolutionY() / 10) {
	  return 'u';
	} else {
	  return 'l';
	}
  }

  twoInt countSidesSize() {///@brief counts weight of each side
	int upperSize{0};
	int lowerSize{0};
	for (const Object &obj : objectsOnField) {
	  switch (determinePosition(&obj)) {
		case 'u':
		  if (obj.IsGrabAble())
			upperSize += obj.getSize().a;
		  break;
		case 'l':
		  if (obj.IsGrabAble())
			lowerSize += obj.getSize().a;
		  break;
	  }
	}
	//std::cout << "==========\nUpper side " << upperSize << "\nLower side " << lowerSize << std::endl;
	return {upperSize, lowerSize};
  }

  void redraw() {///<@brief redraws game field
	reSnap();
	bool hasGrabbedObjects = false;
	for (auto &obj : objectsOnField) {
	  obj.redraw();
	  if (obj.getGrabbed()) {
		hasGrabbedObjects = true;
	  }
	}
	if (!hasGrabbedObjects) {
	  upperLength = countSidesSize().a;
	  lowerLength = countSidesSize().b;
	}
	SDL_SetRenderDrawColor(screenManager->getRenderer(), 255, 80, 80, 255);
	SDL_RenderDrawLine(screenManager->getRenderer(), upperLength - 1, screenManager->getWindowResolutionY() / 2, upperLength - 1, screenManager->getWindowResolutionY() / 2 - 40);
	SDL_RenderDrawLine(screenManager->getRenderer(), upperLength, screenManager->getWindowResolutionY() / 2, upperLength, screenManager->getWindowResolutionY() / 2 - 40);
	SDL_RenderDrawLine(screenManager->getRenderer(), upperLength + 1, screenManager->getWindowResolutionY() / 2, upperLength + 1, screenManager->getWindowResolutionY() / 2 - 40);
	if (upperLength < 100)
	  screenManager->printText(std::to_string(upperLength) + " CM", upperLength, screenManager->getWindowResolutionY() / 2 - 50, {255, 80, 80, 255}, 16, true);
	if (upperLength >= 100)
	  screenManager->printText(std::to_string(upperLength / 100) + " M  " + std::to_string(upperLength % 100) + " CM", upperLength, screenManager->getWindowResolutionY() / 2 - 50, {255, 80, 80, 255}, 16, true);
	SDL_SetRenderDrawColor(screenManager->getRenderer(), 80, 80, 255, 255);
	SDL_RenderDrawLine(screenManager->getRenderer(), lowerLength - 1, screenManager->getWindowResolutionY() / 2, lowerLength - 1, screenManager->getWindowResolutionY() / 2 + 40);
	SDL_RenderDrawLine(screenManager->getRenderer(), lowerLength, screenManager->getWindowResolutionY() / 2, lowerLength, screenManager->getWindowResolutionY() / 2 + 40);
	SDL_RenderDrawLine(screenManager->getRenderer(), lowerLength + 1, screenManager->getWindowResolutionY() / 2, lowerLength + 1, screenManager->getWindowResolutionY() / 2 + 40);
	if (lowerLength < 100)
	  screenManager->printText(std::to_string(lowerLength) + " CM", lowerLength, screenManager->getWindowResolutionY() / 2 + 50, {255, 80, 80, 255}, 16, true);
	if (lowerLength >= 100)
	  screenManager->printText(std::to_string(lowerLength / 100) + " M  " + std::to_string(lowerLength % 100) + " CM", lowerLength, screenManager->getWindowResolutionY() / 2 + 50, {255, 80, 80, 255}, 16, true);
  }

  void checkForGrab() {///<@brief grabs object that is mouse hover if LMB is pressed
	twoInt mouseLocation;
	mouseLocation.a = screenManager->getInputManager()->getMouseCoords().x;
	mouseLocation.b = screenManager->getInputManager()->getMouseCoords().y;

	if (screenManager->getInputManager()->getMouseState() & SDL_BUTTON_LMASK) {
	  for (auto &i : objectsOnField) {
		if (i.getGrabbed())
		  return;
	  }
	  for (auto &i : objectsOnField) {
		if (i.checkCollision({mouseLocation.a, mouseLocation.b}, &objectsOnField)) {
		  return;
		}
	  }
	} else {
	  for (auto &i : objectsOnField) {
		if (i.getGrabbed()) {
		  objectsOnField.remove_if(Object::removalCheck);
		  snapObject(&i);
		  upperLength = countSidesSize().a;
		  lowerLength = countSidesSize().b;
		  i.setIsGrabbed(false);
		  return;
		}
	  }
	}
  }

  void snapObject(Object *object) {
	if (determinePosition(object) == 'u') {
	  object->SetLocation({upperLength, 2 * (screenManager->getWindowResolutionY() / 6) - object->getSize().b});
	}
	if (determinePosition(object) == 'l') {
	  object->SetLocation({lowerLength, 5 * (screenManager->getWindowResolutionY() / 6) - object->getSize().b});
	}
  }
  void reSnap() {
	int upper_length = 0;
	int lower_length = 0;
	for (auto &obj : objectsOnField) {
	  if (!obj.getGrabbed() && obj.IsGrabAble()) {
		if (obj.getLocation().b <= screenManager->getWindowResolutionY() / 2 - screenManager->getWindowResolutionY() / 10) {// upper part of the screen
		  obj.SetLocation({upper_length, obj.getLocation().b});
		  upper_length += obj.getSize().a;
		} else {//lower part of the screen
		  obj.SetLocation({lower_length, obj.getLocation().b});
		  lower_length += obj.getSize().a;
		}
	  }
	}
  }
};

#endif// VL_COURSEWORK_FIELD_HPP
