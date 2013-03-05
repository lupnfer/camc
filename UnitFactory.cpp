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
���ܣ�����һ�������CUnit����������ָ�롣
������nameIndex��ʾҪ���ص�CUnit���������
	  ���ƶ�Ӧ����������Unit.h�ж��壩
------------------------------------------*/
CUnit* CUnitFactory::Lookup(int nameIndex)
{
	int size=m_unitList.size();
	if(nameIndex>=size){
		return NULL;
	}
	return m_unitList[nameIndex];
}