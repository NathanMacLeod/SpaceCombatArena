#pragma once
#include "RigidBody.h"
#include "OctreeNode.h"

class PhysicsEngine {

private:
	Vector3D gravity;
	double timestep;
	double fpsCap;
	double reductionVel;
	double staticVel;
	bool useOctree;
	Vector3D octreeOrigin;
	double octreeSize;
	double octreeMin;
	std::vector<RigidBody*> rigidBodies;
	uint16_t giveID();
	void pushBodiesApart(RigidBody* collider, RigidBody* collidee, const Vector3D nV, double colDepth);
	void resolveImpulses(RigidBody* collider, RigidBody* collidee, const Vector3D nV, Vector3D colPoint, const std::vector<ConvexHull::ColPointInfo> supPoints, double restitution);

	bool getColDetectInfo(RigidBody* b1, RigidBody* b2, std::vector<ConvexHull::ColPointInfo>* supPoints, Vector3D* norm, 
		Vector3D* colPoint, double* colDepth, bool* isFaceCollision, RigidBody** collider, RigidBody** collidee);

	void detectAndResolveCollisions(RigidBody* body1, RigidBody* body2);
	OctreeNode* root;
public:
	PhysicsEngine(double timestep);
	bool bodyInUse(uint16_t bodyID);
	void setGravity(Vector3D gravity);
	void setOctree(bool useOctree, Vector3D octreeOrigin = Vector3D(0, 0, 0), double octreeSize = 1000, double octreeMinSize = 20);
	double getTimestep();
	void iterateEngineTimestep();
	void iterateEngine(double secondsPassed);
	void addRigidBody(RigidBody* body);
	bool removeRigidBody(RigidBody* body);
	void removeAllBodies();
	OctreeNode* getOctreeRoot();
};