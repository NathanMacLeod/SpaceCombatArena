#include "Player.h"
#include "Bullet.h"
#include "Missile.h"
#include "SpaceMinerGame.h"

Player::Player(Vector3D position, GunType gun, SpaceMinerGame* g) {

	this->gun = gun;
	angularDampFactor = 3;
	linearDampFactor = 0.45;

	money = false;
	equippedTool = Guns;

	MovingObject::createShipMesh(true, 35, olc::VERY_DARK_RED, olc::BLACK,
		olc::VERY_DARK_MAGENTA, olc::BLACK, olc::DARK_GREEN, olc::VERY_DARK_GREEN, &body, &model);
	//createStructure();

	grabDist = body->getCollisionRadius() * 2;
	missileRechargeTime.reset();
	missileLockCos = cos(missileLockAngle);

	applyUpgradeProfile(UpgradeProfile());
	
	body->translate(position);
	createRadarPoints();

	initInvPos(g);
	findIVals();
	hp = 100;
	maxHp = 100;

	for (int i = 0; i < Ore::N_TYPES; i++) {
		inventory[i] = 0;
	}

}

Player::UpgradeProfile::UpgradeProfile() {
	gun = Twin;
	hasMissiles = true;
	hasGunsight = true;
	thrustLevel = 1;
	radarLevel = 0;
	inventoryLevel = 0;
}

void Player::damage(double damage) {
	hp -= damage;
	damageDisplayTime.reset();
}

void Player::roundReset() {
	hp = maxHp;
	nRockets = nMaxRockets;
}

bool Player::isExpired() {
	return hp <= 0;
}

void Player::initInvPos(SpaceMinerGame* g) {
	int nRows = 2;
	int gapSize = invSize / 8;
	
	int half = (Ore::N_TYPES - Ore::N_TYPES / 2); 
	int row1Width = invSize * half + gapSize * (half - 1);
	int height = nRows * invSize + (nRows - 1) * gapSize;
	int y = g->ScreenHeight() * 0.75;
	for (int i = 0; i < half; i++) {
		int x = g->ScreenWidth() / 2 - (row1Width / 2) + i * (gapSize + invSize);
		invOrePos[i] = Vector3D(x, y - height / 2.0, 0);
	}
	
	int row2Width = invSize * (Ore::N_TYPES / 2) + gapSize * (Ore::N_TYPES / 2 - 1);
	for (int i = half; i < Ore::N_TYPES; i++) {
		int x = g->ScreenWidth() / 2 - (row2Width / 2) + (i - half) * (gapSize + invSize);
		invOrePos[i] = Vector3D(x, y + gapSize / 2, 0);
	}
}

void Player::drawInventory(SpaceMinerGame* g) {
	static char buff[64];

	static std::string dropMsg("Click on ore to drop one from inventory");
	g->DrawString(g->ScreenWidth() / 2 -4 * dropMsg.size(), g->ScreenHeight() / 2 - 4, dropMsg, olc::WHITE, 1);

	for (int i = 0; i < Ore::N_TYPES; i++) {
		int x = invOrePos[i].x;
		int y = invOrePos[i].y;
		Ore::Material m = (Ore::Material) i;

		if (inventory[i] > 0) {
			olc::Pixel color = Ore::getColor(m);
			olc::Pixel textColor = olc::Pixel(255 - color.r, 255 - color.g, 255 - color.b);

			g->FillRect(x, y, invSize, invSize, color);

			sprintf_s(buff, "%s: %d", Ore::getAbbrev(m), inventory[i]);
			g->DrawString(x + invSize / 2.0 - 4 * strlen(buff), y + invSize / 2.0 - 4, buff, textColor, 1);
		}
		else {
			g->FillRect(x, y, invSize, invSize, olc::Pixel(30, 30, 30));
		}

		if (getInvButtonHovered(m, g->GetMouseX(), g->GetMouseY())) {
			g->DrawRect(x, y, invSize, invSize, olc::RED);
		}
		else {
			g->DrawRect(x, y, invSize, invSize, olc::CYAN);
		}
	}
}

void Player::clickInventory(SpaceMinerGame* game) {
	double dropDist = body->getCollisionRadius();
	for (int i = 0; i < Ore::N_TYPES; i++) {

		Ore::Material m = (Ore::Material)i;
		if(inventory[i] > 0 && getInvButtonHovered(m, game->GetMouseX(), game->GetMouseY())) {
			inventory[i]--;
			Ore* ore = new Ore(getPos().add(getDir().multiply(dropDist)), m);
			ore->getRigidBody()->setVelocity(body->getVelocity());
			game->addOre(ore);
		}
	}
}

bool Player::getInvButtonHovered(Ore::Material m, int mouseX, int mouseY) {
	int i = (int)m;
	int x = invOrePos[i].x;
	int y = invOrePos[i].y;

	return mouseX >= x && mouseX <= x + invSize && mouseY >= y && mouseY <= y + invSize;
}

void Player::applyUpgradeProfile(const UpgradeProfile& upgrades) {
	gun = upgrades.gun;
	hasRockets = upgrades.hasMissiles;
	hasGunsight = upgrades.hasGunsight;
	pitchRate = 6;
	yawRate = 6;
	rollRate = 3;
	forwardThrust = 850 + 300 * upgrades.thrustLevel;
	radarRange = 1 + .5 * upgrades.radarLevel;
	sideThrust = 650;
	verticleThrust = 650;
	nMaxRockets = 2;
	inventoryMaxSize = 12 + 6 * upgrades.inventoryLevel;

	switch (gun) {
	case Default:
		shootTimer = CooldownTimer(0.25);
		break;
	case Twin:
		shootTimer = CooldownTimer(0.17);
		break;
	case Gatling:
		shootTimer = CooldownTimer(0.07);
		break;
	}
}

void Player::createRadarPoints() {
	int nRows = 10;
	int nCols = 18;
	radarPos = Vector3D(4, 2.5, 0);
	
	radarPoints.push_back(radarPos.add(Vector3D(0, -radarSize, 0)));
	radarPoints.push_back(radarPos.add(Vector3D(0, radarSize, 0)));
	for (int i = 0; i < nCols; i++) {
		double theta = 2 * 3.14159 * i / nCols;
		for (int j = 0; j < nRows; j++) {
			double phi = 3.14159 * (j + 1) / (nRows + 1);
			radarPoints.push_back(radarPos.add(Vector3D(sin(phi) * cos(theta), cos(phi), sin(phi) * sin(theta)).multiply(radarSize)));
		}
	}
}

void Player::setEquippedTool(Tool t) {
	switch (t) {
	case Missiles:
		if (hasRockets) {
			if (equippedTool != t) {
				targetLocked = false;
			}
			equippedTool = t;
		}
		break;
	default:
		equippedTool = t;
	}
	
}

void Player::getCameraPosOrient(Vector3D* posOut, Rotor* orientOut) {
	*orientOut = Rotor(Vector3D(0, 1, 0), 3.14159 / 2.0).applyRotor(body->getOrientation());
	*posOut = orientOut->rotate(Vector3D(0, 0, 65)).add(body->getCenterOfMass());
}

void Player::selectTarget(SpaceMinerGame* game) {
	double maxCos = maxLockAng;
	Enemy* newTarget = nullptr;
	Vector3D dir = getDir();
	for (Enemy* e : *game->getEnemies()) {
		if (e == target) {
			continue;
		}
		Vector3D toE = e->getPos().sub(getPos()).getUnitVector();
		double cosAng = toE.dotProduct(dir);
		if (cosAng > maxLockAng) {
			newTarget = e;
			maxCos = cosAng;
		}
	}
	if (newTarget != nullptr && newTarget != target) {
		target = newTarget;
		targetID = target->getRigidBody()->getID();
		targetLocked = false;
		targetLockTime.reset();
	}
}

void Player::update(SpaceMinerGame* game, float fElapsedTime) {
	if (target == nullptr) {
		selectTarget(game);
	}
	else if (!game->getPhysicsEngine()->bodyInUse(targetID) || target->getPos().sub(getPos()).getUnitVector().dotProduct(getDir()) < maxLockAng) {
		target = nullptr;
	}
	else {
		timeStep = fElapsedTime;
		savedPrevVel = targetPrevVel;
		targetPrevVel = target->getRigidBody()->getVelocity();

		if (equippedTool == Missiles && !targetLocked) {
			double targetAngCos = target->getPos().sub(getPos()).getUnitVector().dotProduct(getDir());
			if (targetAngCos >= missileLockCos) {
				targetLockTime.updateTimer(fElapsedTime);
				if (targetLockTime.isReady()) {
					targetLocked = true;
					targetLockTime.reset();
				}
			}
			else {
				targetLocked = false;
				targetLockTime.reset();
			}
		}
	}

	if (nRockets < nMaxRockets) {
		missileRechargeTime.updateTimer(fElapsedTime);
		if (missileRechargeTime.isReady()) {
			nRockets++;
			missileRechargeTime.reset();
		}
	}
	
	damageDisplayTime.updateTimer(fElapsedTime);
	inventoryFullMessageTime.updateTimer(fElapsedTime);
	missileFireRate.updateTimer(fElapsedTime);
	shootTimer.updateTimer(fElapsedTime);
	dampenMotion(fElapsedTime);
	manageChunks(game);
}

Ore* Player::getOreToGrab(SpaceMinerGame* game) {
	double closestAngle = 0;
	Ore* grabbedOre = nullptr;
	for (Ore* o : *game->getOre()) {
		Vector3D toOre = o->getPos().sub(getPos());
		double grabAngle = getDir().dotProduct(toOre.getUnitVector());
		if (toOre.getMagnitudeSquared() < grabDist * grabDist && grabAngle > closestAngle) {
			grabbedOre = o;
			closestAngle = grabAngle;
		}
	}
	return grabbedOre;
}

double Player::getKMToP(Vector3D p) {
	return p.sub(getPos()).getMagnitude() / KM_CONST;
}

Asteroid* Player::getTargetedAsteroid(SpaceMinerGame* game) {
	double closestAsteroid = 0.5 * KM_CONST; //max dist
	Asteroid* asteroid = nullptr;
	double rExpandFactor = 2;
	for (Asteroid* a : *game->getAsteroids()) {
		Vector3D toA = a->getPos().sub(getPos());
		double distSqrd = toA.getMagnitudeSquared();
		if (distSqrd < closestAsteroid * closestAsteroid) {
			double dist = sqrt(distSqrd);
			double ang = acos(getDir().dotProduct(toA.getUnitVector()));
			double r = a->getRigidBody()->getCollisionRadius();
			double reqAng = atan(r * rExpandFactor / dist);
			if (ang <= reqAng) {
				closestAsteroid = dist;
				asteroid = a;
			}
		}
	}
	return asteroid;
}

void Player::pickUpOre(SpaceMinerGame* game) {
	int inventorySize = 0;
	for (int i = 0; i < Ore::N_TYPES; i++) {
		inventorySize += inventory[i];
	}
	if (inventorySize >= inventoryMaxSize) {
		inventoryFullMessageTime.reset();
		return;
	}

	Ore* ore = getOreToGrab(game);
	if (ore != nullptr) {
		ore->pickUp();
		Ore::Material material = ore->getMaterial();
		inventory[(int)material]++;
	}
}

void Player::fireRocket(SpaceMinerGame* game) {
	if (nRockets > 0 && missileFireRate.isReady() && targetLocked) {
		nRockets--;
		double launchVel = 900;
		Vector3D launchDir = body->getOrientation().rotate(Vector3D(0, 1, 0)).multiply(launchVel);
		Missile* missile = new Missile(body->getCenterOfMass(), body->getOrientation(), body->getVelocity().add(launchDir), target, body->getID(), true);
		game->addProjectile(missile);
		missileFireRate.reset();
	}
}

void Player::fireBullet(SpaceMinerGame* game) {
	if (shootTimer.isReady()) {
		shootTimer.reset();

		Vector3D dir = body->getOrientation().rotate(Vector3D(1, 0, 0));
		double spawnDist = 150;

		Vector3D variation(0, 0, 0);
		Vector3D offset;
		double r;
		Vector3D spawnPos = body->getCenterOfMass().add(dir.multiply(spawnDist));

		static int twinDir = 1;
		switch (gun) {

		case Twin:
			r = 22;
			offset = body->getOrientation().rotate(Vector3D(0, 0, 1));
			offset = offset.multiply(r * twinDir);
			twinDir *= -1;
			spawnPos = spawnPos.add(offset);
			break;
		case Gatling:
			variation = Vector3D(0.5 - (float)rand() / RAND_MAX, 0.5 - (float)rand() / RAND_MAX, 0.5 - (float)rand() / RAND_MAX).getUnitVector().multiply(0.04);
			break;
		}


		Bullet* bullet = new Bullet(spawnPos, body->getOrientation(), body->getVelocity().add(dir.add(variation).multiply(bulletVel)), 700, body->getID(), olc::RED, olc::WHITE, true);
		game->addProjectile(bullet);
	}
}

void Player::shoot(SpaceMinerGame* game) {
	switch (equippedTool) {
	case Guns:
		fireBullet(game);
		break;
	case Missiles:
		fireRocket(game);
		break;
	}
}

void Player::manageChunks(SpaceMinerGame* game) {
	int prevX = chunkX;
	int prevY = chunkY;
	int prevZ = chunkZ;
	Vector3D pos = getPos();
	double s = AsteroidChunk::getChunkSize();
	chunkX = (pos.x - s / 2.0) / s;
	chunkY = (pos.y - s / 2.0) / s;
	chunkZ = (pos.z - s / 2.0) /s;
	if (prevX != chunkX || prevY != chunkY || prevZ != chunkZ) {
		AsteroidChunk::loadChunksAtPos(game, chunkX, chunkY, chunkZ, prevX, prevY, prevZ, true);
	}
}


void Player::drawPlayerUI(SpaceMinerGame* g, double FOV) {
	static char buff[64];

	olc::Pixel color = (damageDisplayTime.isReady())? olc::CYAN : olc::RED;

	int centerX = g->ScreenWidth() / 2;
	int centerY = g->ScreenHeight() / 2;

	//draw crosshair
	if (equippedTool != Missiles) {
		double lineWidth = 1;
		double gapSize = 6;
		double size = 10;

		g->FillRect(centerX - (gapSize + size), centerY - lineWidth / 2, size, lineWidth, color);
		g->FillRect(centerX + gapSize, centerY - lineWidth / 2, size, lineWidth, color);
		g->FillRect(centerX - lineWidth / 2.0, centerY - (gapSize + size), lineWidth, size, color);
		g->FillRect(centerX - lineWidth / 2.0, centerY + (gapSize), lineWidth, size, color);
	}
	else {
		double lockOnRadius = FOV * sin(missileLockAngle);
		g->DrawCircle(centerX, centerY, lockOnRadius, color);
		
		if (nRockets == 0) {
			g->DrawString(centerX - 5 * 8, centerY, "NO MISSILE", color, 1);
			int refillBarWidth = 80;
			int refillBarHeight = 8;
			float tLeft = 1 - missileRechargeTime.getTime() / missileRechargeTime.getTimerTime();

			g->FillRect(centerX + refillBarWidth/2 - refillBarWidth * tLeft, centerY + 12, refillBarWidth * tLeft + 1, refillBarHeight, color);
			g->DrawRect(centerX - refillBarWidth/2, centerY + 12, refillBarWidth, refillBarHeight, color);
		}
		else {
			sprintf_s(buff, "MISSILE: %d", nRockets);
			g->DrawString(centerX - 5 * 8, centerY + lockOnRadius + 4, buff, color, 1);
		}
	}
	Vector3D camPos;
	Rotor camOrient;
	getCameraPosOrient(&camPos, &camOrient);
	//highlight enemy target
	
	for (Enemy* enemy : *g->getEnemies()) {
		if (enemy == target) {
			continue;
		}
		double r1Min = 12;
		Vector3D toEnemy = enemy->getPos().sub(getPos());
		if (getDir().dotProduct(toEnemy) > 0) {
			double r1 = enemy->getRigidBody()->getCollisionRadius() * 1.25 * FOV / getDir().dotProduct(toEnemy);
			r1 = std::max(r1Min, r1);
			Vector3D enemyScreenPos = g->getPixelCoord(enemy->getPos(), camPos, camOrient, FOV);
			g->DrawCircle(enemyScreenPos.x, enemyScreenPos.y, r1, color);
		}
	}
	if (target != nullptr) {
		Vector3D toEnemy = target->getPos().sub(getPos());
		if (getDir().dotProduct(toEnemy) > 0) {

			double r1Min = 12;
			
			double r1 = target->getRigidBody()->getCollisionRadius() * 1.25 * FOV / getDir().dotProduct(toEnemy);
			r1 = std::max(r1Min, r1);
			Vector3D enemyScreenPos = g->getPixelCoord(target->getPos(), camPos, camOrient, FOV);
			double enemyDist = getKMToP(target->getPos());
			sprintf_s(buff, "%.2f KM", enemyDist);
			g->DrawString(enemyScreenPos.x - 3 * 8, enemyScreenPos.y + r1 + 4, buff, color, 1);

			if (equippedTool == Guns) {
				g->DrawCircle(enemyScreenPos.x, enemyScreenPos.y, r1, color);

				if (hasGunsight && enemyDist <= 1.5) {
					double r2 = 7;
					Vector3D leadPos = getPos().add(Projectile::calculateLeadPoint(getPos(), target->getPos(), body->getVelocity(), target->getRigidBody()->getVelocity(),
						bulletVel, true, 0, targetPrevVel, timeStep));
					Vector3D leadScreenPos = g->getPixelCoord(leadPos, camPos, camOrient, FOV);
					g->DrawCircle(leadScreenPos.x, leadScreenPos.y, r2, color);

					Vector3D r1ToR2 = leadScreenPos.sub(enemyScreenPos);
					double dist = r1ToR2.getMagnitude();
					if (dist > r2 + r1) {
						Vector3D p1 = enemyScreenPos.add(r1ToR2.multiply(r1 / dist));
						Vector3D p2 = enemyScreenPos.add(r1ToR2.multiply((dist - r2) / dist));
						g->DrawLine(p1.x, p1.y, p2.x, p2.y, color);
					}
				}
			}
			else if (equippedTool == Missiles) {
				olc::Pixel mColor = targetLocked ? olc::RED : color;

				g->DrawRect(enemyScreenPos.x - r1, enemyScreenPos.y - r1, r1 * 2, r1 * 2, mColor);
				double flickerRate = 2;
				double t = flickerRate * fmod(targetLockTime.getTime(), 1.0 / flickerRate);
				if (targetLocked || t > 0.5) {
					g->DrawLine(enemyScreenPos.x, enemyScreenPos.y - r1, enemyScreenPos.x + r1, enemyScreenPos.y, mColor);
					g->DrawLine(enemyScreenPos.x + r1, enemyScreenPos.y, enemyScreenPos.x, enemyScreenPos.y + r1, mColor);
					g->DrawLine(enemyScreenPos.x, enemyScreenPos.y + r1, enemyScreenPos.x - r1, enemyScreenPos.y, mColor);
					g->DrawLine(enemyScreenPos.x - r1, enemyScreenPos.y, enemyScreenPos.x, enemyScreenPos.y - r1, mColor);
				}
			}
			//accel lead
			/*r2 = 7;
			leadPos = getPos().add(Projectile::calculateLeadPoint(getPos(), target->getPos(), body->getVelocity(), target->getRigidBody()->getVelocity(), 
				bulletVel));
			leadScreenPos = g->getPixelCoord(leadPos, camPos, camOrient, FOV);
			g->DrawCircle(leadScreenPos.x, leadScreenPos.y, r2, olc::RED);*/
		}
	}

	int hpWidth = 140;
	int hpHeight = 20;
	int hpY = g->ScreenHeight() / 20;
	olc::Pixel hpCol = ((float)hp / maxHp <= 0.5) ? olc::RED : olc::DARK_CYAN;
	g->FillRect(g->ScreenWidth() / 2 - hpWidth / 2 + hpWidth * ((float)maxHp - hp) / maxHp, hpY, hpWidth * ((float)hp / maxHp), hpHeight, hpCol);
	g->DrawRect(g->ScreenWidth() / 2 - hpWidth / 2, hpY, hpWidth, hpHeight, olc::WHITE);

	sprintf_s(buff, "Score: %d\n", money);
	std::string moneyMsg(buff);
	g->DrawString(g->ScreenHeight() / 24, g->ScreenHeight() / 24, moneyMsg, olc::WHITE, 1);

	if (!inventoryFullMessageTime.isReady()) {
		char s1[] = "INVENTORY FULL";
		g->DrawString(centerX - strlen(s1) * 8, centerY, s1, olc::RED, 2);
		char s2[] = "Hold TAB to manage inventory";
		g->DrawString(centerX - strlen(s2) * 4, centerY + 20, s2, olc::RED, 1);
	}

	if (g->canOpenShop()) {
		static std::string str("Press G to open shop");
		g->DrawString(centerX - str.size() * 8, centerY * 4 / 5, str, olc::WHITE, 2);
	}

	/*Ore* highlightedOre = getOreToGrab(g);
	if (highlightedOre != nullptr) {
		olc::Pixel oreCol = olc::RED;
		double zDist = highlightedOre->getPos().sub(getPos()).dotProduct(getDir());
		if (zDist > 0) {
			double r = highlightedOre->getRigidBody()->getCollisionRadius() * 1.1 * FOV / zDist;
			Vector3D orePos = g->getPixelCoord(highlightedOre->getPos(), camPos, camOrient, FOV);
			g->DrawCircle(orePos.x, orePos.y, r, oreCol);

			Ore::Material material = highlightedOre->getMaterial();
			sprintf_s(buff, "%s\n\n$%d", Ore::getName(material), (int)Ore::getValue(material));
			g->DrawString(orePos.x + r, orePos.y - r, buff, oreCol, 2);
		}
	}*/

	/*Asteroid* asteroid = getTargetedAsteroid(g);
	if (asteroid != nullptr) {
		olc::Pixel astCol = olc::WHITE;
		double zDist = asteroid->getPos().sub(getPos()).dotProduct(getDir());
		double r = asteroid->getRigidBody()->getCollisionRadius() * 1.25 * FOV / zDist;
		Vector3D astPos = g->getPixelCoord(asteroid->getPos(), camPos, camOrient, FOV);

		g->DrawLine(astPos.x - r, astPos.y - r, astPos.x - r + r / 8, astPos.y - r, astCol);
		g->DrawLine(astPos.x - r, astPos.y - r, astPos.x - r, astPos.y - r + r/8, astCol);
		g->DrawLine(astPos.x + r, astPos.y - r, astPos.x + r - r / 8, astPos.y - r, astCol);
		g->DrawLine(astPos.x + r, astPos.y - r, astPos.x + r, astPos.y - r + r / 8, astCol);
		g->DrawLine(astPos.x + r, astPos.y + r, astPos.x + r - r / 8, astPos.y + r, astCol);
		g->DrawLine(astPos.x + r, astPos.y + r, astPos.x + r, astPos.y + r - r / 8, astCol);
		g->DrawLine(astPos.x - r, astPos.y + r, astPos.x - r + r / 8, astPos.y + r, astCol);
		g->DrawLine(astPos.x - r, astPos.y + r, astPos.x - r, astPos.y + r - r / 8, astCol);

		double dist = getKMToP(asteroid->getPos());
		sprintf_s(buff, "%.2f KM", dist);
		g->DrawString(astPos.x - 3 * 8, astPos.y - r - 10, buff, astCol, 1);
		sprintf_s(buff, "%d / %d", (int) asteroid->getHp(), (int) asteroid->getMaxHp());
		g->DrawString(astPos.x - strlen(buff) * 8 / 2, astPos.y + r + 2, buff, astCol, 1);
	}*/

	Rotor rDir(Vector3D(0, 1, 0), 3.14159 / 2.0);
	Rotor inv = body->getOrientation().getInverse();

	double width = radarSize * 0.12 / radarRange;
	double length = radarSize * 0.2 / radarRange;
	std::vector<Vector3D> radarTri;
	radarTri.push_back(Vector3D(-length / 2.0, 0, width / 2.0));
	radarTri.push_back(Vector3D(-length / 2.0, 0, -width / 2.0));
	radarTri.push_back(Vector3D(length / 2.0, 0, 0));
	radarTri.push_back(Vector3D(length, 0, 0));

	std::vector<Vector3D> points = radarTri;
	for (int i = 0; i < points.size(); i++) {
		points[i] = radarPos.add(points[i]);
	}
	Polygon3D p1 = Polygon3D(points.at(0), points.at(1), points.at(2), olc::BLUE, olc::DARK_BLUE);
	g->drawPolygon(p1, Vector3D(), rDir, FOV, true);
	g->draw3DLine(radarPos, points.at(3), Vector3D(), rDir, FOV, olc::BLUE);

	for (Vector3D p : radarPoints) {
		g->draw3DPoint(inv.rotate(p.sub(radarPos)).add(radarPos), Vector3D(), rDir, FOV, color);
	}
	/*for (Ore* o : *g->getOre()) {
		Vector3D toO = o->getPos().sub(getPos());
		if (toO.getMagnitudeSquared() / (KM_CONST * KM_CONST) < radarRange * radarRange) {
			Vector3D oRadarPos = radarPos.add(inv.rotate(toO.multiply(radarSize / (KM_CONST * radarRange))));
			g->draw3DPoint(oRadarPos, Vector3D(), rDir, FOV, olc::YELLOW);
		}
	}*/
	for (Enemy* e : *g->getEnemies()) {
		Vector3D toE = e->getPos().sub(getPos());
		double dist = toE.getMagnitude();
		if (dist / KM_CONST <= radarRange) {
			
			Vector3D eRadarPos = radarPos.add(inv.rotate(toE.multiply(radarSize / (KM_CONST * radarRange))));
			Rotor enemyRelOrient = e->getRigidBody()->getOrientation().applyRotor(inv);
			std::vector<Vector3D> points = radarTri;

			for (int i = 0; i < points.size(); i++) {
				points[i] = eRadarPos.add(enemyRelOrient.rotate(points[i]));
			}
			Polygon3D p1 = Polygon3D(points.at(0), points.at(1), points.at(2), olc::RED, olc::DARK_RED);
			Polygon3D p2 = Polygon3D(points.at(2), points.at(1), points.at(0), olc::RED, olc::DARK_RED);
			
			g->drawPolygon(p1, Vector3D(), rDir, FOV, true);
			g->drawPolygon(p2, Vector3D(), rDir, FOV, true);
			g->draw3DLine(eRadarPos, points.at(3), Vector3D(), rDir, FOV, olc::RED);
		}
	}
}

void Player::performDeathActions(SpaceMinerGame* game) {
	Particle::generateExplosion(game, getPos(), 1.9, 1550, 250, olc::RED);
	Particle::generateExplosion(game, getPos(), 1.0, 1250, 250, olc::WHITE);
	Particle::generateExplosion(game, getPos(), 2.4, 750, 170, olc::YELLOW);

	for (int i = 0; i < 3; i++) {
		double speed = 350;
		Vector3D dir = Vector3D(0.5 - (float)rand() / RAND_MAX, 0.5 - (float)rand() / RAND_MAX, 0.5 - (float)rand() / RAND_MAX).getUnitVector();
		Vector3D vel = body->getVelocity().multiply(1.0 / 3).add(dir.multiply(speed));
		Vector3D rot = Vector3D(0.5 - (float)rand() / RAND_MAX, 0.5 - (float)rand() / RAND_MAX, 0.5 - (float)rand() / RAND_MAX).getUnitVector().multiply(5);

		Ore::Material debrisType = Ore::PlayerDebris;
		
		Ore* ore = new Ore(getPos(), debrisType, (i == 0) ? HeadDebris : WingDebris);
		ore->getRigidBody()->setVelocity(vel);
		ore->getRigidBody()->setAngVelocity(rot);
		game->addOre(ore);

	}
	game->getPhysicsEngine()->removeRigidBody(body);
}

int Player::getMoney() {
	return money;
}

void Player::UpgradeProfile::applyUpgrade(const UpgradeProfile& upgrades) {
	gun = (GunType) std::max<int>(gun, upgrades.gun);
	hasMissiles = hasMissiles || upgrades.hasMissiles;
	hasGunsight = hasGunsight || upgrades.hasGunsight;
	inventoryLevel += upgrades.inventoryLevel;
	thrustLevel += upgrades.thrustLevel;
	radarLevel += upgrades.radarLevel;
}

int* Player::getInventory() {
	return inventory;
}

void Player::alterMoney(int deltaCash) {
	money += deltaCash;
}