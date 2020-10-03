#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <map>
#include <fstream>
#include <limits>
#include <cmath>
#include <iostream>
#include "D:/CppLibs/nlohmann/json.hpp"
#define M_PI 3.14159265358979323846
class Hitbox;

class Hitboxed
{
public:
	std::map<std::string, std::vector<Hitbox>> hitboxes;
	std::string activeHitbox;
	void moveHitboxes(sf::Vector2f v);
	void setPosHitboxes(sf::Vector2f pos);
	void renderHitboxes(sf::RenderTarget& target);
};

class Hitbox
{
public:
	void flip(sf::Sprite sprite);	
	static std::map<std::string, std::vector<Hitbox>> getHitboxes(std::string filename, std::string name);
	static bool isOver(Hitbox a, Hitbox b);
	static bool doesHit(Hitboxed* a, Hitboxed* b);
	static bool isOver(Hitboxed* a, Hitboxed* b);

private:

	std::string label;
	std::string type = "hitbox";
	int start = 0;
	int end = 0;
	sf::Vector2f pos;
	sf::Vector2f anchor;
	sf::Vector2f size;
	float left, right;
	float top, bottom;
	bool flipped;
	float angle;

public:

    sf::Vector2f getLocalPos() const;
    sf::Vector2f getGlobalPos() const;
    void setPos(sf::Vector2f pos);
	void setX(float x);
	void setY(float y);

    std::string getLabel() const;

    std::string getType() const;

    int getStart() const;

    int getEnd() const;

    sf::Vector2f getSize() const;

    float getLeft() const;

    float getRight() const;

    float getBottom() const;

    float getTop() const;

    bool getFlipped() const;

    sf::Vector2f getAnchor() const;

    float getAngle() const;
    void setAngle(float angle);

};

