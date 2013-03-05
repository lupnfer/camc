#pragma once
#include "objbase.h"
#include "Unit.h"
#include <vector>
class CUnitFactory :
	public FCObject
{
public:
	CUnitFactory(void);
	~CUnitFactory(void);
public:
	CUnit* Lookup();
private:
	std::vector<CUnit*> m_unitList;
};
