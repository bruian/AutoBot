#include "CAutoBotAction.h"
//**************************************************
CAutoBotAction::CAutoBotAction()
{
	typecoGUID		= wxT("767FB19B-5F8A-4561-8DFA-4B53547736AE");
	m_ActionType	= 0;
	m_BotId			= wxEmptyString;
	m_GUIDBot		= wxEmptyString;
	m_GUIDRule		= wxEmptyString;
}
//**************************************************
void CAutoBotAction::Execute()
{
	switch(m_ActionType)
	{
	case 0:	//Start Bot
		{
			CAppProperties *ap = wxGetApp().GetAppProperties();
			if(!ap)
			{
				wxGetApp().GetScriptMutex()->Lock();
				wxGetApp().GetError()->AddMessage(ap->GetLngRes(wxT("lr_ComponentError")));
				wxGetApp().GetScriptMutex()->Unlock();
				return;
			}
			wxString botID(m_BotId);
			if(botID.IsEmpty())
			{
				wxGetApp().GetScriptMutex()->Lock();
				wxGetApp().GetError()->AddMessage(ap->GetLngRes(wxT("lr_CAutoBotAction_IdEmpty")));
				wxGetApp().GetScriptMutex()->Unlock();
				return;
			}
			CTreeBot *treeBot = wxGetApp().GetTreeBot();
			if(!treeBot)
			{
				wxGetApp().GetScriptMutex()->Lock();
				wxGetApp().GetError()->AddMessage(ap->GetLngRes(wxT("lr_ComponentError")));
				wxGetApp().GetScriptMutex()->Unlock();
				return;
			}
			if( !treeBot->StartBot(ap->StrToGuid(botID)) )
			{
				wxGetApp().GetScriptMutex()->Lock();
				wxGetApp().GetError()->AddMessage(ap->GetLngRes(wxT("lr_CAutoBotAction_ErrStart")));
				wxGetApp().GetScriptMutex()->Unlock();
			}
		}
		break;
	case 1:	//Stop Bot
		{
			CAppProperties *ap = wxGetApp().GetAppProperties();
			if(!ap)
			{
				wxGetApp().GetScriptMutex()->Lock();
				wxGetApp().GetError()->AddMessage(ap->GetLngRes(wxT("lr_ComponentError")));
				wxGetApp().GetScriptMutex()->Unlock();
				return;
			}
			wxString botID(m_BotId);
			if(botID.IsEmpty())
			{
				wxGetApp().GetScriptMutex()->Lock();
				wxGetApp().GetError()->AddMessage(ap->GetLngRes(wxT("lr_CAutoBotAction_IdEmpty")));
				wxGetApp().GetScriptMutex()->Unlock();
				return;
			}
			CTreeBot *treeBot = wxGetApp().GetTreeBot();
			if(!treeBot)
			{
				wxGetApp().GetScriptMutex()->Lock();
				wxGetApp().GetError()->AddMessage(ap->GetLngRes(wxT("lr_ComponentError")));
				wxGetApp().GetScriptMutex()->Unlock();
				return;
			}

			wxGetApp().GetScriptMutex()->Lock();
			treeBot->GetBot(ap->StrToGuid(botID))->SetStatusBot(STATUS_STOP);
			wxGetApp().GetScriptMutex()->Unlock();
		}
		break;
	case 2:
		break;
	default:
		break;
	}
}