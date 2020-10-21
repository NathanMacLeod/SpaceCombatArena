#pragma once
#include "../OLCEngine/PixelEngine3D.h"
#include "PhysicsObject.h"
#include "../Physics/PhysicsEngine.h"
#include "Asteroid.h";
#include "MovingObject.h"
#include "Player.h"
#include "Particle.h"
#include "Enemy.h"
#include "Ore.h"
#include "SpaceStation.h"
#include "Projectile.h"
#include "ShopMenu.h"

class SpaceMinerGame : public PixelEngine3D {
private:
	std::vector<Projectile*> projectiles;
	std::vector<Asteroid*> asteroids;
	std::vector<Particle*> particles;
	std::vector<Ore*> ores;
	std::vector<Enemy*> enemies;

	std::vector<PhysicsObject*> stuff;

	PhysicsEngine pEngine = PhysicsEngine(0.01667);
	Vector3D cameraPos;
	Rotor cameraOrientation;

	std::vector<Drawable*> walls; //temporary

	Player* player;
	ShopMenu test;
	SpaceStation* station;

	void update(std::vector<Updatable*>* updatables, float fElapsedTime);
	void removeExpired(std::vector<Expireable*>* expirables);

	void readInput(float fElapsedTime);
	void gameUpdate(float fElapsedTime);
	void gameRender();
public:

	PhysicsEngine* getPhysicsEngine();
	std::vector<Enemy*>* getEnemies();
	std::vector<Asteroid*>* getAsteroids();
	std::vector<Ore*>* getOre();
	Player* getPlayer();
	void addOre(Ore* ore);
	void addEnemy(Enemy* enemy);
	void addParticle(Particle* particle);
	void addProjectile(Projectile* projectile);
	bool OnUserCreate();
	bool OnUserUpdate(float fElapsedTime);
};