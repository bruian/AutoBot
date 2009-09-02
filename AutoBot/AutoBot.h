#ifndef _AutoBot_h
#define _AutoBot_h

#define ABV_MAJOR 0
#define ABV_MINOR 6
#define ABV_RELEASE 23

//#define WIN32_LEAN_AND_MEAN
//#define _WIN32_WINNT 0x500
//#define _WINSOCKAPI_

#include "wx/wxprec.h"

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#pragma warning(disable: 4129)
//#pragma warning(default: 4701 4244 4100 4996 4005 4127 4055 4389 4018 4152)

#pragma warning(push)
#pragma warning(disable: 4231 4250 4251 4275 4660 4661 4700 4706 4786 4355 4100 4244 4127)
#define CRYPTOPP_DEFAULT_NO_DLL
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "cryptlib.h"
#include "md5.h"
using namespace CryptoPP::Weak;
#pragma warning(pop)

#include <propgrid.h>
#include <propdev.h>
#include <advprops.h>
#include <manager.h>
#include "wx/wxhtml.h"
#include "wx/propdlg.h"
#include "wx/statline.h"
#include "wx/treectrl.h"
#include "wx/toolbar.h"
#include "wx/grid.h"
#include "wx/spinctrl.h"
#include "wx/artprov.h"
#include "wx/generic/dragimgg.h"
#include "wx/aui/aui.h"
#include "wx/textdlg.h"
#include "wx/utils.h"
#include "wx/progdlg.h"
#include "wx/taskbar.h"
#include "wx/process.h"
#include "wx/aboutdlg.h"
#include "wx/busyinfo.h"
#include "wx/numdlg.h"
#include "wx/timer.h"
#include "wx/dir.h"
#include "wx/fontmap.h"
#include "wx/socket.h"

#include "wx/datectrl.h"
#include "wx/dateevt.h"

#include "wx/regex.h"
#include "wx/sizer.h"
#include "wx/wfstream.h"
#include "wx/mstream.h"
#include "wx/stream.h"
#include "wx/mimetype.h"
#include "wxscintilla.h"

#include "wx/snglinst.h"
#include "wx/tipwin.h"
#include "wx/fs_mem.h"
#include <wx/xml/xml.h>
#include <wx/dynarray.h>
#include <wx/arrimpl.cpp>
#include <wx/clrpicker.h>
#include "wx/dynload.h"
#include "wx/dynlib.h"
//#include "wx/cshelp.h"

//#include "wx/tokenzr.h"
//#include "wx/intl.h"

#pragma warning(push)
#pragma warning(disable: 4231 4250 4251 4275 4660 4661 4700 4706 4786 4355 4100 4244 4127)
#include "sqlite3.h"
#include "Databaselayer.h"
#include "SqliteDatabaseLayer.h"
#include "DatabaseLayerException.h"
#include "SqliteResultSet.h"
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 4231 4250 4251 4275 4660 4661 4700 4706 4786 4355 4100 4244 4127)
#include <script/VMCore.h>
#include <script/Declarator.h>
#pragma warning(pop)

#include "sqstdio.h"
#include "sqstdaux.h"
#include "sqstdsystem.h"
#include "sqstdblob.h"
#include "sqstdmath.h"
#include "sqstdstring.h"

#define scvprintf wxVLogMessage

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <Shlobj.h>
#include <atlconv.h>

#include "FrameCanvas.h"

//#include "EmailSender.h"
//#include "FileList.h"
//#pragma comment(lib, "RakNetLibStaticDebug.lib")
#define wxUSE_DYNLIB_CLASS 1

#include "enum.h"
#include "plugin.h"

//class Plugin;

typedef unsigned __int32 u32;

#define DRAG_NONE     0
#define DRAG_START    1
#define DRAG_DRAGGING 2

#define SHAPE_DRAG_BITMAP 1

#include "Exception.h"
//BEGIN_DECLARE_EVENT_TYPES()
//	DECLARE_EVENT_TYPE(wxEVT_SELECT_TREE_COMMAND, 7777555)
//END_DECLARE_EVENT_TYPES()
//
//DEFINE_EVENT_TYPE(wxEVT_SELECT_TREE_COMMAND)
//
//// it may also be convenient to define an event table macro for this event type
//#define EVT_SELECT_TREE_COMMAND(id, fn) \
//    DECLARE_EVENT_TABLE_ENTRY( \
//        wxEVT_SELECT_TREE_COMMAND, id, wxID_ANY, \
//        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
//        (wxObject *) NULL \
//    ),
//**************************************************
class CException;
class CError;
class CAppProperties;
class CVar;
class CRule;
class CBot;
class CGrid;
class CScintillaFrame;
class CTreeFrame;
class CListFrame;
class CLoginDialog;
class CTaskBarIcon;
class CNodeBot;
class CTreeBot;
class CComponent;
class FrameCanvas;
class CPropertyFrame;
class CSettingsDialog;
class CSpyModalDialog;
//**************************************************
class wxAutoBotApp : public wxApp
{
protected:
	CTaskBarIcon	*m_taskBarIcon;
	CError			*m_Error;		//ќбъект отвечающий за отображение сообщений об ошибках
	CAppProperties	*m_AppProperties;
	CTreeBot		*m_TreeBot;		//ядро приложени€!!
	CPropertyFrame	*m_propertyFrame;
	FrameCanvas		*m_frameCanvas;
	CTreeFrame		*m_treeFrame;
	wxTextCtrl		*m_textFrame;
	CListFrame		*m_listFrame;
	CGrid			*m_gridFrame;
	CScintillaFrame *m_scintillaFrame;
	wxTimer			*m_timer;
	wxMutex			m_ScriptMutex;
	wxMutex			m_ErrorMutex;
	wxSingleInstanceChecker *m_checker;
public:
	virtual bool OnInit();
	virtual int OnExit();
	long stCount;

	CError *GetError() { return m_Error; }
	CAppProperties *GetAppProperties() { return m_AppProperties; }
	CTreeBot *GetTreeBot() { return m_TreeBot; }
	void SetTreeBot(CTreeBot *value) { m_TreeBot = value; }
	CPropertyFrame *GetPropertyFrame() { return m_propertyFrame; }
	void SetPropertyFrame(CPropertyFrame *value) { m_propertyFrame = value; }
	FrameCanvas *GetFrameCanvas() { return m_frameCanvas; }
	void SetFrameCanvas(FrameCanvas *value) { m_frameCanvas = value; }
	CTreeFrame *GetTreeFrame() { return m_treeFrame; }
	void SetTreeFrame(CTreeFrame *value) { m_treeFrame = value; }
	wxTextCtrl *GetTextFrame() { return m_textFrame; }
	void SetTextFrame(wxTextCtrl *value) { m_textFrame = value; }
	CListFrame *GetListFrame() { return m_listFrame; }
	void SetListFrame(CListFrame *value) { m_listFrame = value; }
	CGrid *GetGridFrame() { return m_gridFrame; }
	void SetGridFrame(CGrid *value) { m_gridFrame = value; }
	CTaskBarIcon *GetTaskBarIcon() { return m_taskBarIcon; }
	void SetTaskBarIcon(CTaskBarIcon *value) { m_taskBarIcon = value; }
	CScintillaFrame *GetScintillaFrame() { return m_scintillaFrame; }
	void SetScintillaFrame(CScintillaFrame *value) { m_scintillaFrame = value; }
	wxTimer *GetTimer() { return m_timer; }
	void SetTimer(wxTimer *value) { m_timer = value; }
	void OnTimer(wxTimerEvent& event);
	wxMutex *GetScriptMutex() { return &m_ScriptMutex; }
	wxMutex *GetErrorMutex() { return &m_ErrorMutex; }
	DECLARE_EVENT_TABLE();
};
//**************************************************
DECLARE_APP(wxAutoBotApp);
//**************************************************
#include "Art.h"
#include "Dialog.h"
#include "BotClass.h"
#include "TreeFrame.h"
#include "ListFrame.h"
#include "PropertyFrame.h"
#include "GridFrame.h"
#include "TaskBarIcon.h"
#include "ScintillaFrame.h"
#include "CAutoBotAction.h"
#include "CScriptAction.h"
//**************************************************
class wxAutoBotFrame : public wxFrame
{
public:
	enum MODE
	{
		modeDESIGN,
		modeCYCLE,
		modeTRIGGER,
		modeCOMPONENT,
		modeIF,
		modeTEXT,
		modeEDITTEXT,
		modeLINE
	};

	wxAutoBotFrame(wxWindow * parent, wxWindowID id, 
				   const wxString & title,
				   const wxPoint & pos = wxPoint(10,10),
				   const wxSize & size = wxSize(wxGetDisplaySize().GetWidth()-50,wxGetDisplaySize().GetHeight()-100),
				   long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);
	~wxAutoBotFrame();

	void OnClose(wxCloseEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnProperty(wxCommandEvent& event);
	void OnComponentPanel(wxCommandEvent& event);
	void OnTreeBots(wxCommandEvent& event);
	void OnPaneClose(wxAuiManagerEvent& evt);
	void OnMessageWindow(wxCommandEvent& event);
	void OnVarStack(wxCommandEvent& event);
	void OnPropertyWindow(wxCommandEvent& event);
	void OnWelcomeWindow(wxCommandEvent& event);
	void OnSchematicView(wxCommandEvent& event);
	void OnScintillaView(wxCommandEvent& event);
	void OnNotebookPageClose(wxAuiNotebookEvent& evt);
	void OnNotebookPageChanged(wxAuiNotebookEvent& evt);
	void OnToolbar(wxCommandEvent& event);
	void OnAdd(wxCommandEvent& event);
	void OnAddVar(wxCommandEvent& event);
	void OnDeleteVar(wxCommandEvent& event);
	void OnDelete(wxCommandEvent& event);
	void OnStart(wxCommandEvent& event);
	void OnStop(wxCommandEvent& event);
	void OnPause(wxCommandEvent& event);
	void OnHelp(wxCommandEvent& event);
	void OnSpy(wxCommandEvent& event);
	void OnDebug(wxCommandEvent& event);
	void OnSaveScript(wxCommandEvent& event);
	void OnIconize(wxIconizeEvent& event);
	void OnChar(wxKeyEvent& event);
	void OnShow(wxShowEvent &event);
	void DoUpdate();
	void OnEraseBackground(wxEraseEvent& evt);
	void OnSize(wxSizeEvent& evt);
	void OnUndo(wxCommandEvent& event);
	void OnRedo(wxCommandEvent& event);
	void OnCopy(wxCommandEvent& event);
	void OnCut(wxCommandEvent& event);
	void OnPaste(wxCommandEvent& event);
	void OnSelectAll(wxCommandEvent& event);
	void OnExportToBMP(wxCommandEvent& event);
	void OnPrint(wxCommandEvent& event);
	void OnPrintPreview(wxCommandEvent& event);
	void OnPageSetup(wxCommandEvent& event);

	void OnSlider(wxScrollEvent& event);
	void OnTool(wxCommandEvent& event);

	void OnUpdateStart(wxUpdateUIEvent& event);
	void OnUpdateDebug(wxUpdateUIEvent& event);
	void OnUpdateCopy(wxUpdateUIEvent& event);
	void OnUpdateCut(wxUpdateUIEvent& event);
	void OnUpdatePaste(wxUpdateUIEvent& event);
	void OnUpdateUndo(wxUpdateUIEvent& event);
	void OnUpdateRedo(wxUpdateUIEvent& event);
	void OnUpdateTool(wxUpdateUIEvent& event);
	void OnUpdateScriptToolbar(wxUpdateUIEvent& event);
	
	void OnEdit(wxCommandEvent &event);
	
	wxString GetIntroText();

	MODE GetToolMode(){return m_nToolMode;}
	void SetToolMode(MODE m){m_nToolMode = m;}
	wxAuiManager *GetManager() { return &m_mgr; }

	bool m_fShowShadows;
private:
	FrameCanvas		*CreateFrameCanvasCtrl();
	CTreeFrame		*CreateTreeFrame();
	wxAuiNotebook	*CreateNotebook();
	wxHtmlWindow	*CreateHTMLCtrl(wxWindow* parent = NULL);
	wxTextCtrl		*CreateTextCtrl();
	CListFrame		*CreateListFrame();
	CGrid			*CreateGridCtrl();
	CPropertyFrame	*CreatePropertyGridCtrl();
	CScintillaFrame *CreateScintillaFrameCtrl();
	
	wxMenu			*m_menuView;
	wxMenu			*m_menuFile;
	wxMenu			*m_menuAction;
	wxMenu			*m_menuHelp;
	wxMenu			*m_menuEditor;
	wxMenuBar		*m_menuBar;
	wxSlider		*m_zoomSlider;

	wxAuiToolBar	*m_toolbar;
	wxAuiToolBar	*m_toolscript;
	wxAuiToolBar	*m_toolscheme;
protected:
	wxColourPickerCtrl	*cpicker;
	wxPanel				*m_pCanvasPanel;
	wxSFDiagramManager	m_DiagramManager;
	wxFlexGridSizer		*m_pMainSizer;

	bool m_fShowGrid;
	MODE m_nToolMode;
private:
	wxAuiManager m_mgr;
	DECLARE_EVENT_TABLE();
};
//**************************************************
#endif // __AUTOBOT_H__