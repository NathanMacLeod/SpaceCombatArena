#pragma once
#include "PhysicsObject.h"

class MovingObject : public PhysicsObject {
protected:
	double rollI;
	double pitchI;
	double yawI;

	double linearDampFactor;
	double angularDampFactor;
	
	void findIVals();
	Vector3D accel;
public:
	virtual ~MovingObject();
	virtual void dampenMotion(double timePassed);

	Vector3D getAccel();
	void roll(double accel, double timePassed);
	void pitch(double accel, double timePassed);
	void yaw(double accel, double timePassed);
	void accelForward(double accel, double timePassed);
	void accelDrift(double accel, double timePassed);
	void accelLift(double accel, double timePassed);
};