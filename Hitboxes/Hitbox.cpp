#include "Hitbox.h"

namespace json = nlohmann;
using namespace std;

void Hitbox::flip(sf::Sprite sprite)
{
	pos.x = sprite.getGlobalBounds().width - pos.x - size.x;
	flipped = !flipped;
}

std::map<std::string, std::vector<Hitbox>> Hitbox::getHitboxes(std::string filename, std::string name)
{
	std::map<std::string, std::vector<Hitbox>> hitboxes;
	json::json j;
	std::ifstream file(filename);

	file >> j;
	for(auto& f : j[name].items())
	{
		if (f.key() != "active")
		{
			hitboxes.emplace(f.key(), std::vector<Hitbox>());
			for (auto& g : f.value())
			{
				hitboxes[f.key()].push_back(Hitbox());
				auto& hitbox = hitboxes[f.key()][hitboxes[f.key()].size() - 1];
				hitbox.label = g["label"];
				hitbox.pos = sf::Vector2f(stoi(g["x"].dump()), stoi(g["y"].dump()));
				hitbox.size = sf::Vector2f(stoi(g["width"].dump()), stoi(g["height"].dump()));
				hitbox.type = g["type"];
				hitbox.start = stoi(g["start"].dump());
				hitbox.end = stoi(g["end"].dump());
				hitbox.anchor = sf::Vector2f(0, 0);
				hitbox.right = hitbox.anchor.x + hitbox.pos.x + hitbox.size.x;
				hitbox.left = hitbox.anchor.x + hitbox.pos.x;
				hitbox.top = hitbox.anchor.y + hitbox.pos.y;
				hitbox.bottom = hitbox.anchor.y + hitbox.pos.y + hitbox.size.y;
				hitbox.flipped = false;
			}
		}
	}

	return hitboxes;
}

bool Hitbox::isOver(Hitbox a, Hitbox b)
{
	return (
		(a.right >= b.left && a.right <= b.right && a.bottom <= b.bottom && a.bottom >= b.top) ||
		(a.right >= b.left && a.right <= b.right && a.top >= b.top && a.top <= b.bottom) ||
		(a.left <= b.right && a.left >= b.left && a.bottom <= b.bottom && a.bottom >= b.top) ||
		(a.left <= b.right && a.left >= b.left && a.top >= b.top && a.top <= b.bottom) ||
		(b.right >= a.left && b.right <= a.right && b.bottom <= a.bottom && b.bottom >= a.top) ||
		(b.right >= a.left && b.right <= a.right && b.top >= a.top && b.top <= a.bottom) ||
		(b.left <= a.right && b.left >= a.left && b.bottom <= a.bottom && b.bottom >= a.top) ||
		(b.left <= a.right && b.left >= a.left && b.top >= a.top && b.top <= a.bottom)
		);
}

bool Hitbox::doesHit(Hitboxed* a, Hitboxed* b)
{
	for(Hitbox& hurtbox : a->hitboxes[a->activeHitbox])
	{
		if(hurtbox.type == "hurtbox")
		{
			for(Hitbox& hitbox : b->hitboxes[b->activeHitbox])
			{
				if(hitbox.type == "hitbox" && isOver(hurtbox, hitbox))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool Hitbox::isOver(Hitboxed* a, Hitboxed* b)
{
	for (Hitbox& hurtbox : a->hitboxes[a->activeHitbox])
	{
		for (Hitbox& hitbox : b->hitboxes[b->activeHitbox])
		{
			if (isOver(hurtbox, hitbox))
			{
				return true;
			}
		}
	}
	return false;
}

sf::Vector2f Hitbox::getLocalPos() const
{
    return pos;
}

sf::Vector2f Hitbox::getGlobalPos() const
{
	return (anchor + pos);
}

void Hitbox::setPos(sf::Vector2f pos)
{
	setX(pos.x);
	setY(pos.y);
}

void Hitbox::setX(float x)
{
	anchor.x = x;
	right = anchor.x + pos.x + size.x;
	left = anchor.x + pos.x;
}

void Hitbox::setY(float y)
{
	anchor.y = y;
	bottom = anchor.y + pos.y + size.y;
	top = anchor.y + pos.y;
}
std::string Hitbox::getLabel() const
{
    return label;
}

std::string Hitbox::getType() const
{
    return type;
}

int Hitbox::getStart() const
{
    return start;
}

int Hitbox::getEnd() const
{
    return end;
}

sf::Vector2f Hitbox::getSize() const
{
    return size;
}

float Hitbox::getLeft() const
{
    return left;
}

float Hitbox::getRight() const
{
    return right;
}

float Hitbox::getBottom() const
{
    return bottom;
}

float Hitbox::getTop() const
{
    return top;
}

bool Hitbox::getFlipped() const
{
    return flipped;
}

void Hitboxed::moveHitboxes(sf::Vector2f v)
{
	for (auto& f : hitboxes)
	{
		for (Hitbox& hitbox : f.second)
		{
			hitbox.setPos(hitbox.getAnchor() + v);
		}
	}
}

void Hitboxed::setPosHitboxes(sf::Vector2f pos)
{
	for (auto& f : hitboxes)
	{
		for (Hitbox& hitbox : f.second)
		{
			hitbox.setPos(pos);
		}
	}
}

void Hitboxed::renderHitboxes(sf::RenderTarget& target)
{
	for (auto& hitbox : hitboxes[activeHitbox])
	{
		sf::RectangleShape rect;
		rect.setFillColor((hitbox.getType() == "hitbox" ? sf::Color(0,0,255,150) : sf::Color(255,0,0,150)));
		rect.setPosition(hitbox.getGlobalPos());
		rect.setSize(hitbox.getSize());
		target.draw(rect);
	}
}

sf::Vector2f Hitbox::getAnchor() const
{
    return anchor;
}

