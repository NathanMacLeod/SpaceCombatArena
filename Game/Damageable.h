#pragma once
#include "Expireable.h"

class Damageable : public Expireable {
protected:
	double hp;
	double maxHp;
public:
	virtual ~Damageable();

	virtual void damage(double damage);

	virtual bool isExpired();
};