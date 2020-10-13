#define OLC_PGE_APPLICATION
#include "SpaceMinerGame.h"
#include "Player.h"
#include "Windows.h"

//ScreenHeight()
//ScreenWidth()
//Draw(x, y, olc::Pixel(r, g, b));

std::vector<Polygon3D>* createBox(double x, double y, double z, double xPos, double yPos, double zPos) {
	std::vector<Polygon3D>* polygons = new std::vector<Polygon3D>();
	Vector3D p1 = Vector3D(-x / 2.0, -y / 2.0, -z / 2.0);
	Vector3D p2 = Vector3D(-x / 2.0, y / 2.0, -z / 2.0);
	Vector3D p3 = Vector3D(x / 2.0, -y / 2.0, -z / 2.0);
	Vector3D p4 = Vector3D(x / 2.0, y / 2.0, -z / 2.0);
	Vector3D p5 = Vector3D(-x / 2.0, -y / 2.0, z / 2.0);
	Vector3D p6 = Vector3D(-x / 2.0, y / 2.0, z / 2.0);
	Vector3D p7 = Vector3D(x / 2.0, -y / 2.0, z / 2.0);
	Vector3D p8 = Vector3D(x / 2.0, y / 2.0, z / 2.0);

	Vector3D translation(xPos, yPos, zPos);
	std::vector<Vector3D*> points;
	points.push_back(&p1);
	points.push_back(&p2);
	points.push_back(&p3);
	points.push_back(&p4);
	points.push_back(&p5);
	points.push_back(&p6);
	points.push_back(&p7);
	points.push_back(&p8);
	transformation3D::translatePoints(&points, translation);

	polygons->push_back(Polygon3D(p1, p2, p3, olc::WHITE, olc::BLACK));
	polygons->push_back(Polygon3D(p2, p4, p3, olc::WHITE, olc::BLACK));

	polygons->push_back(Polygon3D(p5, p7, p6, olc::WHITE, olc::BLACK));
	polygons->push_back(Polygon3D(p6, p7, p8, olc::WHITE, olc::BLACK));

	polygons->push_back(Polygon3D(p3, p4, p7, olc::WHITE, olc::BLACK));
	polygons->push_back(Polygon3D(p7, p4, p8, olc::WHITE, olc::BLACK));

	polygons->push_back(Polygon3D(p1, p5, p2, olc::WHITE, olc::BLACK));
	polygons->push_back(Polygon3D(p5, p6, p2, olc::WHITE, olc::BLACK));

	polygons->push_back(Polygon3D(p2, p6, p4, olc::WHITE, olc::BLACK));
	polygons->push_back(Polygon3D(p6, p8, p4, olc::WHITE, olc::BLACK));

	polygons->push_back(Polygon3D(p1, p3, p5, olc::WHITE, olc::BLACK));
	polygons->push_back(Polygon3D(p5, p3, p7, olc::WHITE, olc::BLACK));

	return polygons;
}

std::vector<RigidSurface*>* createRigidBodyFromPolygons(std::vector<Polygon3D>* polygons) {
	std::vector<RigidSurface*>* surfaces = new std::vector<RigidSurface*>();
	for (Polygon3D polygon : *polygons) {
		Vector3D v1 = polygon.p2.sub(polygon.p1);
		Vector3D v2 = polygon.p3.sub(polygon.p1); 
		Vector3D normalVector = (v1.crossProduct(v2)).getUnitVector();
		std::vector<Vector3D> points;
		points.push_back(polygon.p1);
		points.push_back(polygon.p2);
		points.push_back(polygon.p3);
		RigidSurface* surface = new RigidSurface(&points, normalVector);
		surfaces->push_back(surface);
	}
	return surfaces;
}

bool SpaceMinerGame::OnUserCreate() {

	station = new SpaceStation(this, Vector3D(0, 0, 0));

	player = new Player(Vector3D(200, 0, 0), Player::Default);
	pEngine.addRigidBody(player->getRigidBody());


	initZBuffer();
	
	int worldSize = 100000;

	/*std::vector<Polygon3D>* b1polygons = createBox(5000, 100, 5000, 0, 5000 / 2.0, 0);
	std::vector<ConvexHull*>* hulls = new std::vector<ConvexHull*>();
	hulls->push_back(new ConvexHull(createRigidBodyFromPolygons(b1polygons), 1));
	RigidBody* b1 = new RigidBody(*hulls, 1, 1, 0.3, true);
	pEngine.addRigidBody(b1);
	walls.push_back(new PhysicsObject(new PolyModel(b1polygons, b1->getCenterOfMass()), b1));*/

	for (int i = 0; i < 300; i++) {
		Asteroid* asteroid = new Asteroid(Vector3D(worldSize/2.0 * (1 - 2 * (float)rand()/RAND_MAX), worldSize/2.0 * (1 - 2 * (float)rand() / RAND_MAX), worldSize/2.0 * (1 - 2 * (float)rand() / RAND_MAX)), 
			Asteroid::pickAsteroidSize(), 3);
		asteroids.push_back(asteroid);
		pEngine.addRigidBody(asteroid->getRigidBody());
		int v = 200;
		asteroid->getRigidBody()->setVelocity(Vector3D(2 * (rand() % v) - v, 2 * (rand() % v) - v, 2 * (rand() % v) - v));
	}

	double x = 2500;// 52;
	double y = 0;
	double z = 50;
	std::vector<Polygon3D>* b1polygons = createBox(40, 100, 40, x, y, z);
	std::vector<Polygon3D>* b2polygons = createBox(280, 60, 60, x, y - 50, z);
	std::vector<ConvexHull*>* hulls = new std::vector<ConvexHull*>();
	hulls->push_back(new ConvexHull(createRigidBodyFromPolygons(b1polygons), 1));
	hulls->push_back(new ConvexHull(createRigidBodyFromPolygons(b2polygons), 1));
	RigidBody* b1 = new RigidBody(*hulls, 1, 1, 0.3, false);
	int v = 20;
	b1->setAngVelocity(Vector3D(0, 5, 0.00001).multiply(10));

	std::vector<Polygon3D> polygons;

	for (Polygon3D p : *b1polygons) {
		polygons.push_back(p);
	}
	for (Polygon3D p : *b2polygons) {
		polygons.push_back(p);
	}

	stuff.push_back(new PhysicsObject(new PolyModel(&polygons, b1->getCenterOfMass()), b1));

	delete b1polygons;
	delete b2polygons;

	pEngine.addRigidBody(b1);



	pEngine.setGravity(Vector3D(0, 0, 0));
	pEngine.setOctree(true, Vector3D(0, 0, 0), worldSize * 2, 300);

	return true;
}

void SpaceMinerGame::readInput(float fElapsedTime) {
	POINT mouseOld;

	POINT mouseNew;
	GetCursorPos(&mouseNew);

	printf("%d, %d\n", mouseNew, mouseNew);

	if (GetKey(olc::Key::W).bHeld) {
		player->accelForward(650, fElapsedTime);
	}
	if (GetKey(olc::Key::S).bHeld) {
		player->accelForward(-650, fElapsedTime);
	}
	if (GetKey(olc::Key::A).bHeld) {
		player->accelDrift(-650, fElapsedTime);
	}
	if (GetKey(olc::Key::D).bHeld) {
		player->accelDrift(650, fElapsedTime);
	}
	if (GetKey(olc::Key::SPACE).bHeld) {
		player->shoot(this);
	}
	if (GetKey(olc::Key::CTRL).bHeld) {
		player->accelLift(-650, fElapsedTime);
	}
	if (GetKey(olc::Key::UP).bHeld) {
		player->pitch(3, fElapsedTime);
	}
	if (GetKey(olc::Key::LEFT).bHeld) {
		player->yaw(3, fElapsedTime);
	}
	if (GetKey(olc::Key::DOWN).bHeld) {
		player->pitch(-3, fElapsedTime);
	}
	if (GetKey(olc::Key::RIGHT).bHeld) {
		player->yaw(-3, fElapsedTime);
	}
	if (GetKey(olc::Key::Q).bHeld) {
		player->roll(-3, fElapsedTime);
	}
	if (GetKey(olc::Key::E).bHeld) {
		player->roll(3, fElapsedTime);
	}
	if (GetKey(olc::Key::R).bPressed) {
		player->fireRocket(this);
	}
}

void SpaceMinerGame::gameUpdate(float fElapsedTime) {
	pEngine.iterateEngine(fElapsedTime);

	readInput(fElapsedTime);
	player->update(this, fElapsedTime);
	station->update(this, fElapsedTime);

	for (Projectile* p : projectiles) {
		p->update(this, fElapsedTime);
	}
	for (Particle* p : particles) {
		p->update(this, fElapsedTime);
	}
	for (Enemy* e : enemies) {
		e->update(this, fElapsedTime);
	}

	for (int i = 0; i < projectiles.size(); i++) {
		Projectile* p = projectiles.at(i);
		if (p->isExpired()) {
			p->performDeathActions(this);
			projectiles.erase(projectiles.begin() + i);
			i--;
			delete p;
			p = nullptr;
		}
	}

	for (int i = 0; i < particles.size(); i++) {
		Particle* p = particles.at(i);
		if (p->isExpired()) {
			particles.erase(particles.begin() + i);
			i--;
			delete p;
			p = nullptr;
		}
	}

	for (int i = 0; i < enemies.size(); i++) {
		Enemy* e = enemies.at(i);
		if (e->isExpired()) {
			e->performDeathActions(this);
			enemies.erase(enemies.begin() + i);
			i--;
			delete e;
			e = nullptr;
		}
	}

	for (int i = 0; i < asteroids.size(); i++) {
		Asteroid* a = asteroids.at(i);
		if (a->isExpired()) {
			a->performDeathActions(this);
			asteroids.erase(asteroids.begin() + i);
			i--;
			delete a;
			a = nullptr;
		}
	}
}

void SpaceMinerGame::gameRender() {
	static double fv = 300;

	clearZBuffer();
	Clear(olc::BLACK);

	cameraOrientation = Rotor(Vector3D(0, 1, 0), 3.14159 / 2.0).applyRotor(player->getRigidBody()->getOrientation());
	cameraPos = cameraOrientation.rotate(Vector3D(0, 0, 65)).add(player->getRigidBody()->getCenterOfMass());
	//cameraPos = test->body->getCenterOfMass();

	player->draw(this, cameraPos, cameraOrientation, fv);
	station->draw(this, cameraPos, cameraOrientation, fv);
	for (Projectile* p : projectiles) {
		p->draw(this, cameraPos, cameraOrientation, fv);
	}
	for (Asteroid* a : asteroids) {
		a->draw(this, cameraPos, cameraOrientation, fv);
	}
	for (Particle* p : particles) {
		p->draw(this, cameraPos, cameraOrientation, fv);
	}
	for (Drawable* d : walls) {
		d->draw(this, cameraPos, cameraOrientation, fv);
	}
	for (Enemy* e : enemies) {
		e->draw(this, cameraPos, cameraOrientation, fv);
	}
	for (Ore* o : ores) {
		o->draw(this, cameraPos, cameraOrientation, fv);
	}
	for (PhysicsObject* o : stuff) {
		o->draw(this, cameraPos, cameraOrientation, fv);
	}
	//draw octree
	if (false) {
		std::vector<OctreeNode*> nodes;
		OctreeNode* root = pEngine.getOctreeRoot();
		if (root != nullptr) {
			root->getAllNodes(&nodes);
			for (OctreeNode* node : nodes) {
				double s = node->getSize();
				Vector3D p = node->getPos();
				draw3DLine(p.add(Vector3D(0, 0, 0)), p.add(Vector3D(s, 0, 0)), cameraPos, cameraOrientation, fv, olc::GREEN);
				draw3DLine(p.add(Vector3D(s, 0, 0)), p.add(Vector3D(s, 0, s)), cameraPos, cameraOrientation, fv, olc::GREEN);
				draw3DLine(p.add(Vector3D(s, 0, s)), p.add(Vector3D(0, 0, s)), cameraPos, cameraOrientation, fv, olc::GREEN);
				draw3DLine(p.add(Vector3D(0, 0, s)), p.add(Vector3D(0, 0, 0)), cameraPos, cameraOrientation, fv, olc::GREEN);

				draw3DLine(p.add(Vector3D(0, s, 0)), p.add(Vector3D(s, s, 0)), cameraPos, cameraOrientation, fv, olc::GREEN);
				draw3DLine(p.add(Vector3D(s, s, 0)), p.add(Vector3D(s, s, s)), cameraPos, cameraOrientation, fv, olc::GREEN);
				draw3DLine(p.add(Vector3D(s, s, s)), p.add(Vector3D(0, s, s)), cameraPos, cameraOrientation, fv, olc::GREEN);
				draw3DLine(p.add(Vector3D(0, s, s)), p.add(Vector3D(0, s, 0)), cameraPos, cameraOrientation, fv, olc::GREEN);

				draw3DLine(p.add(Vector3D(0, s, 0)), p.add(Vector3D(0, 0, 0)), cameraPos, cameraOrientation, fv, olc::GREEN);
				draw3DLine(p.add(Vector3D(s, s, 0)), p.add(Vector3D(s, 0, 0)), cameraPos, cameraOrientation, fv, olc::GREEN);
				draw3DLine(p.add(Vector3D(s, s, s)), p.add(Vector3D(s, 0, s)), cameraPos, cameraOrientation, fv, olc::GREEN);
				draw3DLine(p.add(Vector3D(0, s, s)), p.add(Vector3D(0, 0, s)), cameraPos, cameraOrientation, fv, olc::GREEN);
			}
		}
	}
}

bool SpaceMinerGame::OnUserUpdate(float fElapsedTime) {
	static double dropTime = 100000;
	static int i = 0;
	dropTime += fElapsedTime;

	if (dropTime > 15 && i++ < 300) {
		dropTime = 0;
		double x = 5;// 52;s
		double y = -70;
		double z = 0;
		Enemy* e = new Enemy(Vector3D(20000, 0, 0), Rotor());
		addEnemy(e);
	}

	auto t1 = std::chrono::system_clock::now();
	gameUpdate(fElapsedTime);
	auto t2 = std::chrono::system_clock::now();
	gameRender();
	auto t3 = std::chrono::system_clock::now();
	
	std::chrono::duration<float> update = t2 - t1;
	std::chrono::duration<float> render = t3 - t2;
	//printf("update: %f, render: %f\n", update.count(), render.count());

	return true;
}

void SpaceMinerGame::update(std::vector<Updatable*>* updatables, float fElapsedTime) {
	for (Updatable* u : *updatables) {
		u->update(this, fElapsedTime);
	}
}

void SpaceMinerGame::removeExpired(std::vector<Expireable*>* expireables) {
	for (int i = 0; i < expireables->size(); i++) {
		Expireable* e = expireables->at(i);
		if (e->isExpired()) {
			e->performDeathActions(this);
			expireables->erase(expireables->begin() + i);
			i--;
			delete e;
			e = nullptr;
		}
	}
}

PhysicsEngine* SpaceMinerGame::getPhysicsEngine() {
	return &pEngine;
}

std::vector<Asteroid*>* SpaceMinerGame::getAsteroids() {
	return &asteroids;
}

void SpaceMinerGame::addOre(Ore* ore) {
	ores.push_back(ore);
	pEngine.addRigidBody(ore->getRigidBody());
}

std::vector<Enemy*>* SpaceMinerGame::getEnemies() {
	return &enemies;
}

Player* SpaceMinerGame::getPlayer() {
	return player;
}

void SpaceMinerGame::addEnemy(Enemy* e) {
	enemies.push_back(e);
	pEngine.addRigidBody(e->getRigidBody());
}

void SpaceMinerGame::addProjectile(Projectile* p) {
	projectiles.push_back(p);
	pEngine.addRigidBody(p->getBody());
}

void SpaceMinerGame::addParticle(Particle* p) {
	particles.push_back(p);
}

int main()
{
	SpaceMinerGame demo;
	if (demo.Construct(960, 540, 2, 2))
		demo.Start();

	return 0;
}
