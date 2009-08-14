#ifndef __CSCRIPTACTION_H__
#define __CSCRIPTACTION_H__
//**************************************************
#include "AutoBot.h"
//**************************************************
// Class: CComponent
// Name: ScriptAction
// Version: 1.0
// Description: Класс компоненты для работы с процессами
//	и приложениями ОС.
// Component GUID: {E2ADA6EC-B9F0-4563-A08D-A3E6EFBDF6A3}
class CScriptAction
{
public:
	wxString typecoGUID;	//GUID типа компоненты

	//EXTERN member
	int					m_ProcessType;		//m1 - тип выполняемой операции над процессами
	const xchar*	m_ProgramName;		//m2 - критерий, Путь к исполняемому файлу процесса
	bool				m_WaitEndProcess;	//m3 - критерий, Ожидание завершения работы процесса
	const xchar*	m_ReturnPID;		//m4 - результат, Process ID
	const xchar*	m_CriterionPID;		//m5 - критерий, Process ID
	int					m_MethodEndProcess;	//m6 - критерий, метод завершения работы процесса
	const xchar*	m_ResultOperation;	//m7 - результат, результат выполнения операции над процессом
	const xchar*	m_ProcessName;		//m8 - критерий, наименование процесса
	const xchar*	m_HandleTopWindow;	//m9 - Результат, хэндл главного окна приложения
	wxString			m_Description;		//m10 - Описание действия компоненты
	wxString			m_GUID;				//m11 - GUID компоненты
public:
	CScriptAction()
	{
		typecoGUID = wxT("E2ADA6EC-B9F0-4563-A08D-A3E6EFBDF6A3");
	};

	void Execute();
};
//**************************************************
//class CScriptAction
//{
//private:
//	eScriptType		m_ScriptType;
//	wxArrayString	m_VarList;
//	wxString		m_Script;
//public:
//	CScriptAction();
//	~CScriptAction() {};
//
//	eScriptType GetScriptType() { return m_ScriptType; }
//	void SetScriptType(eScriptType value) { m_ScriptType = value; }
//	wxArrayString GetVarList() { return m_VarList; }
//	void SetVarList(wxArrayString value) { m_VarList = value; }
//	wxString GetScript() { return m_Script; }
//	void SetScript(wxString value) { m_Script = value; }
//	wxString PackValuesToString();
//	void UnPackValuesFromString(wxString value);
//
//	bool Execute(CBot *bot);
//	bool RunSquirrelBuffer(CBot *bot);
//};
//**************************************************
#endif //__CSCRIPTACTION_H__