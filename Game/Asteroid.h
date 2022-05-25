#pragma once
#include "PhysicsObject.h"
#include "Damageable.h"

class SpaceMinerGame;

class Asteroid : public PhysicsObject, public Damageable {
private:
	static void generateFace(Vector3D p1, Vector3D p2, Vector3D p3, Vector3D p4, int detail, std::vector<Vector3D*>* face_out, std::vector<Vector3D*>* allP_out);
	static void generatePolygons(std::vector<Vector3D*>* face, std::vector<Polygon3D>* polygons, Vector3D center, int detail, olc::Pixel lineColor, olc::Pixel color);
	double size;
	bool unloaded;
public:
	~Asteroid() {};
	Asteroid(Vector3D position, double size);
	static void createRockMesh(Vector3D pos, double size, int detail, double density, double roughness, olc::Pixel lineColor, olc::Pixel color, RigidBody** bodyOut, PolyModel** meshOut);
	static double pickAsteroidSize(float weight);
	void performDeathActions(SpaceMinerGame* game);
	void markUnloaded();
	bool getUnloaded();

	static const int MIN_SIZE = 650;
	static const int MAX_SIZE = 2550;
};