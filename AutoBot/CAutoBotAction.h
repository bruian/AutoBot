#ifndef __CAUTOBOTACTION_H__
#define __CAUTOBOTACTION_H__
//**************************************************
#include "AutoBot.h"
//**************************************************
// Class: CAutoBotAction
// Name: AutoBotAction
// Version: 1.0
// Description: ����� ���������� ��� ������ � ����������
//	� ������������ ��.
// Component GUID: {767FB19B-5F8A-4561-8DFA-4B53547736AE}
class CAutoBotAction
{
public:
	wxString typecoGUID;	//GUID ���� ����������

	//EXTERN member
	//0 - Start Bot, 1 - Stop Bot, 2 - Exit AutoBot
	int				m_ActionType;		//m1 - ��� ����������� �������� ��� �����������
	const xchar*	m_BotId;			//m2 - ������������� ���, ��� Start � Stop bot
	const xchar*	m_GUIDBot;			//m3 - GUID ����
	const xchar*	m_GUIDRule;			//m4 - GUID �������
public:
	CAutoBotAction();

	void Execute();
};
//**************************************************
namespace script
{
	SCRIPT_CLASS_REGISTER_BEGIN( CAutoBotAction )
		.declareClass<CAutoBotAction>(wxT("CComponent_AutoBotAction"))
			.constructor< objOwn >(wxT("create"))
			.method(wxT("Execute"), &CAutoBotAction::Execute)
			.setter(wxT("set_ActionType"), &CAutoBotAction::m_ActionType)
			.setter(wxT("set_BotId"), &CAutoBotAction::m_BotId)
			.setter(wxT("set_GUIDBot"), &CAutoBotAction::m_GUIDBot)
			.setter(wxT("set_GUIDRule"), &CAutoBotAction::m_GUIDRule)
	SCRIPT_CLASS_REGISTER_END( CAutoBotAction )
}
//**************************************************
#endif //__CAUTOBOTACTION_H__