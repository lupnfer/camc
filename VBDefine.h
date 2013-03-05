#pragma once


#include <vector>
#include <deque>
#include <algorithm>
/***************************
*作者：刘建明
*原始完成日期：2006.9.26
*用途：实现虚拟画刷的数据结构定义 
*作者本人修订日期：
*其他人修订（包括修改者，修改日期，修改地方，修改前的内容，修改后内容，作用等）      
***************************/

//===============================================================================================
/* 用于定制墨水的属性，采用论文Computer-Generated Chinese Painting with Physically-Based 
* Ink and Color Diffusion中的定义，把墨水分为水墨和颜料两种，对于水墨，其R,G,B的反射率相同，
* 采用5个等级：0.01,0.1,0.2,0.3,0.4,而对于彩色颜料，则直接采用其正常光下的颜色（R,G,B)作为其吸收率。
* 例如红色颜料其吸收率就为（1，0，0）。
*/


#define APHLA 0.33
#define BETA  0.4
#define RICEPAPER 0
#define SILKPAPER 1

struct PigmentAttr{
	double R_reflectance;
	double G_reflectance;
	double B_reflectance;
	double  m_percentage;//大于纸张纤维通道的墨分子比例
	PigmentAttr(){
		R_reflectance=0.0;
		G_reflectance=0.0;
		B_reflectance=0.0;
		m_percentage=0.0;
	}
	PigmentAttr(const PigmentAttr& pa){
		R_reflectance=pa.R_reflectance;
		G_reflectance=pa.G_reflectance;
		B_reflectance=pa.B_reflectance;
		m_percentage=pa.m_percentage;
	}
};
struct Ink{
	double       m_percentage;//墨水中颜料浓度
	PigmentAttr m_pigAttr;//颜料成分
};

//===============================================================================================
struct Pigment{//表示一定量的颜料
	PigmentAttr m_pigAttr;
	double      m_quan;
	Pigment(){
		m_quan=0;
	}
	Pigment(const Pigment& pig){
		m_pigAttr=pig.m_pigAttr;
		m_quan=pig.m_quan;
	}
};
typedef std::vector<Pigment> PIGMENTLIST;//多种颜料的含量列表
struct Papel{		
	double m_absorb;
	//double m_pigmentAbility;//
	POINT  m_pos;
	int    m_npiple[8];
	double m_waterQuan;
	PIGMENTLIST m_cbList;
	std::deque<Pigment> m_undoList;

	BOOL   m_isDiffused;
public:
	Papel(){
		m_absorb=0.0;
		m_waterQuan=0.0;
		m_isDiffused=false;
		for(int i=0;i<8;++i){
			m_npiple[i]=0;
		}
	}
	Papel(const Papel& pl){
		m_absorb=pl.m_absorb;
		m_waterQuan=pl.m_waterQuan;
		m_pos.x=pl.m_pos.x;
		m_pos.y=pl.m_pos.y;
		m_isDiffused=pl.m_isDiffused;
		for(int i=0;i<8;++i){
			m_npiple[i]=pl.m_npiple[i];
		}
		PIGMENTLIST::const_iterator it=(pl.m_cbList).begin();
		m_cbList.clear();
		for(;it!=(pl.m_cbList).end();++it){
			Pigment pig=(*it);
			m_cbList.push_back(pig);
		}
		std::deque<Pigment>::const_iterator itUndo;
		m_undoList.clear();
		itUndo=(pl.m_undoList).begin();
		for(;itUndo!=(pl.m_undoList).end();++itUndo){
			m_undoList.push_back(*itUndo);
		}
	}
	//同一层增加颜料
	void AddPigmentOnSameLayer(double quan){
/*		PIGMENTLIST::iterator it=cbList.begin();
		PigmentAttr& pa1=pig.m_pigAttr;
		for(;it!=cbList.end();++it){
			PigmentAttr& pa2=it->m_pigAttr;
			if(pa1.B_reflectance==pa2.B_reflectance&&pa1.G_reflectance==pa2.G_reflectance&&pa1.R_reflectance==pa2.R_reflectance)
			{
				it->m_quan+=pig.m_quan;
				break;
			}
		}
		if(it==cbList.end()){
			cbList.push_back(pig);
		}*/
		PIGMENTLIST::reverse_iterator it=m_cbList.rbegin();
		if(it!=m_cbList.rend()){
			it->m_quan+=quan;
		}
	}
	//增加新的颜料层。
	void AddNewLayer(Pigment pig){
		if(m_cbList.empty()){
			pig.m_quan=0.0;
			m_cbList.push_back(pig);
		}else{
			m_cbList[0].m_quan=0.0;
		}
	}
	void Undo(){
		if(m_cbList.empty())
			return;
		Pigment& pig=m_cbList.back();
		m_cbList.pop_back();
		m_undoList.push_back(pig);
	}
	void Redo(){
		if(m_undoList.empty())
			return;
		Pigment& pig=m_undoList.back();
		m_undoList.pop_back();
		m_cbList.push_back(pig);
	}
};
typedef std::deque<Papel> PAPERLINE;
//typedef std::vector<PAPERLINE> PAPERSTRUCT;
//typedef std::deque<Papel*> PAPELLIST;
class PAPERSTRUCT{
public:
	PAPERSTRUCT(){m_ppStruct=NULL;m_row=0;m_col=0;}
	PAPERSTRUCT(const PAPERSTRUCT& ppStruct){	
		resize(ppStruct.rows(),ppStruct.columns());
		for(int i=0;i<m_row;++i){
			PAPERLINE& ppline=ppStruct.getRow(i);
			PAPERLINE::iterator itSrc=ppline.begin();
			PAPERLINE::iterator itDst=m_ppStruct[i].begin();
			for(;itSrc!=ppline.end();++itSrc,++itDst){
				(*itDst)=(*itSrc);
			}
		}
		
	}
	~PAPERSTRUCT(){
		Destroy();
	}
public:
	PAPERLINE* m_ppStruct;
	int  m_row;
	int  m_col;
protected:
	void _Create(int row,int col){
		m_ppStruct=new PAPERLINE[row];
		m_row=row;
		m_col=col;
		for(int i=0;i<row;++i){
			m_ppStruct[i].resize(col);
		}
	}
	void Destroy(){
		if(m_ppStruct==NULL)
			return;
		for(int i=0;i<m_row;++i){
			m_ppStruct[i].clear();
		}
		delete[] m_ppStruct;
		m_ppStruct=NULL;
	}
public:
	void resize(int row,int col){
		Destroy();
		_Create(row,col);
	}
	PAPERLINE& getRow(int row) const{
		if(m_ppStruct==NULL){
			PAPERLINE pp(0);
			return pp;
		}
		return m_ppStruct[row];
	}
	int rows() const{
		return m_row;
	}
	int columns() const{
		return m_col;
	}
	PAPERLINE::iterator GetPapel(int x,int y){
		if(x<0||x>=m_col||y<0||y>=m_row)
		{
			return (m_ppStruct[0]).end();
		}
		PAPERLINE& ppRow=m_ppStruct[y];
		PAPERLINE::iterator it=ppRow.begin()+x;
		return it;
	}
};
