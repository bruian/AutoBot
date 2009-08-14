#ifndef __ENUM_H__
#define __ENUM_H__
//**************************************************
enum ePropertyType
{
	BotTree,
	BotNode,
	Bot,
	Rule
};
enum eNodeType
{
	eNodeType_Root = 0,
	eNodeType_Main,
	eNodeType_Templates,
	eNodeType_Import,
	eNodeType_Net,
	eNodeType_FolderUnderRoot,
	eNodeType_Folder,
	eNodeType_Bot,
	eNodeType_Max
};
//Перечисление типов правил
enum eComponentType
{
	Trigger = 0,	//Триггер
	Condition,		//Условие
	Action,			//Действие
	Cycle,			//Цикл
	Script,			//Скрипт
	ActionMax		//Максима
};
//Перечисление типов триггера
enum eTriggerType
{
	None = 0,
	Timer,
	Event
};
//Перечисление периодичности расписания
enum eOccurs
{
	No = 0,
	Daily,
	Weekly,
	Monthly
};
//Перечисление колонок
enum eColumns
{
	Col_id,
	Col_dscr,
	Col_type,
	Col_value,
	Col_Max
};
//Перечисление типов переменных
enum eVar
{
	Var_bool = 0,
	Var_number,
	Var_string,
	Var_Max
};
//Перечисление названий переменных
static const wxString variables[] =
{
	wxT("bool"),
	wxT("number"),
	wxT("string")
};
//Перечисление булевых
static const wxString booleans[] =
{
	wxT("false"),
	wxT("true")
};
//Структура таблицы переменных
static struct CGridData
{
	int id;
	eVar var;
	wxString value;
} *gs_dataCGrid; /*[] =
{
	{ 1, Var_string, wxT("wxMSW") },
};*/
//**************************************************
enum
{
	TreeFrameIcon_File,
	TreeFrameIcon_FileSelected,
	TreeFrameIcon_Folder,
	TreeFrameIcon_FolderSelected,
	TreeFrameIcon_FolderOpened,
	ID_RunOnStartOS,
	ID_DisplayGroupAppOnStart,
	ID_DisplayLabelOnDesktop,
	ID_MoveInTray,
	ID_Password,
	ID_AllowUpdate,
	ID_CiteUpdate,
	ID_ArrayLanguage,
	ID_DND,
	PU_RESTORE,
	PU_START,
	PU_STOP,
	PU_PAUSE,
	PU_PAUSETIMER,
	PU_EXIT,
	Menu_Save,
	Menu_SaveScript,
	Menu_UndoScript,
	Menu_RedoScript,
	Menu_CutScript,
	Menu_CopyScript,
	Menu_PasteScript,
	Menu_DeleteScript,
	Menu_FindScript,
	Menu_Load,
	Menu_Property,
	Menu_Import,
	Menu_ComponentPanel,
	Menu_TreeBots,
	Menu_MessageWindow,
	Menu_VarStack,
	Menu_PropertyWindow,
	Menu_WelcomeWindow,
	Menu_SchematicView,
	Menu_Toolbar,
	Menu_ScriptWindow,
	Menu_StatisticsWindow,
	Menu_Add,
	Menu_AddVar,
	Menu_Delete,
	Menu_DeleteVar,
	Menu_Start,
	Menu_Stop,
	Menu_Pause,
	Menu_Spy,
	Menu_Debug,
	Tree_Start,
	Tree_Pause,
	Tree_Stop,
	Tree_Add,
	Tree_AddFolder,
	Tree_Delete,
	Toolbar_Ctrl,
	Toolscript_Ctrl,
	Toolscheme_Ctrl,
	Notebook_Ctrl,
	Text_Ctrl,
	List_Ctrl,
	Prop_Ctrl,
	Grid_Ctrl,
	Tree_Ctrl = 1000
};
//**************************************************
//Перечисление хранимых типов в БД BotType
enum eBT
{
	bt_bool = 0,	//0
	bt_int,			//1
	bt_long,		//2
	bt_float,		//3
	bt_char,		//4
	bt_string,		//5
	bt_blob,		//6
	bt_ico,			//7
	bt_png,			//8
	bt_bmp,			//9
	bt_xpm,			//10
	bt_cur,			//11
	bt_html,		//12
	bt_max
};
//**************************************************
//Ico ресурсы
enum eIcoRes
{
	icr_appico = 0, //0
	icr_save,		//1
	icr_load,		//2
	icr_add,		//3
	icr_delete,		//4
	icr_start,		//5
	icr_pause,		//6
	icr_stop,		//7
	icr_addvar,		//8
	icr_deletevar,	//9
	icr_help,		//10
	icr_treeitem1,	//11
	icr_treeitem2,	//12
	icr_treeitem3,	//13
	icr_treeitem4,	//14
	icr_treeitem5,	//15
	icr_spy,		//16
	icr_debug,		//17
	icr_savescript,	//18
	icr_undoscript,	//19
	icr_redoscript,	//20
	icr_cutscript,	//21
	icr_copyscript,	//22
	icr_pastescript,//23
	icr_deletescript,//24
	icr_findscript,	//25
	icr_max
};
//**************************************************
//Ico ресурсы
enum eIcoRes32
{
	icr32_appico = 0, //0
	icr32_SpyOn,	  //1
	icr32_SpyOff,	  //2
	icr32_max
};
//**************************************************
//Image ресурсы
enum eImgRes
{
	imr_comTrigger = 0, //0
	imr_comIf,			//1
	imr_comAction,		//2
	imr_comActionWinDlg,//3
	imr_comActionWinFnd,//4
	imr_comActionProcess,//5
	imr_max
};
//**************************************************
enum eScriptType
{
	eScriptType_Squirrel,
	eScriptType_Max
};
//**************************************************
enum APPIDS
{
	// menu IDs
	IDM_SAVEASBITMAP = wxID_HIGHEST + 1,
	// tool IDs
	IDT_FIRST_TOOLMARKER,
	IDT_GRID,
	IDT_SHADOW,
	IDT_GC,
	IDT_TOOL,
	IDT_CYCLESHP,
	IDT_TRIGGER,
	IDT_COMPONENT,
	IDT_IFSHAPE,
	IDT_TEXTSHP,
	IDT_EDITTEXTSHP,
	IDT_LINESHP,
	IDT_ALIGN_LEFT,
	IDT_ALIGN_RIGHT,
	IDT_ALIGN_TOP,
	IDT_ALIGN_BOTTOM,
	IDT_ALIGN_MIDDLE,
	IDT_ALIGN_CENTER,

	IDT_LAST_TOOLMARKER,
	// other constrols
	IDT_COLORPICKER
};
#endif // __ENUM_H__