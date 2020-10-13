#include "MovingObject.h"
#include "stdio.h"

MovingObject::~MovingObject() {

}

void MovingObject::findIVals() {
	rollI = 1.0 / body->findInverseInertiaOfAxis(Vector3D(1, 0, 0));
	pitchI = 1.0 / body->findInverseInertiaOfAxis(Vector3D(0, 0, 1));
	yawI = 1.0 / body->findInverseInertiaOfAxis(Vector3D(0, 1, 0));
}

void MovingObject::dampenMotion(double timePassed) {
	float t = std::fmin(1, timePassed);
	Vector3D vel = body->getVelocity();
	Vector3D aVel = body->getAngularVelocity();

	body->setVelocity(vel.sub(vel.multiply(t * linearDampFactor)));
	body->setAngVelocity(aVel.sub(aVel.multiply(t * angularDampFactor)));
}

void MovingObject::roll(double accel, double timePassed) {
	Vector3D axis = body->getOrientation().rotate(Vector3D(0, 1, 0));
	Vector3D impulse = body->getOrientation().rotate(Vector3D(0, 0, accel * timePassed * yawI / 2.0));
	body->applyImpulseAtPosition(impulse, body->getCenterOfMass().add(axis));
	body->applyImpulseAtPosition(impulse.getInverse(), body->getCenterOfMass().sub(axis));
}

void MovingObject::pitch(double accel, double timePassed) {
	Vector3D axis = body->getOrientation().rotate(Vector3D(0, 1, 0));
	Vector3D impulse = body->getOrientation().rotate(Vector3D(accel * timePassed * pitchI / 2.0, 0, 0));
	body->applyImpulseAtPosition(impulse, body->getCenterOfMass().add(axis));
	body->applyImpulseAtPosition(impulse.getInverse(), body->getCenterOfMass().sub(axis));
}

void MovingObject::yaw(double accel, double timePassed) {
	Vector3D axis = body->getOrientation().rotate(Vector3D(0, 0, -1));
	Vector3D impulse = body->getOrientation().rotate(Vector3D(accel * timePassed *yawI / 2.0, 0, 0));
	body->applyImpulseAtPosition(impulse, body->getCenterOfMass().add(axis));
	body->applyImpulseAtPosition(impulse.getInverse(), body->getCenterOfMass().sub(axis));
}

void MovingObject::accelForward(double accel, double timePassed) {
	Vector3D axis = body->getOrientation().rotate(Vector3D(1, 0, 0));
	double impulse = accel* timePassed * body->getMass();
	body->applyImpulseAtPosition(axis.multiply(impulse), body->getCenterOfMass());
}

void MovingObject::accelDrift(double accel, double timePassed) {
	Vector3D axis = body->getOrientation().rotate(Vector3D(0, 0, -1));
	double impulse = accel * timePassed * body->getMass();
	body->applyImpulseAtPosition(axis.multiply(impulse), body->getCenterOfMass());
}

void MovingObject::accelLift(double accel, double timePassed) {
	Vector3D axis = body->getOrientation().rotate(Vector3D(0, -1, 0));
	double impulse = accel * timePassed * body->getMass();
	body->applyImpulseAtPosition(axis.multiply(impulse), body->getCenterOfMass());
}