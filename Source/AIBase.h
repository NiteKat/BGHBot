#ifndef AIBASE
#define AIBASE

#include"BWEM 1.3.1/src/bwem.h"

class AIBase
{
public:
	AIBase();
	AIBase(const BWEM::Area *new_area, int new_base_class);

	void setArea(const BWEM::Area *new_area);
	void setBaseClass(int new_base_class);

	const BWEM::Area *getArea();
	int getBaseClass();

private:
	const BWEM::Area *my_area;
	int base_class;
};

#endif