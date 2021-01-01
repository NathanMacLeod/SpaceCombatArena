#pragma once
#include "Vector3D.h"

struct Rotor {
	double a, b, c, d;
	Rotor(double a, double b, double c, double d);
	Rotor(Vector3D unitAxis, double theta);
	Rotor();

	Vector3D rotate(const Vector3D p) const;
	 Rotor applyRotor(const Rotor r) const;
	Rotor getInverse() const;

	bool operator!=(const Rotor& r);
	bool operator==(const Rotor& r);
};