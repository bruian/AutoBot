#ifndef __BOTCLASS_H__
#define __BOTCLASS_H__

#include "AutoBot.h"
//�������� ������� Bot ����������
#define STATUS_STOP 0
#define STATUS_PLAY 1
#define STATUS_PAUSE 2
#define STATUS_ERROR 3
#define STATUS_CONTINUE 4
//**************************************************
//����� ��� �������� � ����������� ������ ������ ����������
class CError : public wxLog //: public wxError
{
public:
	enum eErrorType //���� ������
	{
		eErrorType_APP = 0,	//����� ������ ������, ����� ��������� � ��������� �������������� �������
		eErrorType_DB_Open,	//������ �������� ���� ������
		eErrorType_DB_Image,//������ �������� Image �������
		eErrorType_FS_Dir,
		eErrorType_Max		//������������ ����� ��������� ������
	};
	
	enum eMsgType
	{
		eMsgType_Default = 0,
		eMsgType_NoTxtOutput,
		eMsgType_AppErr,
		eMsgType_RuleMsg,
		eMsgType_RuleErr,
		eMsgType_BotMsg,
		eMsgType_BotErr,
	};
private:
	wxString		m_ErrorMsg;		//���������������� ��������� �� ������
	eErrorType		m_ErrorType;	//��� ������
	bool			m_FatalError;	//=true ���� ������ ��������� (���������� ���������� ������)
	bool			m_Error;		//=true ���� �������� ������ � ������ ����������
	bool			m_SaveLogToBase;//���������� ���������� ��� � ����, ��� ���

	wxArrayString	m_ErrorMsgEnum;	//������ ������ ����� �������� ������
	
	wxArrayString	m_DateTimeMsg;	//������� ���������[0]
	wxArrayString	m_QueueMsg;		//������� ���������[1]
	wxArrayInt		m_QueueMsgType;	//������� ���������[2]
	wxArrayString	m_QueueBotGUID;	//������� ���������[3]
	wxArrayString	m_QueueRuleGUID;//������� ���������[4]

	wxArrayString	m_tmpDateTimeMsg;	//������� ���������[0]
	wxArrayString	m_tmpQueueMsg;		//������� ���������[1]
	wxArrayInt		m_tmpQueueMsgType;	//������� ���������[2]
	wxArrayString	m_tmpQueueBotGUID;	//������� ���������[3]
	wxArrayString	m_tmpQueueRuleGUID; //������� ���������[4]

	void AddQueue(wxString msgTxt, eMsgType MsgType, wxString botGuid, wxString ruleGuid);
public:
	CError();
	~CError();

	//������ ��������� ������
	void SetError(bool Error, wxString ErrorMsg, eErrorType ErrorType, bool FatalError);
	void AddMessage(wxString msgTxt, eMsgType MsgType = eMsgType_Default)
	{
		AddQueue(msgTxt, MsgType, wxEmptyString, wxEmptyString);
	};
	void AddMessage(wxString msgTxt, eMsgType MsgType, wxString botGuid)
	{
		AddQueue(msgTxt, MsgType, botGuid, wxEmptyString);
	};
	void AddMessage(wxString msgTxt, eMsgType MsgType, wxString botGuid, wxString ruleGuid)
	{
		AddQueue(msgTxt, MsgType, botGuid, ruleGuid);
	};

	//��������� ��������� ��������� ������, 
	//return 0 - ������ ���, 1 - ������� ������, 2 - ��������� ������
	int IsError();
	void SaveToDB();
protected:
	virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t)
	{
		AddMessage(szString);
		//wxLog::DoLog(level, szString, t);
	}
	//virtual void DoLogString(const wxChar *szString, time_t t);
};
//**************************************************
class wxDirTraverserSimple : public wxDirTraverser
{
public:
	wxDirTraverserSimple(wxArrayString& files, wxArrayString& dirs) : m_files(files), m_dirs(dirs) { }
	virtual wxDirTraverseResult OnFile(const wxString& filename)
	{
		m_files.Add(filename);
		return wxDIR_CONTINUE;
	}
	virtual wxDirTraverseResult OnDir(const wxString& dirname)
	{
		m_dirs.Add(dirname);
		return wxDIR_CONTINUE;
	}
private:
	wxArrayString& m_files;
	wxArrayString& m_dirs;
};
//**************************************************
class wxDirTraverserFind : public wxDirTraverser
{
public:
	wxDirTraverserFind(wxArrayString& files) : m_files(files) { }
	virtual wxDirTraverseResult OnFile(const wxString& filename)
	{
		m_files.Add(filename);
		return wxDIR_CONTINUE;
	}
	virtual wxDirTraverseResult OnDir(const wxString& dirname)
	{
		m_files.Add(dirname);
		return wxDIR_CONTINUE;
	}
private:
	wxArrayString& m_files;
};
//**************************************************
//����� �������� ����������
class CAppProperties
{
private:
	DatabaseLayer	*m_PropertiesDB;			//���� ������ �������� ����������
	bool			m_FirstStart;				//������ ������
	bool			m_DisplayLabelOnDesktop;	//���������� ����� �� ������� �����
	bool			m_DisplayGroupAppOnStart;	//���������� ������ ���������� � ���� ����
	bool			m_RunOnStartOS;				//��������� ���������� ��� ������ ��
	bool			m_MoveInTray;				//����������� � ���� ��� �������
	wxString		m_HashPassword;				//��� ������ ������� � ���������
	bool			m_AllowUpdate;				//��������� ��������� ���������� �� �����
	wxString		m_CiteUpdate;				//���� ��� ����������
	int				m_Language;					//ID ������������� ����� ����������
	wxString		m_PaneSettings;				//��������� ������������ ��������� ����������
	long			m_TotalNodes;				//���������� ����� � ������

	wxList			m_ComponentList;	//������ ����������� ���������
	wxArrayString	m_ArrayLanguage;	//������ ������ ����������
	wxArrayString	m_ArrayLngRes;		//������ �������� ��������� ��������, ��� Id ��� eLngRes
	wxArrayString	m_ArrayLngId;		//������ �������� ���������������... m_ArrayLngRes == m_ArrayLanguage
	wxImageList		m_ArrayIcon;		//������ ������ ��� ���������� 16*16
	wxImageList		m_ArrayIcon32;		//������ ������ ��� ���������� 32*32
	wxCursor		m_SpyCur;			//������ ������� ������
	void CAppProperties::ApplySettings();
public:
	enum EvtType
	{
		// Event Type
		Event_AddTreeItem = 1,
		Event_AddTreeSubItem,
		Event_DeleteTreeItem,
		Event_BeginDragTreeItem,
		Event_EndDragTreeItem,
		Event_StartItem,
		Event_StopItem,
		Event_PauseItem,
		Event_SelectTreeNode,
		Event_SelectRule,
		Event_AddRule,
		Event_DeleteRule,
		Event_PasteRule,
		Event_ChangeProperty,
		Event_UnSelectRule,
		Event_DebugBot,
		Event_Default
	};
	CAppProperties(wxString path);
	~CAppProperties();

	DatabaseLayer *GetPropertiesDB() { return m_PropertiesDB; }
	wxList	 &GetComponentList()	{ return m_ComponentList; }
	bool GetFirstStart() { return m_FirstStart; }
	void SetFirstStart(bool value) { m_FirstStart = value; }
	wxString GetPaneSetting() { return m_PaneSettings; }
	void SetPaneSetting(wxString value) { m_PaneSettings = value; }
	long GetTotalNodes() { return m_TotalNodes; }
	void SetTotalNodes(long value) { m_TotalNodes = value; }
	wxIcon GetIco(eIcoRes value) { return m_ArrayIcon.GetIcon(value); }
	wxIcon GetIco32(eIcoRes32 value) { return m_ArrayIcon32.GetIcon(value); }
	wxBitmap GetBitmap32(eIcoRes32 value) { return m_ArrayIcon32.GetBitmap(value); }
	wxString GetLngRes(wxString idLngRes);
	bool GetDisplayLabelOnDesktop() { return m_DisplayLabelOnDesktop; }
	void SetDisplayLabelOnDesktop(bool value) { m_DisplayLabelOnDesktop = value; }
	bool GetDisplayGroupAppOnStart() { return m_DisplayGroupAppOnStart; }
	void SetDisplayGroupAppOnStart(bool value) { m_DisplayGroupAppOnStart = value; }
	bool GetRunOnStartOS() { return m_RunOnStartOS; }
	void SetRunOnStartOS(bool value) { m_RunOnStartOS = value; }
	int GetLanguage() { return m_Language; }
	void SetLanguage(int value) { m_Language = value; }
	bool GetMoveInTray() { return m_MoveInTray; }
	void SetMoveInTray(bool value) { m_MoveInTray = value; }
	bool GetAllowUpdate() { return m_AllowUpdate; }
	void SetAllowUpdate(bool value) { m_AllowUpdate = value; }
	wxString GetHashPassword() { return m_HashPassword; }
	void SetHashPassword(wxString value) { m_HashPassword = value; }
	void SetPassword(wxString password);
	wxCursor GetSpyCursor() { return m_SpyCur; }
	CComponent *GetComponent(GUID guid);

	GUID StrToGuid(wxString strGUID);
	wxString GuidToStr(GUID guid);

	wxString GetCiteUpdate() { return m_CiteUpdate; }
	void SetCiteUpdate(wxString value) { m_CiteUpdate = value; }
	wxArrayString GetArrayLanguage() { return m_ArrayLanguage; }
	bool LoadAppProperties();
	bool SaveAppProperties();

	bool LoadPlugIn();
	wxIcon GetIconFromMFS(wxString imPath, long Type);
	wxImage GetImageFromMFS(wxString imPath, long Type);

	bool OnProcess(EvtType evt, wxObject* object, wxEvent* event);
};
//**************************************************
//����� �������� ��� ����������� ��������
class CProgramProcess : public wxProcess
{
public:
	CProgramProcess(wxFrame *parent, const wxString& cmd)
					: wxProcess(parent), m_cmd(cmd)
	{
		m_parent = parent;
	}

	virtual void OnTerminate(int pid, int status);
protected:
	wxFrame *m_parent;
	wxString m_cmd;
};
//**************************************************
class CBotThread : public wxThread
{
public:
	CBotThread(CBot *Bot);
	// thread execution starts here
	virtual void *Entry();
	// called when the thread exits - whether it terminates normally or is
	// stopped with Delete() (but not when it is Kill()ed!)
	virtual void OnExit();
public:
	CBot *m_Bot;
	unsigned m_count;
};
//**************************************************
//����� �������� ����� ����������
class CVar : public wxObject
{
public:
	long		m_Id;	//������������� ����������
	wxString	m_Dscr;	//��������
	eVar		m_Type;	//���
	wxString	m_Value;//��������
public:
	CVar()
	{
		m_Id	= NULL;
		m_Dscr	= wxEmptyString;
		m_Type	= Var_string;
		m_Value = wxEmptyString;
	}
	long GetId() { return m_Id; }
	void SetId(long value) { m_Id = value; }
	wxString GetDescription() { return m_Dscr; }
	void SetDescription(wxString value) { m_Dscr = value; }
	eVar GetType() { return m_Type; }
	void SetType(eVar value) { m_Type = value; }
	wxString GetValue() { return m_Value; }
	void SetValue(wxString value) { m_Value = value; }
};
//**************************************************
class CTrigger
{
private:
	int			 m_TriggerType;		//m3-���� ������� ���� ��������, �� ������������ ��� ��������
	int			 m_PeriodSec;		//m4-
	bool		 m_OnTimetable;		//m5-������� ���������� true = ��, false = ���
	int			 m_Periodicity;		//m6-������������� ������� ������� � ��������
	wxDateTime	 m_DateStart;		//m7-
	wxDateTime	 m_TimeStart;		//m8-
	int			 m_OccursDaily;		//m9-������������� ���������� ����������
	int			 m_EvryN;			//m10-
	int			 m_TimeUnit;		//m11-
	wxDateTime	 m_DailyFreqBegin;	//m12-
	wxDateTime	 m_DailyFreqEnd;	//m13-
	long		 m_Weekly;			//m14-
	long		 m_DayOfWeek;		//m15-�� ����� ���� ������ ������ ������ eDays
	bool		 m_StartRun;		//m16-��������� ������� ������� �� ������� m_DateStart � m_TimeStart
	long		 m_Month;			//m17-
	wxDateTime	 m_StartDateTime;	//����� ������ ���� ��� ������������� ����������
	wxDateTime	 m_PeriodDateTime;	//����� ������������ ��� ������������� �������������
	bool		 m_FlagStartRun;
public:
	CTrigger();

	int GetTriggerType() { return m_TriggerType; }
	void SetTriggerType(int value) { m_TriggerType = value; }
	int GetPeriodSec() { return m_PeriodSec; }
	void SetPeriodSec(int value) { m_PeriodSec = value; }
	bool GetOnTimetable() { return m_OnTimetable; }
	void SetOnTimetable(bool value) { m_OnTimetable = value; }
	int GetPeriodicity() { return m_Periodicity;}
	void SetPeriodicity(int value) { m_Periodicity = value; }
	wxDateTime GetDateStart() { return m_DateStart; }
	void SetDateStart(wxDateTime value) { m_DateStart = value; }
	wxDateTime GetTimeStart() { return m_TimeStart; }
	void SetTimeStart(wxDateTime value) { m_TimeStart = value; }
	int GetOccursDaily() { return m_OccursDaily; }
	void SetOccursDaily(int value) { m_OccursDaily = value; }
	int GetEvryN() { return m_EvryN; }
	void SetEvryN(int value) { m_EvryN = value; }
	int GetTimeUnit() { return m_TimeUnit; }
	void SetTimeUnit(int value) { m_TimeUnit = value; }
	wxDateTime GetDailyFreqBegin() { return m_DailyFreqBegin; }
	void SetDailyFreqBegin(wxDateTime value) { m_DailyFreqBegin = value; }
	wxDateTime GetDailyFreqEnd() { return m_DailyFreqEnd; }
	void SetDailyFreqEnd(wxDateTime value) { m_DailyFreqEnd = value; }
	int GetWeekly() { return m_Weekly; }
	void SetWeekly(int value) { m_Weekly = value; }
	long GetDayOfWeek() { return m_DayOfWeek; }
	void SetDayOfWeek(long value) { m_DayOfWeek = value; }
	bool GetStartRun() { return m_StartRun; }
	void SetStartRun(bool value) { m_StartRun = value; }
	long GetMonth() { return m_Month; }
	void SetMonth(long value) { m_Month = value; }

	wxDateTime GetStartDateTime() { return m_StartDateTime; }
	void SetStartDateTime(wxDateTime value) { m_StartDateTime = value; }

	wxDateTime GetPeriodDateTime() { return m_PeriodDateTime; }
	void SetPeriodDateTime(wxDateTime value) { m_PeriodDateTime = value; }

	bool Check(CBot *bot, size_t *delay);
};
//**************************************************
class CComponent : public wxObject
{
private:
	GUID			m_coGUID;
	wxString		m_Name;
	int				m_State;
	wxString		m_Path;
	wxXmlDocument	*m_XMLStruct;
	wxString		m_XLSInterface;
	wxString		m_XLSCodegeneration;
	eComponentType	m_ComponentType;
	wxImage			m_Image;
	Plugin			*m_Plugin;
	wxPluginLibrary	*m_Library;
public:
	CComponent()
	{
		m_XMLStruct = new wxXmlDocument();
		m_Plugin = (Plugin*)NULL;
		m_Library = (wxPluginLibrary*)NULL;
	}
	~CComponent()
	{
		wxDELETE(m_XMLStruct);
		wxDELETE(m_Plugin);
		wxDELETE(m_Library);
	}

	GUID	 GetGUIDComponent()				{ return m_coGUID; }
	void	 SetGUIDComponent(GUID guid)	{ m_coGUID = guid; }
	wxString GetName()						{ return m_Name; }
	void	 SetName(wxString value)		{ m_Name = value; }
	wxXmlDocument *GetXMLStruct()			{ return m_XMLStruct; }
	void	 SetXMLStruct(wxXmlDocument *value) { m_XMLStruct = value; }
	int		 GetState()						{ return m_State; }
	void	 SetState(int value)			{ m_State = value; }
	wxString GetPath()						{ return m_Path; }
	void	 SetPath(wxString value)		{ m_Path = value; }
	eComponentType GetComponentType()		{ return m_ComponentType; }
	void	 SetComponentType(eComponentType value) { m_ComponentType = value; }
	Plugin	 *GetPlugin()					{ return m_Plugin; }
	void	 SetPlugin(Plugin *value)		{ m_Plugin = value; }
	wxPluginLibrary *GetLibrary()			{ return m_Library; }
	void	 SetLibrary(wxPluginLibrary *value) { m_Library = value; }

	void Link(script::VMCore *ptCore);

	wxImage	 &GetImage() { return m_Image; }
	void	 SetImage(wxImage value) { m_Image = value; }
};
//**************************************************
//����� RULE ������� - ������� ���������
class CRule : public wxObject
{
private:
	//CTrigger		*m_Trigger;
	CComponent		*m_Component;
	bool			m_EXECUTE;		//���� �������, ������������ ��� ������ ������� ������� � ����������
									//�������� �� ������� ������ �� ����������, ���������� ������� ������� ���� TRUE
	wxArrayString	m_Values;
	GUID			m_guidRule;
public:
	CRule();
	~CRule();

	CComponent *GetComponent()					{ return m_Component; }
	void	 SetComponent(CComponent *value)	{ m_Component = value; }
	//CTrigger *GetTrigger()						{ return m_Trigger; }
	//void	 SetTrigger(CTrigger *value)		{ m_Trigger = value; }
	bool	 GetExecute()						{ return m_EXECUTE; }
	void	 SetExecute(bool value)				{ m_EXECUTE = value; }
	GUID	 GetGUIDRule()						{ return m_guidRule; }
	void	 SetGUIDRule(GUID guid)				{ m_guidRule = guid; }

	bool ConvStrToDate(const wxString value, wxDateTime *ConvDate);
	bool ConvStrToTime(const wxString value, wxDateTime *ConvTime);

	bool CriterionOK(wxString criterion);
	wxString GetValue(wxString valueID);
	void SetValue(wxString valueID, wxString value);
	wxArrayString GetValues() { return m_Values; }
	void SetValues(wxArrayString value) { m_Values = value; }
	wxString PackValues();
	void UnPackValues(wxString values);
};
//**************************************************
struct IF
{
	wxString _IF;
	int		 _CONDITION;
};
//**************************************************
struct UNIT
{
	int		 _ID;
	wxString _TYPE;
	wxString _UNIT;
	int		 _PARENT;
	int		 _CONDITION;
};
//**************************************************
struct REL
{
	int	_SOURCE;
	int	_TARGET;
	int _CONDITION;
};
//**************************************************
WX_DECLARE_OBJARRAY(IF, ArrayOfIF);
WX_DECLARE_OBJARRAY(UNIT, ArrayOfUNIT);
WX_DECLARE_OBJARRAY(REL, ArrayOfREL);
//**************************************************
//����� Bot ������� - ����� ������ ���������
class CBot : public wxObject
{
public:
	enum eDebug
	{
		Debug_No = 1,
		Debug_Wait,
		Debug_Down,
		Debug_Up
	};
protected:
	CBotThread *m_CurrThread;
private:
	wxList	  m_listRule;			//������� ������ ��������� ����
	wxList	  m_listVariables;		//������ ����������� ���������� ��� ������ ��������� ����
	wxString  m_nameBot;			//������������ ����
	wxString  m_descrBot;			//������� �������� ����
	wxXmlDocument *m_shapeDiagram;	//xml ���� ����������� ���������� ����� ����
	int		  m_statusWork;			//������ ������ ���� 
	long	  m_lastNumVar;
	int		  m_DebugLine;			//������ � �������
	eDebug	  m_DebugState;			//��������� ������� �� ������ ������
	GUID	  m_GUIDBot;			//GUID ����
	bool	  m_ChangedBot;			//������� ��������� � ����, ���� ��������� ����, �� ��� �����������������
	bool	  m_LoadedBot;			//������� ������������ ����
	script::VMCore *m_VMCore;		//���� ����������� ������ ��� �����
public:
	CBot(bool createGuid);
	~CBot();
	
	CBotThread *GetThread()			{ return m_CurrThread; }
	void SetThread(CBotThread *value){ m_CurrThread = value; }
	long	 GetTotalRule()			{ return m_listRule.GetCount(); }
	wxList	 &GetListRule()			{ return m_listRule; }
	wxList	 &GetListVariables()	{ return m_listVariables; }
	long	 GetTotalVariables()	{ return m_listVariables.GetCount(); }
	int		 GetStatusBot()			{ return m_statusWork; }
	void	 SetStatusBot(int value){ m_statusWork = value; }
	wxString GetDescrBot()			{ return m_descrBot; }
	void	 SetDescrBot(wxString value) { m_descrBot = value; }
	wxString GetNameBot()			{ return m_nameBot; }
	void	 SetNameBot(wxString value) { m_nameBot = value; }
	long	 GetLastNumVar()		{ return m_lastNumVar; }
	void	 SetLastNumVar(long value)	{ m_lastNumVar = value; }
	GUID	 GetGUIDBot()			{ return m_GUIDBot; }
	void	 SetGUIDBot(GUID guid)	{ m_GUIDBot = guid; }
	wxXmlDocument *GetShapeDiagram()		{ return m_shapeDiagram; }
	void	 SetShapeDiagram(wxXmlDocument *value) { m_shapeDiagram = value; }

	CRule *AddRule(GUID componentGuid, GUID *guidRule);
	bool DeleteRule(const GUID guidRule);
	CVar* GetVariables(long varId);
	CRule* GetRule(const GUID guid);
	int GetDebugLine() { return m_DebugLine; }
	void SetDebugLine(int value) { m_DebugLine = value; }
	eDebug GetDebugState() { return m_DebugState; }
	void SetDebugState(eDebug value) { m_DebugState = value; }
	script::VMCore *GetVMCore() { return m_VMCore; }

	//IsChangedBot - ����� ����������, �������� �� ���,
	bool IsChangedBot() { return m_ChangedBot; }
	void SetChangedBot(bool value) { m_ChangedBot = value; }

	void ExpandXMLRule(wxXmlNode *GetChildren, CRule *rule, wxArrayString *script, size_t *index, wxString objectName, wxString sCompile);
	void ExpandXMLNode(wxXmlNode *GetChildren, ArrayOfUNIT &ArrUnit, ArrayOfREL &ArrRel, int parent);
	//����� ��������� � ���������, ���� ����������� ��� ������ �� ����� �������, ���� �����
	long IfGet(ArrayOfIF *ArrIf, wxString guid);
	//����� ��������� � ��������� ���� ����� ����������� ��������� ���� ����
	long IndexLast(ArrayOfIF *ArrIf, wxArrayString *ArrScr);

	wxString GetValueFromMember(wxString value);
	void SetValueFromMember(wxString VarToSet, wxString value);

	bool Compile(wxString *sCode, wxString *strTrigger);
	void InitVMCore();
	wxString  m_Temp;
};
//**************************************************
//����� ���� ������ Bot � Node ���������
class CNodeBot : public wxObject
{
private:
	wxString  m_NameNode;	//������������ ���� ������
	wxString  m_DbNodePath;	//���� � ������������ �� ���� ������
	eNodeType m_NodeType;	//��� ���� ������
	wxList	  m_ListNode;	//������ ����� � ���� ������
	wxList	  m_ListBot;	//������ ����� � ���� ������
	GUID	  m_GUIDNode;	//���������� ������������� ���� ������
public:
	CNodeBot();
	~CNodeBot();
	//AttachBase - ����������� �� � ������������ ������� �����
	//path - ���� � ��
	bool AttachBase(wxString path);
	//DetachBase - ���������� �� � ������������ ������� �����
	bool DetachBase();
	//GetNodePath - �������� ���� � ������������ ���� ������
	wxString GetNodePath() { return m_DbNodePath; }
	//SetNodePath - ���������� ���� � ������������ ���� ������
	void	 SetNodePath(wxString value) { m_DbNodePath = value; }

	//AddBot - ������� ���� � �������� � ������� ���� ������
	//IN:nameBot - ����������� ��� ����, OUT:BotGUID - ����������� ���� ����, 
	//OUT:num - ���������� ����� � ������ �����
	CBot *AddBot(const wxString nameBot, GUID* BotGUID, int* num);
	//GetBot - �������� ���� �� ��������������, guid - ������������� �� �������� ����� �������� ���
	CBot* GetBot(const GUID guid);
	//LoadBot - �������� ������ � ���� �� ��,
	//guid - ������������ ����, ������� ��������� ���������, bot - ��������� �� ��� � ������� ���������
	bool LoadBot(GUID guid, CBot *bot);
	//SaveBot - ���������� ������ � ���� � ��, bot - ��������� �� ��� ������� ����� ���������
	bool SaveBot(CBot *bot, SqliteDatabaseLayer *pDataBase);
	//DeleteBot - ������� ����. guid - ������������� ����, �������� ����� �������.
	bool DeleteBot(const GUID guid);
	bool ClearNode();

	//GetListNode - �������� ������ ����� ������������� �������� ����
	wxList	 &GetListNode()	{ return m_ListNode; }
	//GetListBot - �������� ������ ����� ������������� �������� ����
	wxList	 &GetListBot()	{ return m_ListBot; }
	//GetTotalBot - ����� ����� � ������� ����
	long	 GetTotalBot()	{ return m_ListBot.GetCount(); }
	//GetTotalNode - ����� ����� � ������� ����
	long	 GetTotalNode()	{ return m_ListNode.GetCount(); }
	//GetGUIDNode - �������� GUID ������������� ����
	GUID	 GetGUIDNode()	{ return m_GUIDNode; }
	//SetGUIDNode - ���������� GUID ������������� ����
	void	 SetGUIDNode(GUID guid)	{ m_GUIDNode = guid; }
	//GetNodeName - �������� ��� ����
	wxString GetNodeName()	{ return m_NameNode; }
	//SetNodeName - ���������� ��� ����
	void	 SetNodeName(const wxString str) { m_NameNode = str; }
	//GetNodeType - �������� ��� ����
	eNodeType GetNodeType() { return m_NodeType; }
	//SetNodeType - ���������� ��� ����
	void	 SetNodeType(eNodeType value) { m_NodeType = value; }
};
//**************************************************
//����� ������ Bot ��������!
class CTreeBot
{
private:
	CBot		*m_ActiveBot;	//��������� �� �������� ���
	CRule		*m_ActiveRule;	//�������� - ���������� �������
	CNodeBot	*m_ActiveNode;	//��������� �� �������� ����
	CBot		*m_DebugBot;	//��������� �� ������������ ���
	CNodeBot	m_RootNode;		//�������� ���� � ������ �����
	wxList		m_ListClipboardRule;	//������ ������ � ������ ������
public:
	CTreeBot();
	~CTreeBot();
	bool LoadTreeBot();
	bool SaveTreeBot();
	bool LoadTreeNode();

	bool m_isCut;
	CNodeBot *GetRootNode() { return &m_RootNode; }
	bool CanChangeTree(GUID guid);

	bool StartBot(GUID guid);
	bool StopBot(const GUID guid);
	bool PauseBot(const GUID guid);
	bool DebugBot();
	bool PauseAllBot();
	bool UnPauseAllBot();
	bool StopAllBot(bool ApplyStatus = false);
	bool StartAllBot(bool ApplyStatus = false);

	wxList *GetListClipboardRule() { return &m_ListClipboardRule; }
	CRule *GetClipboardRule(const GUID guid);
	CNodeBot *GetActiveNode();
	CNodeBot *GetNode(GUID guid);
	CBot *GetBot(GUID guid);
	CNodeBot *GetParentNode(GUID guid);
	int GetLevelNode(CNodeBot *value);
	void SetActiveNode(CNodeBot *value) { m_ActiveNode = value; }
	void SetActiveBot(CBot *value) { m_ActiveBot = value; }
	void UnactiveBot() { m_ActiveBot = (CBot *)NULL; }
	CBot* GetActiveBot() { return m_ActiveBot; }
	void SetActiveRule(CRule *value) { m_ActiveRule = value; }
	CRule* GetActiveRule() { return m_ActiveRule; }
	void UnActiveRule() { m_ActiveRule = (CRule *)NULL; }
	CBot *GetDebugBot() { return m_DebugBot; }
	void SetDebugBot(CBot *value) { m_DebugBot = value; }

	CBot *CopyBot(GUID parentSrc, GUID parentDst, GUID child);
};
#endif //__BotCLASS_H__