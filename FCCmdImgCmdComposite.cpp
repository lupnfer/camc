#include "StdAfx.h"
#include ".\fccmdimgcmdcomposite.h"
#include "ObjCanvas.h"
#include "ObjProgress.h"
#include "FColor.h"

//=================================================================================
// 组合数个命令/composite image effect command
FCCmdImgCmdComposite::~FCCmdImgCmdComposite()
{
	while (!m_cmdList.empty())
	{
		FCCmdArtPrider   * node = m_cmdList.back() ;
		m_cmdList.pop_back() ;
		delete node ;
	}
}
void  FCCmdImgCmdComposite::PushImgCommand (FCCmdArtPrider * cmd)
{
	if (cmd != NULL)
		m_cmdList.push_back(cmd) ;
}
void  FCCmdImgCmdComposite::Execute (FCObjCanvas & canvas, FCObjProgress * Percent)
{
	for (int i=0 ; i < (int)m_cmdList.size() ; i++)
		m_cmdList[i]->Execute (canvas, Percent) ;
}
void  FCCmdImgCmdComposite::Undo ()
{
	for (int i=(int)m_cmdList.size() - 1 ; i >= 0 ; i--)
		m_cmdList[i]->Undo () ;
}
void  FCCmdImgCmdComposite::Redo ()
{
	for (int i=0 ; i < (int)m_cmdList.size() ; i++)
		m_cmdList[i]->Redo () ;
}
