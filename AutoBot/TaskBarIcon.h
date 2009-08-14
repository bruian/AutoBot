#ifndef __TASKBARICON_H__
#define __TASKBARICON_H__
//**************************************************
#include "AutoBot.h"
//**************************************************
class CTaskBarIcon: public wxTaskBarIcon
{
private:
	bool m_pause;
public:
	CTaskBarIcon()
	{
		SetPause(false);
	}
	~CTaskBarIcon(){};

	bool GetPause() { return m_pause; }
	void SetPause(bool value) { m_pause = value; }
    void OnLeftButtonDClick(wxTaskBarIconEvent& event);
    void OnMenuRestore(wxCommandEvent& event);
    void OnMenuExit(wxCommandEvent& event);
    void OnMenuStart(wxCommandEvent& event);
    void OnMenuStop(wxCommandEvent& event);
	void OnMenuPause(wxCommandEvent& event);
	void OnMenuPauseTimer(wxCommandEvent& event);
    void OnMenuUICheckmark(wxUpdateUIEvent& event);
    void OnMenuSub(wxCommandEvent& event);
    virtual wxMenu *CreatePopupMenu();

DECLARE_EVENT_TABLE()
};
//**************************************************
#endif //__TASKBARICON_H__