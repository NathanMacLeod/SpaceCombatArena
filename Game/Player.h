#pragma once
#include "MovingObject.h"
#include "Updatable.h"
#include "CooldownTimer.h"
#include "Ore.h"
//#include "SpaceMinerGame.h"

class Player : public MovingObject, public Updatable {
public:
	enum GunType { Default, Twin, Gatling };
private:
	int inventory[Ore::N_TYPES];
	CooldownTimer shootTimer = CooldownTimer(0.25);
	GunType gun;

	void createStructure();

public:
	Player(Vector3D position, GunType gun);
	void update(SpaceMinerGame* game, float fElapsedTime);
	void shoot(SpaceMinerGame* game);
	void fireRocket(SpaceMinerGame* game);
};