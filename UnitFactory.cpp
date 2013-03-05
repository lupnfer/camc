#include "StdAfx.h"
#include ".\unitfactory.h"

CUnitFactory::CUnitFactory(void)
{
}

CUnitFactory::~CUnitFactory(void)
{
	std::vector<CUnit*>::iterator it;
	for(it=m_unitList.begin();it!=m_unitList.end();it++){
		delete it;
		it=NULL;
	}
	m_unitList.clear();
}
);
/*----------------------------------------
功能：返回一个具体的CUnit的子类对象的指针。
参数：nameIndex表示要返回的CUnit的子类对象
	  名称对应的整数（在Unit.h中定义）
------------------------------------------*/
CUnit* CUnitFactory::Lookup(int nameIndex)
{
	int size=m_unitList.size();
	if(nameIndex>=size){
		return NULL;
	}
	return m_unitList[nameIndex];
}