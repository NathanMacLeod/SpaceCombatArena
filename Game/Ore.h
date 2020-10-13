#pragma once
#include "PhysicsObject.h"
#include "Updatable.h"

class Ore : public PhysicsObject, public Updatable {
public:
	static enum Material { Iron, Copper, Gold, Silver, Plutonium, Cybernium, Anthium };
	static const int N_TYPES = 7;

	Ore(Vector3D pos, Material type);
	double getValue();
	static Material pickTypeRandomly();
	
	void update(SpaceMinerGame* game, float fElapsedTime);
	void performDeathActions(SpaceMinerGame* game);
private:
	double dampenVal = 1.5;
	Material material;
	void dampen(float fElapsedTime);
};