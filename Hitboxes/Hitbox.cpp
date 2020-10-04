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
				hitbox.angle = stof(g["angle"].dump());
			}
		}
	}

	return hitboxes;
}

// Given three colinear points p, q, r, the function checks if 
// point q lies on line segment 'pr' 
bool onSegment(sf::Vector2f p, sf::Vector2f q, sf::Vector2f r)
{
	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
		return true;

	return false;
}

// To find orientation of ordered triplet (p, q, r). 
// The function returns following values 
// 0 --> p, q and r are colinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise 
int orientation(sf::Vector2f p, sf::Vector2f q, sf::Vector2f r)
{
	// See https://www.geeksforgeeks.org/orientation-3-ordered-points/ 
	// for details of below formula. 
	int val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // colinear 

	return (val > 0) ? 1 : 2; // clock or counterclock wise 
}

// The main function that returns true if line segment 'p1q1' 
// and 'p2q2' intersect. 
bool doIntersect(sf::Vector2f p1, sf::Vector2f q1, sf::Vector2f p2, sf::Vector2f q2)
{
	// Find the four orientations needed for general and 
	// special cases 
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case 
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases 
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1 
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	// p1, q1 and q2 are colinear and q2 lies on segment p1q1 
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2 
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2 
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases 
}

bool Hitbox::isOver(Hitbox a, Hitbox b)
{
	double c = 2 * M_PI / 360;
	vector<sf::Vector2f> av;
	av.push_back(sf::Vector2f(a.left, a.top));
	av.push_back(sf::Vector2f(a.left + a.size.x * cos(a.angle*c), a.top + a.size.x * sin(a.angle * c)));
	av.push_back(sf::Vector2f(a.left + a.size.x * cos(a.angle * c) - a.size.y * sin(a.angle * c), a.top + a.size.x * sin(a.angle * c) + a.size.y * cos(a.angle * c)));
	av.push_back(sf::Vector2f(a.left - a.size.y * sin(a.angle * c), a.top + a.size.y * cos(a.angle * c)));
	av.push_back(sf::Vector2f(a.left, a.top));
	vector<sf::Vector2f> bv;
	bv.push_back(sf::Vector2f(b.left, b.top));
	bv.push_back(sf::Vector2f(b.left + b.size.x * cos(b.angle * c), b.top + b.size.x * sin(b.angle * c)));
	bv.push_back(sf::Vector2f(b.left + b.size.x * cos(b.angle * c) - b.size.y * sin(b.angle * c), b.top + b.size.x * sin(b.angle * c) + b.size.y * cos(b.angle * c)));
	bv.push_back(sf::Vector2f(b.left - b.size.y * sin(b.angle * c), b.top + b.size.y * cos(b.angle * c)));
	bv.push_back(sf::Vector2f(b.left, b.top));

	
	for(int i = 0; i < av.size() - 1; i++)
	{
		for (int j = 0; j < bv.size() - 1; j++)
		{
			if (doIntersect(av[i], av[i + 1], bv[j], bv[j + 1]))
				return true;
		}
	}

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

void Hitbox::setType(std::string type)
{
	this->type = type;
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

void Hitbox::setSize(sf::Vector2f size)
{
	this->size = size;
	bottom = anchor.y + pos.y + size.y;
	top = anchor.y + pos.y;
	right = anchor.x + pos.x + size.x;
	left = anchor.x + pos.x;
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
	double c = 2 * M_PI / 360;
	for (auto& hitbox : hitboxes[activeHitbox])
	{
		sf::RectangleShape rect;
		rect.setFillColor((hitbox.getType() == "hitbox" ? sf::Color(0,0,255,150) : sf::Color(255,0,0,150)));
		rect.setPosition(hitbox.getGlobalPos());
		rect.setSize(hitbox.getSize());
		rect.rotate(hitbox.getAngle());
		target.draw(rect);
	}
}

void Hitboxed::setAngleHitboxes(double degrees)
{
	for (auto& f : hitboxes)
	{
		for (Hitbox& hitbox : f.second)
		{
			hitbox.setAngle(degrees);
		}
	}
}

void Hitboxed::rotateHitboxes(double degrees)
{
	for (auto& f : hitboxes)
	{
		for (Hitbox& hitbox : f.second)
		{
			hitbox.setAngle(hitbox.getAngle() + degrees);
		}
	}
}

void Hitboxed::setActiveHitbox(std::string label)
{
	activeHitbox = label;
}

sf::Vector2f Hitbox::getAnchor() const
{
    return anchor;
}


double Hitbox::getAngle() const
{
    return angle;
}

void Hitbox::setAngle(double degrees)
{
    this->angle = degrees;
}

void Hitbox::rotate(double degrees)
{
	angle = getAngle() + degrees;
}
