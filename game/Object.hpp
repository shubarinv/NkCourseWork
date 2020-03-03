//
// Created by vhund on 02.02.2020.
//

#ifndef VL_COURSEWORK_OBJECT_HPP
#define VL_COURSEWORK_OBJECT_HPP

#include <SDL_log.h>
#include <SDL_render.h>

#include <iostream>
#include <string>

#include "game.hpp"
#include "screenManager.hpp"

class Object {
  struct twoInt {
	int a{0}, b{0};
  };

 private:
  ScreenManager *screenManager{};
  twoInt location;
  int widthSpawnOverride = 0;

 public:
  void SetLocation(const twoInt &location) {
	Object::location = location;
  }

 private:
  SDL_Texture *sprite;
  twoInt size;
  twoInt offset;
  bool isGrabAble{true};

 public:
  [[nodiscard]] bool IsGrabAble() const {
	return isGrabAble;
  }

 public:
  [[nodiscard]] const twoInt &getLocation() const {
	return location;
  }

  [[nodiscard]] const twoInt &getSize() const {
	return size;
  }

 private:
  std::string spriteName{};
  bool isGrabbed{false};

 public:
  [[nodiscard]] bool getGrabbed() const {
	return isGrabbed;
  }

  void redraw() {
	updateLocation();
	screenManager->renderTexture(sprite, location.a, location.b, size.a, size.b);
	if (!isGrabAble) printObjectSize();
  }

  void setIsGrabbed(bool _isGrabbed) {
	isGrabbed = _isGrabbed;
  }

  void setOffset(int x, int y) {
	offset.a = x;
	offset.b = y;
  }

  bool checkCollision(twoInt x_y, std::list<Object> *objects) {
	if ((x_y.a >= location.a && x_y.a <= location.a + size.a) && (x_y.b >= location.b && x_y.b <= location.b + size.b)) {
	  if (!isGrabAble) {
		if (widthSpawnOverride > 0) objects->emplace_back(spriteName, screenManager, location.a, location.b, widthSpawnOverride, true);
		else
		  objects->emplace_back(spriteName, screenManager, location.a, location.b, size.a, true);
		objects->back().setOffset(location.a - x_y.a, location.b - x_y.b);
		objects->back().setIsGrabbed(true);
		return true;
	  }
	  setOffset(location.a - x_y.a, location.b - x_y.b);
	  setIsGrabbed(true);
	  return true;
	}
	isGrabbed = false;
	return false;
  }

  void updateLocation() {
	if (isGrabbed) {
	  location.a = screenManager->getInputManager()->getMouseCoords().x + offset.a;///< x
	  location.b = screenManager->getInputManager()->getMouseCoords().y + offset.b;///< y
	}
  }

  static bool removalCheck(const Object &obj) {
	return (obj.isGrabAble && (obj.getLocation().b < 0));
  }

  explicit Object(std::string _fileName = "", ScreenManager *pScreenManager = nullptr, int x = 100, int y = 200, int _width = -1, bool _isGrabAble = true, int _widthSpawnOverride = 0) {
	if (pScreenManager == nullptr) {
	  SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Object::Object() got empty screenManager");
	  throw(std::runtime_error("Object::Object() pScreenManager is NULL"));
	}
	screenManager = pScreenManager;
	spriteName = std::move(_fileName);
	sprite = screenManager->loadTexture(spriteName);
	isGrabAble = _isGrabAble;
	location.a = x;
	location.b = y;
	if (_width == -1) {
	  size.a = ScreenManager::getTextureSize(sprite).a * screenManager->getWindowResolutionX() / 1280;
	  size.b = ScreenManager::getTextureSize(sprite).b * screenManager->getWindowResolutionY() / 720;
	} else {
	  size.a = (int)(_width * (screenManager->getWindowResolutionX() / 1920.f));
	  if (size.a < size.b) {
		size.b = ((float)ScreenManager::getTextureSize(sprite).a / ScreenManager::getTextureSize(sprite).b) * size.a;
	  } else {
		size.b = ((float)ScreenManager::getTextureSize(sprite).b / ScreenManager::getTextureSize(sprite).a) * size.a;
	  }
	}
	widthSpawnOverride = _widthSpawnOverride;
  }

  void printObjectSize() {
	if (widthSpawnOverride > 0) {
	  if (widthSpawnOverride < 100) { screenManager->printText(std::to_string(widthSpawnOverride) + " См", location.a + size.a / 2, location.b - 5, {255, 0, 0},
															   15, true); }
	  if (widthSpawnOverride >= 100) { screenManager->printText(std::to_string(widthSpawnOverride / 100) + " М   " + std::to_string(widthSpawnOverride % 100) + " CM", location.a + size.a / 2, location.b - 5, {255, 0, 0}, 15, true); }
	} else {
	  if (size.a < 100) { screenManager->printText(std::to_string(size.a) + " См", location.a + size.a / 2, location.b - 5, {255, 0, 0},
												   15, true); }
	  if (size.a >= 100) { screenManager->printText(std::to_string(size.a / 100) + " М   " + std::to_string(size.a % 100) + " CM", location.a + size.a / 2, location.b - 5, {255, 0, 0}, 15, true); }
	}
  }
};

#endif//VL_COURSEWORK_OBJECT_HPP
