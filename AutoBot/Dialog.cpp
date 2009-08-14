//**************************************************
#include "Dialog.h"
//**************************************************
IMPLEMENT_CLASS(CSettingsDialog, wxPropertySheetDialog)
//**************************************************
BEGIN_EVENT_TABLE(CSettingsDialog, wxPropertySheetDialog)
	EVT_BUTTON(wxID_OK, CSettingsDialog::OnOK)
	EVT_TEXT_ENTER(ID_CiteUpdate,    CSettingsDialog::OnTextEdit)
	EVT_TEXT_ENTER(ID_Password,    CSettingsDialog::OnTextEdit)
	EVT_CHECKBOX(ID_DisplayLabelOnDesktop, CSettingsDialog::OnChech)
	EVT_CHECKBOX(ID_DisplayGroupAppOnStart, CSettingsDialog::OnChech)
	EVT_CHECKBOX(ID_RunOnStartOS, CSettingsDialog::OnChech)
	EVT_CHECKBOX(ID_MoveInTray, CSettingsDialog::OnChech)
	EVT_CHECKBOX(ID_AllowUpdate, CSettingsDialog::OnChech)
	EVT_CHOICE(ID_ArrayLanguage, CSettingsDialog::OnChoose)
END_EVENT_TABLE()
//**************************************************
void CSettingsDialog::OnOK(wxCommandEvent& event)
{
	CAppProperties *appProp = wxGetApp().GetAppProperties();
	appProp->SetDisplayLabelOnDesktop(m_DisplayLabelOnDesktop);
	appProp->SetDisplayGroupAppOnStart(m_DisplayGroupAppOnStart);
	appProp->SetRunOnStartOS(m_RunOnStartOS);
	appProp->SetMoveInTray(m_MoveInTray);
	appProp->SetAllowUpdate(m_AllowUpdate);
	appProp->SetCiteUpdate(m_CiteUpdate);
	appProp->SetPassword(m_Password);
	if(appProp->GetLanguage() != m_Language)
		wxMessageBox(appProp->GetLngRes(wxT("lr_lngChg")));
	appProp->SetLanguage(m_Language);
	appProp->SaveAppProperties();
	event.Skip();
}
//**************************************************
void CSettingsDialog::OnChech(wxCommandEvent& event)
{
	switch(event.GetId())
	{
	case ID_DisplayLabelOnDesktop:
		m_DisplayLabelOnDesktop = event.IsChecked();
		break;
	case ID_DisplayGroupAppOnStart:
		m_DisplayGroupAppOnStart = event.IsChecked();
		break;
	case ID_RunOnStartOS:
		m_RunOnStartOS = event.IsChecked();
		break;
	case ID_MoveInTray:
		m_MoveInTray = event.IsChecked();
		break;
	case ID_AllowUpdate:
		m_AllowUpdate = event.IsChecked();
		break;
	}
}
//**************************************************
void CSettingsDialog::OnTextEdit(wxCommandEvent& event)
{
	switch(event.GetId())
	{
	case ID_CiteUpdate:
		m_CiteUpdate = event.GetString();
		break;
	case ID_Password:
		m_Password = event.GetString();
		break;
	}
}

//**************************************************
void CSettingsDialog::OnChoose(wxCommandEvent &event)
{
	m_Language = event.GetInt();
}
//**************************************************
CSettingsDialog::CSettingsDialog(wxWindow* win)
{
	CAppProperties *appProp = wxGetApp().GetAppProperties();
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);

	int tabImage1 = -1;
	int tabImage2 = -1;

	int resizeBorder = wxRESIZE_BORDER;
	m_imageList = NULL;

	Create(win, wxID_ANY, appProp->GetLngRes(wxT("lr_menuPropertyName")), wxDefaultPosition, wxDefaultSize,
		   wxDEFAULT_DIALOG_STYLE| (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, resizeBorder));

	// If using a toolbook, also follow Mac style and don't create buttons
	CreateButtons(wxOK | wxCANCEL |(int)wxPlatform::IfNot(wxOS_WINDOWS_CE, wxHELP));

	wxBookCtrlBase* notebook = GetBookCtrl();
	notebook->SetImageList(m_imageList);

	m_DisplayLabelOnDesktop = appProp->GetDisplayLabelOnDesktop();
	m_DisplayGroupAppOnStart = appProp->GetDisplayGroupAppOnStart();
	m_RunOnStartOS = appProp->GetRunOnStartOS();
	m_MoveInTray = appProp->GetMoveInTray();
	m_AllowUpdate = appProp->GetAllowUpdate();
	m_Password = appProp->GetHashPassword();
	m_CiteUpdate = appProp->GetCiteUpdate();
	m_Language = appProp->GetLanguage();

	wxPanel* generalSettings = CreateGeneralSettingsPage(notebook);
	wxPanel* additionalSettings = CreateAdditionalSettingsPage(notebook);

	notebook->AddPage(generalSettings, appProp->GetLngRes(wxT("lr_prpageGeneral")), true, tabImage1);
	notebook->AddPage(additionalSettings, appProp->GetLngRes(wxT("lr_prpageAdditional")), false, tabImage2);

	LayoutDialog();
}
//**************************************************
CSettingsDialog::~CSettingsDialog()
{
	delete m_imageList;
}
//**************************************************
wxPanel* CSettingsDialog::CreateGeneralSettingsPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);
	CAppProperties *appProp = wxGetApp().GetAppProperties();

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    //ID_DisplayLabelOnDesktop
    wxBoxSizer* itemSizer1 = new wxBoxSizer( wxHORIZONTAL );
	wxCheckBox* checkBox1 = new wxCheckBox(panel, ID_DisplayLabelOnDesktop, appProp->GetLngRes(wxT("lr_prpageDisplayLabelOnDesktop")), wxDefaultPosition, wxDefaultSize);
	if(m_DisplayLabelOnDesktop)
		checkBox1->Set3StateValue(wxCHK_CHECKED);
	else
		checkBox1->Set3StateValue(wxCHK_UNCHECKED);
    itemSizer1->Add(checkBox1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add(itemSizer1, 0, wxGROW|wxALL, 0);

    //ID_DisplayGroupAppOnStart
    wxBoxSizer* itemSizer2 = new wxBoxSizer( wxHORIZONTAL );
	wxCheckBox* checkBox2 = new wxCheckBox(panel, ID_DisplayGroupAppOnStart, appProp->GetLngRes(wxT("lr_prpageDisplayGroupAppOnStart")), wxDefaultPosition, wxDefaultSize);
	if(m_DisplayGroupAppOnStart)
		checkBox2->Set3StateValue(wxCHK_CHECKED);
	else
		checkBox2->Set3StateValue(wxCHK_UNCHECKED);
	checkBox2->Enable(false);
    itemSizer2->Add(checkBox2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add(itemSizer2, 0, wxGROW|wxALL, 0);

    //ID_RunOnStartOS
    wxBoxSizer* itemSizer3 = new wxBoxSizer( wxHORIZONTAL );
	wxCheckBox* checkBox3 = new wxCheckBox(panel, ID_RunOnStartOS, appProp->GetLngRes(wxT("lr_prpageRunOnStartOS")), wxDefaultPosition, wxDefaultSize);
	if(m_RunOnStartOS)
		checkBox3->Set3StateValue(wxCHK_CHECKED);
	else
		checkBox3->Set3StateValue(wxCHK_UNCHECKED);
    itemSizer3->Add(checkBox3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add(itemSizer3, 0, wxGROW|wxALL, 0);

    //ID_MoveInTray
    wxBoxSizer* itemSizer4 = new wxBoxSizer( wxHORIZONTAL );
	wxCheckBox* checkBox4 = new wxCheckBox(panel, ID_MoveInTray, appProp->GetLngRes(wxT("lr_prpageMoveInTray")), wxDefaultPosition, wxDefaultSize);
	if(m_MoveInTray)
		checkBox4->Set3StateValue(wxCHK_CHECKED);
	else
		checkBox4->Set3StateValue(wxCHK_UNCHECKED);
    itemSizer4->Add(checkBox4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add(itemSizer4, 0, wxGROW|wxALL, 0);

	//ID_Password
    wxStaticBox* staticBox1 = new wxStaticBox(panel, wxID_ANY, appProp->GetLngRes(wxT("lr_prpagePassword")));
    wxBoxSizer* itemSizer5 = new wxStaticBoxSizer( staticBox1, wxHORIZONTAL );

    wxTextCtrl* textCtrl = new wxTextCtrl(panel, ID_Password, wxEmptyString, wxDefaultPosition,
										  wxSize(150, wxDefaultCoord),wxTE_PASSWORD|wxEVT_COMMAND_TEXT_ENTER);
	textCtrl->SetValue(m_Password);
    itemSizer5->Add(textCtrl, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add(itemSizer5, 0, wxGROW|wxLEFT|wxRIGHT, 5);

//    wxString autoSaveLabel = _("&Auto-save every");
//    wxString minsLabel = _("mins");
//
//    wxBoxSizer* itemSizer12 = new wxBoxSizer( wxHORIZONTAL );
//    wxCheckBox* checkBox12 = new wxCheckBox(panel, ID_AUTO_SAVE, autoSaveLabel, wxDefaultPosition, wxDefaultSize);
//
//#if wxUSE_SPINCTRL
//    wxSpinCtrl* spinCtrl12 = new wxSpinCtrl(panel, ID_AUTO_SAVE_MINS, wxEmptyString,
//        wxDefaultPosition, wxSize(40, wxDefaultCoord), wxSP_ARROW_KEYS, 1, 60, 1);
//#endif
//
//    itemSizer12->Add(checkBox12, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
//#if wxUSE_SPINCTRL
//    itemSizer12->Add(spinCtrl12, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
//#endif
//    itemSizer12->Add(new wxStaticText(panel, wxID_STATIC, minsLabel), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
//    item0->Add(itemSizer12, 0, wxGROW|wxALL, 0);
//
//    //// TOOLTIPS
//
//    wxBoxSizer* itemSizer8 = new wxBoxSizer( wxHORIZONTAL );
//    wxCheckBox* checkBox6 = new wxCheckBox(panel, ID_SHOW_TOOLTIPS, _("Show &tooltips"), wxDefaultPosition, wxDefaultSize);
//    itemSizer8->Add(checkBox6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
//    item0->Add(itemSizer8, 0, wxGROW|wxALL, 0);
//

	topSizer->Add( item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    return panel;
}
//**************************************************
wxPanel* CSettingsDialog::CreateAdditionalSettingsPage(wxWindow* parent)
{
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
	CAppProperties *appProp = wxGetApp().GetAppProperties();

	//ID_Update
	wxStaticBox* staticBox1 = new wxStaticBox(panel, wxID_ANY, appProp->GetLngRes(wxT("lr_prpagePopertyUpdateName")));
	wxBoxSizer* itemSizer1 = new wxStaticBoxSizer( staticBox1, wxVERTICAL );
	wxCheckBox* checkBox1 = new wxCheckBox(panel, ID_AllowUpdate, appProp->GetLngRes(wxT("lr_prpageAllowUpdate")), wxDefaultPosition, wxDefaultSize);
	if(m_AllowUpdate)
		checkBox1->Set3StateValue(wxCHK_CHECKED);
	else
		checkBox1->Set3StateValue(wxCHK_UNCHECKED);
	itemSizer1->Add(checkBox1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	//itemSizer1->Add(5, 5, 1, wxALL, 0);
	//itemSizer1->Add(new wxStaticText(panel, wxID_ANY, appProp->GetLngRes(lr_prpageCiteUpdate)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	wxTextCtrl* textCtrl = new wxTextCtrl(panel, ID_CiteUpdate, wxEmptyString, wxDefaultPosition, wxSize(150, wxDefaultCoord));
	textCtrl->SetValue(m_CiteUpdate);
	itemSizer1->Add(textCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	item0->Add(itemSizer1, 0, wxGROW|wxALL, 5);

	//ID_ArrayLanguage
	wxStaticBox* staticBox2 = new wxStaticBox(panel, wxID_ANY, appProp->GetLngRes(wxT("lr_prpageLanguageInterface")));
	wxBoxSizer* styleSizer = new wxStaticBoxSizer(staticBox2, wxVERTICAL);
	wxBoxSizer* itemSizer2 = new wxBoxSizer( wxHORIZONTAL );
	wxChoice* choice2 = new wxChoice(panel, ID_ArrayLanguage, wxDefaultPosition, wxSize(150, wxDefaultCoord), appProp->GetArrayLanguage());

	//itemSizer2->Add(new wxStaticText(panel, wxID_ANY, _("&Window:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	//itemSizer2->Add(5, 5, 1, wxALL, 0);
	choice2->Select(m_Language);
	itemSizer2->Add(choice2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	styleSizer->Add(itemSizer2, 0, wxGROW|wxALL, 5);
	item0->Add(styleSizer, 0, wxGROW|wxALL, 5);
	topSizer->Add( item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
	//topSizer->AddSpacer(5);

	panel->SetSizer(topSizer);
	topSizer->Fit(panel);

	return panel;
}
//**************************************************
BEGIN_EVENT_TABLE(CSpyModalDialog, wxDialog)
	EVT_BUTTON(wxID_ANY, CSpyModalDialog::OnButton)
	EVT_MOUSE_EVENTS(CSpyModalDialog::OnMouseEvent)
	//EVT_LEFT_DOWN(CSpyModalDialog::OnLeftDown)
END_EVENT_TABLE()
//**************************************************
void CSpyModalDialog::OnMouseEvent(wxMouseEvent &event)
{
	if((event.Dragging() || event.LeftDown()))
	{
		::SetCursor((HCURSOR)wxGetApp().GetAppProperties()->GetSpyCursor().GetHCURSOR());
		if (!HasCapture())
		{
			CaptureMouse();
		}
		POINT coord; 
		GetCursorPos(&coord);
		HWND hwndParent = WindowFromPoint(coord);
		if(hwndParent != NULL)
		{
			//if(m_oldWndHWND != hwndParent)
			//{
				HDC hdc;
				RECT rc;
				wxScreenDC dc;
				if(m_oldWndHWND != hwndParent)
				{
					//HWND redr = GetWindow(m_oldWndHWND, GW_OWNER);
					//GetWindowRect(m_oldWndHWND, &rc);
					::InvalidateRect(NULL, NULL/*&rc*/, true);
				}
				m_oldWndHWND = hwndParent;

				GetWindowRect(hwndParent, &rc);
				hdc = GetWindowDC(hwndParent);

				dc.SetHDC(hdc);
				dc.SetPen(wxPen(*wxRED, 2, wxSOLID));
				dc.DrawLine(0, 0, rc.right-rc.left, 0);
				dc.DrawLine(rc.right-rc.left, 0, rc.right-rc.left, rc.bottom-rc.top);
				dc.DrawLine(rc.right-rc.left, rc.bottom-rc.top, 0, rc.bottom-rc.top);
				dc.DrawLine(0, rc.bottom-rc.top, 0, 0);
			
				ReleaseDC(hwndParent, hdc);
			//}
		}
	}
	else if(event.LeftUp())
	{
		if (HasCapture()) 
		{
			// uncapture mouse
			ReleaseMouse();
		}
		::InvalidateRect(NULL, NULL, true);
	
		PWINDOWINFO pwi = new WINDOWINFO(); 
		LPWSTR pszMem;
		int cTxtLen;
		CAppProperties *ap = wxGetApp().GetAppProperties();

		cTxtLen = GetWindowTextLength(m_oldWndHWND);
		pszMem  = (LPWSTR) VirtualAlloc((LPVOID) NULL, (DWORD) (cTxtLen + 1), MEM_COMMIT, PAGE_READWRITE);
		GetWindowText(m_oldWndHWND, pszMem, cTxtLen + 1);
		wxString strResult(pszMem);
		VirtualFree(pszMem, 0, MEM_RELEASE);

		wxString str;
		str.Pad(20/*+wxString("Capture Window").Len()*/, '=');
		wxGetApp().GetTextFrame()->SetDefaultStyle(wxTextAttr(*wxBLACK));
		wxGetApp().GetTextFrame()->AppendText(str);
		wxGetApp().GetTextFrame()->AppendText(ap->GetLngRes(wxT("lr_capturewindow")));
		wxGetApp().GetTextFrame()->AppendText(str+wxT("\n"));
		wxGetApp().GetTextFrame()->AppendText(ap->GetLngRes(wxT("lr_prpgridFndWinComLabelWin"))+wxT(":"));
		wxGetApp().GetTextFrame()->SetDefaultStyle(wxTextAttr(*wxBLACK, *wxCYAN));
		wxGetApp().GetTextFrame()->AppendText(strResult+wxT("\n"));
		if(GetWindowInfo(m_oldWndHWND, pwi))
		{
			wxGetApp().GetTextFrame()->SetDefaultStyle(wxTextAttr(*wxBLACK, *wxWHITE));
			wxGetApp().GetTextFrame()->AppendText(ap->GetLngRes(wxT("lr_prpgridFndWinComClassWin"))+wxT(":"));
			wxGetApp().GetTextFrame()->SetDefaultStyle(wxTextAttr(*wxBLACK, *wxCYAN));
			wxGetApp().GetTextFrame()->AppendText(wxString::Format(wxT("%d"), pwi->atomWindowType) + wxT("\n"));
		}
		wxGetApp().GetTextFrame()->SetDefaultStyle(wxTextAttr(*wxBLACK, *wxWHITE));
		//wxGetApp().GetTextFrame()->AppendText(str+wxT("\n"));
		wxDELETE(pwi);
	}
	event.Skip();
}
//**************************************************
CSpyModalDialog::CSpyModalDialog(wxWindow *parent)
: wxDialog(parent, wxID_ANY, wxGetApp().GetAppProperties()->GetLngRes(wxT("lr_menuSpyName")))
{
	CAppProperties *appProp = wxGetApp().GetAppProperties();
    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* itemSizer1 = new wxBoxSizer( wxVERTICAL );
	itemSizer1->Add(new wxStaticText(this, wxID_ANY, appProp->GetLngRes(wxT("lr_spydlgIntro")),wxDefaultPosition,wxSize(250, 50)), 0, wxALL|wxGROW, 5);

	//wxIcon icon("Spy1.ico",wxBITMAP_TYPE_ICO,32,32);

    wxButton *btnOk = new wxButton(this, wxID_CANCEL, _T("&Close"));
	sizerTop->Add(itemSizer1, 0, wxGROW|wxALL, 5 );
	m_bitmap = new wxStaticBitmap(this, wxID_ANY, wxBitmap(appProp->GetIco32(icr32_SpyOn)));
	m_bitmap->SetEventHandler(this);
	sizerTop->Add(m_bitmap,0,wxALIGN_CENTER | wxALL, 5);
	sizerTop->Add(btnOk, 0, wxALIGN_CENTER | wxALL, 5);

    SetSizer(sizerTop);
    sizerTop->SetSizeHints(this);
    sizerTop->Fit(this);
	
	m_oldWndHWND = NULL;
	wxGetApp().GetTopWindow()->Show(false);
    //m_btnModal->SetFocus();
    //m_btnModal->SetDefault();
}
//**************************************************
void CSpyModalDialog::OnButton(wxCommandEvent& event)
{
//    if ( event.GetEventObject() == m_btnDelete )
//    {
//        delete m_btnModal;
//        m_btnModal = NULL;
//
//        m_btnDelete->Disable();
//    }
//    else if ( event.GetEventObject() == m_btnModal )
//    {
//#if wxUSE_TEXTDLG
//        wxGetTextFromUser(_T("Dummy prompt"),
//                          _T("Modal dialog called from dialog"),
//                          wxEmptyString, this);
//#else
//        wxMessageBox(_T("Modal dialog called from dialog"));
//#endif // wxUSE_TEXTDLG
//    }
//    else if ( event.GetEventObject() == m_btnModeless )
//    {
//        (new CSpyModalDialog(this))->Show();
//    }
//    else
//    {
	wxGetApp().GetTopWindow()->Show(true);
	wxGetApp().GetTopWindow()->SetWindowStyle(wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);
    event.Skip();
    //}
}
//**************************************************
BEGIN_EVENT_TABLE(CLoginDialog, wxDialog)
	EVT_BUTTON(wxID_OK, CLoginDialog::OnButtonOk)
END_EVENT_TABLE()
//**************************************************
void CLoginDialog::OnButtonOk(wxCommandEvent& event)
{
	wxString StringForHash = PasswordCtrl.GetValue();

	byte abDigest[MD5::DIGESTSIZE];
	MD5().CalculateDigest(abDigest, (const byte *)StringForHash.c_str(), StringForHash.Len() * sizeof(wchar_t));
	wxString hash;
	for(int i = 0; i < MD5::DIGESTSIZE; i++)
		hash += wxString::Format(_T("%2.2x"), abDigest[i]);

	if(wxGetApp().GetAppProperties()->GetHashPassword().c_str() != hash)
	{
		wxMessageBox(wxT("Wrong password."), wxT("AutoBot"), wxICON_ERROR);
		PasswordCtrl.SetFocus();
		return;
	}
	event.Skip();
}
//**************************************************
CLoginDialog::CLoginDialog()
{
	Create(NULL, wxID_ANY, wxT("AutoBot - authorize"));

	PasswordCtrl.Create(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);

	wxBoxSizer *lpVertSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *lpHorSizer = new wxBoxSizer(wxHORIZONTAL);

	lpVertSizer->Add(new wxStaticText(this, wxID_ANY , wxT("Password:")), 0, wxLEFT | wxRIGHT, 15);
	lpVertSizer->Add(&PasswordCtrl, 0, wxEXPAND | wxALL, 15);
	lpVertSizer->Add(lpHorSizer, 0, wxALIGN_CENTER);

	wxButton* okbtn = new wxButton(this, wxID_OK , wxT("OK"));
	okbtn->SetDefault();
	lpHorSizer->Add(okbtn, 0, wxALL, 15);
	lpHorSizer->Add(new wxButton(this, wxID_CANCEL , wxT("Cancel")), 0, wxTOP | wxBOTTOM | wxRIGHT, 15);
	SetSizerAndFit(lpVertSizer);
	Center();
}
//**************************************************
CLoginDialog::~CLoginDialog()
{
}
//**************************************************