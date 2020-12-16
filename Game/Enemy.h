#pragma once
#include "MovingObject.h"
#include "Updatable.h"
#include "CooldownTimer.h"
#include "Damageable.h"
#include "../Physics/ConvexHull.h"

class Enemy : public MovingObject, public Updatable, public Damageable {

public:

	enum EnemyType {Normal, Elite};

	Enemy(Vector3D pos, Rotor orientation, EnemyType type=Normal);
	~Enemy() {}
	void performDeathActions(SpaceMinerGame* game);
	
	void damage(double damage);
	void update(SpaceMinerGame* game, float fElapsedTime);
	void shoot(SpaceMinerGame* game);
	void aimAtTarget(Vector3D relPos, double angleOff, float fElapsedTime);
	void warpParticles(SpaceMinerGame* game);
	//void draw(PixelEngine3D* g, Vector3D cameraPos, Rotor cameraDir, double FOV);

private:

	EnemyType type;
	CooldownTimer shootTimer = CooldownTimer(0.4);
	CooldownTimer missileTimer = CooldownTimer(25);

	Vector3D smokePoint;
	double bulletVel = 3250;
	double fireCone = cos(3.14159 / 7);
	bool damping = true;
	double size;

};