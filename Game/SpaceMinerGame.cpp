#define OLC_PGE_APPLICATION
#include "SpaceMinerGame.h"
#include "Player.h"

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

	//station = new SpaceStation(this, Vector3D(0, 0, 0));

	//shop->setIsOpen(true);
	 
	initZBuffer();

	initMenu();

	/*std::vector<Polygon3D>* b1polygons = createBox(5000, 100, 5000, 0, 5000 / 2.0, 0);
	std::vector<ConvexHull*>* hulls = new std::vector<ConvexHull*>();
	hulls->push_back(new ConvexHull(createRigidBodyFromPolygons(b1polygons), 1));
	RigidBody* b1 = new RigidBody(*hulls, 1, 1, 0.3, true);
	pEngine.addRigidBody(b1);
	walls.push_back(new PhysicsObject(new PolyModel(b1polygons, b1->getCenterOfMass()), b1));*/

	/*for (int i = 0; i < 300; i++) {
		Asteroid* asteroid = new Asteroid(Vector3D(worldSize/2.0 * (1 - 2 * (float)rand()/RAND_MAX), worldSize/2.0 * (1 - 2 * (float)rand() / RAND_MAX), worldSize/2.0 * (1 - 2 * (float)rand() / RAND_MAX)),
			Asteroid::pickAsteroidSize((float) rand() / RAND_MAX));
		asteroids.push_back(asteroid);
		pEngine.addRigidBody(asteroid->getRigidBody());
		int v = 200;
		asteroid->getRigidBody()->setVelocity(Vector3D(2 * (rand() % v) - v, 2 * (rand() % v) - v, 2 * (rand() % v) - v));
	}*/
	
	pEngine.setGravity(Vector3D(0, 0, 0));

	return true;
}

void SpaceMinerGame::readInput(float fElapsedTime) {

	if (GetKey(olc::Key::TAB).bHeld && false) {
		tabHeld = true;
	}
	else {
		tabHeld = false;
	}

	if (GetMouse(0).bPressed && (currentState == MainMenu || currentState == Instructions)) {
		clickMainMenu();
	}

	//if (GetKey(olc::Key::ESCAPE).bPressed) {
	//	shop->setIsOpen(false);
	//}

	if (player != nullptr) {
		//if (shop->getOpen()) {
		///	if (GetMouse(0).bPressed) {
		//		shop->clickShop(this);
		//	}
		//}
		if (!tabHeld) {
			int mouseX = GetMouseX();
			int mouseY = GetMouseY();

			float mouseSens = 1;
			//float movX = (float) mouseX / mouseSens;
			float movX = (mouseX - ScreenWidth() / 2.0) / (0.33 * ScreenWidth() / 2.0);
			if (abs(movX) > 1) {
				movX = (movX > 0) ? 1 : -1;
			}
			//float movY = (float)mouseY / mouseSens;
			float movY = (mouseY - ScreenHeight() / 2.0) / (0.33 * ScreenWidth() / 2.0);
			if (abs(movY) > 1) {
				movY = (movY > 0) ? 1 : -1;
			}

			if (movY != 0) {
				player->pitch(-movY, fElapsedTime);
			}
			if (movX != 0) {
				player->yaw(-movX, fElapsedTime);
			}

			if (GetKey(olc::Key::W).bHeld) {
				player->accelForward(1, fElapsedTime);
			}
			if (GetKey(olc::Key::S).bHeld) {
				player->accelForward(-1, fElapsedTime);
			}
			if (GetKey(olc::Key::A).bHeld) {
				player->accelDrift(-1, fElapsedTime);
			}
			if (GetKey(olc::Key::D).bHeld) {
				player->accelDrift(1, fElapsedTime);
			}
			if (GetKey(olc::Key::SPACE).bHeld) {
				player->shoot(this);
			}
			if (GetKey(olc::Key::CTRL).bHeld) {
				player->accelLift(-1, fElapsedTime);
			}
			if (GetKey(olc::Key::SHIFT).bHeld) {
				player->accelLift(1, fElapsedTime);
			}
			if (GetKey(olc::Key::UP).bHeld) {
				player->pitch(1, fElapsedTime);
			}
			if (GetKey(olc::Key::LEFT).bHeld) {
				player->yaw(1, fElapsedTime);
			}
			if (GetKey(olc::Key::DOWN).bHeld) {
				player->pitch(-1, fElapsedTime);
			}
			if (GetKey(olc::Key::RIGHT).bHeld) {
				player->yaw(-1, fElapsedTime);
			}
			if (GetKey(olc::Key::Q).bHeld) {
				player->roll(-1, fElapsedTime);
			}
			if (GetKey(olc::Key::E).bHeld) {
				player->roll(1, fElapsedTime);
			}
			//if (GetKey(olc::Key::F).bPressed) {
			//	player->pickUpOre(this);
			//}
			//if (GetKey(olc::Key::G).bPressed && canOpenShop()) {
				//shop->setIsOpen(true);
			//}
			if (GetKey(olc::Key::K1).bPressed) {
				player->setEquippedTool(Player::Guns);
			}
			if (GetKey(olc::Key::K2).bPressed) {
				player->setEquippedTool(Player::Missiles);
			}
			if (GetMouse(0).bPressed) {
				player->selectTarget(this);
			}
		}
		else {
			if (GetMouse(0).bPressed) {
				player->clickInventory(this);
			}
		}
	}
}

void SpaceMinerGame::handleGameStates(float fElapsedTime) {
	switch (currentState) {
	case WaveMessage:
		displayWaveMessage.updateTimer(fElapsedTime);
		if (displayWaveMessage.isReady()) {
			currentState = InWave;
		}
		break;
	case InWave:
		if (enemies.size() == 0) {
			if (nSubWave <= 0) {
				currentState = InbetweenWave;
				roundEndDelay.reset();
			}
			else {
				nSubWave--;
				int nEnemies = 1 + currWave / 2;
				float eliteProb = 1.0 - 1.0 / (0.33 * currWave);

				int nElite = 0;
				int nNormal = 0;
				for (int i = 0; i < nEnemies; i++) {
					float randF = (float)rand() / RAND_MAX;
					if (randF < eliteProb) {
						nElite++;
					}
					else {
						nNormal++;
					}
				}

				spawnEnemies(nNormal, nElite);
			}
		}
		break;
	case InbetweenWave:
		//if (!roundEndDelay.isReady()) {
		roundEndDelay.updateTimer(fElapsedTime);
		if (roundEndDelay.isReady()) {
			//shop->setIsOpen(true);
			displayWaveMessage.reset();
			currWave++;
			nSubWave = 3;
			player->roundReset();
			currentState = WaveMessage;
		}
		break;
	case GameOver:
		cameraPos = cameraPos.add(panCameraDir.multiply(-250 * fElapsedTime));
		gameOverMSGDelay.updateTimer(fElapsedTime);
		if (gameOverMSGDelay.isReady()) {
			gameOverMsgTime.updateTimer(fElapsedTime);
			if (gameOverMsgTime.isReady()) {
				currentState = MainMenu;
				initMenu();
			}
		}
		break;
	case MainMenu:
	case Instructions:
		cameraOrientation = cameraOrientation.applyRotor(Rotor(Vector3D(0, 1, 0), fElapsedTime * 0.1));
		break;
	}
}

void SpaceMinerGame::clearEverything() {
	AsteroidChunk::unloadAllChunks(this);
	pEngine.removeAllBodies();

	for (Projectile* p : projectiles) {
		delete p;
	}
	projectiles.clear();

	for (Asteroid* a : asteroids) {
		delete a;
	}
	asteroids.clear();

	for (Particle* p : particles) {
		delete p;
	}
	particles.clear();

	for (Ore* o : ores) {
		delete o;
	}
	ores.clear();

	for (Enemy* e : enemies) {
		delete e;
	}
	enemies.clear();

	if (player != nullptr) {
		delete player;
	}
}

void SpaceMinerGame::initMenu() {
	clearEverything();
	double density = 0.5 * 350 / (worldSize * worldSize * worldSize);
	AsteroidChunk::setUniverseProperties(2000, worldSize, density);
	AsteroidChunk::loadChunksAtPos(this, 0, 0, 0);
	cameraPos = Vector3D(0, 0, 0);
	cameraOrientation = Rotor();
}

void SpaceMinerGame::gameUpdate(float fElapsedTime) {
	if (player != nullptr) {
		pEngine.setOctree(true, player->getPos(), worldSize * 1.5, 300);
	}
	pEngine.iterateEngine(fElapsedTime);

	readInput(fElapsedTime);
	if (player != nullptr) {
		player->update(this, fElapsedTime);
	}
	//station->update(this, fElapsedTime);
	handleGameStates(fElapsedTime);

	for (Projectile* p : projectiles) {
		p->update(this, fElapsedTime);
	}
	for (Particle* p : particles) {
		p->update(this, fElapsedTime);
	}
	for (Enemy* e : enemies) {
		e->update(this, fElapsedTime);
	}
	for (Ore* o : ores) {
		o->update(this, fElapsedTime);
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
		if (a->getUnloaded()) {
			pEngine.removeRigidBody(a->getRigidBody());
			asteroids.erase(asteroids.begin() + i);
			i--;
			delete a;
			a = nullptr;
		}
		else if (a->isExpired()) {
			a->performDeathActions(this);
			asteroids.erase(asteroids.begin() + i);
			i--;
			delete a;
			a = nullptr;
		}
	}

	for (int i = 0; i < ores.size(); i++) {
		Ore* o = ores.at(i);
		if (o->isExpired()) {
			o->performDeathActions(this);
			ores.erase(ores.begin() + i);
			i--;
			delete o;
			o = nullptr;
		}
	}

	if (player != nullptr && player->isExpired()) {
		currentState = GameOver;
		panCameraDir = cameraOrientation.rotate(Vector3D(0, 0, 1));
		cameraPos = cameraPos.add(panCameraDir.multiply(-450));
		gameOverMSGDelay.reset();
		gameOverMsgTime.reset();
		player->performDeathActions(this);
		player = nullptr;
	}
}

void SpaceMinerGame::gameRender() {
	static double fv = 300;

	clearZBuffer();
	Clear(olc::BLACK);

	if (player != nullptr) {
		player->getCameraPosOrient(&cameraPos, &cameraOrientation);
		player->draw(this, cameraPos, cameraOrientation, fv);
	}
	//cameraPos = test->body->getCenterOfMass();

	//station->draw(this, cameraPos, cameraOrientation, fv);
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
	//for (PhysicsObject* o : stuff) {
	//	o->draw(this, cameraPos, cameraOrientation, fv);
	//}
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
	clearZBuffer();
	if (!tabHeld && player != nullptr) {
		player->drawPlayerUI(this, fv);
	}
	else {
		//player->drawInventory(this);
	}
	//if (shop->getOpen()) {
	//	shop->draw(this);
	//}
	if (currentState == WaveMessage) {
		static char buff[64];
		sprintf_s(buff, "WAVE %d", currWave);
		int fontSize = 4;
		std::string waveMsg(buff);
		DrawString(ScreenWidth() / 2 - 4 * fontSize * waveMsg.size(), ScreenHeight() / 2 - 4 * fontSize, waveMsg, olc::WHITE, fontSize);
	}
	else if (currentState == GameOver && gameOverMSGDelay.isReady()) {
		int fontSize = 4;
		std::string msg("GAME OVER");
		DrawString(ScreenWidth() / 2 - 4 * fontSize * msg.size(), ScreenHeight() / 2 - 4 * fontSize, msg, olc::WHITE, fontSize);
	}
	else if (currentState == MainMenu) {
		displayMainMenu();
	}
	else if (currentState == Instructions) {
		displayControls();
	}
}

bool SpaceMinerGame::OnUserUpdate(float fElapsedTime) {
	static double dropTime = 100000;
	static int i = 0;
	dropTime += fElapsedTime;

	/*if (dropTime > 0.1 && i < 300 && enemies.size() < 0) {
		i++;
		dropTime = 0;
		double x = 5;// 52;s
		double y = -70;
		double z = 0;
		Enemy* e = new Enemy(Vector3D(20000, 0, 0), Rotor());
		addEnemy(e);
	}*/

	auto t1 = std::chrono::system_clock::now();
	gameUpdate(fElapsedTime);
	auto t2 = std::chrono::system_clock::now();
	gameRender();
	auto t3 = std::chrono::system_clock::now();
	
	std::chrono::duration<float> time = t3 - t1;
	std::chrono::duration<float> update = t2 - t1;
	std::chrono::duration<float> render = t3 - t2;

	//printf("update / render: %f\n", update.count() / render.count());

	return true;
}

void SpaceMinerGame::spawnEnemies(int nNormal, int nElite) {
	double theta = 2 * 3.14159 * (double)rand() / RAND_MAX;
	double phi = 3.14159 * (double)rand() / RAND_MAX;
	double r = Player::KM_CONST * (0.75 + 0.5 * (double)rand() / RAND_MAX);

	Rotor A(Vector3D(1, 0, 0), 2 * 3.14159 * (double)rand() / RAND_MAX);
	Rotor B(Vector3D(1, 0, 0), 2 * 3.14159 * (double)rand() / RAND_MAX);
	Rotor C(Vector3D(1, 0, 0), 2 * 3.14159 * (double)rand() / RAND_MAX);
	Vector3D pos = player->getPos().add(Vector3D(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi)).multiply(r));
	Rotor orient = A.applyRotor(B).applyRotor(C);

	double formationAngle = 0.4;
	double dist = 2100;
	Vector3D rearDir = Vector3D(-formationAngle * dist, 0, 0);
	Vector3D sideDir = Vector3D(0, (1 - formationAngle) * dist, 0);

	for (int i = 0; i < nNormal + nElite; i++) {
		int depth = (i + 1) / 2;
		Vector3D ePos = pos.add(rearDir.add(sideDir).multiply(depth));
		sideDir = sideDir.multiply(-1);

		Enemy* e;
		if (i < nElite) {
			e = new Enemy(ePos, orient, Enemy::Elite);
		}
		else {
			e = new Enemy(ePos, orient);
		}
		e->warpParticles(this);
		addEnemy(e);
	}
	
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

void SpaceMinerGame::displayControls() {
	int y = instructionsMsgY * ScreenHeight();
	for (int i = 0; i < nInstrMsg; i++) {
		DrawString(ScreenWidth() / 2 - 4 * instrSize * instructions[i].size(), y, instructions[i], olc::WHITE, instrSize);
		y += 12 * instrSize;
	}

	bool hovered = getMenuButtonHovered(ScreenHeight() * backButtonY, menuSize, backMsg.size());
	DrawString(ScreenWidth() / 2 - 4 * menuSize * backMsg.size(), ScreenHeight() * backButtonY, backMsg, (hovered? olc::GREY : olc::WHITE), menuSize);
}

void SpaceMinerGame::displayMainMenu() {
	int titleSize = 5;
	std::string title("SPACE COMBAT ARENA");
	DrawString(ScreenWidth() / 2 - 4 * titleSize * title.size(), ScreenHeight() * 0.25 - 4 * titleSize, title, olc::WHITE, titleSize);

	bool hovered;
	hovered = getMenuButtonHovered(ScreenHeight() * playButtonY, menuSize, playStr.size());
	DrawString(ScreenWidth() / 2 - 4 * menuSize * playStr.size(), ScreenHeight() * playButtonY - 4 * menuSize, playStr, (hovered? olc::GREY : olc::WHITE), menuSize);

	hovered = getMenuButtonHovered(ScreenHeight() * instructionButtonY, menuSize, instrStr.size());
	DrawString(ScreenWidth() / 2 - 4 * menuSize * instrStr.size(), ScreenHeight() * instructionButtonY - 4 * menuSize, instrStr, (hovered ? olc::GREY : olc::WHITE), menuSize);
}

void SpaceMinerGame::clickMainMenu() {
	if (currentState == MainMenu) {
		//clicking on 'Play' button
		if (getMenuButtonHovered(ScreenHeight() * playButtonY, menuSize, playStr.size())) {
			currWave = 0;
			currentState = InbetweenWave;
			AsteroidChunk::unloadAllChunks(this);
			player = new Player(Vector3D(200, 0, 0), Player::Twin, this);
			pEngine.addRigidBody(player->getRigidBody());
		}
		else if (getMenuButtonHovered(ScreenHeight() * instructionButtonY, menuSize, instrStr.size())) {
			currentState = Instructions;
		}
	}
	else if (currentState == Instructions) {
		if (getMenuButtonHovered(ScreenHeight() * backButtonY, menuSize, backMsg.size())) {
			currentState = MainMenu;
		}
	}
}

bool SpaceMinerGame::getMenuButtonHovered(int yVal, int textSize, int textLen) {
	int mX = GetMouseX();
	int mY = GetMouseY();
	
	int bWidth = 8 * textSize * textLen;
	int bHeight = 8 * textSize;
	int bX = (ScreenWidth() - bWidth) / 2;
	int bY = yVal - bHeight / 2;

	return mX >= bX && mX <= bX + bWidth && mY >= bY && mY <= bY + bHeight;
}

PhysicsEngine* SpaceMinerGame::getPhysicsEngine() {
	return &pEngine;
}

std::vector<Ore*>* SpaceMinerGame::getOre() {
	return &ores;
}

std::vector<Asteroid*>* SpaceMinerGame::getAsteroids() {
	return &asteroids;
}

void SpaceMinerGame::addAsteroid(Asteroid* asteroid) {
	asteroids.push_back(asteroid);
	pEngine.addRigidBody(asteroid->getRigidBody());
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

bool SpaceMinerGame::canOpenShop() {
	return false;
	//return station->canOpenShop(player);
}

int main()
{
	SpaceMinerGame demo;
	if (demo.Construct(960, 540, 2, 2))
		demo.Start();

	return 0;
}
