#ifndef RESOURCE
#define RESOURCE

#include<BWAPI.h>
#include<vector>

class Resource
{
public:
	Resource(BWAPI::Unit unit);
	bool operator==(const Resource compare) const;

	void addWorker(BWAPI::Unit unit);
	void removeWorker(BWAPI::Unit unit);

	bool exists();
	int getNumberAssigned();
	BWAPI::Unit getUnit();
	const std::vector<BWAPI::Unit> getAssignedWorkers();
	
private:
	BWAPI::Unit my_unit;
	std::vector<BWAPI::Unit> assigned_workers;
};

#endif