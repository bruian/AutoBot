//**************************************************
#include "TaskBarIcon.h"
//**************************************************
BEGIN_EVENT_TABLE(CTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE, CTaskBarIcon::OnMenuRestore)
    EVT_MENU(PU_EXIT,    CTaskBarIcon::OnMenuExit)
    EVT_MENU(PU_START,   CTaskBarIcon::OnMenuStart)
    EVT_MENU(PU_STOP,	 CTaskBarIcon::OnMenuStop)
    EVT_MENU(PU_PAUSE,	 CTaskBarIcon::OnMenuPause)
	EVT_MENU(PU_PAUSETIMER,	 CTaskBarIcon::OnMenuPauseTimer)
    //EVT_UPDATE_UI(PU_CHECKMARK,CTaskBarIcon::OnMenuUICheckmark)
    EVT_TASKBAR_LEFT_DCLICK  (CTaskBarIcon::OnLeftButtonDClick)
    //EVT_MENU(PU_SUB1, CTaskBarIcon::OnMenuSub)
    //EVT_MENU(PU_SUB2, CTaskBarIcon::OnMenuSub)
END_EVENT_TABLE()
//**************************************************
void CTaskBarIcon::OnMenuRestore(wxCommandEvent& )
{
	if(wxGetApp().GetAppProperties()->GetHashPassword() != wxEmptyString)
	{
		CLoginDialog Login;
		if(Login.ShowModal() != wxID_OK)
		{
			return;
		}
	}
	wxGetApp().GetTopWindow()->Show(true);
	wxGetApp().GetTopWindow()->SetWindowStyle(wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);
	wxGetApp().GetTopWindow()->Centre();
}
//**************************************************
void CTaskBarIcon::OnMenuExit(wxCommandEvent& )
{
	wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, wxID_EXIT);
	wxPostEvent(wxGetApp().GetTopWindow(), ev);
}
//**************************************************
void CTaskBarIcon::OnMenuStart(wxCommandEvent& WXUNUSED(event))
{
	wxTimer *timer = wxGetApp().GetTimer();
	if(timer != NULL)
	{
		timer->Stop();
	}

	if(GetPause())
	{
		wxGetApp().GetTreeBot()->UnPauseAllBot();
		SetIcon(wxGetApp().GetAppProperties()->GetIco(icr_appico));
	}
	else
	{
		wxGetApp().GetTreeBot()->StartAllBot(true);
	}
	SetPause(false);
}
////**************************************************
//void CTaskBarIcon::OnMenuUICheckmark(wxUpdateUIEvent &event)
//{
//       event.Check( check );
//}
//**************************************************
void CTaskBarIcon::OnMenuStop(wxCommandEvent& WXUNUSED(event))
{
	wxTimer *timer = wxGetApp().GetTimer();
	if(timer != NULL)
	{
		timer->Stop();
	}
	wxGetApp().GetTreeBot()->StopAllBot();
}
//**************************************************
void CTaskBarIcon::OnMenuPause(wxCommandEvent& WXUNUSED(event))
{
	if(GetPause())
	{
		wxTimer *timer = wxGetApp().GetTimer();
		if(timer != NULL)
		{
			timer->Stop();
		}
		wxGetApp().GetTreeBot()->UnPauseAllBot();
		SetPause(false);
		SetIcon(wxGetApp().GetAppProperties()->GetIco(icr_appico));
	}
	else
	{
		wxGetApp().GetTreeBot()->PauseAllBot();
		SetIcon(wxGetApp().GetAppProperties()->GetIco(icr_pause));
		SetPause(true);
	}
}
//**************************************************
void CTaskBarIcon::OnMenuPauseTimer(wxCommandEvent& WXUNUSED(event))
{
	if(GetPause())
	{
		wxTimer *timer = wxGetApp().GetTimer();
		if(timer != NULL)
		{
			timer->Stop();
		}

		wxGetApp().GetTreeBot()->UnPauseAllBot();
		SetPause(false);
		SetIcon(wxGetApp().GetAppProperties()->GetIco(icr_appico));
	}
	else
	{
		long pausNum = NULL;
		pausNum = wxGetNumberFromUser(wxGetApp().GetAppProperties()->GetLngRes(wxT("lr_pauseMsg")), 
										   wxT(""), 
										   wxGetApp().GetAppProperties()->GetLngRes(wxT("lr_pauseontimer")), 
										   360, 1, 86400);

		wxTimer *timer = wxGetApp().GetTimer();
		if(timer == NULL)
		{
			timer = new wxTimer();
			wxGetApp().SetTimer(timer);
		}
		wxGetApp().GetTimer()->Start(pausNum*1000, wxTIMER_ONE_SHOT);

		wxGetApp().GetTreeBot()->PauseAllBot();
		SetIcon(wxGetApp().GetAppProperties()->GetIco(icr_pause));
		SetPause(true);
	}
}
//**************************************************
//void CTaskBarIcon::OnMenuSub(wxCommandEvent&)
//{
//    wxMessageBox(wxT("You clicked on a submenu!"));
//}
//**************************************************
wxMenu *CTaskBarIcon::CreatePopupMenu()
{
	CAppProperties *ap = wxGetApp().GetAppProperties();
	wxMenu *menu = new wxMenu;
	menu->Append(PU_RESTORE, ap->GetLngRes(wxT("lr_trayRestore")));
	menu->AppendSeparator();
	menu->Append(PU_START, ap->GetLngRes(wxT("lr_start")));
	menu->Append(PU_STOP, ap->GetLngRes(wxT("lr_stop")));
	menu->Append(PU_PAUSE, ap->GetLngRes(wxT("lr_pause")), wxT(""), true/*wxITEM_CHECK*/);
	menu->Append(PU_PAUSETIMER, ap->GetLngRes(wxT("lr_pauseontimer")), wxT(""), true/*wxITEM_CHECK*/);
	menu->AppendSeparator();
    //menu->AppendSeparator();
    //wxMenu *submenu = new wxMenu;
    //submenu->Append(PU_SUB1, _T("One submenu"));
    //submenu->AppendSeparator();
    //submenu->Append(PU_SUB2, _T("Another submenu"));
    //menu->Append(PU_SUBMAIN, _T("Submenu"), submenu);
	menu->Append(PU_EXIT, ap->GetLngRes(wxT("lr_menuExitName")));
	return menu;
}
//**************************************************
void CTaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
	if(wxGetApp().GetAppProperties()->GetHashPassword() != wxEmptyString)
	{
		CLoginDialog Login;
		if(Login.ShowModal() != wxID_OK)
		{
			return;
		}
	}
	wxGetApp().GetTopWindow()->Show(true);
	wxGetApp().GetTopWindow()->SetWindowStyle(wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);
}
//**************************************************