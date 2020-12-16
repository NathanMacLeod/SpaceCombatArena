#pragma once
#include "MovingObject.h"
#include "Updatable.h"
#include "CooldownTimer.h"
#include "Enemy.h"
#include "Ore.h"
#include "Asteroid.h"
#include "Damageable.h"
//#include "SpaceMinerGame.h"

class Player : public MovingObject, public Updatable, public Damageable {
public:
	static const int KM_CONST = 20000;
	enum GunType { Default, Twin, Gatling };
	enum Tool { Guns, Missiles };

	struct UpgradeProfile {
		UpgradeProfile();
		void applyUpgrade(const UpgradeProfile& upgrades);
		GunType gun;
		bool hasMissiles;
		bool hasGunsight;
		int inventoryLevel;
		int thrustLevel;
		int radarLevel;
	};

	void applyUpgradeProfile(const UpgradeProfile& upgrades);
	int* getInventory();
	Player(Vector3D position, GunType gun, SpaceMinerGame* g);
	void setEquippedTool(Tool t);
	void update(SpaceMinerGame* game, float fElapsedTime);
	void shoot(SpaceMinerGame* game);
	void drawPlayerUI(SpaceMinerGame* g, double FOV);
	void drawInventory(SpaceMinerGame* g);
	void getCameraPosOrient(Vector3D* posOut, Rotor* orientOut);
	void pickUpOre(SpaceMinerGame* game);
	void selectTarget(SpaceMinerGame* game);
	void clickInventory(SpaceMinerGame* game);
	void alterMoney(int deltaCash);
	void damage(double damage);
	void roundReset();
	void performDeathActions(SpaceMinerGame* game);
	bool isExpired();
	int getMoney();
private:
	
	double radarRange = 1;
	double radarSize = 0.7;
	std::vector<Vector3D> radarPoints;
	Vector3D radarPos;
	 
	int invSize = 80;
	Vector3D invOrePos[Ore::N_TYPES];
	void initInvPos(SpaceMinerGame* g);
	bool getInvButtonHovered(Ore::Material m, int mouseX, int mouseY);

	Enemy* target;
	uint16_t targetID;
	Vector3D targetPrevVel;
	double timeStep;
	Vector3D savedPrevVel;
	bool targetLocked;
	CooldownTimer targetLockTime = CooldownTimer(2);
	CooldownTimer missileRechargeTime = CooldownTimer(20);
	CooldownTimer missileFireRate = CooldownTimer(0.5);
	CooldownTimer damageDisplayTime = CooldownTimer(0.5);
	double missileLockAngle = 3.14159 / 8;
	double missileLockCos;

	int chunkX = -1;
	int chunkY = -1;
	int chunkZ = -1;

	Tool equippedTool;

	double grabDist;
	double maxGrabAng = cos(3.14159 / 8);
	double maxLockAng = cos(3.14159 * 60.0 / 180);
	double bulletVel = 4500;
	CooldownTimer inventoryFullMessageTime = CooldownTimer(1.5);
	int inventoryMaxSize = 10;
	int inventory[Ore::N_TYPES];
	int money;
	CooldownTimer shootTimer = CooldownTimer(0.25);
	GunType gun;
	int nMaxRockets;
	int nRockets;

	double getKMToP(Vector3D p);
	Asteroid* getTargetedAsteroid(SpaceMinerGame* game);
	Ore* getOreToGrab(SpaceMinerGame* game);
	void fireRocket(SpaceMinerGame* game);
	void fireBullet(SpaceMinerGame* game);
	void createRadarPoints();
	void manageChunks(SpaceMinerGame* game);
	void loadChunks(SpaceMinerGame* game, int prevX, int prevY, int prevZ, int currX, int currY, int currZ);

	bool hasRockets;
	bool hasGunsight;
};