#include "Enemy.h"
#include "Player.h"
#include "Bullet.h"
#include "Missile.h"
#include "SpaceMinerGame.h"

Enemy::Enemy(Vector3D pos, Rotor orientation, Enemy::EnemyType type) {
	//createStructure();

	size = 35;

	switch (type) {
	case Elite:
		forwardThrust = 750;
		shootTimer = CooldownTimer(0.3);
		maxHp = 150;
		pitchRate = 2.1;
		MovingObject::createShipMesh(false, size, olc::VERY_DARK_MAGENTA, olc::BLACK,
			olc::DARK_CYAN, olc::BLACK, olc::DARK_GREEN, olc::VERY_DARK_GREEN, &body, &model);
		break;
	default:
		maxHp = 100;
		forwardThrust = 550;
		pitchRate = 1.7;
		MovingObject::createShipMesh(false, size, olc::VERY_DARK_MAGENTA, olc::BLACK,
			olc::VERY_DARK_MAGENTA, olc::BLACK, olc::DARK_GREEN, olc::VERY_DARK_GREEN, &body, &model);
	}

	
	body->setToOrientation(orientation);
	body->translate(pos);

	this->type = type;

	hp = maxHp;

	angularDampFactor = 0.8;
	linearDampFactor = 0.12;

	findIVals();
	yawRate = 0.6;
	rollRate = 0.7;

	std::vector<Vector3D>* potentialSmokePoints = body->getAllPoints();
	int index = rand() % potentialSmokePoints->size();
	smokePoint = potentialSmokePoints->at(index);
}

void Enemy::update(SpaceMinerGame* game, float fElapsedTime) {
	dampenMotion(fElapsedTime);
	shootTimer.updateTimer(fElapsedTime);
	missileTimer.updateTimer(fElapsedTime);

	accelForward(1, fElapsedTime);
	Player* p = game->getPlayer();
	if (p != nullptr) {

		Vector3D dir = getDir();
		Vector3D toPlayer = p->getPos().sub(getPos());
		Vector3D toPlayerUnit = toPlayer.getUnitVector();

		

		if (dir.dotProduct(toPlayerUnit) > fireCone) {
			shoot(game);
		}

		Vector3D leadTarget = Projectile::calculateLeadPoint(getPos(), p->getPos(), body->getVelocity(), p->getRigidBody()->getVelocity(), bulletVel);

		aimAtTarget(((getDir().dotProduct(toPlayer) > 0) ? leadTarget : toPlayer), acos(dir.dotProduct(leadTarget.getUnitVector())), fElapsedTime);
	}

	if (hp / maxHp < 0.5) {
		Vector3D smokePWorld = getPos().add(body->getOrientation().rotate(smokePoint));
		Vector3D originVar = Vector3D((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX).multiply(0.75 * size);
		Particle::generateExplosion(game, smokePWorld.add(originVar), 0.7, 50, 2, olc::RED);
		Particle::generateExplosion(game, smokePWorld.add(originVar), 0.9, 100, 1, olc::GREEN);
		Particle::generateExplosion(game, smokePWorld.add(originVar), 4.5, 250, 1, olc::DARK_GREY);
	}
}

void Enemy::aimAtTarget(Vector3D relPos, double angleOff, float fElapsedTime) {
	Vector3D pitchDir = body->getOrientation().rotate(Vector3D(0, 1, 0));
	Vector3D yawDir = body->getOrientation().rotate(Vector3D(0, 0, -1));

	if (relPos.dotProduct(yawDir) > 0) {
		yaw(-1, fElapsedTime);
	}
	else {
		yaw(1, fElapsedTime);
	}

	if (relPos.dotProduct(pitchDir) > 0) {
		pitch(-1, fElapsedTime);
	}
	else {
		//printf("Pitch Down\n");
		pitch(0.5, fElapsedTime);
	}

	if (angleOff > 3.14159 / 9) {
		if (relPos.dotProduct(yawDir) > 0) {
			roll(1, fElapsedTime);
		}
		else {
			roll(-1, fElapsedTime);
		}
	}

}

void Enemy::damage(double damage) {
	Damageable::damage(damage);
}

void Enemy::performDeathActions(SpaceMinerGame* game) {
	Particle::generateExplosion(game, getPos(), 0.9, 1550, 250, olc::MAGENTA);
	Particle::generateExplosion(game, getPos(), 1.0, 1250, 250, olc::GREEN);
	Particle::generateExplosion(game, getPos(), 1.1, 750, 170, olc::RED);
	game->playSoundEffect(SpaceMinerGame::Explosion);

	for (int i = 0; i < 3; i++) {
		double speed = 350;
		Vector3D dir = Vector3D(0.5 - (float)rand() / RAND_MAX, 0.5 - (float)rand() / RAND_MAX, 0.5 - (float)rand() / RAND_MAX).getUnitVector();
		Vector3D vel = body->getVelocity().multiply(1.0 / 3).add(dir.multiply(speed));
		Vector3D rot = Vector3D(0.5 - (float)rand() / RAND_MAX, 0.5 - (float)rand() / RAND_MAX, 0.5 - (float)rand() / RAND_MAX).getUnitVector().multiply(5);

		Ore::Material debrisType;
		switch (type) {
		case Elite:
			debrisType = Ore::EliteDebris;
			break;
		case Normal:
			debrisType = Ore::NormalDebris;
			break;
		default:
			Ore::NormalDebris;
		}
		Ore* ore = new Ore(getPos(), debrisType, (i == 0)? HeadDebris : WingDebris);
		ore->getRigidBody()->setVelocity(vel);
		ore->getRigidBody()->setAngVelocity(rot);
		game->addOre(ore);

	}
	if (game->getPlayer() != nullptr) {
		game->getPlayer()->alterMoney(((type == Elite) ? 3000 : 1000));
	}
	game->getPhysicsEngine()->removeRigidBody(body);
}

void Enemy::shoot(SpaceMinerGame* game) {

	if (type == Enemy::Elite && missileTimer.isReady()) {
		missileTimer.reset();
		shootTimer.setCustum(2.0);

		Vector3D dir = body->getOrientation().rotate(Vector3D(1, 0, 0));
		double spawnDist = 150;

		double launchVel = 900;
		Vector3D launchDir = body->getOrientation().rotate(Vector3D(0, 1, 0)).multiply(launchVel);
		Missile* missile = new Missile(body->getCenterOfMass(), body->getOrientation(), body->getVelocity().add(launchDir), game->getPlayer(), body->getID(), false);
		game->addProjectile(missile);
	}
	else if (shootTimer.isReady()) {
		shootTimer.reset();

		Vector3D dir = body->getOrientation().rotate(Vector3D(1, 0, 0));
		double spawnDist = 150;
		Bullet* bullet = new Bullet(body->getCenterOfMass().add(dir.multiply(spawnDist)), body->getOrientation(), body->getVelocity().add(dir.multiply(bulletVel)), 150, body->getID(), olc::GREEN, olc::WHITE, false);

		//Missile* missile = new Missile(body->getCenterOfMass(), body->getOrientation(), body->getVelocity(), game->getPlayer(), body->getID());
		game->addProjectile(bullet);
	}
	
}

void Enemy::warpParticles(SpaceMinerGame* game) {
	Vector3D pos = getPos();
	Vector3D dir = getDir().getInverse();
	Particle::generateExplosion(game, pos, 1.5, 2600, 250, olc::WHITE);

	double trailRadius = 350;
	int nTrail = 1000;
	int nParticles = 2;
	for (int i = 0; i < nTrail; i++) {
		pos = pos.add(dir.multiply(trailRadius / 1.5));
		Particle::generateExplosion(game, pos, 1.4 + 0.1 * (double) (nTrail - i) / nTrail, trailRadius, nParticles, olc::WHITE);
	}
}

//void Enemy::draw(PixelEngine3D* g, Vector3D cameraPos, Rotor cameraDir, double FOV) {
//	debugDraw(g, cameraPos, cameraDir, FOV);
//}