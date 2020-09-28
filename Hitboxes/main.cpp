#include "Hitbox.h"
using namespace std;

class Player : public Hitboxed
{

};

int main()
{
	auto a = Hitbox::getHitboxes("hitboxes.json", "player");
	
	Player p1;
	Player p2;
	p1.hitboxes = a;
	p1.activeHitbox = "punch";
	p2.activeHitbox = "still";
	p2.hitboxes = a;

	p2.moveHitboxes(sf::Vector2f(250, 0));
	sf::RenderWindow window(sf::VideoMode(800, 600), "Window");

	while(window.isOpen())
	{
		p2.moveHitboxes(sf::Vector2f(-0.01, 0));
		if (Hitbox::doesHit(&p1, &p2))
			cout << "ok" << endl;
		window.clear();
		p1.renderHitboxes(window);
		p2.renderHitboxes(window);
		window.display();
	}

	system("pause");
	return 0;
}