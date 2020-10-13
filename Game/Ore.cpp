#include "Ore.h"
#include "SpaceMinerGame.h"
#include "Asteroid.h"


Ore::Ore(Vector3D pos, Material type) {
	double density = 1;
	double roughness = 0.2;
	double detail = 2;
	double size = 70;
	material = type;

	olc::Pixel lineColor;
	olc::Pixel color;

	switch (type) {
	case Iron:
		density = 1;
		lineColor = olc::VERY_DARK_RED;
		color = olc::VERY_DARK_GREY;
		break;
	case Copper:
		density = 1;
		lineColor = olc::VERY_DARK_RED;
		color = olc::Pixel(122, 45, 20);
		break;
	case Silver:
		density = 1;
		lineColor = olc::WHITE;
		color = olc::GREY;
		break;
	case Gold:
		density = 3;
		lineColor = olc::VERY_DARK_YELLOW;
		color = olc::YELLOW;
		break;
	case Plutonium:
		density = 1;
		lineColor = olc::GREEN;
		color = olc::DARK_GREY;
		break;
	case Cybernium:
		size = 100;
		roughness = 0.1;
		detail = 1;
		density = 1;
		lineColor = olc::CYAN;
		color = olc::DARK_CYAN;
		break;
	case Anthium:
		detail = 3;
		roughness = 0.4;
		density = 1;
		lineColor = olc::MAGENTA;
		color = olc::DARK_MAGENTA;
		break;
	}

	Asteroid::createRockMesh(pos, size, detail , density, roughness, lineColor, color, &body, &model);
}

Ore::Material Ore::pickTypeRandomly() {
	static float probVals[N_TYPES] =
	{
		35, //iron
		18, //copper
		4, //gold
		10, //silver
		2, //plutonium
		0.6, //cybernium
		0.35, //anthium
	};

	static float total = -1;
	if (total == -1) {
		total = 0;
		for (int i = 0; i < N_TYPES; i++) {
			total += probVals[i];
		}
		printf("total: %f\n", total);
	}

	int chosenVal = 1 + (fmod(rand(), total));
	for (int i = 0; i < N_TYPES; i++) {
		chosenVal -= probVals[i];
		if (chosenVal <= 0) {
			return (Ore::Material) i;
		}
	}
	
	//shouldnt reach here
	return Iron;
}

double Ore::getValue() {
	switch (material) {
	case Iron:
		return 1;
	case Copper:
		return 2;
	case Silver:
		return 10;
	case Gold:
		return 25;
	case Plutonium:
		return 50;
	case Cybernium:
		return 150;
	case Anthium:
		return 500;
	}
}

void Ore::performDeathActions(SpaceMinerGame* game) {
	game->getPhysicsEngine()->removeRigidBody(body);
}

void Ore::dampen(float fElapsedTime) {
	float t = std::fmin(1, fElapsedTime);
	Vector3D vel = body->getVelocity();
	body->setVelocity(vel.sub(vel.multiply(t * dampenVal)));
}

void Ore::update(SpaceMinerGame* game, float fElapsedTime) {
	dampen(fElapsedTime);
}