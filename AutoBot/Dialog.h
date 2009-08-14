#ifndef __DIALOG_H__
#define __DIALOG_H__
//**************************************************
#include "AutoBot.h"
//**************************************************
class CSettingsDialog : public wxPropertySheetDialog
{
	DECLARE_CLASS(CSettingsDialog)
public:
    CSettingsDialog(wxWindow* parent);
    ~CSettingsDialog();

	void OnOK(wxCommandEvent& event);
	void OnChech(wxCommandEvent& event);
	void OnTextEdit(wxCommandEvent& event);
	void OnChoose(wxCommandEvent &event);
    wxPanel* CreateGeneralSettingsPage(wxWindow* parent);
    wxPanel* CreateAdditionalSettingsPage(wxWindow* parent);
	bool m_DisplayLabelOnDesktop;
	bool m_DisplayGroupAppOnStart;
	bool m_RunOnStartOS;
	bool m_MoveInTray;
	bool m_AllowUpdate;
	wxString m_Password;
	wxString m_CiteUpdate;
	int m_Language;
protected:
    wxImageList*    m_imageList;
	DECLARE_EVENT_TABLE()
};
//**************************************************
class CSpyModalDialog : public wxDialog
{
public:
    CSpyModalDialog(wxWindow *parent);
	void OnMouseEvent(wxMouseEvent& event);
    void OnButton(wxCommandEvent& event);
private:
    wxButton *m_btnModal,
             *m_btnModeless,
             *m_btnDelete;
	wxStaticBitmap *m_bitmap;

	HWND	 m_oldWndHWND;
	
    DECLARE_EVENT_TABLE()
};
//**************************************************
class CLoginDialog: public wxDialog
{
public:
	CLoginDialog();
	~CLoginDialog();
	wxComboBox LoginsList;
	wxTextCtrl PasswordCtrl;
private:
	//void OnClose(wxCloseEvent &event);
	void OnButtonOk(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};
//**************************************************
#endif //__DIALOG_H__