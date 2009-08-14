//**************************************************
#include "AutoBot.h"
//**************************************************
IMPLEMENT_APP(wxAutoBotApp) ;
//**************************************************
BEGIN_EVENT_TABLE(wxAutoBotApp, wxApp)
	EVT_TIMER(wxID_ANY, wxAutoBotApp::OnTimer)
END_EVENT_TABLE()
//**************************************************
BEGIN_EVENT_TABLE(wxAutoBotFrame, wxFrame)
	EVT_ERASE_BACKGROUND(wxAutoBotFrame::OnEraseBackground)
	EVT_SIZE(wxAutoBotFrame::OnSize)
	EVT_ICONIZE(wxAutoBotFrame::OnIconize)
	EVT_CHAR(wxAutoBotFrame::OnChar)
	EVT_CLOSE(wxAutoBotFrame::OnClose)
	EVT_MENU(wxID_EXIT,				wxAutoBotFrame::OnExit)
	EVT_MENU(wxID_ABOUT,			wxAutoBotFrame::OnAbout)
	EVT_MENU(Menu_Save,				wxAutoBotFrame::OnSave)
	EVT_MENU(Menu_Property,			wxAutoBotFrame::OnProperty)
	EVT_MENU(Menu_ComponentPanel,	wxAutoBotFrame::OnComponentPanel)
	EVT_MENU(Menu_TreeBots,			wxAutoBotFrame::OnTreeBots)
	EVT_MENU(Menu_MessageWindow,	wxAutoBotFrame::OnMessageWindow)
	EVT_MENU(Menu_VarStack,			wxAutoBotFrame::OnVarStack)
	EVT_MENU(Menu_PropertyWindow,	wxAutoBotFrame::OnPropertyWindow)
	EVT_MENU(Menu_WelcomeWindow,	wxAutoBotFrame::OnWelcomeWindow)
	EVT_MENU(Menu_SchematicView,	wxAutoBotFrame::OnSchematicView)
	EVT_MENU(Menu_ScriptWindow,		wxAutoBotFrame::OnScintillaView)
	EVT_MENU(Menu_Toolbar,			wxAutoBotFrame::OnToolbar)
	EVT_MENU(Menu_Add,				wxAutoBotFrame::OnAdd)
	EVT_MENU(Menu_Delete,			wxAutoBotFrame::OnDelete)
	EVT_MENU(Menu_Start,			wxAutoBotFrame::OnStart)
	EVT_MENU(Menu_Stop,				wxAutoBotFrame::OnStop)
	EVT_MENU(Menu_Pause,			wxAutoBotFrame::OnPause)
	EVT_MENU(wxID_HELP,				wxAutoBotFrame::OnHelp)
	EVT_MENU(Menu_AddVar,			wxAutoBotFrame::OnAddVar)
	EVT_MENU(Menu_DeleteVar,		wxAutoBotFrame::OnDeleteVar)
	EVT_MENU(Menu_Spy,				wxAutoBotFrame::OnSpy)
	EVT_MENU(Menu_Debug,			wxAutoBotFrame::OnDebug)
	EVT_MENU(Menu_SaveScript,		wxAutoBotFrame::OnSaveScript)
	EVT_MENU(wxID_UNDO,				wxAutoBotFrame::OnUndo)
	EVT_MENU(wxID_REDO,				wxAutoBotFrame::OnRedo)
	EVT_MENU(wxID_COPY,				wxAutoBotFrame::OnCopy)
	EVT_MENU(wxID_CUT,				wxAutoBotFrame::OnCut)
	EVT_MENU(wxID_PASTE,			wxAutoBotFrame::OnPaste)
	EVT_MENU(wxID_SELECTALL,		wxAutoBotFrame::OnSelectAll)
	EVT_MENU(IDM_SAVEASBITMAP,		wxAutoBotFrame::OnExportToBMP)
	EVT_MENU(wxID_PRINT,			wxAutoBotFrame::OnPrint)
	EVT_MENU(wxID_PREVIEW,			wxAutoBotFrame::OnPrintPreview)
	EVT_MENU(wxID_PAGE_SETUP,		wxAutoBotFrame::OnPageSetup)
	EVT_UPDATE_UI(Menu_Start,		wxAutoBotFrame::OnUpdateStart)
	EVT_UPDATE_UI(Menu_Debug,		wxAutoBotFrame::OnUpdateDebug)
	EVT_UPDATE_UI(wxID_COPY,		wxAutoBotFrame::OnUpdateCopy)
	EVT_UPDATE_UI(wxID_CUT,			wxAutoBotFrame::OnUpdateCut)
	EVT_UPDATE_UI(wxID_PASTE,		wxAutoBotFrame::OnUpdatePaste)
	EVT_UPDATE_UI(wxID_UNDO,		wxAutoBotFrame::OnUpdateUndo)
	EVT_UPDATE_UI(wxID_REDO,		wxAutoBotFrame::OnUpdateRedo)
	EVT_MENU(Menu_UndoScript,		wxAutoBotFrame::OnEdit)
	EVT_MENU(Menu_RedoScript,		wxAutoBotFrame::OnEdit)
	EVT_MENU(Menu_CutScript,		wxAutoBotFrame::OnEdit)
	EVT_MENU(Menu_CopyScript,		wxAutoBotFrame::OnEdit)
	EVT_MENU(Menu_PasteScript,		wxAutoBotFrame::OnEdit)
	EVT_MENU(Menu_DeleteScript,		wxAutoBotFrame::OnEdit)
	EVT_MENU(Menu_FindScript,		wxAutoBotFrame::OnEdit)
	EVT_UPDATE_UI(Toolscript_Ctrl,	wxAutoBotFrame::OnUpdateScriptToolbar)
	EVT_COMMAND_SCROLL(wxID_ZOOM_FIT, wxAutoBotFrame::OnSlider)
	EVT_TOOL_RANGE(IDT_FIRST_TOOLMARKER, IDT_LAST_TOOLMARKER, wxAutoBotFrame::OnTool)
	EVT_UPDATE_UI_RANGE(IDT_FIRST_TOOLMARKER, IDT_LAST_TOOLMARKER, wxAutoBotFrame::OnUpdateTool)
	EVT_SHOW(wxAutoBotFrame::OnShow)
	EVT_AUI_PANE_CLOSE(wxAutoBotFrame::OnPaneClose)
	EVT_AUINOTEBOOK_PAGE_CLOSE(Notebook_Ctrl, wxAutoBotFrame::OnNotebookPageClose)
	EVT_AUINOTEBOOK_PAGE_CHANGED(Notebook_Ctrl, wxAutoBotFrame::OnNotebookPageChanged)
END_EVENT_TABLE()
//**************************************************
//**Реализация класса Окна-Приложения
wxAutoBotFrame::wxAutoBotFrame(wxWindow * parent, 
							   wxWindowID id, 
							   const wxString & title,
							   const wxPoint & pos, 
							   const wxSize & size, 
							   long style):wxFrame(parent, id, title, pos, size, style)
{
	CAppProperties *ap = wxGetApp().GetAppProperties();
	//PushEventHandler(new AutoBotEvtHandler());
	// сказать wxAuiManager управлять этим окном
	m_mgr.SetManagedWindow(this);
	// зададим минимально возможной размер окна
	SetMinSize(wxSize(400,300));

	//Задаём иконку приложения
	SetIcon(ap->GetIco(icr_appico));

	//Строим меню приложения
	m_menuFile = new wxMenu;
	m_menuFile->Append(Menu_Save, ap->GetLngRes(wxT("lr_menuSaveName")));
	//m_menuFile->Append(Menu_Load, ap->GetLngRes(wxT("lr_menuLoadName")));
	//m_menuFile->Enable(Menu_Load, false);
	m_menuFile->AppendSeparator();
	m_menuFile->Append(Menu_Property, ap->GetLngRes(wxT("lr_menuPropertyName")));
	//m_menuFile->Append(Menu_Import, ap->GetLngRes(wxT("lr_menuImportName")));
	m_menuFile->AppendSeparator();
	m_menuFile->Append(wxID_EXIT, ap->GetLngRes(wxT("lr_menuExitName")));

	m_menuView = new wxMenu;
	m_menuView->AppendCheckItem(Menu_ComponentPanel, ap->GetLngRes(wxT("lr_menuComponentPanelName")), ap->GetLngRes(wxT("lr_menuComponentPanelDesc")));
	m_menuView->Check(Menu_ComponentPanel, true);
	m_menuView->AppendCheckItem(Menu_TreeBots, ap->GetLngRes(wxT("lr_menuTreeBotsName")), ap->GetLngRes(wxT("lr_menuTreeBotsDesc")));
	m_menuView->Check(Menu_TreeBots, true);
	m_menuView->AppendCheckItem(Menu_MessageWindow, ap->GetLngRes(wxT("lr_menuMessageWindowName")), ap->GetLngRes(wxT("lr_menuMessageWindowDesc")));
	m_menuView->Check(Menu_MessageWindow, true);
	m_menuView->AppendCheckItem(Menu_VarStack, ap->GetLngRes(wxT("lr_menuVarStackName")), ap->GetLngRes(wxT("lr_menuVarStackDesc")));
	m_menuView->Check(Menu_VarStack, true);
	m_menuView->AppendCheckItem(Menu_PropertyWindow, ap->GetLngRes(wxT("lr_menuPropertyWindowName")), ap->GetLngRes(wxT("lr_menuPropertyWindowDesc")));
	m_menuView->Check(Menu_PropertyWindow, true);
	m_menuView->AppendCheckItem(Menu_WelcomeWindow, ap->GetLngRes(wxT("lr_menuWelcomeWindowName")), ap->GetLngRes(wxT("lr_menuWelcomeWindowDesc")));
	m_menuView->Check(Menu_WelcomeWindow, true);
	m_menuView->AppendCheckItem(Menu_SchematicView, ap->GetLngRes(wxT("lr_menuSchematicViewName")), ap->GetLngRes(wxT("lr_menuSchematicViewDesc")));
	m_menuView->Check(Menu_SchematicView, true);
	m_menuView->AppendSeparator();
	m_menuView->AppendCheckItem(Menu_Toolbar, ap->GetLngRes(wxT("lr_menuToolbarName")), ap->GetLngRes(wxT("lr_menuToolbarDesc")));
	m_menuView->Check(Menu_Toolbar, true);
	m_menuView->AppendSeparator();
	m_menuView->AppendCheckItem(Menu_ScriptWindow, ap->GetLngRes(wxT("lr_menuScriptWindowName")), ap->GetLngRes(wxT("lr_menuScriptWindowDesc")));
	m_menuView->Check(Menu_ScriptWindow, true);
	m_menuView->AppendCheckItem(Menu_StatisticsWindow, ap->GetLngRes(wxT("lr_menuStatisticsWindowName")), ap->GetLngRes(wxT("lr_menuStatisticsWindowDesc")));
	m_menuView->Check(Menu_StatisticsWindow, false);
	m_menuView->Enable(Menu_StatisticsWindow, false);

	m_menuAction = new wxMenu;
	m_menuAction->Append(Menu_Add, ap->GetLngRes(wxT("lr_add")));
	m_menuAction->Append(Menu_Delete, ap->GetLngRes(wxT("lr_delete")));
	m_menuAction->AppendSeparator();
	m_menuAction->Append(Menu_Start, ap->GetLngRes(wxT("lr_start")));
	m_menuAction->Append(Menu_Stop, ap->GetLngRes(wxT("lr_stop")));
	m_menuAction->Append(Menu_Pause, ap->GetLngRes(wxT("lr_pause")));
	m_menuAction->AppendSeparator();
	m_menuAction->Append(Menu_Debug, ap->GetLngRes(wxT("lr_debug")));
	m_menuAction->AppendSeparator();
	m_menuAction->Append(Menu_AddVar, ap->GetLngRes(wxT("lr_menuAddVarName")));
	m_menuAction->Append(Menu_DeleteVar, ap->GetLngRes(wxT("lr_menuDeleteVarName")));
	m_menuAction->AppendSeparator();
	m_menuAction->Append(Menu_Spy, ap->GetLngRes(wxT("lr_menuSpyName")));

	m_menuHelp = new wxMenu;
	m_menuHelp->Append(wxID_HELP, ap->GetLngRes(wxT("lr_menuHelpName")));
	m_menuHelp->Append(wxID_ABOUT, ap->GetLngRes(wxT("lr_menuAboutName")));

	m_menuEditor = new wxMenu;
	m_menuEditor->Append(Menu_SaveScript, ap->GetLngRes(wxT("lr_savescript")));
	m_menuEditor->AppendSeparator();
	m_menuEditor->Append(Menu_UndoScript, wxT("&Undo\tCtrl+Z"));
	m_menuEditor->Append(Menu_RedoScript, wxT("&Redo\tCtrl+Shift+Z"));
	m_menuEditor->AppendSeparator();
	m_menuEditor->Append(Menu_CutScript, ap->GetLngRes(wxT("lr_cut")) + wxT("\tCtrl+X"));
	m_menuEditor->Append(Menu_CopyScript, ap->GetLngRes(wxT("lr_copy")) + wxT("\tCtrl+C"));
	m_menuEditor->Append(Menu_PasteScript, ap->GetLngRes(wxT("lr_paste")) + wxT("\tCtrl+V"));
	m_menuEditor->Append(Menu_DeleteScript, ap->GetLngRes(wxT("lr_delete")) + wxT("\tDel"));
	//m_menuEditor->AppendSeparator();
	//m_menuEditor->Append(Menu_FindScript, ap->GetLngRes(wxT("lr_find")) + wxT("\tCtrl+F"));

	m_menuBar = new wxMenuBar;
	m_menuBar->Append(m_menuFile, ap->GetLngRes(wxT("lr_menuFileName")));
	m_menuBar->Append(m_menuView, ap->GetLngRes(wxT("lr_menuViewName")));
	m_menuBar->Append(m_menuAction, ap->GetLngRes(wxT("lr_menuActionName")));
	m_menuBar->Append(m_menuEditor, ap->GetLngRes(wxT("lr_menuEditor")));
	m_menuBar->Append(m_menuHelp, ap->GetLngRes(wxT("lr_menuHelp2Name")));

	// ... и присоединяем к менюбару
	SetMenuBar(m_menuBar);

	// создадим строку состояния
	CreateStatusBar();
	GetStatusBar()->SetStatusText(ap->GetLngRes(wxT("lr_ready")));

	// создадим панель инструментов
	wxIcon iconAdd(ap->GetIco(icr_add));
	wxIcon iconSave(ap->GetIco(icr_save));
	//wxIcon iconLoad(ap->GetIco(icr_load));
	wxIcon iconStop(ap->GetIco(icr_stop));
	wxIcon iconHelp(ap->GetIco(icr_help));
	wxIcon iconStart(ap->GetIco(icr_start));
	wxIcon iconPause(ap->GetIco(icr_pause));
	wxIcon iconDelete(ap->GetIco(icr_delete));
	wxIcon iconAddVar(ap->GetIco(icr_addvar));
	wxIcon iconDeleteVar(ap->GetIco(icr_deletevar));
	wxIcon iconSpy(ap->GetIco(icr_spy));
	wxIcon iconDebug(ap->GetIco(icr_debug));
	wxIcon iconSaveScript(ap->GetIco(icr_savescript));
	wxIcon iconUndoScript(ap->GetIco(icr_undoscript));
	wxIcon iconRedoScript(ap->GetIco(icr_redoscript));
	wxIcon iconCutScript(ap->GetIco(icr_cutscript));
	wxIcon iconCopyScript(ap->GetIco(icr_copyscript));
	wxIcon iconPasteScript(ap->GetIco(icr_pastescript));
	wxIcon iconDeleteScript(ap->GetIco(icr_deletescript));
	wxIcon iconFindScript(ap->GetIco(icr_findscript));

	//wxAuiToolBarItemArray prepend_items;
	//wxAuiToolBarItemArray append_items;
	//wxAuiToolBarItem item;
	//item.SetKind(wxITEM_SEPARATOR);
	//append_items.Add(item);
	//item.SetKind(wxITEM_NORMAL);
	//item.SetId(ID_CustomizeToolbar);
	//item.SetLabel(_("Customize..."));
	//append_items.Add(item);
	
	wxAuiToolBar *m_toolbar = new wxAuiToolBar(this, Toolbar_Ctrl, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW);
	m_toolbar->SetToolBitmapSize(wxSize(16,16));
	m_toolbar->AddTool(Menu_Save, ap->GetLngRes(wxT("lr_save")),
					 wxBitmap(iconSave), ap->GetLngRes(wxT("lr_menuSaveName")), wxITEM_NORMAL);
	//m_toolbar->AddTool(Menu_Load, ap->GetLngRes(wxT("lr_open")),
	//				 wxBitmap(iconLoad), ap->GetLngRes(wxT("lr_menuLoadName")), wxITEM_NORMAL);
	m_toolbar->AddSeparator();
	m_toolbar->AddTool(Menu_Add, ap->GetLngRes(wxT("lr_add")),
					 wxBitmap(iconAdd), ap->GetLngRes(wxT("lr_toolAddDesc")), wxITEM_NORMAL);
	m_toolbar->AddTool(Menu_Delete, ap->GetLngRes(wxT("lr_delete")),
					 wxBitmap(iconDelete), ap->GetLngRes(wxT("lr_toolDeleteDesc")), wxITEM_NORMAL);
	m_toolbar->AddSeparator();
	m_toolbar->AddTool(wxID_COPY, wxT("Copy"), wxArtProvider::GetBitmap(wxART_COPY, wxART_MENU), wxT("Copy to clipboard"),wxITEM_NORMAL);
	m_toolbar->AddTool(wxID_CUT, wxT("Cut"), wxArtProvider::GetBitmap(wxART_CUT, wxART_MENU), wxT("Cut to clipboard"),wxITEM_NORMAL);
	m_toolbar->AddTool(wxID_PASTE, wxT("Paste"), wxArtProvider::GetBitmap(wxART_PASTE, wxART_MENU), wxT("Paste from clipboard"),wxITEM_NORMAL);
	m_toolbar->AddSeparator();
	m_toolbar->AddTool(Menu_Start, ap->GetLngRes(wxT("lr_start")),
					 wxBitmap(iconStart), ap->GetLngRes(wxT("lr_start")), wxITEM_NORMAL);
	m_toolbar->AddTool(Menu_Pause, ap->GetLngRes(wxT("lr_pause")),
					 wxBitmap(iconPause), ap->GetLngRes(wxT("lr_pause")), wxITEM_NORMAL);
	m_toolbar->AddTool(Menu_Stop, ap->GetLngRes(wxT("lr_stop")),
					 wxBitmap(iconStop), ap->GetLngRes(wxT("lr_stop")), wxITEM_NORMAL);
	m_toolbar->AddSeparator();
	m_toolbar->AddTool(Menu_Debug, ap->GetLngRes(wxT("lr_debug")),
					 wxBitmap(iconDebug), ap->GetLngRes(wxT("lr_debug")), wxITEM_NORMAL);
	m_toolbar->AddSeparator();
	m_toolbar->AddTool(Menu_AddVar, ap->GetLngRes(wxT("lr_menuAddVarName")),
					 wxBitmap(iconAddVar), ap->GetLngRes(wxT("lr_toolAddVarDesc")), wxITEM_NORMAL);
	m_toolbar->AddTool(Menu_DeleteVar, ap->GetLngRes(wxT("lr_menuDeleteVarName")),
					 wxBitmap(iconDeleteVar), ap->GetLngRes(wxT("lr_toolDeleteVarDesc")), wxITEM_NORMAL);
	m_toolbar->AddSeparator();
	m_toolbar->AddTool(Menu_Spy, ap->GetLngRes(wxT("lr_menuSpyName")),
					 wxBitmap(iconSpy), ap->GetLngRes(wxT("lr_menuSpyName")), wxITEM_NORMAL);
	m_toolbar->AddSeparator();
	m_toolbar->AddTool(wxID_HELP, ap->GetLngRes(wxT("lr_menuHelpName")),
					 wxBitmap(iconHelp), ap->GetLngRes(wxT("lr_menuHelpName")), wxITEM_NORMAL);
	//m_toolbar->SetCustomOverflowItems(prepend_items, append_items);
	m_toolbar->Realize();

	wxAuiToolBar *m_toolscript = new wxAuiToolBar(this, Toolscript_Ctrl, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW);
	m_toolscript->SetToolBitmapSize(wxSize(16,16));
	m_toolscript->AddTool(Menu_SaveScript, ap->GetLngRes(wxT("lr_savescript")),
						wxBitmap(iconSaveScript), ap->GetLngRes(wxT("lr_savescript")), wxITEM_NORMAL);
	m_toolscript->AddSeparator();
	m_toolscript->AddTool(Menu_UndoScript, wxT("Undo"), wxBitmap(iconUndoScript), wxT("Undo"), wxITEM_NORMAL);
	m_toolscript->AddTool(Menu_RedoScript, wxT("Redo"), wxBitmap(iconRedoScript), wxT("Redo"), wxITEM_NORMAL);
	m_toolscript->AddSeparator();
	m_toolscript->AddTool(Menu_CutScript, ap->GetLngRes(wxT("lr_cut")), 
						wxBitmap(iconCutScript), ap->GetLngRes(wxT("lr_cut")), wxITEM_NORMAL);
	m_toolscript->AddTool(Menu_CopyScript, ap->GetLngRes(wxT("lr_copy")), 
						wxBitmap(iconCopyScript), ap->GetLngRes(wxT("lr_copy")), wxITEM_NORMAL);
	m_toolscript->AddTool(Menu_PasteScript, ap->GetLngRes(wxT("lr_paste")), 
						wxBitmap(iconPasteScript), ap->GetLngRes(wxT("lr_paste")), wxITEM_NORMAL);
	m_toolscript->AddTool(Menu_DeleteScript, ap->GetLngRes(wxT("lr_delete")), 
						wxBitmap(iconDeleteScript), ap->GetLngRes(wxT("lr_delete")), wxITEM_NORMAL);
	//m_toolscript->AddSeparator();
	//m_toolscript->AddTool(Menu_FindScript, ap->GetLngRes(wxT("lr_find")), 
	//					wxBitmap(iconFindScript), ap->GetLngRes(wxT("lr_find")), wxITEM_NORMAL);
	m_toolscript->Realize();

	m_pMainSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	m_pMainSizer->AddGrowableCol( 0 );
	m_pMainSizer->AddGrowableRow( 0 );
	m_pMainSizer->SetFlexibleDirection( wxBOTH );
	m_pMainSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	m_pCanvasPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_pMainSizer->Add( m_pCanvasPanel, 1, wxEXPAND, 5 );

	wxAuiToolBar *m_toolscheme = new wxAuiToolBar(this, Toolscheme_Ctrl, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW);
	m_zoomSlider = new wxSlider( m_toolscheme, wxID_ZOOM_FIT, 50, 2, 99, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	m_zoomSlider->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	m_zoomSlider->SetToolTip( wxT("Set scheme scale") );

	m_pMainSizer->Add( m_zoomSlider, 0, wxEXPAND, 5 );
	m_toolscheme->SetToolBitmapSize(wxSize(16,16));
	m_toolscheme->AddTool(wxID_PRINT, wxT("Print"), wxArtProvider::GetBitmap(wxART_PRINT, wxART_MENU), wxT("Print..."),wxITEM_NORMAL);
	m_toolscheme->AddTool(wxID_PREVIEW, wxT("Preview"), wxArtProvider::GetBitmap(wxART_FIND, wxART_MENU), wxT("Print preview..."),wxITEM_NORMAL);
	m_toolscheme->AddSeparator();
	m_toolscheme->AddTool(wxID_UNDO, wxT("Undo"), wxArtProvider::GetBitmap(wxART_UNDO, wxART_MENU), wxT("Undo"),wxITEM_NORMAL);
	m_toolscheme->AddTool(wxID_REDO, wxT("Redo"), wxArtProvider::GetBitmap(wxART_REDO, wxART_MENU), wxT("Redo"),wxITEM_NORMAL);
	m_toolscheme->AddSeparator();
	//m_toolscheme->AddTool(IDT_GRID, wxT("Grid"), wxBitmap(Grid_xpm), wxT("Show/hide grid"),wxITEM_CHECK);//Глюк какой то
	m_toolscheme->AddTool(IDT_GRID, wxT("Grid"), wxBitmap(Grid_xpm), wxT("Show/hide grid"),wxITEM_NORMAL);
	m_toolscheme->AddTool(IDT_TOOL, wxT("Tool"), wxBitmap(Tool_xpm), wxT("Design tool"),wxITEM_NORMAL);
	m_toolscheme->AddSeparator();
	m_toolscheme->AddTool(IDT_ALIGN_LEFT, wxT("Align left"), wxBitmap(AlignLeft_xpm), wxT("Align selected shapes to the left"),wxITEM_NORMAL);
	m_toolscheme->AddTool(IDT_ALIGN_RIGHT, wxT("Align right"), wxBitmap(AlignRight_xpm), wxT("Align selected shapes to the right"),wxITEM_NORMAL);
	m_toolscheme->AddTool(IDT_ALIGN_TOP, wxT("Align top"), wxBitmap(AlignTop_xpm), wxT("Align selected shapes to the top"),wxITEM_NORMAL);
	m_toolscheme->AddTool(IDT_ALIGN_BOTTOM, wxT("Align bottom"), wxBitmap(AlignBottom_xpm), wxT("Align selected shapes to the bottom"),wxITEM_NORMAL);
	m_toolscheme->AddTool(IDT_ALIGN_MIDDLE, wxT("Align middle"), wxBitmap(AlignMiddle_xpm), wxT("Align selected shapes to the middle"),wxITEM_NORMAL);
	m_toolscheme->AddTool(IDT_ALIGN_CENTER, wxT("Align center"), wxBitmap(AlignCenter_xpm), wxT("Align selected shapes to the center"),wxITEM_NORMAL);
	m_toolscheme->AddSeparator();
	m_toolscheme->AddControl(m_zoomSlider);
	m_toolscheme->Realize();

	wxGetApp().SetTreeFrame(CreateTreeFrame());
	m_mgr.AddPane(wxGetApp().GetTreeFrame(), wxAuiPaneInfo().
				  Name(wxT("TreeFrame")).Caption(ap->GetLngRes(wxT("lr_bottreeCapt"))).
				  Left().Layer(1).Position(1));

	m_mgr.AddPane(m_toolbar, wxAuiPaneInfo().
				  Name(wxT("Toolbar")).Caption(ap->GetLngRes(wxT("lr_toolbarCapt"))).
				  ToolbarPane().Top().Row(1).
				  LeftDockable(false).RightDockable(false));

	m_mgr.AddPane(m_toolscript, wxAuiPaneInfo().
				  Name(wxT("Toolscript")).Caption(ap->GetLngRes(wxT("lr_toolbarCapt"))).
				  ToolbarPane().Top().Row(1).
				  LeftDockable(false).RightDockable(false));

	m_mgr.AddPane(m_toolscheme, wxAuiPaneInfo().
				  Name(wxT("Toolscheme")).Caption(ap->GetLngRes(wxT("lr_toolbarCapt"))).
				  ToolbarPane().Top().Row(1).
				  LeftDockable(false).RightDockable(false));

	m_mgr.AddPane(CreateNotebook(), wxAuiPaneInfo().Name(wxT("notebook_content")).
				  CenterPane().PaneBorder(false).CloseButton(true).MaximizeButton(true));

	wxGetApp().SetTextFrame(CreateTextCtrl());
	m_mgr.AddPane(wxGetApp().GetTextFrame(), wxAuiPaneInfo().
				  Name(wxT("TextCtrl")).Caption(ap->GetLngRes(wxT("lr_messagewindowCapt"))).
				  Bottom().Layer(1).Position(1));

	wxGetApp().SetListFrame(CreateListFrame());
	m_mgr.AddPane(wxGetApp().GetListFrame(), wxAuiPaneInfo().
				  Name(wxT("ListFrame")).Caption(ap->GetLngRes(wxT("lr_componentpanelCapt"))).
				  Top().Layer(1).Position(1));

	wxGetApp().SetGridFrame(CreateGridCtrl());
	m_mgr.AddPane(wxGetApp().GetGridFrame(), wxAuiPaneInfo().
				  Name(wxT("GridCtrl")).Caption(ap->GetLngRes(wxT("lr_stackvarCapt"))).
				  Bottom().Layer(1).Position(1));

	wxGetApp().SetPropertyFrame(CreatePropertyGridCtrl());
	m_mgr.AddPane(wxGetApp().GetPropertyFrame(), wxAuiPaneInfo().
				  Name(wxT("PropGridCtrl")).Caption(ap->GetLngRes(wxT("lr_propertyobjectCapt"))).
				  Right().Layer(1).Position(1));

	//if(ap->GetPaneSetting() != wxEmptyString)
	//	m_mgr.LoadPerspective(ap->GetPaneSetting());

	m_mgr.Update();
	
	wxLog::SetActiveTarget(wxGetApp().GetError());
	m_fShowShadows = false;
	m_nToolMode = modeDESIGN;

	wxGetApp().GetTreeBot()->StartAllBot();
}
//**************************************************
void wxAutoBotApp::OnTimer(wxTimerEvent& WXUNUSED(event))
{
	GetTaskBarIcon()->SetIcon(GetAppProperties()->GetIco(icr_appico));
	GetTreeBot()->UnPauseAllBot();
}
//**************************************************
//Запуск приложения функция которая стартует первая
bool wxAutoBotApp::OnInit()
{
	//Прежде всего создаём обработчика ошибок, ведь он ответственен за отображение ошибок
	m_Error = new CError();
	wxFileSystem::AddHandler(new wxMemoryFSHandler());

	const wxString name = wxString::Format(wxT("AutoBot-%s"), wxGetUserId().c_str());
	GetError()->AddMessage(wxT("Start: ") + name, CError::eMsgType_NoTxtOutput);
	m_checker = new wxSingleInstanceChecker(name);
	if(m_checker->IsAnotherRunning())
	{
		GetError()->AddMessage(wxT("Fatal: AutoBot already start"), CError::eMsgType_AppErr);
		return false;
	}

	wxString DBPath;
	if(wxGetCwd() == wxGetHomeDir())
	{
		HKEY hk;
		wxString KeyPath(wxT("Software\\Microsoft\\Windows\\CurrentVersion\\Run")); 
		if(!RegOpenKeyEx(HKEY_CURRENT_USER, KeyPath, 0, KEY_READ, &hk))
		{
			DWORD dwType;
			DWORD BufferSize;
			RegQueryValueEx(hk, wxT("AutoBot"), NULL, &dwType, NULL, &BufferSize);
			if (dwType == REG_SZ)
			{
				PUTSTR szData = new WCHAR[BufferSize+1];
				RegQueryValueEx(hk, wxT("AutoBot"), NULL, NULL, (LPBYTE)szData, &BufferSize);
				DBPath.Append(szData, BufferSize);
				delete szData;
			}
			RegCloseKey(hk);
			DBPath = wxPathOnly(DBPath) + wxT("\\abBase.db3");
		}
	}
	else
	{
		DBPath = wxGetCwd() + wxT("\\abBase.db3");
	}
	
	//Загружаем свойства приложения
	m_AppProperties = new CAppProperties(DBPath);
	if(GetError()->IsError())
	{
		GetError()->AddMessage(wxT("Fatal: Impossible create object AppProperties"), CError::eMsgType_AppErr);
		return false;
	}
	if(!m_AppProperties->LoadAppProperties())
	{
		GetError()->AddMessage(wxT("Fatal: Impossible load properties"), CError::eMsgType_AppErr);
		return false;
	}
	if(!m_AppProperties->LoadPlugIn())
	{
		GetError()->AddMessage(wxT("Fatal: Impossible load plugin"), CError::eMsgType_AppErr);
		return false;
	}

	wxSocketBase::Initialize();
	//Создаём ядро приложения класс CTreeBot, 
	//он загружает все настройки приложения, инициализирует переменные, 
	//проверяет работоспособность компонентов, загружает языковые ресурсы,
	//инициализирует классы ботов, загружает настройки ботов
	SetTreeBot(new CTreeBot());
	if (!GetTreeBot()->LoadTreeBot()) //Прочитаем всю структуру дерева из бд
	{
		GetError()->AddMessage(GetAppProperties()->GetLngRes(wxT("lr_TreeBotCantCreate")), CError::eMsgType_AppErr);
		return false;
	}

	if((GetAppProperties()->GetHashPassword() != wxEmptyString) && (!GetAppProperties()->GetMoveInTray()))
	{
		CLoginDialog Login;
		if(Login.ShowModal() != wxID_OK)
		{
			GetError()->AddMessage(GetAppProperties()->GetLngRes(wxT("lr_WrongPassword")), CError::eMsgType_AppErr);
			return false;
		}
	}

	SetFrameCanvas((FrameCanvas *)NULL);
	SetPropertyFrame((CPropertyFrame *)NULL);
	SetTreeFrame((CTreeFrame *)NULL);
	SetTextFrame((wxTextCtrl *)NULL);
	SetListFrame((CListFrame *)NULL);
	SetGridFrame((CGrid *)NULL);
	SetTimer((wxTimer *)NULL);
	SetScintillaFrame((CScintillaFrame *)NULL);

	SetTaskBarIcon(new CTaskBarIcon());
	
	if (!GetTaskBarIcon()->SetIcon(GetAppProperties()->GetIco(icr_appico), wxT("AutoBot")))
	{
		GetError()->AddMessage(GetAppProperties()->GetLngRes(wxT("lr_CantSetIcon")), CError::eMsgType_AppErr);
	}

	wxAutoBotFrame *frame = new wxAutoBotFrame(NULL, wxID_ANY, wxT("AutoBot"));
	SetTopWindow(frame);
	frame->Show(!GetAppProperties()->GetMoveInTray());
	//Сюда добавим вызов диалога настроек если у нас первый запуск
	if(GetAppProperties()->GetFirstStart())
	{
		CSettingsDialog dialog(frame);
		dialog.ShowModal();
		GetAppProperties()->SetFirstStart(false);
	}

	return true;
}
//**************************************************
int wxAutoBotApp::OnExit()
{
	wxGetApp().SetTextFrame(NULL);
	wxWindowDisabler disableAll;
	wxBusyInfo info(GetAppProperties()->GetLngRes(wxT("lr_busywin")), NULL);

	GetTaskBarIcon()->RemoveIcon();
	GetTreeBot()->SaveTreeBot();

	GetAppProperties()->SaveAppProperties();
	wxDELETE(m_TreeBot);
	wxDELETE(m_AppProperties);
	wxDELETE(m_taskBarIcon);
	GetError()->AddMessage(wxT("Stop: AutoBot"), CError::eMsgType_NoTxtOutput);
	delete wxLog::SetActiveTarget(NULL);
	//wxDELETE(m_Error);
	wxDELETE(m_checker);
	wxDELETE(m_timer);
	
	return 0;
}
//**************************************************
void wxAutoBotFrame::OnIconize(wxIconizeEvent& WXUNUSED(event))
{
	Show(false);
}
//**************************************************
void wxAutoBotFrame::OnClose(wxCloseEvent& WXUNUSED(event))
{
	//PopEventHandler(true);
	wxLog::EnableLogging(false);
	CBot *bot = wxGetApp().GetTreeBot()->GetActiveBot();
	if(bot)
		wxGetApp().GetFrameCanvas()->SaveCanvasToDoc(wxGetApp().GetTreeBot()->GetActiveBot()->GetShapeDiagram());
	m_DiagramManager.Clear();
	m_DiagramManager.SetShapeCanvas(NULL);

	wxDELETE(m_pCanvasPanel);
	wxDELETE(m_pMainSizer);
	Destroy();
	//Close(true);
}
//**************************************************
void wxAutoBotFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
	m_DiagramManager.Clear();
	m_DiagramManager.SetShapeCanvas(NULL);

	Destroy();
}
//**************************************************
void wxAutoBotFrame::OnUndo(wxCommandEvent& WXUNUSED(event))
{
	wxGetApp().GetFrameCanvas()->Undo();
}
//**************************************************
void wxAutoBotFrame::OnUpdateUndo(wxUpdateUIEvent& event)
{
	event.Enable(wxGetApp().GetFrameCanvas()->CanUndo());
}
//**************************************************
void wxAutoBotFrame::OnUpdateDebug(wxUpdateUIEvent& event)
{
	if(wxGetApp().GetTreeBot()->GetActiveBot())
	{
		if(wxGetApp().GetTreeBot()->GetActiveBot()->GetStatusBot() == STATUS_PLAY)
		{
			if(wxGetApp().GetTreeBot()->GetActiveBot()->GetDebugState() == CBot::Debug_No)
				event.Enable(false);
			else
				event.Enable(true);
		}
		else
			event.Enable(true);
	}
	else
	{
		event.Enable(true);
	}
}
//**************************************************
void wxAutoBotFrame::OnUpdateStart(wxUpdateUIEvent& event)
{
	if(wxGetApp().GetTreeBot()->GetActiveBot())
	{
		if(wxGetApp().GetTreeBot()->GetActiveBot()->GetStatusBot() == STATUS_PLAY)
			event.Enable(false);
		else
			event.Enable(true);
	}
	else
	{
		event.Enable(true);
	}
}
//**************************************************
void wxAutoBotFrame::OnRedo(wxCommandEvent& WXUNUSED(event))
{
	wxGetApp().GetFrameCanvas()->Redo();
}
//**************************************************
void wxAutoBotFrame::OnUpdateRedo(wxUpdateUIEvent& event)
{
	event.Enable(wxGetApp().GetFrameCanvas()->CanRedo());
}
//**************************************************
void wxAutoBotFrame::OnCopy(wxCommandEvent& WXUNUSED(event))
{
	wxGetApp().GetFrameCanvas()->Copy();
}
//**************************************************
void wxAutoBotFrame::OnUpdateCopy(wxUpdateUIEvent& event)
{
	event.Enable(wxGetApp().GetFrameCanvas()->CanCopy());
}
//**************************************************
void wxAutoBotFrame::OnCut(wxCommandEvent& WXUNUSED(event))
{
	wxGetApp().GetFrameCanvas()->Cut();
}
//**************************************************
void wxAutoBotFrame::OnUpdateCut(wxUpdateUIEvent& event)
{
	event.Enable(wxGetApp().GetFrameCanvas()->CanCut());
}
//**************************************************
void wxAutoBotFrame::OnPaste(wxCommandEvent& WXUNUSED(event))
{
	wxGetApp().GetFrameCanvas()->Paste();
}
//**************************************************
void wxAutoBotFrame::OnUpdatePaste(wxUpdateUIEvent& event)
{
	event.Enable(wxGetApp().GetFrameCanvas()->CanPaste());
}
//**************************************************
void wxAutoBotFrame::OnSelectAll(wxCommandEvent& WXUNUSED(event))
{
	wxGetApp().GetFrameCanvas()->SelectAll();
}
//**************************************************
void wxAutoBotFrame::OnExportToBMP(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog dlg(this, wxT("Export canvas to BMP..."), wxGetCwd(), wxT(""), wxT("BMP Files (*.bmp)|*.bmp"), wxSAVE);

	if(dlg.ShowModal() == wxID_OK)
	{
		wxGetApp().GetFrameCanvas()->SaveCanvasToBMP(dlg.GetPath());
	}
}
//**************************************************
void wxAutoBotFrame::OnTool(wxCommandEvent& event)
{
	if(wxGetApp().GetFrameCanvas()->GetMode() == FrameCanvas::modeCREATECONNECTION)wxGetApp().GetFrameCanvas()->AbortInteractiveConnection();

	switch(event.GetId())
	{
		case IDT_GRID:
			m_fShowGrid = !m_fShowGrid;
			if( m_fShowGrid )
			{
				wxGetApp().GetFrameCanvas()->AddStyle(wxSFShapeCanvas::sfsGRID_SHOW);
				wxGetApp().GetFrameCanvas()->AddStyle(wxSFShapeCanvas::sfsGRID_USE);
			}
			else
			{
				wxGetApp().GetFrameCanvas()->RemoveStyle(wxSFShapeCanvas::sfsGRID_SHOW);
				wxGetApp().GetFrameCanvas()->RemoveStyle(wxSFShapeCanvas::sfsGRID_USE);
			}
			wxGetApp().GetFrameCanvas()->Refresh(false);
			break;
		case IDT_IFSHAPE:
			m_nToolMode = modeIF;
			break;
		case IDT_EDITTEXTSHP:
			m_nToolMode = modeEDITTEXT;
			break;
		case IDT_LINESHP:
			m_nToolMode = modeLINE;
			break;
		case IDT_CYCLESHP:
			m_nToolMode = modeCYCLE;
			break;
		case IDT_TRIGGER:
			m_nToolMode = modeTRIGGER;
			break;
		case IDT_COMPONENT:
			m_nToolMode = modeCOMPONENT;
			break;
		case IDT_TEXTSHP:
			m_nToolMode = modeTEXT;
			break;
		case IDT_TOOL:
			m_nToolMode = modeDESIGN;
			break;
		case IDT_ALIGN_LEFT:
			wxGetApp().GetFrameCanvas()->AlignSelected(wxSFShapeCanvas::halignLEFT, wxSFShapeCanvas::valignNONE);
			break;
		case IDT_ALIGN_RIGHT:
			wxGetApp().GetFrameCanvas()->AlignSelected(wxSFShapeCanvas::halignRIGHT, wxSFShapeCanvas::valignNONE);
			break;
		case IDT_ALIGN_CENTER:
			wxGetApp().GetFrameCanvas()->AlignSelected(wxSFShapeCanvas::halignCENTER, wxSFShapeCanvas::valignNONE);
			break;
		case IDT_ALIGN_TOP:
			wxGetApp().GetFrameCanvas()->AlignSelected(wxSFShapeCanvas::halignNONE, wxSFShapeCanvas::valignTOP);
			break;
		case IDT_ALIGN_BOTTOM:
			wxGetApp().GetFrameCanvas()->AlignSelected(wxSFShapeCanvas::halignNONE, wxSFShapeCanvas::valignBOTTOM);
			break;
		case IDT_ALIGN_MIDDLE:
			wxGetApp().GetFrameCanvas()->AlignSelected(wxSFShapeCanvas::halignNONE, wxSFShapeCanvas::valignMIDDLE);
			break;
		default:
			event.Skip();
			break;
	}
}
//**************************************************
void wxAutoBotFrame::OnUpdateScriptToolbar(wxUpdateUIEvent& event)
{
	event.Enable(wxGetApp().GetScintillaFrame()->IsEnabled());
}
//**************************************************
void wxAutoBotFrame::OnUpdateTool(wxUpdateUIEvent& event)
{
	switch(event.GetId())
	{
		case IDT_GRID:
			event.Check(m_fShowGrid);
			break;
		case IDT_GC:
			event.Check( wxSFShapeCanvas::IsGCEnabled() );
			break;
		case IDT_CYCLESHP:
			event.Check(m_nToolMode == modeCYCLE);
			break;
		case IDT_IFSHAPE:
			event.Check(m_nToolMode == modeIF);
			break;
		case IDT_EDITTEXTSHP:
			event.Check(m_nToolMode == modeEDITTEXT);
			break;
		case IDT_LINESHP:
			event.Check(m_nToolMode == modeLINE);
			break;
		case IDT_TRIGGER:
			event.Check(m_nToolMode == modeTRIGGER);
			break;
		case IDT_COMPONENT:
			event.Check(m_nToolMode == modeCOMPONENT);
			break;
		case IDT_TEXTSHP:
			event.Check(m_nToolMode == modeTEXT);
			break;
		case IDT_TOOL:
			event.Check(m_nToolMode == modeDESIGN);
			break;
		case IDT_ALIGN_RIGHT:
		case IDT_ALIGN_LEFT:
		case IDT_ALIGN_TOP:
		case IDT_ALIGN_BOTTOM:
		case IDT_ALIGN_MIDDLE:
		case IDT_ALIGN_CENTER:
			event.Enable(wxGetApp().GetFrameCanvas()->CanAlignSelected());
			break;
		default:
			event.Skip();
			break;
	}
}
//**************************************************
void wxAutoBotFrame::OnPrint(wxCommandEvent& WXUNUSED(event))
{
	wxGetApp().GetFrameCanvas()->Print();
}
//**************************************************
void wxAutoBotFrame::OnPrintPreview(wxCommandEvent& WXUNUSED(event))
{
	wxGetApp().GetFrameCanvas()->PrintPreview();
}
//**************************************************
void wxAutoBotFrame::OnPageSetup(wxCommandEvent& WXUNUSED(event))
{
	wxGetApp().GetFrameCanvas()->PageSetup();
}
//**************************************************
void wxAutoBotFrame::OnSlider(wxScrollEvent& WXUNUSED(event))
{
	wxGetApp().GetFrameCanvas()->SetScale(double(m_zoomSlider->GetValue())/50);
	wxGetApp().GetFrameCanvas()->Refresh(false);
}
//**************************************************
wxAutoBotFrame::~wxAutoBotFrame()
{
	wxGetApp().GetAppProperties()->SetPaneSetting(m_mgr.SavePerspective());
	m_mgr.UnInit();
	//if(wxGetApp().GetTextFrame() != NULL)
		//delete wxGetApp().GetTextFrame();
	wxGetApp().Exit();
}
//**************************************************
void wxAutoBotFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo info;
	info.SetName(wxT("AutoBot"));
	info.SetVersion(wxMAKE_VERSION_DOT_STRING_T(ABV_MAJOR, ABV_MINOR, ABV_RELEASE));
	info.SetDescription(wxT("Program for automation of the execution function OS"));
	info.SetCopyright(wxT("(C) 2007-2008 Bruian"));
	info.AddDeveloper(wxT("Dergach Victor"));
	// we can add a second developer
	info.AddDeveloper(wxT("A.N. Other see on web site and documentation"));
	info.SetWebSite(wxT("http://www.bruian.com/"), wxT("AutoBot & Bruian web site"));
	// or we can add several persons at once like this
	static const wxChar *docwriters[] =
	{
		wxT("Dergach Victor"),
	};
	info.SetDocWriters(wxArrayString(WXSIZEOF(docwriters), docwriters));
	info.SetLicence(wxString::FromAscii(
	"AutoBot Licence\n"
	"========================================\n"
	"\n"
	"Copyright (c) 2007-2008 Dergach Victor\n"
	"\n"
	"  Everyone is permitted to copy and distribute verbatim copies	\n"
	"  of this program, but changing it is not allowed.				\n"
	"\n" ));

	wxAboutBox(info);
}
//**************************************************
CTreeFrame* wxAutoBotFrame::CreateTreeFrame()
{
	CTreeFrame* tree = new CTreeFrame(this, Tree_Ctrl,
									  wxDefaultPosition, wxSize(160,250),
									  wxTR_DEFAULT_STYLE | wxTR_EDIT_LABELS | wxSUNKEN_BORDER);
	tree->RefreshTree();
	return tree;
}
//**************************************************
FrameCanvas* wxAutoBotFrame::CreateFrameCanvasCtrl()
{
	// set shape canvas and associate it with diagram manager
	FrameCanvas* frameCanvas = new FrameCanvas(&m_DiagramManager, this, wxID_ANY);
	wxSFShapeCanvas::EnableGC( false );
	
	frameCanvas->Disable();
	wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
	mainSizer->Add(frameCanvas, 1, wxEXPAND, 0);
	m_pCanvasPanel->SetSizer(mainSizer);
	m_pCanvasPanel->Layout();

	return frameCanvas;
}
//**************************************************
CScintillaFrame* wxAutoBotFrame::CreateScintillaFrameCtrl()
{
	CScintillaFrame* scintillaFrame = new CScintillaFrame(this);
	scintillaFrame->Enable(false);

	return scintillaFrame;
}
//**************************************************
wxAuiNotebook* wxAutoBotFrame::CreateNotebook()
{
	//create the notebook off-window to avoid flicker
	wxSize client_size = GetClientSize();

	wxAuiNotebook* ctrl = new wxAuiNotebook(this, Notebook_Ctrl,
											wxPoint(client_size.x, client_size.y),
											wxSize(430,200),
											wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER);
	wxGetApp().SetFrameCanvas(CreateFrameCanvasCtrl());
	wxGetApp().SetScintillaFrame(CreateScintillaFrameCtrl());
	wxBitmap page_bmp = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));
	ctrl->AddPage(CreateHTMLCtrl(ctrl), wxGetApp().GetAppProperties()->GetLngRes(wxT("lr_tabhtmlCapt")), false, page_bmp);
	ctrl->AddPage(wxGetApp().GetFrameCanvas(), wxGetApp().GetAppProperties()->GetLngRes(wxT("lr_tabschemeCapt")), false, page_bmp);
	ctrl->AddPage(wxGetApp().GetScintillaFrame(), wxGetApp().GetAppProperties()->GetLngRes(wxT("lr_tabscriptCapt")), false, page_bmp);
	return ctrl;
}
//**************************************************
wxHtmlWindow* wxAutoBotFrame::CreateHTMLCtrl(wxWindow* parent)
{
	if(!parent)
		parent = this;

	wxHtmlWindow* ctrl = new wxHtmlWindow(parent, wxID_ANY, wxDefaultPosition,
										  wxSize(400,300));
	ctrl->LoadPage(wxT("memory:wellcome.htm"));
	//ctrl->SetPage(GetIntroText());
	return ctrl;
}
//**************************************************
wxTextCtrl* wxAutoBotFrame::CreateTextCtrl()
{
	return new wxTextCtrl(this, Text_Ctrl, wxString::Format(wxT("%s\n"),wxGetApp().GetAppProperties()->GetLngRes(wxT("lr_txtMessageGreeting"))),
						  wxPoint(0,0), wxSize(150,90),
						  wxTE_RICH | wxTE_MULTILINE | wxTE_AUTO_URL);
}
//**************************************************
CListFrame* wxAutoBotFrame::CreateListFrame()
{
	CListFrame* listFrame = new CListFrame(this, List_Ctrl, wxDefaultPosition, wxDefaultSize,
										   wxLC_ICON | wxSUNKEN_BORDER | wxLC_EDIT_LABELS);

	listFrame->SetImageListNormal(new wxImageList(55, 35, true));
	CAppProperties *aProp = wxGetApp().GetAppProperties();
	
	if(aProp->GetComponentList().GetCount() > 0)
	{
		wxList::compatibility_iterator node = aProp->GetComponentList().GetFirst();
		while (node)
		{
			CComponent *comp = (CComponent *)node->GetData();
			wxImage image = comp->GetImage(); image.Rescale(55,35,wxIMAGE_QUALITY_HIGH);
			listFrame->GetImageListNormal()->Add(wxBitmap(image), wxBITMAP_TYPE_PNG);
			node = node->GetNext();
		}
		listFrame->SetImageList(listFrame->GetImageListNormal(), wxIMAGE_LIST_NORMAL);
		
		int listIndex = 0;
		node = aProp->GetComponentList().GetFirst();
		while (node)
		{
			CComponent *comp = (CComponent *)node->GetData();
			listFrame->InsertItem(listIndex, aProp->GetLngRes(comp->GetName()),	listIndex);
			listIndex++;
			node = node->GetNext();
		}
	}
	return listFrame;
}
//**************************************************
CGrid* wxAutoBotFrame::CreateGridCtrl()
{
	CGrid *grid = new CGrid(this, Grid_Ctrl, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	grid->CreateGrid(0,4);

	grid->SetColLabelValue(Col_id, wxT("Id"));
	//grid->SetColumnWidth(Col_dscr, 30);
	//grid->AutoSizeRows();
	grid->SetColLabelValue(Col_dscr, wxGetApp().GetAppProperties()->GetLngRes(wxT("lr_description")));
	grid->SetColLabelValue(Col_type, wxGetApp().GetAppProperties()->GetLngRes(wxT("lr_type")));
	grid->SetColLabelValue(Col_value, wxGetApp().GetAppProperties()->GetLngRes(wxT("lr_value")));

	grid->RegisterDataType( wxGRID_VALUE_NUMBER,
							new wxGridCellNumberRenderer,
							new wxGridCellNumberEditor);
	grid->RegisterDataType( wxGRID_VALUE_TEXT,
							new wxGridCellStringRenderer,
							new wxGridCellTextEditor);
	return grid;
}
//**************************************************
CPropertyFrame* wxAutoBotFrame::CreatePropertyGridCtrl()
{
	CPropertyFrame* pg = new CPropertyFrame(this,
		Prop_Ctrl,
		wxDefaultPosition,
		wxSize( 200, 90 ),
		wxPG_BOLD_MODIFIED |
		wxPG_SPLITTER_AUTO_CENTER |
		//wxPG_AUTO_SORT |
		//wxPG_HIDE_MARGIN | wxPG_STATIC_SPLITTER |
		wxPG_TOOLTIPS |
		//wxPG_NOCATEGORIES |
		wxTAB_TRAVERSAL |
		wxPG_TOOLBAR |
		wxPG_DESCRIPTION |
		wxSUNKEN_BORDER,
		wxPropertyGridNameStr );

	pg->SetExtraStyle(wxPG_EX_MODE_BUTTONS|wxPG_EX_HELP_AS_TOOLTIPS);
	pg->RegisterAdvancedPropertyClasses();
	pg->RegisterAdditionalEditors();
	
	return pg;
}
//**************************************************
wxString wxAutoBotFrame::GetIntroText()
{
	const char* text = "<html></html>";
	return wxString::FromAscii(text);
}
//**************************************************
void wxAutoBotFrame::DoUpdate()
{
	m_mgr.Update();
}
//**************************************************
void wxAutoBotFrame::OnEraseBackground(wxEraseEvent& event)
{
	event.Skip();
}
//**************************************************
void wxAutoBotFrame::OnSize(wxSizeEvent& event)
{
	event.Skip();
}
//**************************************************
void wxAutoBotFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
	CBot *bot = wxGetApp().GetTreeBot()->GetActiveBot();
	if(bot)
		wxGetApp().GetFrameCanvas()->SaveCanvasToDoc(wxGetApp().GetTreeBot()->GetActiveBot()->GetShapeDiagram());
	wxGetApp().GetTreeBot()->SaveTreeBot();
}
//**************************************************
void wxAutoBotFrame::OnProperty(wxCommandEvent& WXUNUSED(event))
{
	CSettingsDialog dialog(this);
	dialog.ShowModal();
}
//**************************************************
void wxAutoBotFrame::OnComponentPanel(wxCommandEvent& event)
{
	m_mgr.GetPane(wxT("ListFrame")).Show(event.IsChecked()).Top().Layer(1).Position(1);
	m_mgr.Update();
}
//**************************************************
void wxAutoBotFrame::OnTreeBots(wxCommandEvent& event)
{
	m_mgr.GetPane(wxT("TreeFrame")).Show(event.IsChecked()).Left().Layer(1).Position(1);
	m_mgr.Update();
}
//**************************************************
void wxAutoBotFrame::OnMessageWindow(wxCommandEvent& event)
{
	m_mgr.GetPane(wxT("TextCtrl")).Show(event.IsChecked()).Bottom().Layer(1).Position(1);
	m_mgr.Update();
}
//**************************************************
void wxAutoBotFrame::OnVarStack(wxCommandEvent& event)
{
	m_mgr.GetPane(wxT("GridCtrl")).Show(event.IsChecked()).Bottom().Layer(1).Position(1);
	m_mgr.Update();
}
//**************************************************
void wxAutoBotFrame::OnToolbar(wxCommandEvent& event)
{
	m_mgr.GetPane(wxT("Toolbar")).Show(event.IsChecked()).ToolbarPane().Top().Row(1).LeftDockable(false).RightDockable(false);
	m_mgr.Update();
}
//**************************************************
void wxAutoBotFrame::OnPropertyWindow(wxCommandEvent& event)
{
	m_mgr.GetPane(wxT("PropGridCtrl")).Show(event.IsChecked()).Right().Layer(1).Position(1);
	m_mgr.Update();
}
//**************************************************
void wxAutoBotFrame::OnWelcomeWindow(wxCommandEvent& WXUNUSED(event))
{
	bool find = false;
	wxAuiPaneInfo &pane = m_mgr.GetPane(wxT("notebook_content"));
	wxAuiNotebook* nb = (wxAuiNotebook*)pane.window;
	for(size_t i = 0, count = nb->GetPageCount(); i < count; ++i)
	{
		if (nb->GetPage(i)->IsKindOf(CLASSINFO(wxHtmlWindow)))
		{
			nb->DeletePage(i);
			find = true;
			break;
		}
	}

	if(!find)
	{
		wxBitmap page_bmp = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));
		nb->AddPage(CreateHTMLCtrl(nb), wxGetApp().GetAppProperties()->GetLngRes(wxT("lr_tabhtmlCapt")), true, page_bmp);
	}

	nb->Update();
	m_mgr.Update();
}
//**************************************************
void wxAutoBotFrame::OnSchematicView(wxCommandEvent& WXUNUSED(event))
{
	bool find = false;
	wxAuiPaneInfo &pane = m_mgr.GetPane(wxT("notebook_content"));
	wxAuiNotebook* nb = (wxAuiNotebook*)pane.window;
	for(size_t i = 0, count = nb->GetPageCount(); i < count; ++i)
	{
		if (nb->GetPage(i)->IsKindOf(CLASSINFO(FrameCanvas)))
		{
			nb->DeletePage(i);
			find = true;
			break;
		}
	}

	if(!find)
	{
		wxBitmap page_bmp = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));
		wxGetApp().SetFrameCanvas(CreateFrameCanvasCtrl());
		nb->AddPage(wxGetApp().GetFrameCanvas(), wxGetApp().GetAppProperties()->GetLngRes(wxT("lr_tabschemeCapt")), true, page_bmp);
		wxGetApp().GetFrameCanvas()->Refresh();
	}
	nb->Refresh();
	m_mgr.Update();
}
//**************************************************
void wxAutoBotFrame::OnScintillaView(wxCommandEvent& WXUNUSED(event))
{
	bool find = false;
	wxAuiPaneInfo &pane = m_mgr.GetPane(wxT("notebook_content"));
	wxAuiNotebook* nb = (wxAuiNotebook*)pane.window;
	for(size_t i = 0, count = nb->GetPageCount(); i < count; ++i)
	{
		if (nb->GetPage(i)->IsKindOf(CLASSINFO(CScintillaFrame)))
		{
			nb->DeletePage(i);
			find = true;
			break;
		}
	}

	if(!find)
	{
		wxBitmap page_bmp = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));
		wxGetApp().SetScintillaFrame(CreateScintillaFrameCtrl());
		nb->AddPage(wxGetApp().GetScintillaFrame(), wxGetApp().GetAppProperties()->GetLngRes(wxT("lr_scriptactionName")), true, page_bmp);
		wxGetApp().GetFrameCanvas()->Refresh();
	}
	nb->Refresh();
	m_mgr.Update();
}
//**************************************************
void wxAutoBotFrame::OnPaneClose(wxAuiManagerEvent& evt)
{
	wxMenuBar *menuBar = GetMenuBar();
	if(evt.pane->name == wxT("ListFrame"))
	{
		menuBar->Check(Menu_ComponentPanel, false);
	}
	else if(evt.pane->name == wxT("TreeFrame"))
	{
		menuBar->Check(Menu_TreeBots, false);
	}
	else if(evt.pane->name == wxT("TextCtrl"))
	{
		menuBar->Check(Menu_MessageWindow, false);
	}
	else if(evt.pane->name == wxT("GridCtrl"))
	{
		menuBar->Check(Menu_VarStack, false);
	}
	else if(evt.pane->name == wxT("PropGridCtrl"))
	{
		menuBar->Check(Menu_PropertyWindow, false);
	}
	else if(evt.pane->name == wxT("Toolbar"))
	{
		menuBar->Check(Menu_Toolbar, false);
	}
}
//**************************************************
void wxAutoBotFrame::OnNotebookPageClose(wxAuiNotebookEvent& evt)
{
	wxMenuBar *menuBar = GetMenuBar();
	wxAuiNotebook* ctrl = (wxAuiNotebook*)evt.GetEventObject();
	if (ctrl->GetPage(evt.GetSelection())->IsKindOf(CLASSINFO(wxHtmlWindow)))
	{
		menuBar->Check(Menu_WelcomeWindow, false);
	}
	else if (ctrl->GetPage(evt.GetSelection())->IsKindOf(CLASSINFO(FrameCanvas)))
	{
		wxGetApp().SetFrameCanvas((FrameCanvas *)NULL);
		menuBar->Check(Menu_SchematicView, false);
	}
	else if (ctrl->GetPage(evt.GetSelection())->IsKindOf(CLASSINFO(CScintillaFrame)))
	{
		wxGetApp().SetScintillaFrame((CScintillaFrame *)NULL);
		menuBar->Check(Menu_ScriptWindow, false);
	}
}
//**************************************************
void wxAutoBotFrame::OnNotebookPageChanged(wxAuiNotebookEvent& evt)
{
	wxAuiNotebook* ctrl = (wxAuiNotebook*)evt.GetEventObject();
	if (ctrl->GetPage(evt.GetSelection())->IsKindOf(CLASSINFO(CScintillaFrame)))
	{
		CRule* myRule = wxGetApp().GetTreeBot()->GetActiveRule();
		if(myRule != NULL)
		{
		}
	}
	else
	{
		wxGetApp().GetScintillaFrame()->ClearAll();
		wxGetApp().GetScintillaFrame()->Enable(false);
	}

	evt.Skip();
}
//**************************************************
void wxAutoBotFrame::OnAdd(wxCommandEvent& WXUNUSED(event))
{
	if(!wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_AddTreeItem, this, (wxEvent *)NULL))
		return;
}
//**************************************************
void wxAutoBotFrame::OnDelete(wxCommandEvent& WXUNUSED(event))
{
	if(!wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_DeleteTreeItem, this, (wxEvent *)NULL))
		return;
}
//**************************************************
void wxAutoBotFrame::OnStart(wxCommandEvent& WXUNUSED(event))
{
	if(!wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_StartItem, this, (wxEvent *)NULL))
		return;
}
//**************************************************
void wxAutoBotFrame::OnStop(wxCommandEvent& WXUNUSED(event))
{
	if(!wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_StopItem, this, (wxEvent *)NULL))
		return;
}
//**************************************************
void wxAutoBotFrame::OnPause(wxCommandEvent& WXUNUSED(event))
{
	if(!wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_PauseItem, this, (wxEvent *)NULL))
		return;
}
//**************************************************
void wxAutoBotFrame::OnHelp(wxCommandEvent& WXUNUSED(event))
{
}
//**************************************************
void wxAutoBotFrame::OnSpy(wxCommandEvent& WXUNUSED(event))
{
	CSpyModalDialog dlg(this);
	dlg.ShowModal();
}
//**************************************************
void wxAutoBotFrame::OnAddVar(wxCommandEvent& WXUNUSED(event))
{
	CBot *bot = wxGetApp().GetTreeBot()->GetActiveBot();
	if(bot == NULL)
		return;
	CVar *var = new CVar();
	var->SetId(bot->GetLastNumVar() + 1);
	bot->SetLastNumVar(var->GetId());
	bot->GetListVariables().Append(var);

	wxGetApp().GetGridFrame()->RefreshTable();
}
//**************************************************
void wxAutoBotFrame::OnDeleteVar(wxCommandEvent& WXUNUSED(event))
{
	CBot *bot = wxGetApp().GetTreeBot()->GetActiveBot();
	if(bot == NULL)
		return;
	if(bot->GetTotalVariables() <= 0)
		return;
	wxList::compatibility_iterator node = bot->GetListVariables().Item(wxGetApp().GetGridFrame()->GetCursorRow());
	bot->GetListVariables().DeleteNode(node);

	wxGetApp().GetGridFrame()->RefreshTable();
}
//**************************************************
void wxAutoBotFrame::OnChar(wxKeyEvent &event)
{
	event.Skip();
}
//**************************************************
void wxAutoBotFrame::OnShow(wxShowEvent &event)
{
	event.Skip();
}
//**************************************************
void wxAutoBotFrame::OnDebug(wxCommandEvent& WXUNUSED(event))
{
	if(!wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_DebugBot, this, (wxEvent *)NULL))
		return;
}
//**************************************************
void wxAutoBotFrame::OnSaveScript(wxCommandEvent& WXUNUSED(event))
{
	CRule* myRule = wxGetApp().GetTreeBot()->GetActiveRule();
	if(myRule != NULL)
	{
		if(myRule->GetComponent()->GetComponentType() == Script)
		{
			myRule->SetValue(wxT("m4"), wxGetApp().GetScintillaFrame()->GetText());
		}
	}
}
//**************************************************
void wxAutoBotFrame::OnEdit(wxCommandEvent &event)
{
	if(wxGetApp().GetScintillaFrame())
		if(wxGetApp().GetScintillaFrame()->IsShown())
			wxGetApp().GetScintillaFrame()->ProcessEvent(event);
	event.Skip();
}
//**************************************************