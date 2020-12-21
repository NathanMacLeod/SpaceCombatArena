#pragma once
#include "../OLCEngine/PixelEngine3D.h"
#include "PhysicsObject.h"
#include "../Physics/PhysicsEngine.h"
#include "Asteroid.h";
#include "MovingObject.h"
#include "CooldownTimer.h"
#include "Player.h"
#include "Particle.h"
#include "Enemy.h"
#include "Ore.h"
#include "SpaceStation.h"
#include "Projectile.h"
#include "ShopMenu.h"
#include "AsteroidChunk.h"

class SpaceMinerGame : public PixelEngine3D {
public:

	PhysicsEngine* getPhysicsEngine();
	std::vector<Enemy*>* getEnemies();
	std::vector<Asteroid*>* getAsteroids();
	std::vector<Ore*>* getOre();
	Player* getPlayer();
	void addAsteroid(Asteroid* asteroid);
	void addOre(Ore* ore);
	void addEnemy(Enemy* enemy);
	void addParticle(Particle* particle);
	void addProjectile(Projectile* projectile);
	bool OnUserCreate();
	bool OnUserUpdate(float fElapsedTime);
	bool OnUserDestroy();
	bool canOpenShop();
	enum SFX { Pew, Damage, Explosion, Hit, Rocket, Spawn };
	void playSoundEffect(SFX soundEffect);

private:
	std::vector<Projectile*> projectiles;
	std::vector<Asteroid*> asteroids;
	std::vector<Particle*> particles;
	std::vector<Ore*> ores;
	std::vector<Enemy*> enemies;

	std::vector<int> damageSounds;
	std::vector<int> explosionSounds;
	std::vector<int> pewSounds;
	std::vector<int> rocketSounds;
	std::vector<int> hitSounds;
	std::vector<int> spawnSounds;
	int growlLoop;
	bool growlActive;
	int toneLoop;
	bool toneActive;

	enum WaveState {InWave, WaveMessage, InbetweenWave, GameOver, MainMenu, Instructions};
	WaveState currentState = MainMenu;
	int currWave = 0;
	int nSubWave = 0;
	CooldownTimer displayWaveMessage = CooldownTimer(2.5);
	CooldownTimer roundEndDelay = CooldownTimer(2.5);
	CooldownTimer gameOverMSGDelay = CooldownTimer(2.5);
	CooldownTimer gameOverMsgTime = CooldownTimer(2.5);
	CooldownTimer spawnDelay = CooldownTimer(2.0);
	float playButtonY = 0.55;
	float instructionButtonY = 0.75;
	float instructionsMsgY = 0.25;
	float backButtonY = 0.75;

	PhysicsEngine pEngine = PhysicsEngine(0.01667);
	Vector3D cameraPos;
	Vector3D panCameraDir;
	Rotor cameraOrientation;

	std::vector<Drawable*> walls; //temporary

	Player* player;
	bool tabHeld;

	double worldSize = 100000;

	int menuSize = 3;
	int instrSize = 2;
	std::string playStr = std::string("PLAY");
	std::string instrStr = std::string("CONTROLS");
	static const int nInstrMsg = 5;
	std::string instructions[nInstrMsg] = { std::string("Controls:"), std::string("WASD for movement, QE to roll, mouse for orientation"),
		std::string("Space bar is to shoot"), std::string("1 switches to guns, 2 switches to missiles"), std::string("Mouse left click will change selected targret") };
	
	std::string backMsg = std::string("RETURN");

	void update(std::vector<Updatable*>* updatables, float fElapsedTime);
	void removeExpired(std::vector<Expireable*>* expirables);

	void manageLoopedAudio();
	void clearEverything();
	void initMenu();
	void displayControls();
	void displayMainMenu();
	bool getMenuButtonHovered(int yVal, int textSize, int textLen);
	void clickMainMenu();
	void readInput(float fElapsedTime);
	void gameUpdate(float fElapsedTime);
	void spawnEnemies(int nNormal, int nElite);
	void gameRender();
	void handleGameStates(float fElapsedTime);
	void initAudio();
};