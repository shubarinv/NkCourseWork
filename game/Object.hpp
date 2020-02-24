//
// Created by vhund on 02.02.2020.
//

#ifndef VL_COURSEWORK_OBJECT_HPP
#define VL_COURSEWORK_OBJECT_HPP


#include <string>
#include <iostream>
#include <SDL_render.h>
#include <SDL_log.h>
#include "screenManager.hpp"
#include "game.hpp"

class Object {
	struct twoInt {
		int a{0}, b{0};
	};
private:
	ScreenManager *screenManager{};
	twoInt location;
	SDL_Texture *sprite;
	twoInt size;
	twoInt offset;
	bool isGrabAble{true};
public:
	[[nodiscard]] const twoInt &getLocation() const {
		return location;
	}

	[[nodiscard]] const twoInt &getSize() const {
		return size;
	}

private:
	std::string spriteName{};
	int weight{0};
	bool isGrabbed{false};
public:
	[[nodiscard]] int getWeight() const {
		return weight;
	}

	[[nodiscard]] bool getGrabbed() const {
		return isGrabbed;
	}

	void redraw() {
		updateLocation();
		screenManager->renderTexture(sprite, location.a, location.b, size.a, size.b);
		printWeightLabel();
//		std::cout<<this<< " is grabbed: "<<getGrabbed()<<std::endl;
	}

	void setIsGrabbed(bool _isGrabbed) {
		isGrabbed = _isGrabbed;
	}

	void setOffset(int x, int y) {
		offset.a = x;
		offset.b = y;
	}

	bool checkCollision(twoInt x_y, std::list<Object> *objects) {
		if ((x_y.a >= location.a && x_y.a <= location.a + size.a) &&
		    (x_y.b >= location.b && x_y.b <= location.b + size.b)) {
			//		std::cout << "Hover over: " << this << std::endl;
			if(!isGrabAble){
				objects->emplace_back(weight,spriteName,screenManager,location.a,location.b,size.a,size.b,true,true);
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
			location.a = screenManager->getInputManager()->getMouseCoords().x + offset.a; ///< x
			location.b = screenManager->getInputManager()->getMouseCoords().y + offset.b; ///< y
		}
	}


	static bool removalCheck(const Object &obj) {
		return (obj.isGrabAble&&(obj.getLocation().b<0));
	}
	void setWeight(int _weight) {
		if (_weight >= 0)
			weight = _weight;
		else {
			std::cout << "Invalid Weight: _weight is negative" << std::endl;
			weight = 0;
		}
	}

	explicit Object(int _weight = 0, std::string _fileName = "", ScreenManager *pScreenManager = nullptr, int x = 100,
	                int y = 200, int w = 40, int h = 40, bool ignoreSizeLimit = false, bool _isGrabAble = true) {
		if (pScreenManager == nullptr) {
			SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Object::Object() got empty screenManager");
			throw (std::runtime_error("Object::Object() pScreenManager is NULL"));
		}
		screenManager = pScreenManager;

		if (_weight >= 0)
			weight = _weight;
		else {
			std::cout << "Invalid Weight: _weight is negative" << std::endl;
			weight = 0;
		}

		spriteName = std::move(_fileName);
		sprite = screenManager->loadTexture(spriteName);
		isGrabAble = _isGrabAble;
		location.a = x;
		location.b = y;

		if (w == -1 || h == -1) {
			size.a = ScreenManager::getTextureSize(sprite).a;
			size.b = ScreenManager::getTextureSize(sprite).a;
		} else {
			if (!ignoreSizeLimit) {
				if (w < 40) size.a = 40;
				if (h < 40) {
					size.b = 40;
					return;
				}
			}
			size.a = w;
			size.b = h;

		}
	}

///@todo print size instead
	void printWeightLabel() {
		if (weight >= 1000000) {
			screenManager->printText(std::to_string(weight / 1000000) + " T", location.a + size.a / 2,
			                         location.b + size.b / 2, {255, 0, 0}, 15, true);;
		} else if (weight >= 1000 && weight < 1000000) {
			screenManager->printText(std::to_string((int) (weight / 1000)) + " KG", location.a + size.a / 2,
			                         location.b + size.b / 2, {255, 0, 0}, 15, true);
		} else if (weight < 1000) {
			screenManager->printText(std::to_string(weight) + " G", location.a + size.a / 2, location.b + size.b / 2,
			                         {255, 0, 0}, 15,
			                         true);
		}
	}




};


#endif //VL_COURSEWORK_OBJECT_HPP
