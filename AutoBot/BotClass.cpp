//Реализация классов Bot приложения
#include "BotClass.h"
//#include "plugin.h"
//IMPLEMENT_ABSTRACT_CLASS(Plugin, wxObject)

WX_DEFINE_OBJARRAY(ArrayOfIF);
WX_DEFINE_OBJARRAY(ArrayOfUNIT);
WX_DEFINE_OBJARRAY(ArrayOfREL);
//**************************************************
void printfunc(HSQUIRRELVM v, const SQChar *s, ...)
{
	wxGetApp().GetErrorMutex()->Lock();
	va_list vl;
	va_start(vl, s);
	scvprintf(s, vl);
	va_end(vl);
	wxGetApp().GetErrorMutex()->Unlock();
}
//**************************************************
//Реализация класса ошибок приложения
CError::CError()
{
	//Добавляем описание первых двух ошибок на английском языке.
	//После успешной загрузки базы данных приложения, пополняем список из ресурсов
	//в соответствии с предпочитаемым языком пользователя
	m_ErrorMsgEnum.Alloc(eErrorType_Max);
	m_ErrorMsgEnum.Add(wxT("Unknown error of the application!"));
	m_ErrorMsgEnum.Add(wxT("Error of the opening database!"));
	m_ErrorMsgEnum.Add(wxT("Error of the loading image resourse!"));
	m_ErrorMsgEnum.Add(wxT("Can't create directory!"));

	//Зададим члены класса (по умолчанию)
	m_ErrorMsg	 = wxEmptyString;
	m_ErrorType	 = eErrorType_APP;
	m_FatalError = false;
	m_Error		 = false;
	m_SaveLogToBase = true;
}
//**************************************************
CError::~CError()
{
	m_tmpDateTimeMsg	= m_DateTimeMsg;
	m_tmpQueueMsg		= m_QueueMsg;
	m_tmpQueueMsgType	= m_QueueMsgType;
	m_tmpQueueBotGUID	= m_QueueBotGUID;
	m_tmpQueueRuleGUID	= m_QueueRuleGUID;

	m_DateTimeMsg.Clear();
	m_QueueMsg.Clear();
	m_QueueMsgType.Clear();
	m_QueueBotGUID.Clear();
	m_QueueRuleGUID.Clear();

	SaveToDB();

	m_tmpDateTimeMsg.Clear();
	m_tmpQueueMsg.Clear();
	m_tmpQueueMsgType.Clear();
	m_tmpQueueBotGUID.Clear();
	m_tmpQueueRuleGUID.Clear();
}
//**************************************************
void CError::SetError(bool Error = false, wxString ErrorMsg = wxEmptyString, 
					  eErrorType ErrorType = eErrorType_APP, bool FatalError = false)
{
	m_Error		 = Error;
	m_FatalError = FatalError;
	m_ErrorMsg	 = ErrorMsg;
	m_ErrorType	 = ErrorType;
}
//**************************************************
int CError::IsError()
{
	if(m_Error)
	{//Ошибка есть
		if(m_FatalError)
		{//Ошибка фатальная
			wxLogFatalError(wxT("%1$s - %2$s"), m_ErrorMsgEnum.Item(m_ErrorType), m_ErrorMsg);
			return 2;
		}
		else
		{
			wxLogError(wxT("%1$s - %2$s"), m_ErrorMsgEnum.Item(m_ErrorType), m_ErrorMsg);
			return 1;
		}
	}
	return 0;
}
//**************************************************
void CError::AddQueue(wxString msgTxt, eMsgType MsgType = eMsgType_Default, 
					  wxString botGuid = wxEmptyString, wxString ruleGuid = wxEmptyString)
{
	wxGetApp().GetErrorMutex()->Lock();
	wxString timstmp(wxDateTime::Now().FormatDate() + wxT(" - ") + wxDateTime::Now().FormatTime());
	m_DateTimeMsg.Add(timstmp);
	m_QueueMsg.Add(msgTxt);
	m_QueueMsgType.Add(MsgType);
	m_QueueBotGUID.Add(botGuid);
	m_QueueRuleGUID.Add(ruleGuid);

	switch(MsgType)
	{
	case eMsgType_Default:
		if(wxGetApp().GetTextFrame())
		{
			wxGetApp().GetTextFrame()->SetDefaultStyle(wxTextAttr(*wxBLACK));
			wxGetApp().GetTextFrame()->AppendText(timstmp + wxT(" - ") + msgTxt);
			wxGetApp().GetTextFrame()->AppendText(wxT("\n"));
		}
		break;
	case eMsgType_NoTxtOutput:
		break;
	case eMsgType_AppErr:
		break;
	case eMsgType_RuleMsg:
		break;
	case eMsgType_RuleErr:
		break;
	case eMsgType_BotMsg:
		if(wxGetApp().GetTextFrame())
		{
			wxGetApp().GetTextFrame()->SetDefaultStyle(wxTextAttr(*wxBLACK));
			wxGetApp().GetTextFrame()->AppendText(timstmp + wxT(" - ") + msgTxt);
			wxGetApp().GetTextFrame()->AppendText(wxT("\n"));
		}
		break;
	case eMsgType_BotErr:
		if(wxGetApp().GetTextFrame())
		{
			wxGetApp().GetTextFrame()->SetDefaultStyle(wxTextAttr(*wxRED));
			wxGetApp().GetTextFrame()->AppendText(timstmp + wxT(" - ") + msgTxt);
			wxGetApp().GetTextFrame()->AppendText(wxT("\n"));
		}
		break;
	}
	wxGetApp().GetErrorMutex()->Unlock();

	wxGetApp().GetErrorMutex()->Lock();
	if(m_QueueMsg.Count() > 100)
	{
		m_tmpDateTimeMsg	= m_DateTimeMsg;
		m_tmpQueueMsg		= m_QueueMsg;
		m_tmpQueueMsgType	= m_QueueMsgType;
		m_tmpQueueBotGUID	= m_QueueBotGUID;
		m_tmpQueueRuleGUID	= m_QueueRuleGUID;

		m_DateTimeMsg.Clear();
		m_QueueMsg.Clear();
		m_QueueMsgType.Clear();
		m_QueueBotGUID.Clear();
		m_QueueRuleGUID.Clear();

		if(m_SaveLogToBase)
			SaveToDB();

		m_tmpDateTimeMsg.Clear();
		m_tmpQueueMsg.Clear();
		m_tmpQueueMsgType.Clear();
		m_tmpQueueBotGUID.Clear();
		m_tmpQueueRuleGUID.Clear();
	}
	wxGetApp().GetErrorMutex()->Unlock();
}
//**************************************************
void CError::SaveToDB()
{
	CAppProperties *app = wxGetApp().GetAppProperties();
	SqliteDatabaseLayer *DB = new SqliteDatabaseLayer();
	PreparedStatement *pStatement(NULL);
	wxString strLogDb = wxT("log.db3");
	bool bCreate = !wxFileExists(strLogDb);
	if(bCreate)
	{
		try
		{
			DB->Open(strLogDb);
		}
		catch(DatabaseLayerException & e)
		{
			wxDELETE(DB);
			SetError(true, e.GetErrorMessage(), wxGetApp().GetError()->eErrorType_DB_Open, false);
			wxGetApp().GetErrorMutex()->Unlock();
			return;
		}

		try
		{
			DB->RunQuery(wxT("CREATE TABLE _Message(_DateTime varchar(30), _Message varchar(250), _MessageType integer,\
							 _BotGUID varchar(36), _RuleGUID varchar(36));"), true);
		}
		catch(DatabaseLayerException & e) { wxUnusedVar(e); }
	}
	else
	{
		try
		{
			DB->Open(strLogDb);
		}
		catch(DatabaseLayerException & e)
		{
			wxDELETE(DB);
			SetError(true, e.GetErrorMessage(), wxGetApp().GetError()->eErrorType_DB_Open, false);
			wxGetApp().GetErrorMutex()->Unlock();
			return;
		}
	}
	try
	{
		DB->BeginTransaction();

		for(size_t i = 0; i < m_tmpDateTimeMsg.Count(); i++)
		{
			pStatement = DB->PrepareStatement(
				wxT("INSERT INTO _Message(_DateTime, _Message, _MessageType, _BotGUID, _RuleGUID) VALUES (?, ?, ?, ?, ?);"));
			if (pStatement)
			{
				pStatement->SetParamString(1, m_tmpDateTimeMsg.Item(i));
				pStatement->SetParamString(2, m_tmpQueueMsg.Item(i));
				pStatement->SetParamInt(3, m_tmpQueueMsgType.Item(i));
				pStatement->SetParamString(4, m_tmpQueueBotGUID.Item(i));
				pStatement->SetParamString(5, m_tmpQueueRuleGUID.Item(i));
				pStatement->RunQuery();
				DB->CloseStatement(pStatement);
				pStatement = NULL;
			}
		}
	}
	catch(DatabaseLayerException & e)
	{
		wxUnusedVar(e);
		if(pStatement)
		{
			DB->CloseStatement(pStatement);
			pStatement = NULL;
		}
		DB->RollBack();
		wxDELETE(DB);
		wxGetApp().GetErrorMutex()->Unlock();
		return;
	}
	DB->Commit();
	DB->ExecuteQuery(wxT("vacuum;"));
	DB->Close();
	wxDELETE(DB);
}
//**************************************************
//Реализация класса CAppProperties
CAppProperties::CAppProperties(wxString path)
{
	m_PropertiesDB = new SqliteDatabaseLayer();
	bool bCreate = !wxFileExists(path);
	if(bCreate)
	{
		wxGetApp().GetError()->SetError(true, wxT("Database does not exist!"), wxGetApp().GetError()->eErrorType_DB_Open, true);
		return;
	}
	try
	{
		m_PropertiesDB->Open(path);
	}
	catch(DatabaseLayerException & e)
	{
		wxGetApp().GetError()->SetError(true, e.GetErrorMessage(), wxGetApp().GetError()->eErrorType_DB_Open, true);
		return;
	}

	wxInitAllImageHandlers();

	m_ArrayIcon.Create(16,16,true,icr_max);
	m_ArrayIcon32.Create(32,32,true,icr32_max);
}
//**************************************************
CAppProperties::~CAppProperties()
{
	m_ComponentList.DeleteContents(true);
	m_ComponentList.Clear();
	m_ArrayLanguage.Clear();
	m_ArrayLngRes.Clear();
	m_ArrayLngId.Clear();
	m_ArrayIcon.RemoveAll();
	m_ArrayIcon32.RemoveAll();
	
	if(m_PropertiesDB)
	{
		if(m_PropertiesDB->IsOpen())
		{
			m_PropertiesDB->Close();
		}
		wxDELETE(m_PropertiesDB);
	}
}
//**************************************************
void CAppProperties::ApplySettings()
{
	HKEY hk;
	//Настройка авто запуска приложения
	wxString exeName = wxGetCwd() + wxT("\\AutoBot.exe");
	wxString Buf(wxT("Software\\Microsoft\\Windows\\CurrentVersion\\Run")); 
	if(!RegOpenKeyEx(HKEY_CURRENT_USER, Buf, 0, KEY_WRITE, &hk))
	{//Открытие ветки реестра
		if(GetRunOnStartOS())
		{//Добавление автозапуска
			RegSetValueEx(hk, wxT("AutoBot"), 0, REG_SZ,
						  (LPBYTE) exeName.wx_str(), (DWORD) (exeName.size()+1)*sizeof(wxChar));
		}
		else
		{//Удаление автозапуска
			RegDeleteValue(hk, wxT("AutoBot"));
		}
		RegCloseKey(hk);
	}

	//Установка в автозапуске ярлыка
	//wxString autorunpath;
	//wxString KeyPath(wxT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders")); 
	//if(!RegOpenKeyEx(HKEY_CURRENT_USER, KeyPath, 0, KEY_READ, &hk))
	//{
	//	DWORD dwType;
	//	DWORD BufferSize;
	//	RegQueryValueEx(hk, wxT("Startup"), NULL, &dwType, NULL, &BufferSize);
	//	if (dwType == REG_EXPAND_SZ)
	//	{
	//		PCTSTR szData = new CHAR[BufferSize+1];
	//		RegQueryValueEx(hk, wxT("Startup"), NULL, NULL, (LPBYTE)szData, &BufferSize);
	//		autorunpath.Append(szData, BufferSize);
	//		delete szData;
	//	}
	//	RegCloseKey(hk);
	//}

	LPITEMIDLIST pidl; 
	wxChar buffer[512];

	SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl);
	if (pidl != NULL)
	{
		SHGetPathFromIDList(pidl, buffer);
	}
	wxString dirDsk(buffer);
	wxString filenames = dirDsk + wxT("\\AutoBot.lnk");

	wxChar autostart[512];
	SHGetSpecialFolderLocation(NULL, CSIDL_STARTUP, &pidl);
	if (pidl != NULL)
	{
		SHGetPathFromIDList(pidl, autostart);
	}
	wxString autostartDir(autostart);
	wxString autostartNames = autostartDir + wxT("\\AutoBot.lnk");
	
	if(GetRunOnStartOS())
	{
		if(!wxFileExists(autostartNames))
		{
			IShellLink *pSL;
			IPersistFile *pPF;
			HRESULT hRes;
			int iIcon = 0;
			// Получение экземпляра компонента "Ярлык" 
			hRes = CoCreateInstance(CLSID_ShellLink, 0, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)&pSL); 
			if(SUCCEEDED(hRes))
			{
				hRes = pSL->SetPath(exeName);
				if(SUCCEEDED(hRes))
				{
					hRes = pSL->SetWorkingDirectory(wxGetCwd());
					if(SUCCEEDED(hRes))
					{
						wxBitmap bmp = GetBitmap32(icr32_appico);
						bmp.SaveFile(wxT("autobot.ico"), wxBITMAP_TYPE_ICO);
						hRes = pSL->SetIconLocation(wxGetCwd()+wxT("\\autobot.ico"), iIcon);
						if(SUCCEEDED(hRes))
						{
							// Получение компонента хранилища параметров 
							hRes = pSL->QueryInterface(IID_IPersistFile,(LPVOID *)&pPF);
							if(SUCCEEDED(hRes))
							{
								USES_CONVERSION;
								wxChar *oChar = (wxChar *)autostartNames.c_str();
								LPOLESTR oleStr = T2OLE(oChar);
								// Сохранение созданного ярлыка
								hRes = pPF->Save(oleStr, TRUE);
								pPF->Release();
							}
						}
					}
				}
				pSL->Release();
			}
		}
	}
	else
	{
		wxRemoveFile(autostartNames);
	}

	if(GetDisplayLabelOnDesktop())
	{
		if(!wxFileExists(filenames))
		{
			IShellLink *pSL;
			IPersistFile *pPF;
			HRESULT hRes;
			int iIcon = 0;
			// Получение экземпляра компонента "Ярлык" 
			hRes = CoCreateInstance(CLSID_ShellLink, 0, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)&pSL); 
			if(SUCCEEDED(hRes))
			{
				hRes = pSL->SetPath(exeName);
				if(SUCCEEDED(hRes))
				{
					hRes = pSL->SetWorkingDirectory(wxGetCwd());
					if(SUCCEEDED(hRes))
					{
						wxBitmap bmp = GetBitmap32(icr32_appico);
						bmp.SaveFile(wxT("autobot.ico"), wxBITMAP_TYPE_ICO);
						hRes = pSL->SetIconLocation(wxGetCwd()+wxT("\\autobot.ico"), iIcon);
						if(SUCCEEDED(hRes))
						{
							// Получение компонента хранилища параметров 
							hRes = pSL->QueryInterface(IID_IPersistFile,(LPVOID *)&pPF);
							if(SUCCEEDED(hRes))
							{
								USES_CONVERSION;
								wxChar *oChar = (wxChar *)filenames.c_str();
								LPOLESTR oleStr = T2OLE(oChar);
								// Сохранение созданного ярлыка
								hRes = pPF->Save(oleStr, TRUE);
								pPF->Release();
							}
						}
					}
				}
				pSL->Release();
			}
		}
	}
	else
	{
		wxRemoveFile(filenames);
	}
}

const xchar code_1[] = 
    xSTRING("\
            myBasic <- CComponent_FileAction.create();  \
            myBasic.Execute();    \
    ")
    ;
//**************************************************
bool CAppProperties::LoadPlugIn()
{
	wxString path;
	wchar_t buf[512];
	*buf = '\0';
	GetModuleFileName(NULL, buf, 511);
	path = buf;

	wxString argv0 = wxTheApp->argv[0];

	if (wxIsAbsolutePath(argv0))
		path = argv0;
	else
	{
		wxPathList pathlist;
		pathlist.AddEnvList(wxT("PATH"));
		path = pathlist.FindAbsoluteValidPath(argv0);
	}

	wxFileName filenames(path);
	filenames.Normalize();
	path = filenames.GetFullPath();

	wxString strDir = wxPathOnly(path) + wxT("\\Plugin");
	wxDir dir(strDir);
	if (!dir.IsOpened())
		return FALSE;
	
	//scan for plugins
	wxString filename;
	wxString ext = wxT("*.plug");
	bool cont = dir.GetFirst(&filename, ext, wxDIR_FILES );
	while( cont )
	{
		/* явным образом проецируем DLL на адресное пространство нашего процесса */
		HMODULE hModule = LoadLibrary(dir.GetName()+wxT("\/")+filename);
		/* проверяем успешность загрузки */
		_ASSERT(hModule != NULL);
		/*
			определяем при помощи typedef новый тип - указатель на вызываемую функцию. 
			Очень важно знать типы и количество аргументов, а также тип возвращаемого 
			результата 
		*/
		PluginStartupInfo plugInfo;
		memset(&plugInfo,0,sizeof(PluginStartupInfo));

		//typedef void (*DLLFunctionPtr) (HWND);
		typedef void (*GetPluginInfoPtr) (struct PluginStartupInfo *);
		//typedef int (*PGetSum)(const int, const int);
		//typedef void (*InitClassPtr) (script::VMCore *);
		/* пытаемся получить адрес функции getSum */
		//PGetSum pGetSum = (PGetSum)GetProcAddress(hModule, "getSum");
		
		GetPluginInfoPtr pProc = (GetPluginInfoPtr)GetProcAddress(hModule, "GetPluginInfo"); 
		/* проверяем успешность получения адреса */
		_ASSERT(pProc != NULL);

		(pProc)(&plugInfo);
		/* выгружаем библиотеку из памяти */
		//bool b = FreeLibrary(hModule);
		/* проверяем корректность выгрузки */
		//_ASSERT(b);
		CComponent *comp = GetComponent(StrToGuid(plugInfo.ComponentGUID));
		if(comp)
		{
			comp->SetLibrary(hModule);
		}
		else
		{
			//delete component
			bool b = FreeLibrary(hModule);
			/* проверяем корректность выгрузки */
			_ASSERT(b);
		}
		cont = dir.GetNext(&filename);
	}
	return true;
}
//**************************************************
bool CAppProperties::LoadAppProperties()
{
	//Опишем переменные для извлекаемых значения БД
	wxString _Name	= wxEmptyString;
	wxString _Path	= wxEmptyString;
	wxString _FileName	= wxEmptyString;
	wxString _XML_Interface = wxEmptyString;
	wxString _XML_Codegeneration = wxEmptyString;
	wxString _SubItem	= wxEmptyString;
	wxString _Values	= wxEmptyString;
	wxString _DefValues	= wxEmptyString;
	int		 _Type		= NULL;
	int		 _State		= NULL;
	
	//Опишем переменные для временных значений
	int		saveImageNum = NULL;
	int		_Id			= NULL;
	int		_Bytes		= NULL;
	int		_WRes		= NULL;
	int		_HRes		= NULL;
	int		_Order		= NULL;
	int		_LanguageId	= NULL;
	bool	tmpBool		= false;
	double	tmpDouble	= NULL;
	wxCharBuffer tmpChar;

	PreparedStatement *pStatement(NULL);
	SqliteResultSet *dbResSet(NULL);

	//Import PLUGIN - Begin
	wchar_t buf[512]; *buf = '\0';
	GetModuleFileName(NULL, buf, 511);
	wxString path(buf);
	wxString argv0 = wxTheApp->argv[0];

	if (wxIsAbsolutePath(argv0))
		path = argv0;
	else
	{
		wxPathList pathlist;
		pathlist.AddEnvList(wxT("PATH"));
		path = pathlist.FindAbsoluteValidPath(argv0);
	}

	wxFileName filenames(path);
	filenames.Normalize();
	path = filenames.GetFullPath();

	wxString strDir = wxPathOnly(path) + wxT("\\Plugin");
	wxDir dir(strDir);
	if (!dir.IsOpened())
		return FALSE;

	SqliteDatabaseLayer *DB = new SqliteDatabaseLayer();
	PreparedStatement *pStatementPropertiesDB(NULL);
	SqliteResultSet *dbResSetPropertiesDB(NULL);

	wxMemoryBuffer *memBufBlobFile(NULL);
	wxMemoryInputStream *memInBlobFile(NULL);
	wxFileOutputStream *file_output(NULL);
	wxMemoryBuffer *memBufBlobXml(NULL);
	wxMemoryBuffer *memBufBlobImage(NULL);
	int _BytesFile = NULL;
	int _BytesXml = NULL;
	int _BytesImage = NULL;

	wxString filename;
	wxString ext = wxT("*.db3");
	bool cont = dir.GetFirst(&filename, ext, wxDIR_FILES );
	while ( cont )
	{
		try
		{
			DB->Open(dir.GetName()+wxT("\/")+filename);
			dbResSet = (SqliteResultSet *)DB->ExecuteQuery(wxT("SELECT * FROM _RegComponents ORDER BY _Order"));
			while(dbResSet->Next())
			{
				memBufBlobFile = new wxMemoryBuffer(NULL);
				memBufBlobXml = new wxMemoryBuffer(NULL);
				memBufBlobImage = new wxMemoryBuffer(NULL);
				_SubItem = dbResSet->GetResultString(1);
				_Order = dbResSet->GetResultInt(2);
				_Name	 = dbResSet->GetResultString(3);
				_State	 = dbResSet->GetResultInt(4);
				_FileName = dbResSet->GetResultString(5);
				dbResSet->GetResultBlob(6, *(memBufBlobFile));
				dbResSet->GetResultBlob(7, *(memBufBlobXml));
				dbResSet->GetResultBlob(10, *(memBufBlobImage));
				_BytesFile	 = memBufBlobFile->GetBufSize();
				if(_BytesFile == 0)
					continue;
				_BytesXml	 = memBufBlobXml->GetBufSize();
				if(_BytesXml == 0)
					continue;
				_BytesImage	 = memBufBlobImage->GetBufSize();
				if(_BytesImage == 0)
					continue;

				pStatementPropertiesDB = GetPropertiesDB()->PrepareStatement(wxT("SELECT * FROM _RegComponents WHERE _Id = ?"));
				if(pStatementPropertiesDB)
				{
					pStatementPropertiesDB->SetParamString(1, _SubItem);
					dbResSetPropertiesDB = (SqliteResultSet *)pStatementPropertiesDB->RunQueryWithResults();
					if(dbResSetPropertiesDB->Next())
					{
						dbResSetPropertiesDB->Close();
						GetPropertiesDB()->CloseStatement(pStatementPropertiesDB);
						pStatementPropertiesDB = NULL;
						GetPropertiesDB()->BeginTransaction();
						pStatementPropertiesDB = GetPropertiesDB()->PrepareStatement(wxT("UPDATE _RegComponents SET _Order=?, _Name=?, _State=?, _Path=?, _XML=?, _Image=? WHERE _Id=?"));
					}
					else
					{
						dbResSetPropertiesDB->Close();
						GetPropertiesDB()->CloseStatement(pStatementPropertiesDB);
						pStatementPropertiesDB = NULL;
						GetPropertiesDB()->BeginTransaction();
						pStatementPropertiesDB = GetPropertiesDB()->PrepareStatement(wxT("INSERT INTO _RegComponents (_Order, _Name, _State, _Path, _XML, _Image, _Id ) VALUES (?, ?, ?, ?, ?, ?, ?)"));
					}
				
					if(pStatementPropertiesDB)
					{
						pStatementPropertiesDB->SetParamInt(1, _Order);
						pStatementPropertiesDB->SetParamString(2, _Name);
						pStatementPropertiesDB->SetParamInt(3, _State);
						pStatementPropertiesDB->SetParamString(4, _FileName);
						pStatementPropertiesDB->SetParamBlob(5, memBufBlobXml->GetData(), _BytesXml-1024);
						pStatementPropertiesDB->SetParamBlob(6, *(memBufBlobImage));
						pStatementPropertiesDB->SetParamString(7, _SubItem);
						pStatementPropertiesDB->RunQuery();
						GetPropertiesDB()->CloseStatement(pStatementPropertiesDB);
						pStatementPropertiesDB = NULL;
						GetPropertiesDB()->Commit();

						memInBlobFile = new wxMemoryInputStream((const void *)memBufBlobFile->GetData(), memBufBlobFile->GetBufSize() - 1024);
						file_output = new wxFileOutputStream(dir.GetName()+wxT("\/")+_FileName);
						file_output->Write(*memInBlobFile);
						file_output->Close();
						memInBlobFile->Reset();
					}
					else
					{
						GetPropertiesDB()->RollBack();
						continue;
					}
				}
				wxDELETE(file_output);
				wxDELETE(memInBlobFile);
				wxDELETE(memBufBlobFile);
				wxDELETE(memBufBlobXml);
				wxDELETE(memBufBlobImage);
			}
			dbResSet->Close();
			dbResSet = NULL;

			GetPropertiesDB()->BeginTransaction();
			dbResSet = (SqliteResultSet *)DB->ExecuteQuery(wxT("SELECT * FROM _ResLanguage"));
			while(dbResSet->Next())
			{
				_SubItem = dbResSet->GetResultString(1);
				_LanguageId = dbResSet->GetResultInt(2);
				_Values = dbResSet->GetResultString(3);
				pStatementPropertiesDB = GetPropertiesDB()->PrepareStatement( wxT("DELETE FROM _ResLanguage WHERE _Id = ? AND _LanguageId = ?") );
				if(pStatementPropertiesDB)
				{
					pStatementPropertiesDB->SetParamString(1, _SubItem);
					pStatementPropertiesDB->SetParamInt(2, _LanguageId);
					pStatementPropertiesDB->RunQuery();
					GetPropertiesDB()->CloseStatement(pStatementPropertiesDB);
					pStatementPropertiesDB = NULL;
				}
				else continue;

				pStatementPropertiesDB = GetPropertiesDB()->PrepareStatement(wxT("INSERT INTO _ResLanguage (_Id, _LanguageId, _Value) VALUES (?, ?, ?)"));
				if(pStatementPropertiesDB)
				{
					pStatementPropertiesDB->SetParamString(1, _SubItem);
					pStatementPropertiesDB->SetParamInt(2, _LanguageId);
					pStatementPropertiesDB->SetParamString(3, _Values);
					pStatementPropertiesDB->RunQuery();
					GetPropertiesDB()->CloseStatement(pStatementPropertiesDB);
					pStatementPropertiesDB = NULL;
				}
			}
			GetPropertiesDB()->Commit();
			dbResSet->Close();
			dbResSet = NULL;
			DB->Close();
		}
		catch(DatabaseLayerException & e)
		{
			dbResSet->Close();
			dbResSet = NULL;
			GetPropertiesDB()->RollBack();
			DB->Close();
			wxGetApp().GetError()->AddMessage(e.GetErrorMessage(), CError::eMsgType_AppErr);
			continue;
		}
		wxRemoveFile(dir.GetName()+wxT("\/")+filename);
		cont = dir.GetNext(&filename);
	}
	wxDELETE(DB);
	//Import PLUGIN - end

	//Load app properties
	try
	{
		//Первый этап "Чтения основных настроек" секция @MainProperties@
		dbResSet = (SqliteResultSet *)GetPropertiesDB()->ExecuteQuery(wxT("SELECT * FROM _AppProperties WHERE _Item = 'MainProperties'"));
		while(dbResSet->Next())
		{
			_SubItem	= dbResSet->GetResultString(2);
			_Type		= dbResSet->GetResultInt(3);
			_Values		= dbResSet->GetResultString(4);
			_DefValues	= dbResSet->GetResultString(5);

			switch(_Type)
			{
			case bt_bool:
				if(_Values == wxT("0")) tmpBool = false; else tmpBool = true;
				break;
			case bt_int:
			case bt_long:
			case bt_float:
				if(!_Values.ToDouble(&tmpDouble)) tmpDouble = NULL;
				break;
			case bt_char:
				tmpChar = _Values.ToAscii();
				break;
			case bt_string:
			default:
				break;
			}

			if(_SubItem == wxT("FirstStart"))
				m_FirstStart = tmpBool;
			else if(_SubItem == wxT("DisplayLabelOnDesktop"))
				m_DisplayLabelOnDesktop = tmpBool;
			else if(_SubItem == wxT("DisplayGroupAppOnStart"))
				m_DisplayGroupAppOnStart = tmpBool;
			else if(_SubItem == wxT("RunOnStartOS"))
				m_RunOnStartOS = tmpBool;
			else if(_SubItem == wxT("MoveInTray"))
				m_MoveInTray = tmpBool;
			else if(_SubItem == wxT("HashPassword"))
				m_HashPassword = _Values;
			else if(_SubItem == wxT("CiteUpdate"))
				m_CiteUpdate = _Values;
			else if(_SubItem == wxT("AllowUpdate"))
				m_AllowUpdate = tmpBool;
			else if(_SubItem == wxT("Language"))
				m_Language = (int)tmpDouble;
			else if(_SubItem == wxT("PaneSettings"))
			{
				if(_Values.size() < 1)
					m_PaneSettings = wxEmptyString;
				else
					m_PaneSettings = _Values;
			}
			else if(_SubItem == wxT("TotalNodes"))
				m_TotalNodes = (long)tmpDouble;
		}
		dbResSet->Close();

		//Второй этап "Чтение списка языков"
		if(m_ArrayLanguage.Count() > 0)
			m_ArrayLanguage.Clear();
		
		dbResSet = (SqliteResultSet *)GetPropertiesDB()->ExecuteQuery(wxT("SELECT * FROM _AppProperties WHERE _Item = 'Language' ORDER BY _Values"));
		while(dbResSet->Next())
		{
			_SubItem	= dbResSet->GetResultString(2);
			m_ArrayLanguage.Add(_SubItem);
		}
		dbResSet->Close();

		//Третий этап "Чтение языковых ресурсов"
		if(m_ArrayLngRes.Count() > 0)
			m_ArrayLngRes.Clear();

		if(m_ArrayLngId.Count() > 0)
			m_ArrayLngId.Clear();

		pStatement = GetPropertiesDB()->PrepareStatement(wxT("SELECT * FROM _ResLanguage WHERE _LanguageId = ?1"));
		if (pStatement)
		{
			pStatement->SetParamInt(1, m_Language);
			dbResSet = (SqliteResultSet *)pStatement->RunQueryWithResults();
			while(dbResSet->Next())
			{
				_SubItem = dbResSet->GetResultString(1);
				m_ArrayLngId.Add(_SubItem);
				_Values = dbResSet->GetResultString(3);
				m_ArrayLngRes.Add(_Values);
			}
			dbResSet->Close();
			GetPropertiesDB()->CloseStatement(pStatement);
			pStatement = NULL;
		}

		//Четвёртый этап "Чтение File ресурсов"
		if(m_ArrayIcon.GetImageCount() > 0)
			m_ArrayIcon.RemoveAll();
		if(m_ArrayIcon32.GetImageCount() > 0)
			m_ArrayIcon32.RemoveAll();

		wxMemoryBuffer *memBuf(NULL);
		wxFileOutputStream *file_output(NULL);
		wxMemoryInputStream *memIn(NULL);
		wxImage *im;
		dbResSet = (SqliteResultSet *)GetPropertiesDB()->ExecuteQuery(wxT("SELECT * FROM _ResFiles ORDER BY _Id"));
		while(dbResSet->Next())
		{
			memBuf = new wxMemoryBuffer(NULL);
			_Id		 = dbResSet->GetResultInt(1);
			_Type	 = dbResSet->GetResultInt(2);
			_WRes	 = dbResSet->GetResultInt(3);
			_HRes	 = dbResSet->GetResultInt(4);
			dbResSet->GetResultBlob(5, *memBuf);
			_Bytes	 = memBuf->GetBufSize();
			if(_Bytes == 0)
				continue;

			memIn = new wxMemoryInputStream((const void *)memBuf->GetData(), _Bytes - 1024);
			switch(_Type)
			{
			case bt_ico:
				{
					file_output = new wxFileOutputStream(wxT("file.ico"));
					file_output->Write(*memIn);
					file_output->Close();
					wxIcon ico(wxT("file.ico"), wxBITMAP_TYPE_ICO, _WRes, _HRes);
					if(!ico.IsOk())
					{
						wxGetApp().GetError()->SetError(true, wxT("ico"), wxGetApp().GetError()->eErrorType_DB_Image, true);
						wxDELETE(memIn);
						wxDELETE(file_output);
						return false;
					}
					
					if((_WRes == 16) && (_HRes == 16))
						m_ArrayIcon.Add(ico);
					else
						m_ArrayIcon32.Add(ico);
					
					wxRemoveFile(wxT("file.ico"));
					file_output->Reset();
					wxDELETE(file_output);
					
					if(_Id == 0)
					{
						memIn->SeekI(0);
						file_output = new wxFileOutputStream(wxT("file.ico"));
						file_output->Write(*memIn);
						file_output->Close();
						file_output->Reset();
						wxDELETE(file_output);

						wxIcon icos(wxT("file.ico"), wxBITMAP_TYPE_ICO, 32, 32);
						if(!icos.IsOk())
						{
							wxGetApp().GetError()->SetError(true, wxT("ico"), wxGetApp().GetError()->eErrorType_DB_Image, true);
							wxDELETE(memIn);
							return false;
						}
						m_ArrayIcon32.Add(icos);
						wxRemoveFile(wxT("file.ico"));
					}
				}
				break;
			case bt_png:
				{
					wxString text;
					text.Printf(wxT("Shape0%d.png"),++saveImageNum);
					im = new wxImage(*memIn, wxBITMAP_TYPE_PNG, -1);
					if(!im->IsOk())
					{
						wxGetApp().GetError()->SetError(true, wxT("png"), wxGetApp().GetError()->eErrorType_DB_Image, true);
						wxDELETE(memIn);
						wxDELETE(im);
						return false;
					}
					wxMemoryFSHandler::AddFile(text, *im, wxBITMAP_TYPE_PNG);
					wxDELETE(im);
				}
				break;
			case bt_xpm:
				{
					file_output = new wxFileOutputStream(wxT("file.xpm"));
					file_output->Write(*memIn);
					file_output->Close();
					wxIcon ico(wxT("file.xpm"), wxBITMAP_TYPE_XPM, _WRes, _HRes);
					if(!ico.IsOk())
					{
						wxGetApp().GetError()->SetError(true, wxT("xpm"), wxGetApp().GetError()->eErrorType_DB_Image, true);
						wxDELETE(file_output);
						wxDELETE(memIn);
						return false;
					}
					m_ArrayIcon.Add(ico);
					wxRemoveFile(wxT("file.xpm"));
					file_output->Reset();
					wxDELETE(file_output);
				}
				break;
			case bt_cur:
				{
					file_output = new wxFileOutputStream(wxT("spy.cur"));
					file_output->Write(*memIn);
					file_output->Close();
					file_output->Reset();
					wxDELETE(file_output);
					m_SpyCur = wxCursor(wxT("spy.cur"), wxBITMAP_TYPE_CUR);
					wxRemoveFile(wxT("spy.cur"));
				}
				break;
			case bt_html:
				{
					wxMemoryFSHandler::AddFile(wxT("wellcome.htm"), (const void *)memBuf->GetData(), _Bytes - 1024);
				}
			default:
				break;
			}
			memIn->Reset();
			wxDELETE(memIn);
			wxDELETE(memBuf);
		}
		dbResSet->Close();

		wxMemoryBuffer *memBufXml(NULL);
		wxMemoryInputStream *memInXml(NULL);
		//Пятый этап чтение зарегистрированных компонент
		dbResSet = (SqliteResultSet *)GetPropertiesDB()->ExecuteQuery(wxT("SELECT * FROM _RegComponents ORDER BY _Order"));
		while(dbResSet->Next())
		{
			memBuf = new wxMemoryBuffer(NULL);
			memBufXml = new wxMemoryBuffer(NULL);
			_SubItem = dbResSet->GetResultString(1);
			_Name	 = dbResSet->GetResultString(3);
			_State	 = dbResSet->GetResultInt(4);
			_Path	 = dbResSet->GetResultString(5);
			dbResSet->GetResultBlob(6, *memBufXml);
			//_XML	 = dbResSet->GetResultString(6);
			_XML_Interface = dbResSet->GetResultString(7);
			_XML_Codegeneration = dbResSet->GetResultString(8);
			dbResSet->GetResultBlob(9, *memBuf);
			_Bytes	 = memBuf->GetBufSize();
			if(_Bytes == 0)
				continue;
			
			CComponent *comp = new CComponent();
			comp->SetGUIDComponent(StrToGuid(_SubItem));
			if(wxT("FB1AEFD9-37F9-446d-B436-C5E7FD2AD0D3") == _SubItem)
				comp->SetComponentType(Trigger);
			else if(wxT("FAD5CEC0-9014-438d-9A5D-1B4E4C3C2A55") == _SubItem)
				comp->SetComponentType(Condition);
			else if(wxT("F45B0A34-82BD-41d4-9DCE-7C459CD6A827") == _SubItem)
				comp->SetComponentType(Cycle);
			else if(wxT("E2ADA6EC-B9F0-4563-A08D-A3E6EFBDF6A3") == _SubItem)
				comp->SetComponentType(Script);
			else
				comp->SetComponentType(Action);
			comp->SetName(_Name);
			comp->SetState(_State);
			comp->SetPath(_Path);
			if(memBufXml->GetBufSize() > 0)
			{
				//Интересная бага, при создании wxMemoryBuffer, резервируется +1024 байта (лишние)
				memInXml = new wxMemoryInputStream((const void *)memBufXml->GetData(), memBufXml->GetBufSize() - 1024);
				comp->GetXMLStruct()->Load(*memInXml);
			}

			memIn = new wxMemoryInputStream((const void *)memBuf->GetData(), _Bytes - 1024);
			im = new wxImage(*memIn, wxBITMAP_TYPE_PNG, -1);
			if(!im->IsOk())
			{
				wxGetApp().GetError()->SetError(true, wxT("png"), wxGetApp().GetError()->eErrorType_DB_Image, true);
				wxDELETE(im);
				wxDELETE(memIn);
				wxDELETE(memBuf);
				return false;
			}
			comp->SetImage(*im);
			GetComponentList().Append(comp);

			memIn->Reset();
			wxDELETE(im);
			wxDELETE(memIn);
			wxDELETE(memBuf);
			wxDELETE(memInXml);
			wxDELETE(memBufXml);
		}
		dbResSet->Close();
		dbResSet = NULL;
	}
	catch(DatabaseLayerException & e)
	{
		pStatement = NULL;
		dbResSet = NULL;
		wxFAIL_MSG(e.GetErrorMessage());
		return false;
	}
	ApplySettings();
	return true;
}
//**************************************************
wxIcon CAppProperties::GetIconFromMFS(wxString imPath, long Type)
{
	wxIcon icon;
	wxFileSystem fs;
	wxFSFile *file = fs.OpenFile(imPath);
	if(file!=NULL)
	{
		wxImage *image = new wxImage(wxBufferedInputStream(*file->GetStream()), Type);
		image->SaveFile(wxT("tmpico.img"), Type);
		icon.LoadFile(wxT("tmpico.img"), Type);
		wxRemoveFile(wxT("tmpico.img"));
		wxDELETE(file);
		wxDELETE(image);
	}
	return icon;
}
//**************************************************
wxImage CAppProperties::GetImageFromMFS(wxString imPath, long Type)
{
	wxImage image;
	wxFileSystem fs;
	wxFSFile *file = fs.OpenFile(imPath);
	if(file!=NULL)
	{
		image = wxImage(wxBufferedInputStream(*file->GetStream()), Type);
		wxDELETE(file);
	}
	return image;
}
//**************************************************
bool CAppProperties::SaveAppProperties()
{
	CAppProperties *ap = wxGetApp().GetAppProperties();
	SqliteDatabaseLayer *DB = (SqliteDatabaseLayer *)GetPropertiesDB();
	PreparedStatement *pStatement(NULL);

	try
	{
		DB->BeginTransaction();
		pStatement = ap->GetPropertiesDB()->PrepareStatement( wxT("UPDATE _AppProperties SET _Values=? WHERE _Item=? AND _SubItem=?") );
		if (pStatement)
		{
			pStatement->SetParamInt(1, (int)m_FirstStart);
			pStatement->SetParamString(2, wxT("MainProperties"));
			pStatement->SetParamString(3, wxT("FirstStart"));
			pStatement->RunQuery();

			pStatement->SetParamInt(1, (int)m_DisplayLabelOnDesktop);
			pStatement->SetParamString(2, wxT("MainProperties"));
			pStatement->SetParamString(3, wxT("DisplayLabelOnDesktop"));
			pStatement->RunQuery();

			pStatement->SetParamInt(1, (int)m_DisplayGroupAppOnStart);
			pStatement->SetParamString(2, wxT("MainProperties"));
			pStatement->SetParamString(3, wxT("DisplayGroupAppOnStart"));
			pStatement->RunQuery();

			pStatement->SetParamInt(1, (int)m_RunOnStartOS);
			pStatement->SetParamString(2, wxT("MainProperties"));
			pStatement->SetParamString(3, wxT("RunOnStartOS"));
			pStatement->RunQuery();

			pStatement->SetParamInt(1, (int)m_MoveInTray);
			pStatement->SetParamString(2, wxT("MainProperties"));
			pStatement->SetParamString(3, wxT("MoveInTray"));
			pStatement->RunQuery();

			pStatement->SetParamString(1, m_HashPassword);
			pStatement->SetParamString(2, wxT("MainProperties"));
			pStatement->SetParamString(3, wxT("HashPassword"));
			pStatement->RunQuery();

			pStatement->SetParamInt(1, m_AllowUpdate);
			pStatement->SetParamString(2, wxT("MainProperties"));
			pStatement->SetParamString(3, wxT("AllowUpdate"));
			pStatement->RunQuery();

			pStatement->SetParamString(1, m_CiteUpdate);
			pStatement->SetParamString(2, wxT("MainProperties"));
			pStatement->SetParamString(3, wxT("CiteUpdate"));
			pStatement->RunQuery();

			pStatement->SetParamInt(1, m_Language);
			pStatement->SetParamString(2, wxT("MainProperties"));
			pStatement->SetParamString(3, wxT("Language"));
			pStatement->RunQuery();

			pStatement->SetParamString(1, m_PaneSettings);
			pStatement->SetParamString(2, wxT("MainProperties"));
			pStatement->SetParamString(3, wxT("PaneSettings"));
			pStatement->RunQuery();

			pStatement->SetParamDouble(1, m_TotalNodes);
			pStatement->SetParamString(2, wxT("MainProperties"));
			pStatement->SetParamString(3, wxT("TotalNodes"));
			pStatement->RunQuery();

			DB->CloseStatement(pStatement);
			pStatement = NULL;
		}
		else
		{
			DB->RollBack();
			return false;
		}
		DB->Commit();
	}
	catch(DatabaseLayerException & e)
	{
		DB->RollBack();
		pStatement = NULL;
		wxFAIL_MSG(e.GetErrorMessage());
		return false;
	}
	//ApplySettings();
	return true;
}
//**************************************************
GUID CAppProperties::StrToGuid(wxString strGUID)
{
	RPC_WSTR GuidStringNode = (RPC_WSTR)strGUID.wc_str();
	GUID guid;
	if(UuidFromString(GuidStringNode, &guid) != RPC_S_OK)
		UuidCreateNil(&guid);
	
	return guid;
}
//**************************************************
wxString CAppProperties::GuidToStr(GUID guid)
{
	RPC_WSTR GuidStringNode = NULL;
	UuidToString(&guid, &GuidStringNode);
	return wxString((wxChar *)GuidStringNode);
}
//**************************************************
void CAppProperties::SetPassword(wxString password) 
{
	if(password == wxEmptyString)
	{
		SetHashPassword(wxEmptyString);
		return;
	}
	wxString StringForHash = password;
	byte abDigest[MD5::DIGESTSIZE];
	MD5().CalculateDigest(abDigest, (const byte *)StringForHash.c_str(), StringForHash.Len() * sizeof(wchar_t));
	wxString hash;
	for(int i = 0; i < MD5::DIGESTSIZE; i++)
		hash += wxString::Format(_T("%2.2x"), abDigest[i]);
	SetHashPassword(hash);
}
//**************************************************
wxString CAppProperties::GetLngRes(wxString idLngRes)
{
	for(size_t i = 0; i < m_ArrayLngId.Count(); i++)
	{
		if(m_ArrayLngId.Item(i) == idLngRes)
			return m_ArrayLngRes.Item(i); 
	}
	return wxEmptyString;
}
//**************************************************
CComponent *CAppProperties::GetComponent(GUID guid)
{
	wxList::compatibility_iterator node = GetComponentList().GetFirst();
	while (node)
	{
		CComponent *comp = (CComponent *)node->GetData();
		if(comp->GetGUIDComponent() == guid)
		{
			return comp;
		}
		node = node->GetNext();
	}
	return (CComponent *)NULL;
}
//**************************************************
bool CAppProperties::OnProcess(EvtType evt = Event_Default, wxObject* object = (wxObject*)NULL, wxEvent* event = (wxEvent *)NULL)
{
	CTreeFrame *treeframe = (CTreeFrame *)NULL;
	wxTreeItemId selectItemId;
	TreeItemData *selectItem = (TreeItemData*)NULL;

	switch(evt)
	{
	case Event_AddTreeItem:
	case Event_AddTreeSubItem:
	case Event_SelectTreeNode:
	case Event_DeleteTreeItem:
	case Event_BeginDragTreeItem:
	case Event_EndDragTreeItem:
	case Event_StartItem:
	case Event_StopItem:
	case Event_PauseItem:
		{
			treeframe = wxGetApp().GetTreeFrame();
			if(!treeframe)
				return false;

			selectItemId	= treeframe->GetSelection();
			if(!selectItemId)
				return false;

			selectItem		= (TreeItemData *)treeframe->GetItemData(selectItemId);
			if(!selectItem)
				return false;
		}
	}

	switch(evt)
	{
	case Event_SelectRule:
		{
			CRule *rule = (CRule *)object;
			wxGetApp().GetTreeBot()->SetActiveRule(rule);
			wxGetApp().GetPropertyFrame()->RefreshPropertyGrid(Rule);
			if(rule->GetComponent()->GetComponentType() == Script)
			{
				wxGetApp().GetScintillaFrame()->ClearAll();
				//wxGetApp().GetScintillaFrame()->AddText(myRule->GetScriptAction()->GetScript());
				wxGetApp().GetScintillaFrame()->SetText(rule->GetValue(wxT("m4")));
				wxGetApp().GetScintillaFrame()->Enable();
			}
		}
		break;
	case Event_AddTreeItem:
		{
			if(!selectItem->isNode())
				selectItemId = treeframe->GetItemParent(selectItemId);

			CNodeBot *nodeBot  = wxGetApp().GetTreeBot()->GetActiveNode();
			if(!nodeBot)
				return false;

			if(wxGetApp().GetTreeBot()->GetLevelNode(nodeBot) == 0)
				return false;

			if(wxGetApp().GetTreeBot()->GetActiveBot())
			{
				wxGetApp().GetTreeBot()->GetActiveBot()->SetDebugState(CBot::Debug_No);
				wxGetApp().GetTreeBot()->SetDebugBot((CBot *)NULL);
			}

			GUID itemGUID;
			int iNumBot;
			CBot *newBot = nodeBot->AddBot(GetLngRes(wxT("lr_Bot")) + wxT(" #"), &itemGUID, &iNumBot);
			if (newBot)
			{
				wxString text;
				text.Printf(wxT("%s #%d"), GetLngRes(wxT("lr_Bot")), iNumBot);
				treeframe->AppendItem(selectItemId, text, TreeFrameIcon_File, TreeFrameIcon_FileSelected, new TreeItemData(itemGUID, false));

				wxGetApp().GetError()->AddMessage(wxString::Format(wxT("%s - %s"), GetLngRes(wxT("lr_BotCreateSuccess")), GuidToStr(itemGUID)), CError::eMsgType_Default, GuidToStr(itemGUID));
				treeframe->Refresh();
			}
		}
		break;
	case Event_AddTreeSubItem:
		{
			if(!selectItem->isNode())
				selectItemId = treeframe->GetItemParent(selectItemId);

			CNodeBot *nodeBot  = wxGetApp().GetTreeBot()->GetActiveNode();
			if(!nodeBot)
				return false;

			if(wxGetApp().GetTreeBot()->GetActiveBot())
			{
				wxGetApp().GetTreeBot()->GetActiveBot()->SetDebugState(CBot::Debug_No);
				wxGetApp().GetTreeBot()->SetDebugBot((CBot *)NULL);
			}

			int lvl = wxGetApp().GetTreeBot()->GetLevelNode(nodeBot);
			if((lvl == 2) || (lvl == -1))
				return false;
			
			CNodeBot *newNodeBot = new CNodeBot();
			if(!newNodeBot)
				return false;
			
			if(lvl == 0)
				newNodeBot->SetNodeType(eNodeType_FolderUnderRoot);
			else
				newNodeBot->SetNodeType(eNodeType_Folder);
			nodeBot->GetListNode().Append(newNodeBot);
			
			wxString text;	text.Printf(wxT("%s #%d"), GetLngRes(wxT("lr_Node")), nodeBot->GetListNode().GetCount());
			newNodeBot->SetNodeName(text);

			treeframe->AppendItem(selectItemId, wxString::Format(wxT("%s #%d"), GetLngRes(wxT("lr_Node")), nodeBot->GetListNode().GetCount()), 
						TreeFrameIcon_Folder, TreeFrameIcon_FolderSelected, 
						new TreeItemData(newNodeBot->GetGUIDNode(), true));

			wxGetApp().GetError()->AddMessage(wxString::Format(wxT("%s - %s"), GetLngRes(wxT("lr_SucsNodeCreate")), GuidToStr(newNodeBot->GetGUIDNode())));
			treeframe->Refresh();
		}
		break;
	case Event_SelectTreeNode:
		{
			CNodeBot *nodeBot = wxGetApp().GetTreeBot()->GetNode(selectItem->GetGUID());
			if(!nodeBot)
				return false;
			
			if(wxGetApp().GetTreeBot()->GetActiveBot())
			{
				wxGetApp().GetTreeBot()->GetActiveBot()->SetDebugState(CBot::Debug_No);
				wxGetApp().GetTreeBot()->SetDebugBot((CBot *)NULL);
				wxGetApp().GetFrameCanvas()->SaveCanvasToDoc(wxGetApp().GetTreeBot()->GetActiveBot()->GetShapeDiagram());
			}

			if(wxGetApp().GetFrameCanvas()->GetDiagramManager())
			{
				wxGetApp().GetFrameCanvas()->GetDiagramManager()->Clear();
				wxGetApp().GetFrameCanvas()->ClearCanvasHistory();
				wxGetApp().GetFrameCanvas()->Disable();
			}

			wxGetApp().GetTreeBot()->SetActiveNode(nodeBot);
			if(selectItem->isNode())
			{
				wxGetApp().GetPropertyFrame()->RefreshPropertyGrid(BotNode);
				wxGetApp().GetTreeBot()->UnactiveBot();
			}
			else
			{
				CBot *bot = nodeBot->GetBot(selectItem->GetGUID());
				if(bot)
				{
					if(!bot->IsChangedBot())
					{
						wxBusyInfo info(GetLngRes(wxT("lr_WaitWhileLoadBot")), NULL);
						wxGetApp().GetTreeBot()->GetActiveNode()->LoadBot(bot->GetGUIDBot(), bot);
					}
					wxGetApp().GetFrameCanvas()->Enable();
					wxGetApp().GetTreeBot()->SetActiveBot(bot);
					wxGetApp().GetPropertyFrame()->RefreshPropertyGrid(Bot);
					wxGetApp().GetFrameCanvas()->LoadCanvasFromDoc(wxGetApp().GetTreeBot()->GetActiveBot()->GetShapeDiagram());
					wxGetApp().GetGridFrame()->RefreshTable();
				}
			}
			wxGetApp().GetFrameCanvas()->Refresh();
		}
		break;
	case Event_DeleteTreeItem:
		{
			if(selectItem->isNode())
			{
				CNodeBot *nodeBot = wxGetApp().GetTreeBot()->GetActiveNode();
				if(!nodeBot)
					return false;

				wxString name(nodeBot->GetNodeName());
				//Это узел ботов
				int res = wxMessageBox(wxString::Format(wxT("%s - %s?"), GetLngRes(wxT("lr_YouReallyClearNode")), name),
										wxT("wxAUI"), wxYES_NO);
				if (res != wxYES)
					return false;

				wxGetApp().GetTreeBot()->SetActiveNode(wxGetApp().GetTreeBot()->GetRootNode());
				wxGetApp().GetTreeBot()->UnactiveBot();
				wxGetApp().GetTreeBot()->UnActiveRule();

				if(nodeBot != wxGetApp().GetTreeBot()->GetRootNode())
				{
					nodeBot->ClearNode();
					treeframe->DeleteChildren(selectItemId);

					wxGetApp().GetError()->AddMessage(wxString::Format(wxT("%s - %s"), GetLngRes(wxT("lr_SucsNodeClear")), name));
				}
				
				if(wxGetApp().GetTreeBot()->CanChangeTree(selectItem->GetGUID()))
				{
					CNodeBot *parentNodebot = wxGetApp().GetTreeBot()->GetParentNode(nodeBot->GetGUIDNode());
					if(!parentNodebot)
						return false;
					parentNodebot->GetListNode().DeleteContents(true);
					parentNodebot->GetListNode().DeleteObject(nodeBot);
					treeframe->Delete(selectItemId);
				}
				wxGetApp().GetPropertyFrame()->RefreshPropertyGrid(Bot);
				treeframe->Refresh();
			}
			else
			{
				CBot *bot = wxGetApp().GetTreeBot()->GetActiveBot();
				if(!bot)
					return false;
				bot->SetChangedBot(true);
				
				int res = wxMessageBox(wxString::Format(wxT("%s - %s?"), GetLngRes(wxT("lr_YouReallyDeleteBot")), bot->GetNameBot()),
										wxT("wxAUI"), wxYES_NO);
				if (res != wxYES)
					return false;
				
				wxString guid = wxGetApp().GetAppProperties()->GuidToStr(bot->GetGUIDBot());
				if(wxGetApp().GetTreeBot()->GetActiveNode()->DeleteBot(bot->GetGUIDBot()))
				{
					wxGetApp().GetError()->AddMessage(wxString::Format(wxT("%s - %s"), GetLngRes(wxT("lr_SucsBotDelete")), guid));
					wxGetApp().GetTreeBot()->UnactiveBot();
					wxGetApp().GetTreeBot()->UnActiveRule();
					treeframe->Delete(selectItemId);
					wxGetApp().GetPropertyFrame()->RefreshPropertyGrid(Bot);
					treeframe->Refresh();
				}
			}
		}
		break;
	case Event_BeginDragTreeItem:
		{
			if(!selectItem->isNode())
			{
				treeframe->SetDraggedItem(selectItemId);
				((wxTreeEvent*)event)->Allow();
			}
			else
			{
				((wxTreeEvent*)event)->Veto(); 
				return false;
			}
		}
		break;
	case Event_EndDragTreeItem:
		{
			wxTreeItemId itemSrc = treeframe->GetDraggedItem(),
						 itemDst = ((wxTreeEvent*)event)->GetItem();
			treeframe->SetDraggedItem((wxTreeItemId)NULL);
			
			if(treeframe->GetRootItem() == itemDst)
				return false;

			TreeItemData *datDst = (TreeItemData *)treeframe->GetItemData(itemDst);
			if(!datDst->isNode())
				return false;

			if(!itemDst.IsOk())
			{
				return false;
			}

			TreeItemData *datParentSrc = (TreeItemData *)treeframe->GetItemData(treeframe->GetItemParent(itemSrc));
			TreeItemData *datParentDst = (TreeItemData *)treeframe->GetItemData(itemDst);
			TreeItemData *datItem = (TreeItemData *)treeframe->GetItemData(itemSrc);
			CBot *bot = wxGetApp().GetTreeBot()->CopyBot(datParentSrc->GetGUID(), datParentDst->GetGUID(), datItem->GetGUID());
			if(bot == NULL)
				return false;
			bot->SetChangedBot(true);
			treeframe->AppendItem(itemDst, bot->GetNameBot(), TreeFrameIcon_File, TreeFrameIcon_FileSelected, new TreeItemData(bot->GetGUIDBot(), false));
			
			CNodeBot *nodebot = wxGetApp().GetTreeBot()->GetNode(datParentSrc->GetGUID());
			if(nodebot)
			{
				if(nodebot->DeleteBot(datItem->GetGUID()))
				{
					CNodeBot *activeNode = wxGetApp().GetTreeBot()->GetNode(datParentDst->GetGUID());
					if(activeNode)
					{
						wxGetApp().GetTreeBot()->SetActiveNode(activeNode);
						wxGetApp().GetTreeBot()->SetActiveBot(bot);
						wxGetApp().GetTreeBot()->UnActiveRule();
					}
					else
					{
						wxGetApp().GetTreeBot()->SetActiveNode((CNodeBot*)NULL);
						wxGetApp().GetTreeBot()->UnactiveBot();
						wxGetApp().GetTreeBot()->UnActiveRule();
					}
					treeframe->Delete(itemSrc);
					treeframe->SelectItem(itemDst);
					wxGetApp().GetPropertyFrame()->RefreshPropertyGrid(Bot);
					treeframe->Refresh();
				}
			}
		}
		break;
	case Event_StartItem:
		{
			if(selectItemId == treeframe->GetRootItem()) return false;

			wxGetApp().GetTaskBarIcon()->SetIcon(GetIco(icr_appico));
			wxTimer *timer = wxGetApp().GetTimer();
			if(timer)
				timer->Stop();

			if(wxGetApp().GetTreeBot()->GetActiveBot())
			{
				wxGetApp().GetTreeBot()->GetActiveBot()->SetDebugState(CBot::Debug_No);
				wxGetApp().GetTreeBot()->SetDebugBot((CBot *)NULL);
				wxGetApp().GetFrameCanvas()->SaveCanvasToDoc(wxGetApp().GetTreeBot()->GetActiveBot()->GetShapeDiagram());
			}

			if(wxGetApp().GetTreeBot()->GetActiveNode()->GetGUIDNode() == selectItem->GetGUID())
			{
				wxGetApp().GetTreeBot()->StartAllBot(true);
			}
			else if(wxGetApp().GetTreeBot()->StartBot(selectItem->GetGUID()))
			{
				treeframe->Refresh();
				wxGetApp().GetGridFrame()->RefreshTable();
			}
		}
		break;
	case Event_StopItem:
		{
			if(selectItemId == treeframe->GetRootItem()) return false;

			wxGetApp().GetTaskBarIcon()->SetIcon(GetIco(icr_appico));
			wxTimer *timer = wxGetApp().GetTimer();
			if(timer)
				timer->Stop();
			if(wxGetApp().GetTreeBot()->GetActiveBot())
			{
				wxGetApp().GetTreeBot()->GetActiveBot()->SetDebugState(CBot::Debug_No);
				wxGetApp().GetTreeBot()->SetDebugBot((CBot *)NULL);
			}

			if(wxGetApp().GetTreeBot()->GetActiveNode()->GetGUIDNode() == selectItem->GetGUID())
			{
				wxGetApp().GetTreeBot()->StopAllBot(true);
			}
			else if(wxGetApp().GetTreeBot()->StopBot(selectItem->GetGUID()))
			{
				treeframe->Refresh();
				wxGetApp().GetGridFrame()->RefreshTable();
			}
		}
		break;
	case Event_PauseItem:
		{
			if(selectItemId == treeframe->GetRootItem()) return false;

			wxGetApp().GetTaskBarIcon()->SetIcon(GetIco(icr_appico));
			wxTimer *timer = wxGetApp().GetTimer();
			if(timer)
				timer->Stop();

			if(wxGetApp().GetTreeBot()->GetActiveNode()->GetGUIDNode() == selectItem->GetGUID())
			{
				wxGetApp().GetTreeBot()->StopAllBot();
			}
			else if(wxGetApp().GetTreeBot()->PauseBot(selectItem->GetGUID()))
			{
				treeframe->Refresh();
			}
		}
		break;
	case Event_AddRule:
		{
			CBot* Bot = wxGetApp().GetTreeBot()->GetActiveBot();
			if(!Bot) 
				return false;
			
			int selComponent = -1;
			for(int id = 0; id < wxGetApp().GetListFrame()->GetItemCount(); id++)
			{
				if(wxGetApp().GetListFrame()->GetItemState(id, wxLIST_STATE_SELECTED) & wxLIST_STATE_SELECTED)
				{
					selComponent = id;
					break;
				}
			}

			if(selComponent == -1)
				return false;
			
			CComponent *comp = (CComponent *)wxGetApp().GetAppProperties()->GetComponentList().Item(selComponent)->GetData();
			if(!comp)
				return false;
			GUID itemGUID;
			if(Bot->AddRule(comp->GetGUIDComponent(), &itemGUID))
			{
				Bot->SetChangedBot(true);
				wxGetApp().GetTreeBot()->SetActiveRule(Bot->GetRule(itemGUID));
				wxGetApp().GetPropertyFrame()->RefreshPropertyGrid(Rule);
			}
			else
				return false;
		}
		break;
	case Event_UnSelectRule:
		{
			wxGetApp().GetTreeBot()->UnActiveRule();
			wxGetApp().GetPropertyFrame()->RefreshPropertyGrid(Bot);
		}
		break;
	case Event_DeleteRule:
		{
			wxString guid;
			if(object->IsKindOf(CLASSINFO(wxSFIfShape)))
				guid = ((wxSFIfShape *)object)->GetShapeGUID();
			else if(object->IsKindOf(CLASSINFO(wxSFTrigger)))
				guid = ((wxSFTrigger *)object)->GetShapeGUID();
			else if(object->IsKindOf(CLASSINFO(wxSFComponent)))
				guid = ((wxSFComponent *)object)->GetShapeGUID();
			else if(object->IsKindOf(CLASSINFO(wxSFCycle)))
				guid = ((wxSFCycle *)object)->GetShapeGUID();
			if(!guid.IsEmpty())
			{
				CBot *bot = wxGetApp().GetTreeBot()->GetActiveBot();
				if(bot)
				{
					bot->SetChangedBot(true);
					return bot->DeleteRule(wxGetApp().GetAppProperties()->StrToGuid(guid));
				}
			}
		}
		break;
	case Event_DebugBot:
		{
			wxBusyInfo info(wxT("..."), NULL);
			wxGetApp().GetTreeBot()->DebugBot();
			wxGetApp().GetGridFrame()->RefreshTable();
			wxGetApp().GetFrameCanvas()->Refresh();
		}
		break;
	default:
		break;
	}

	return true;
}
//**************************************************
//Реализация класса CTreeBot
CTreeBot::CTreeBot()
{
	m_isCut = false;
	m_ActiveNode = (CNodeBot *)NULL;
	m_ActiveBot	 = (CBot *)NULL;
	m_ActiveRule = (CRule *)NULL;
	m_DebugBot	 = (CBot *)NULL;
}
//**************************************************
CTreeBot::~CTreeBot()
{
	StopAllBot();
	m_ActiveNode = (CNodeBot *)NULL;
	m_ActiveBot	 = (CBot *)NULL;
	m_ActiveRule = (CRule *)NULL;
	m_ListClipboardRule.DeleteContents(m_isCut);
	m_ListClipboardRule.Clear();
}
//**************************************************
bool CTreeBot::CanChangeTree(GUID guid)
{
	CNodeBot *nodeBot = GetNode(guid);
	if(GetNode(guid) == NULL)
		return true;
	else
		if( (nodeBot->GetNodeType() == eNodeType_Root) ||
			(nodeBot->GetNodeType() == eNodeType_Main) ||
			(nodeBot->GetNodeType() == eNodeType_Import) ||
			(nodeBot->GetNodeType() == eNodeType_Templates))
			return false;
		else
			return true;
}
//**************************************************
CNodeBot *CTreeBot::GetActiveNode()
{
	if(m_ActiveNode != NULL)
		return m_ActiveNode;
	else
		return (CNodeBot *)NULL;
}
//**************************************************
// Class: CTreeBot
// Name: GetNode
// Dscr: Метод для поиска узла дерева по GUID
CNodeBot *CTreeBot::GetNode(GUID guid)
{
	if(GetRootNode() == NULL)
		return NULL;
	if(GetRootNode()->GetGUIDNode() == guid)
		return GetRootNode();
	wxList::compatibility_iterator nodeRoot;
	//Пробежимся по ботам корня дерева
	if(GetRootNode()->GetListBot().GetCount() > 0)
	{
		nodeRoot = GetRootNode()->GetListBot().GetFirst();
		while (nodeRoot)
		{
			CBot *bot = (CBot *)nodeRoot->GetData();
			if(bot->GetGUIDBot() == guid)
			{
				return GetRootNode();
			}
			nodeRoot = nodeRoot->GetNext();
		}
	}
	if(GetRootNode()->GetListNode().GetCount() > 0)
	{
		//Пробежимся по узлам корня дерева
		wxList::compatibility_iterator nodeLevel1 = GetRootNode()->GetListNode().GetFirst();
		while (nodeLevel1)
		{
			//Посмотрим на соответсвие узлы корня дерева
			CNodeBot *nodebot = (CNodeBot *)nodeLevel1->GetData();
			if(nodebot->GetGUIDNode() == guid)
			{
				return nodebot;
			}

			wxList::compatibility_iterator BotNodeLevel1 = nodebot->GetListBot().GetFirst();
			while(BotNodeLevel1)
			{
				CBot *botlevel1 = (CBot *)BotNodeLevel1->GetData();
				if(botlevel1->GetGUIDBot() == guid)
				{
					return nodebot;
				}
				BotNodeLevel1 = BotNodeLevel1->GetNext();
			}

			wxList::compatibility_iterator nodeLevel2 = nodebot->GetListNode().GetFirst();
			while(nodeLevel2)
			{
				CNodeBot *nodebot2 = (CNodeBot *)nodeLevel2->GetData();
				if(nodebot2->GetGUIDNode() == guid)
				{
					return nodebot2;
				}

				wxList::compatibility_iterator BotNodeLevel2 = nodebot2->GetListBot().GetFirst();
				while(BotNodeLevel2)
				{
					CBot *botlevel2 = (CBot *)BotNodeLevel2->GetData();
					if(botlevel2->GetGUIDBot() == guid)
					{
						return nodebot2;
					}
					BotNodeLevel2 = BotNodeLevel2->GetNext();
				}
				nodeLevel2 = nodeLevel2->GetNext();
			}
			nodeLevel1 = nodeLevel1->GetNext();
		}
	}
	return NULL;
}
//**************************************************
// Class: CTreeBot
// Name: GetBot
// Dscr: Метод для поиска узла дерева по GUID
CBot *CTreeBot::GetBot(GUID guid)
{
	if(GetRootNode() == NULL)
		return (CBot*)NULL;
	wxList::compatibility_iterator nodeRoot;
	//Пробежимся по ботам корня дерева
	if(GetRootNode()->GetListBot().GetCount() > 0)
	{
		nodeRoot = GetRootNode()->GetListBot().GetFirst();
		while (nodeRoot)
		{
			CBot *bot = (CBot *)nodeRoot->GetData();
			if(bot->GetGUIDBot() == guid)
			{
				return bot;
			}
			nodeRoot = nodeRoot->GetNext();
		}
	}

	if(GetRootNode()->GetListNode().GetCount() > 0)
	{
		//Пробежимся по узлам корня дерева
		wxList::compatibility_iterator nodeLevel1 = GetRootNode()->GetListNode().GetFirst();
		while (nodeLevel1)
		{
			//Посмотрим на соответсвие узлы корня дерева
			CNodeBot *nodebot = (CNodeBot *)nodeLevel1->GetData();
			wxList::compatibility_iterator BotNodeLevel1 = nodebot->GetListBot().GetFirst();
			while(BotNodeLevel1)
			{
				CBot *botlevel1 = (CBot *)BotNodeLevel1->GetData();
				if(botlevel1->GetGUIDBot() == guid)
				{
					return botlevel1;
				}
				BotNodeLevel1 = BotNodeLevel1->GetNext();
			}

			wxList::compatibility_iterator nodeLevel2 = nodebot->GetListNode().GetFirst();
			while(nodeLevel2)
			{
				CNodeBot *nodebot2 = (CNodeBot *)nodeLevel2->GetData();
				wxList::compatibility_iterator BotNodeLevel2 = nodebot2->GetListBot().GetFirst();
				while(BotNodeLevel2)
				{
					CBot *botlevel2 = (CBot *)BotNodeLevel2->GetData();
					if(botlevel2->GetGUIDBot() == guid)
					{
						return botlevel2;
					}
					BotNodeLevel2 = BotNodeLevel2->GetNext();
				}
				nodeLevel2 = nodeLevel2->GetNext();
			}
			nodeLevel1 = nodeLevel1->GetNext();
		}
	}
	return (CBot*)NULL;
}
//**************************************************
CNodeBot *CTreeBot::GetParentNode(GUID guid)
{
	if(GetRootNode() == NULL)
		return NULL;
	if(GetRootNode()->GetGUIDNode() == guid)
		return GetRootNode();
	wxList::compatibility_iterator nodeRoot;
	//Пробежимся по ботам корня дерева
	if(GetRootNode()->GetListBot().GetCount() > 0)
	{
		nodeRoot = GetRootNode()->GetListBot().GetFirst();
		while (nodeRoot)
		{
			CBot *bot = (CBot *)nodeRoot->GetData();
			if(bot->GetGUIDBot() == guid)
			{
				return GetRootNode();
			}
			nodeRoot = nodeRoot->GetNext();
		}
	}
	if(GetRootNode()->GetListNode().GetCount() > 0)
	{
		//Пробежимся по узлам корня дерева
		wxList::compatibility_iterator nodeLevel1 = GetRootNode()->GetListNode().GetFirst();
		while (nodeLevel1)
		{
			//Посмотрим на соответсвие узлы корня дерева
			CNodeBot *nodebot = (CNodeBot *)nodeLevel1->GetData();
			if(nodebot->GetGUIDNode() == guid)
			{
				return GetRootNode();
			}

			wxList::compatibility_iterator BotNodeLevel1 = nodebot->GetListBot().GetFirst();
			while(BotNodeLevel1)
			{
				CBot *botlevel1 = (CBot *)BotNodeLevel1->GetData();
				if(botlevel1->GetGUIDBot() == guid)
				{
					return nodebot;
				}
				BotNodeLevel1 = BotNodeLevel1->GetNext();
			}

			wxList::compatibility_iterator nodeLevel2 = nodebot->GetListNode().GetFirst();
			while(nodeLevel2)
			{
				CNodeBot *nodebot2 = (CNodeBot *)nodeLevel2->GetData();
				if(nodebot2->GetGUIDNode() == guid)
				{
					return nodebot;
				}

				wxList::compatibility_iterator BotNodeLevel2 = nodebot2->GetListBot().GetFirst();
				while(BotNodeLevel2)
				{
					CBot *botlevel2 = (CBot *)BotNodeLevel2->GetData();
					if(botlevel2->GetGUIDBot() == guid)
					{
						return nodebot2;
					}
					BotNodeLevel2 = BotNodeLevel2->GetNext();
				}
				nodeLevel2 = nodeLevel2->GetNext();
			}
			nodeLevel1 = nodeLevel1->GetNext();
		}
	}
	return NULL;
}
//**************************************************
int CTreeBot::GetLevelNode(CNodeBot *value)
{
	if(GetRootNode() == NULL)
		return -1;
	if(GetRootNode() == value)
		return 0;
	if(GetRootNode()->GetListNode().GetCount() > 0)
	{
		//Пробежимся по узлам корня дерева
		wxList::compatibility_iterator nodeLevel1 = GetRootNode()->GetListNode().GetFirst();
		while (nodeLevel1)
		{
			//Посмотрим на соответсвие узлы корня дерева
			CNodeBot *nodebot = (CNodeBot *)nodeLevel1->GetData();
			if(nodebot == value)
			{
				return 1;
			}

			wxList::compatibility_iterator nodeLevel2 = nodebot->GetListNode().GetFirst();
			while(nodeLevel2)
			{
				CNodeBot *nodebot2 = (CNodeBot *)nodeLevel2->GetData();
				if(nodebot2 == value)
				{
					return 2;
				}
				nodeLevel2 = nodeLevel2->GetNext();
			}
			nodeLevel1 = nodeLevel1->GetNext();
		}
	}
	return -1;
}
//**************************************************
bool CTreeBot::LoadTreeNode()
{
	CAppProperties *ap = wxGetApp().GetAppProperties();
	wxFileDialog dialog(wxGetApp().GetTopWindow(),
						ap->GetLngRes(wxT("lr_open")),
						wxEmptyString,
						wxEmptyString,
						wxT("SQLite 3 db (*.db3)|*.db3"));

	dialog.CentreOnParent();
	dialog.SetDirectory(wxGetHomeDir());

	if(dialog.ShowModal() == wxID_OK)
	{
		PreparedStatement *pStatement(NULL);
		SqliteResultSet *dbResSet(NULL);
		wxString _Id;

		SqliteDatabaseLayer *DB = new SqliteDatabaseLayer();
		try
		{
			DB->Open(dialog.GetPath());
		}
		catch(DatabaseLayerException & e)
		{
			wxDELETE(DB);
			wxGetApp().GetError()->SetError(true, e.GetErrorMessage(), wxGetApp().GetError()->eErrorType_DB_Open, false);
			return false;
		}

		dbResSet = (SqliteResultSet *)DB->ExecuteQuery(wxT("SELECT DISTINCT _ParentId FROM _Bot WHERE _ParentId NOT IN ( SELECT DISTINCT _Id FROM _Node )"));
		if(dbResSet->Next())
		{
			_Id = dbResSet->GetResultString(1);
		}
		else
		{
			wxDELETE(DB);
			wxGetApp().GetError()->AddMessage(ap->GetLngRes(wxT("lr_CantFindRecordAboutRoot")));
			return false;
		}
		dbResSet->Close();

		if(wxGetApp().GetTreeBot()->GetNode(ap->StrToGuid(_Id)))
		{
			wxDELETE(DB);
			wxGetApp().GetError()->AddMessage(ap->GetLngRes(wxT("lr_ThisNodeIsLoad")));
			return false;
		}

		wxString sName;
		wxTextEntryDialog TextDialog(wxGetApp().GetTopWindow(),
								wxEmptyString,
								ap->GetLngRes(wxT("lr_EnterNameNode")),
								wxEmptyString,
								wxOK | wxCANCEL);

		if (TextDialog.ShowModal() == wxID_OK)
		{
			sName = TextDialog.GetValue();
		}

		if(sName.IsEmpty())
			sName = wxT("New node");

		CNodeBot *treeNode = new CNodeBot();
		treeNode->SetGUIDNode(ap->StrToGuid(_Id));
		treeNode->SetNodeName(sName);
		treeNode->SetNodePath(dialog.GetPath());
		treeNode->SetNodeType(eNodeType_FolderUnderRoot);
		GetRootNode()->GetListNode().Append(treeNode);
		treeNode->AttachBase(dialog.GetPath());

		CTreeFrame *treeframe = wxGetApp().GetTreeFrame();
		treeframe->RefreshTree();
		wxGetApp().GetTreeBot()->UnactiveBot();
		wxGetApp().GetTreeBot()->UnActiveRule();
		wxGetApp().GetPropertyFrame()->RefreshPropertyGrid(BotNode);
		wxGetApp().GetError()->AddMessage(wxString::Format(wxT("%s - %s"), ap->GetLngRes(wxT("lr_BotCreateSuccess")), _Id));

		wxDELETE(DB);
		return true;
	}
	return false;
}
//**************************************************
bool CTreeBot::LoadTreeBot()
{
	CAppProperties *app = wxGetApp().GetAppProperties();
	PreparedStatement *pStatement(NULL);
	SqliteResultSet *dbResSet(NULL);

	wxString _Id;
	wxString _ActiveBot;
	wxString _NameNode;
	wxString _DbNodePath;
	int _NodeType = NULL;
	
	//Проверим есть ли в таблице бд корень дерева
	dbResSet = (SqliteResultSet *)app->GetPropertiesDB()->ExecuteQuery(wxT("SELECT * FROM _NodeBot WHERE _NodeType = 0"));
	if(dbResSet->Next())
	{
		_Id			= dbResSet->GetResultString(1);
		_ActiveBot	= dbResSet->GetResultString(2);
		_NodeType	= dbResSet->GetResultInt(3);
		_NameNode		= dbResSet->GetResultString(4);
		GetRootNode()->SetGUIDNode(app->StrToGuid(_Id));
		GetRootNode()->SetNodeType(eNodeType(_NodeType));
		GetRootNode()->SetNodeName(_NameNode);
	}
	else
	{
		wxGetApp().GetError()->SetError(true, app->GetLngRes(wxT("lr_CantFindRecordAboutRoot")), wxGetApp().GetError()->eErrorType_DB_Open, true);
		return false;
	}
	dbResSet->Close();
	
	//Получим все записи о структуре дерева ботов
	dbResSet = (SqliteResultSet *)app->GetPropertiesDB()->ExecuteQuery(wxT("SELECT * FROM _NodeBot WHERE _NodeType <> 0"));
	while(dbResSet->Next())
	{
		_Id				= dbResSet->GetResultString(1);
		_ActiveBot		= dbResSet->GetResultString(2);
		_NodeType		= dbResSet->GetResultInt(3);
		_NameNode		= dbResSet->GetResultString(4);
		_DbNodePath		= dbResSet->GetResultString(5);
		
		CNodeBot *nodeBot = new CNodeBot();
		//Добавим узел с ботами в корень дерева ботов
		GetRootNode()->GetListNode().Append(nodeBot);
		nodeBot->SetGUIDNode(app->StrToGuid(_Id));
		nodeBot->SetNodeType(eNodeType(_NodeType));
		nodeBot->SetNodeName(_NameNode);
		nodeBot->SetNodePath(_DbNodePath);
		//Подключим БД с ботами для данного узла
		nodeBot->AttachBase(_DbNodePath);
	}
	dbResSet->Close();

	return true;
}
//**************************************************
bool CTreeBot::SaveTreeBot()
{
	CAppProperties *app = wxGetApp().GetAppProperties();
	PreparedStatement *pStatement(NULL);
	SqliteResultSet *dbResSet(NULL);
	wxArrayString asNodes;

	//Начинаем транзакцию записи в дерева ботов
	app->GetPropertiesDB()->BeginTransaction();
	//Обновим информацию о корне дерева
	pStatement = app->GetPropertiesDB()->PrepareStatement( wxT("UPDATE _NodeBot SET _Id=?, _NodeType=?, _NameNode=?, _DbNodePath=? WHERE _NodeType = 0") );
	if (pStatement)
	{
		CNodeBot *nodeBot = GetRootNode();
		asNodes.Add(app->GuidToStr(nodeBot->GetGUIDNode()));
		pStatement->SetParamString(1, app->GuidToStr(nodeBot->GetGUIDNode()));
		pStatement->SetParamInt(2, nodeBot->GetNodeType());
		pStatement->SetParamString(3, nodeBot->GetNodeName());
		pStatement->SetParamString(4, nodeBot->GetNodePath());
		pStatement->RunQuery();
		app->GetPropertiesDB()->CloseStatement(pStatement);
		pStatement = NULL;
	}
	else
	{
		app->GetPropertiesDB()->RollBack();
		return false;
	}

	//Запишем все подключенные ветки дерева ботов
	if(GetRootNode()->GetListNode().GetCount())
	{
		wxList::compatibility_iterator node = GetRootNode()->GetListNode().GetFirst();
		while (node)
		{
			CNodeBot *nodeBot = (CNodeBot *)node->GetData();
			asNodes.Add(app->GuidToStr(nodeBot->GetGUIDNode()));
			if(nodeBot->GetNodePath().IsEmpty())
			{
				node = node->GetNext();
				continue;
			}

			if(nodeBot->GetNodeType() == eNodeType_FolderUnderRoot)
			{
				pStatement = app->GetPropertiesDB()->PrepareStatement( wxT("DELETE FROM _NodeBot WHERE _Id = ?") );
				if (pStatement)
				{
					pStatement->SetParamString(1, app->GuidToStr(nodeBot->GetGUIDNode()));
					pStatement->RunQuery();
					app->GetPropertiesDB()->CloseStatement(pStatement);
					pStatement = NULL;
				}
				else
				{
					app->GetPropertiesDB()->RollBack();
					return false;
				}

				pStatement = app->GetPropertiesDB()->PrepareStatement( wxT("INSERT INTO _NodeBot(_Id, _ActiveBot, _NodeType, _NameNode, _DbNodePath) VALUES (?, ?, ?, ?, ?);") );
				if (pStatement)
				{
					pStatement->SetParamString(1, app->GuidToStr(nodeBot->GetGUIDNode()));
					pStatement->SetParamString(2, wxEmptyString);
					pStatement->SetParamInt(3, nodeBot->GetNodeType());
					pStatement->SetParamString(4, nodeBot->GetNodeName());
					pStatement->SetParamString(5, nodeBot->GetNodePath());
					pStatement->RunQuery();
					app->GetPropertiesDB()->CloseStatement(pStatement);
					pStatement = NULL;
					if(!nodeBot->DetachBase())
					{
						app->GetPropertiesDB()->RollBack();
						return false;
					}
				}
				else
				{
					app->GetPropertiesDB()->RollBack();
					return false;
				}
			}
			else
			{
				pStatement = app->GetPropertiesDB()->PrepareStatement( wxT("UPDATE _NodeBot SET _NodeType=?, _NameNode=?, _DbNodePath=? WHERE _Id = ?") );
				if (pStatement)
				{
					pStatement->SetParamString(4, app->GuidToStr(nodeBot->GetGUIDNode()));
					pStatement->SetParamInt(1, nodeBot->GetNodeType());
					pStatement->SetParamString(2, nodeBot->GetNodeName());
					pStatement->SetParamString(3, nodeBot->GetNodePath());
					pStatement->RunQuery();
					app->GetPropertiesDB()->CloseStatement(pStatement);
					pStatement = NULL;
					if(!nodeBot->DetachBase())
					{
						app->GetPropertiesDB()->RollBack();
						return false;
					}
				}
				else
				{
					app->GetPropertiesDB()->RollBack();
					return false;
				}
			}
			node = node->GetNext();
		}
		
		wxArrayString asNothing;
		DatabaseResultSet *drs;
		pStatement = app->GetPropertiesDB()->PrepareStatement( wxT("SELECT _Id FROM _NodeBot") );
		if(pStatement)
		{
			drs = pStatement->RunQueryWithResults();
			while(drs->Next())
			{
				bool bHave = false;
				wxString str = drs->GetResultString(1);
				for(size_t i = 0; i < asNodes.Count(); i++)
				{
					if(asNodes.Item(i).Upper() == str.Upper())
					{
						bHave = true;
						break;
					}
				}
				
				if(!bHave)
				{
					asNothing.Add(str);
				}
			}
			drs->Close();
			app->GetPropertiesDB()->CloseStatement(pStatement);
			pStatement = NULL;
		}
		
		for(size_t i = 0; i < asNothing.Count(); i++)
		{
			pStatement = app->GetPropertiesDB()->PrepareStatement( wxT("DELETE FROM _NodeBot WHERE _Id = ?") );
			if (pStatement)
			{
				pStatement->SetParamString(1, asNothing.Item(i));
				pStatement->RunQuery();
				app->GetPropertiesDB()->CloseStatement(pStatement);
				pStatement = NULL;
			}
		}
	}
	app->GetPropertiesDB()->Commit();
	return true;
}
//**************************************************
bool CTreeBot::StartBot(GUID guid)
{
	CNodeBot *nodebot = GetNode(guid);
	if(nodebot == NULL)
		return false;
	if(nodebot->GetTotalBot() <= 0)
		return false;

	wxList::compatibility_iterator node = nodebot->GetListBot().GetFirst();
	while (node)
	{
		CBot *bot = (CBot *)node->GetData();
		if(guid == bot->GetGUIDBot())
		{
			if(!bot->IsChangedBot())
			{
				if(!nodebot->LoadBot(bot->GetGUIDBot(), bot))
					return false;
			}
			if(bot->GetTotalRule() <= 0)
				return false;	//Нет вообще правил в массиве

			if(bot->GetStatusBot() == STATUS_PAUSE)
			{
				bot->SetStatusBot(STATUS_PLAY);
			}
			else if(bot->GetStatusBot() != STATUS_PLAY)
			{
				bot->SetDebugLine(0);
				bot->SetThread((CBotThread *)NULL);
				CBotThread *thread = new CBotThread(bot);
				wxGetApp().stCount = 0;
				if(thread->Create() != wxTHREAD_NO_ERROR)
				{
					wxLogError(wxT("Can't create thread!"));
					return false;
				}
				bot->SetThread(thread);
				thread->Run();
			}
			break;
		}
		node = node->GetNext();
	}
	return true;
}
//**************************************************
bool CTreeBot::DebugBot()
{
	CBot *bot = GetActiveBot();
	if( !bot )
		return false;
	if( !GetDebugBot() )
	{
		if( (bot->GetStatusBot() == STATUS_PLAY) || (bot->GetStatusBot() == STATUS_PAUSE) )
		{
			return false;
		}

		if( bot->GetThread() )
		{
			CBotThread *thread = bot->GetThread();
			thread->Delete();
			bot->SetThread(NULL);
			//return false;
		}
		SetDebugBot(bot);
	}

	if(bot->GetDebugState() == CBot::Debug_No)
	{
		bot->SetDebugState(CBot::Debug_Wait);
		StartBot(bot->GetGUIDBot());
	}
	else if(bot->GetDebugState() == CBot::Debug_Wait)
	{
		bot->SetDebugState(CBot::Debug_Down);
	}
	return true;
}
//**************************************************
bool CTreeBot::StopBot(const GUID guid)
{
	CNodeBot *nodebot = GetActiveNode();
	if(nodebot == NULL)
		return false;
	if(!nodebot->GetNodeType() == eNodeType_Main)
		return false;
	if(nodebot->GetTotalBot() <= 0)
		return false;
	wxList::compatibility_iterator node = nodebot->GetListBot().GetFirst();
	while (node)
	{
		CBot *bot = (CBot *)node->GetData();
		if(guid == bot->GetGUIDBot())
		{
			bot->SetChangedBot(true);
			bot->SetDebugLine(0);
			bot->SetDebugState(CBot::Debug_No);
			if(bot->GetVMCore())
				sq_setbreak(bot->GetVMCore()->getVM());
			if(bot->GetStatusBot() == STATUS_PLAY)
			{
				wxBusyInfo info(wxGetApp().GetAppProperties()->GetLngRes(wxT("lr_WaitWhileStopBot")), NULL);
				CBotThread *thread = bot->GetThread();
				thread->Delete();
				while(bot->GetStatusBot() == STATUS_PLAY)
				{
					::Sleep(100);
				}
			}
			bot->SetThread(NULL);
			break;
		}
		node = node->GetNext();
	}
	return true;
}
//**************************************************
bool CTreeBot::PauseBot(const GUID guid)
{
	CNodeBot *nodebot = GetActiveNode();
	if(nodebot == NULL)
		return false;
	if(!nodebot->GetNodeType() == eNodeType_Main)
		return false;
	if(nodebot->GetTotalBot() <= 0)
		return false;
	wxList::compatibility_iterator node = nodebot->GetListBot().GetFirst();
	while (node)
	{
		CBot *bot = (CBot *)node->GetData();
		if(guid == bot->GetGUIDBot())
		{
			if(bot->GetStatusBot() == STATUS_PAUSE)
			{
				bot->SetStatusBot(STATUS_PLAY);
			}
			else
			{
				bot->SetStatusBot(STATUS_PAUSE);
			}
			break;
		}
		node = node->GetNext();
	}
	return true;
}
//**************************************************
bool CTreeBot::StopAllBot(bool ApplyStatus)
{
	CNodeBot *nodebot = (CNodeBot *)NULL;
	
	if(ApplyStatus)
		nodebot = GetActiveNode();
	else
		nodebot = GetRootNode();

	if(nodebot == NULL)
		return false;
	
	wxBusyInfo info(wxGetApp().GetAppProperties()->GetLngRes(wxT("lr_WaitWhileStopBot")), NULL);
	CBot *bot = (CBot*)NULL;
	wxList::compatibility_iterator ci_bot = nodebot->GetListBot().GetFirst();
	while (ci_bot)
	{
		bot = (CBot *)ci_bot->GetData();
		bot->SetDebugLine(0);
		bot->SetDebugState(CBot::Debug_No);
		if(bot->GetVMCore())
			sq_setbreak(bot->GetVMCore()->getVM());
		if(bot->GetStatusBot() == STATUS_PLAY)
		{
			CBotThread *thread = bot->GetThread();
			thread->Delete();
			while(bot->GetStatusBot() == STATUS_PLAY)
			{
				::Sleep(100);
			}
		}
		bot->SetThread(NULL);

		ci_bot = ci_bot->GetNext();
	}

	wxList::compatibility_iterator ci_node = nodebot->GetListNode().GetFirst();
	while(ci_node)
	{
		CNodeBot *node = (CNodeBot *)ci_node->GetData();
		ci_bot = node->GetListBot().GetFirst();
		while(ci_bot)
		{
			bot = (CBot *)ci_bot->GetData();
			bot->SetDebugLine(0);
			bot->SetDebugState(CBot::Debug_No);
			if(bot->GetVMCore())
				sq_setbreak(bot->GetVMCore()->getVM());
			if(bot->GetStatusBot() == STATUS_PLAY)
			{
				CBotThread *thread = bot->GetThread();
				thread->Delete();
				while(bot->GetStatusBot() == STATUS_PLAY)
				{
					::Sleep(100);
				}
			}
			bot->SetThread(NULL);

			ci_bot = ci_bot->GetNext();
		}

		wxList::compatibility_iterator ci_node2 = node->GetListNode().GetFirst();
		while(ci_node2)
		{
			CNodeBot *node2 = (CNodeBot *)ci_node2->GetData();
			ci_bot = node2->GetListBot().GetFirst();
			while(ci_bot)
			{
				bot = (CBot *)ci_bot->GetData();
				bot->SetDebugLine(0);
				bot->SetDebugState(CBot::Debug_No);
				if(bot->GetVMCore())
					sq_setbreak(bot->GetVMCore()->getVM());
				if(bot->GetStatusBot() == STATUS_PLAY)
				{
					CBotThread *thread = bot->GetThread();
					thread->Delete();
					while(bot->GetStatusBot() == STATUS_PLAY)
					{
						::Sleep(100);
					}
				}
				bot->SetThread(NULL);

				ci_bot = ci_bot->GetNext();
			}
			ci_node2 = ci_node2->GetNext();
		}
		ci_node = ci_node->GetNext();
	}
	return true;
}
//**************************************************
bool CTreeBot::PauseAllBot()
{
	CNodeBot *nodebot = GetActiveNode();
	if(nodebot == NULL)
		return false;
	if(!nodebot->GetNodeType() == eNodeType_Main)
		return false;
	if(nodebot->GetTotalBot() <= 0)
		return false;
	wxList::compatibility_iterator node = nodebot->GetListBot().GetFirst();
	while (node)
	{
		CBot *bot = (CBot *)node->GetData();
		if(bot->GetStatusBot() == STATUS_PLAY)
		{
			bot->SetStatusBot(STATUS_PAUSE);
		}
		node = node->GetNext();
	}
	return true;
}
//**************************************************
bool CTreeBot::UnPauseAllBot()
{
	CNodeBot *nodebot = GetActiveNode();
	if(nodebot == NULL)
		return false;
	if(!nodebot->GetNodeType() == eNodeType_Main)
		return false;
	if(nodebot->GetTotalBot() <= 0)
		return false;
	wxList::compatibility_iterator node = nodebot->GetListBot().GetFirst();
	while (node)
	{
		CBot *bot = (CBot *)node->GetData();
		if(bot->GetStatusBot() == STATUS_PAUSE)
		{
			bot->SetStatusBot(STATUS_PLAY);
		}
		node = node->GetNext();
	}
	return true;
}
//**************************************************
bool CTreeBot::StartAllBot(bool ApplyStatus)
{
	CNodeBot *nodebot = (CNodeBot *)NULL;
	
	if(ApplyStatus)
		nodebot = GetActiveNode();
	else
		nodebot = GetRootNode();

	if(nodebot == NULL)
		return false;

	CBot *bot = (CBot*)NULL;
	wxList::compatibility_iterator ci_bot = nodebot->GetListBot().GetFirst();
	while (ci_bot)
	{
		bot = (CBot *)ci_bot->GetData();
		bot->SetDebugLine(0);
		if(ApplyStatus)
			bot->SetStatusBot(STATUS_PLAY);
		if(bot->GetStatusBot() == STATUS_PLAY)
		{
			bot->SetStatusBot(STATUS_CONTINUE);
			StartBot(bot->GetGUIDBot());
		}

		ci_bot = ci_bot->GetNext();
	}

	wxList::compatibility_iterator ci_node = nodebot->GetListNode().GetFirst();
	while(ci_node)
	{
		CNodeBot *node = (CNodeBot *)ci_node->GetData();
		ci_bot = node->GetListBot().GetFirst();
		while(ci_bot)
		{
			bot = (CBot *)ci_bot->GetData();
			bot->SetDebugLine(0);
			if(ApplyStatus)
				bot->SetStatusBot(STATUS_PLAY);
			if(bot->GetStatusBot() == STATUS_PLAY)
			{
				bot->SetStatusBot(STATUS_CONTINUE);
				StartBot(bot->GetGUIDBot());
			}

			ci_bot = ci_bot->GetNext();
		}

		wxList::compatibility_iterator ci_node2 = node->GetListNode().GetFirst();
		while(ci_node2)
		{
			CNodeBot *node2 = (CNodeBot *)ci_node2->GetData();
			ci_bot = node2->GetListBot().GetFirst();
			while(ci_bot)
			{
				bot = (CBot *)ci_bot->GetData();
				bot->SetDebugLine(0);
				if(ApplyStatus)
					bot->SetStatusBot(STATUS_PLAY);
				if(bot->GetStatusBot() == STATUS_PLAY)
				{
					bot->SetStatusBot(STATUS_CONTINUE);
					StartBot(bot->GetGUIDBot());
				}

				ci_bot = ci_bot->GetNext();
			}
			ci_node2 = ci_node2->GetNext();
		}
		ci_node = ci_node->GetNext();
	}
	return true;
}
//**************************************************
CBot *CTreeBot::CopyBot(GUID parentSrc, GUID parentDst, GUID child)
{
	CNodeBot *srcNode = GetNode(parentSrc);
	if(srcNode == NULL)
		return NULL;
	CNodeBot *dstNode = GetNode(parentDst);
	if(dstNode == NULL)
		return NULL;
	CBot *bot = srcNode->GetBot(child);
	if(bot == NULL)
		return NULL;
	
	CBot *copyBot = new CBot(true);
	copyBot->SetChangedBot(true);
	copyBot->SetDebugState(CBot::Debug_No);
	copyBot->SetDebugLine(0);
	copyBot->SetNameBot(bot->GetNameBot());
	copyBot->SetDescrBot(bot->GetDescrBot());
	copyBot->SetStatusBot(bot->GetStatusBot());
	wxXmlDocument *newDoc = copyBot->GetShapeDiagram();
	wxDELETE(newDoc);
	newDoc = new wxXmlDocument(*bot->GetShapeDiagram());
	copyBot->SetShapeDiagram(newDoc);

	if(bot->GetTotalRule() > 0)
	{
		wxList::compatibility_iterator ciRule = bot->GetListRule().GetFirst();
		while (ciRule)
		{
			CRule *rule = (CRule *)ciRule->GetData();
			CRule *ruleCopy = new CRule();
			ruleCopy->SetGUIDRule(rule->GetGUIDRule());
			ruleCopy->SetComponent(rule->GetComponent());
			ruleCopy->SetExecute(rule->GetExecute());
			ruleCopy->SetValues(rule->GetValues());
			copyBot->GetListRule().Append(ruleCopy);
			ciRule = ciRule->GetNext();
		}
	}

	if(bot->GetTotalVariables() > 0)
	{
		wxList::compatibility_iterator ciVar = bot->GetListVariables().GetFirst();
		while(ciVar)
		{
			CVar *var = (CVar *)ciVar->GetData();
			CVar *varCopy = new CVar();
			varCopy->SetId(var->GetId());
			varCopy->SetDescription(var->GetDescription());
			varCopy->SetType(var->GetType());
			varCopy->SetValue(var->GetValue());
			copyBot->GetListVariables().Append(varCopy);
			ciVar = ciVar->GetNext();
		}
	}
	copyBot->SetLastNumVar(bot->GetLastNumVar());
	dstNode->GetListBot().Append(copyBot);
	return copyBot;
}
//**************************************************
CRule *CTreeBot::GetClipboardRule(const GUID guid)
{
	if(m_ListClipboardRule.GetCount() <= 0)
		return (CRule *)NULL;
	wxList::compatibility_iterator node = m_ListClipboardRule.GetFirst();
	while (node)
	{
		CRule *Rule = (CRule *)node->GetData();
		if(guid == Rule->GetGUIDRule())
		{
			return Rule;
		}
		node = node->GetNext();
	}
	return (CRule *)NULL;
}
//**************************************************
CBotThread::CBotThread(CBot *Bot) : wxThread()
{
	m_Bot = Bot;
}
//**************************************************
void CBotThread::OnExit()
{
}
//**************************************************
void print_squirrel_stack(HSQUIRRELVM v)
{
	wxLogMessage(wxT("--------------------------------------------------------------"));
	int count = sq_gettop(v);
	for(int i = 1; i <= count; ++i)
	{
		int negative = i - (count + 1);
		switch(sq_gettype(v, i))
		{
		case OT_NULL:
			wxLogMessage(wxT("%d: null"), negative);
			break;
		case OT_INTEGER:
			{
				SQInteger val;
				sq_getinteger(v, i, &val);
				wxLogMessage(wxT("%d: integer (%d)"), negative, val);
				break;
			}
		case OT_FLOAT:
			{
				float val;
				sq_getfloat(v, i, &val);
				wxLogMessage(wxT("%d: float (%f)"), negative, val);
				break;
			}
		case OT_STRING:
			{
				const wchar_t* val;
				sq_getstring(v, i, &val);
				wxLogMessage(wxT("%d: string (%ls)"), negative, val);
				break;
			}
		case OT_TABLE:
			wxLogMessage(wxT("%d: table"), negative);
			 break;
		case OT_ARRAY:
			wxLogMessage(wxT("%d: array"), negative);
			break;
		case OT_USERDATA:
			wxLogMessage(wxT("%d: userdata"), negative);
			break;
		case OT_CLOSURE:
			wxLogMessage(wxT("%d: closure(function)"), negative);
			break;
		case OT_NATIVECLOSURE:
			wxLogMessage(wxT("%d: native closure(C function)"), negative);
			break;
		case OT_GENERATOR:
			wxLogMessage(wxT("%d: generator"), negative);
			break;
		case OT_USERPOINTER:
			wxLogMessage(wxT("%d: userpointer"), negative);
			break;
		case OT_THREAD:
			wxLogMessage(wxT("%d: thread"), negative);
			break;
		case OT_CLASS:
			wxLogMessage(wxT("%d: class"), negative);
			break;
		case OT_INSTANCE:
			wxLogMessage(wxT("%d: instance"), negative);
			break;
		default:
			wxLogMessage(wxT("%d: unknown?!?"), negative);
			break;
		}
	}
	wxLogMessage(wxT("--------------------------------------------------------------"));
}
//**************************************************
int debug_hook(HSQUIRRELVM v)
{
	SQUserPointer up;
	int event_type,line;
	const SQChar *src, *func, *ev;
	sq_getinteger(v,2,&event_type);
	sq_getstring(v,3,&src);
	sq_getinteger(v,4,&line);
	sq_getstring(v,5,&func);
	sq_getuserpointer(v,-1,&up);

	CBotThread *cBT = (CBotThread*)up;
	if(!cBT) return 0;

	while(cBT->m_Bot->GetDebugState() == CBot::Debug_Wait)
	{
		Sleep(10);
	}
	
	if(cBT->m_Bot->GetDebugState() == CBot::Debug_Down)
	{
		cBT->m_Bot->SetDebugLine(line);
		cBT->m_Bot->SetDebugState(CBot::Debug_Wait);
		wxLogMessage(wxT("%d"), line);
	}
	else if(cBT->m_Bot->GetDebugState() == CBot::Debug_No)
	{
		sq_setbreak(v);
	}

	return 0;
}
//**************************************************
void *CBotThread::Entry()
{
	//Проверим, назначен ли потоку бот
	if(!m_Bot)
		return NULL;

	CAppProperties *ap = wxGetApp().GetAppProperties();
	CError *er = wxGetApp().GetError();
	wxString strGUIDBot = ap->GuidToStr(m_Bot->GetGUIDBot());

	m_Bot->InitVMCore();
	//Попытка создать триггер
	CTrigger *trigger = new CTrigger();
	if(trigger == NULL)
	{
		wxGetApp().GetScriptMutex()->Lock();
		er->AddMessage(ap->GetLngRes(wxT("lr_BotTriggerFaultMsg")), CError::eMsgType_BotErr, strGUIDBot);
		wxGetApp().GetScriptMutex()->Unlock();
		m_Bot->SetStatusBot(STATUS_ERROR);
		return NULL;
	}
	
	wxGetApp().GetScriptMutex()->Lock();
	//Прилинкуем доступные компоненты
	wxList::compatibility_iterator ciComp = ap->GetComponentList().GetFirst();
	while (ciComp)
	{
		CComponent *comp = (CComponent *)ciComp->GetData();
		//if(comp->GetPlugin())
		//{
		//	comp->GetPlugin()->InitClass(m_Bot->GetVMCore());
		//}
		//else
		//{
			comp->Link(m_Bot->GetVMCore());
		//}
		//er->AddMessage(ap->GetLngRes(wxT("lr_BotComponentLinkMsg"))+wxT(" ")+comp->GetName(), CError::eMsgType_BotMsg);
		ciComp = ciComp->GetNext();
	}
	wxGetApp().GetScriptMutex()->Unlock();

	//Перед исполнением, откомпилируем скрипт бота
	if(m_Bot->GetDebugState() != CBot::Debug_No)
	{
		sq_pushregistrytable(m_Bot->GetVMCore()->getVM());
		sq_pushstring(m_Bot->GetVMCore()->getVM(), wxT("_sqdebughook_"), -1);
		sq_pushuserpointer(m_Bot->GetVMCore()->getVM(), this);
		sq_newclosure(m_Bot->GetVMCore()->getVM(), debug_hook, 1);
		sq_createslot(m_Bot->GetVMCore()->getVM(), -3);

		sq_pushstring(m_Bot->GetVMCore()->getVM(), wxT("_sqdebughook_"), -1);
		sq_rawget(m_Bot->GetVMCore()->getVM(), -2);
		sq_setdebughook(m_Bot->GetVMCore()->getVM());
		sq_pop(m_Bot->GetVMCore()->getVM(), 1);
		sq_enabledebuginfo(m_Bot->GetVMCore()->getVM(), SQTrue);
	}

	//Привяжем переменные, и скомпилируем схему в код скрипта
	wxString sCode;
	wxString sTrigger;
	wxGetApp().GetScriptMutex()->Lock();
	if(!m_Bot->Compile(&sCode, &sTrigger))
	{
		er->AddMessage(ap->GetLngRes(wxT("lr_BotCompileFaultMsg")), CError::eMsgType_BotErr, strGUIDBot);
		wxDELETE(trigger);
		m_Bot->SetStatusBot(STATUS_ERROR);
		return NULL;
	}
	wxGetApp().GetScriptMutex()->Unlock();

	//Скомпилируем код скрипта в байт код виртуальной машины Squirrel
	try
	{
		if(SQ_FAILED(sq_compilebuffer(m_Bot->GetVMCore()->getVM(), sCode.c_str(), sCode.Len(), _SC("script compiler"), SQTrue)))
		{
			wxGetApp().GetScriptMutex()->Lock();
			er->AddMessage(ap->GetLngRes(wxT("lr_BotCompileFaultMsg")), CError::eMsgType_BotErr, strGUIDBot);
			wxGetApp().GetScriptMutex()->Unlock();
			wxDELETE(trigger);
			m_Bot->SetStatusBot(STATUS_ERROR);
			return NULL;
		}
	}
	catch(CException &e)
	{
		wxGetApp().GetScriptMutex()->Lock();
		er->AddMessage(ap->GetLngRes(wxT("lr_BotCompileFaultMsg")), CError::eMsgType_BotErr, strGUIDBot);
		wxGetApp().GetScriptMutex()->Unlock();
		wxDELETE(trigger);
		m_Bot->SetStatusBot(STATUS_ERROR);
		return NULL;
	}

	//Заполняем триггер значениями согласно описанному правилу
	CRule *rule = m_Bot->GetRule(ap->StrToGuid(sTrigger));
	if(!rule)
	{
		wxGetApp().GetScriptMutex()->Lock();
		er->AddMessage(ap->GetLngRes(wxT("lr_BotTriggerFaultMsg")), CError::eMsgType_BotErr, strGUIDBot);
		wxGetApp().GetScriptMutex()->Unlock();
		wxDELETE(trigger);
		m_Bot->SetStatusBot(STATUS_ERROR);
		return NULL;
	}
	
	double val = -1;
	long lval = -1;
	wxDateTime dt = wxDateTime::Now();
	rule->GetValue(wxT("m3")).ToDouble(&val); trigger->SetTriggerType(val); val = -1;
	rule->GetValue(wxT("m4")).ToDouble(&val); trigger->SetPeriodSec(val); val = -1;
	if(rule->GetValue(wxT("m5")) == wxT("true")) 
		trigger->SetOnTimetable(true); 
	else 
		trigger->SetOnTimetable(false);
	rule->GetValue(wxT("m6")).ToDouble(&val); trigger->SetPeriodicity(val); val = -1;
	rule->ConvStrToDate(rule->GetValue(wxT("m7")), &dt);
	trigger->SetDateStart(dt); dt = wxDateTime::Now();
	rule->ConvStrToDate(rule->GetValue(wxT("m8")), &dt);
	trigger->SetTimeStart(dt); dt = wxDateTime::Now();
	rule->GetValue(wxT("m9")).ToDouble(&val); trigger->SetOccursDaily(val); val = -1;
	rule->GetValue(wxT("m10")).ToDouble(&val); trigger->SetEvryN(val); val = -1;
	rule->GetValue(wxT("m11")).ToDouble(&val); trigger->SetTimeUnit(val); val = -1;
	rule->ConvStrToTime(rule->GetValue(wxT("m12")), &dt);
	trigger->SetDailyFreqBegin(dt); dt = wxDateTime::Now();
	rule->ConvStrToTime(rule->GetValue(wxT("m13")), &dt);
	trigger->SetDailyFreqEnd(dt); dt = wxDateTime::Now();
	rule->GetValue(wxT("m14")).ToLong(&lval); trigger->SetWeekly(lval); lval = -1;
	rule->GetValue(wxT("m15")).ToLong(&lval); trigger->SetDayOfWeek(lval); lval = -1;
	if(rule->GetValue(wxT("m16")) == wxT("true")) 
		trigger->SetStartRun(true); 
	else 
		trigger->SetStartRun(false);
	rule->GetValue(wxT("m17")).ToLong(&lval); trigger->SetMonth(lval); lval = -1;

	size_t delay = 0;
	m_Bot->SetStatusBot(STATUS_PLAY);
	if(m_Bot->GetDebugState() != CBot::Debug_No)
	{
		//Блок который исполняет скомпилированный заранее байт код
		wxGetApp().GetScriptMutex()->Lock();
		er->AddMessage(ap->GetLngRes(wxT("lr_BotRunMsg")), CError::eMsgType_BotMsg, strGUIDBot);
		wxGetApp().GetScriptMutex()->Unlock();
		try
		{
			sq_pushroottable(m_Bot->GetVMCore()->getVM());
			sq_call(m_Bot->GetVMCore()->getVM(), 1, false, false);
		}
		catch (CException &e)
		{
			wxGetApp().GetScriptMutex()->Lock();
			er->AddMessage(ap->GetLngRes(wxT("lr_BotCompileFaultMsg")), CError::eMsgType_BotErr, strGUIDBot);
			wxGetApp().GetScriptMutex()->Unlock();
			wxDELETE(trigger);
			m_Bot->SetStatusBot(STATUS_ERROR);
			return NULL;
		}
	}
	else
	{
		while(m_Bot->GetStatusBot() != STATUS_STOP)
		{
			if ( TestDestroy() )
				break;
			if((trigger->Check(m_Bot, &delay)) && (m_Bot->GetStatusBot() == STATUS_PLAY))
			{
				//Блок который исполняет скомпилированный заранее байт код
				wxGetApp().GetScriptMutex()->Lock();
				er->AddMessage(ap->GetLngRes(wxT("lr_BotRunMsg")), CError::eMsgType_BotMsg, strGUIDBot);
				wxGetApp().GetScriptMutex()->Unlock();
				try
				{
					sq_pushroottable(m_Bot->GetVMCore()->getVM());
					SQRESULT res = sq_call(m_Bot->GetVMCore()->getVM(), 1, false, true);
				}
				catch (CException &e)
				{
					wxGetApp().GetScriptMutex()->Lock();
					er->AddMessage(ap->GetLngRes(wxT("lr_BotCompileFaultMsg")), CError::eMsgType_BotErr, strGUIDBot);
					wxGetApp().GetScriptMutex()->Unlock();
					wxDELETE(trigger);
					m_Bot->SetStatusBot(STATUS_ERROR);
					return NULL;
				}
			}
			wxThread::Sleep(delay);
		}
	}

	//Извлечение переменных из виртуальной машины обратно в таблицу переменных
	SQInteger top = sq_gettop(m_Bot->GetVMCore()->getVM());
	wxList::compatibility_iterator varnode = m_Bot->GetListVariables().GetFirst();
	while (varnode)
	{
		CVar *var = (CVar *)varnode->GetData();
		sq_pushroottable(m_Bot->GetVMCore()->getVM());
		sq_pushstring(m_Bot->GetVMCore()->getVM(), wxString::Format(wxT("var_%d"), var->GetId()), -1);
		SQRESULT res = sq_get(m_Bot->GetVMCore()->getVM(), -2);
		SQObjectType sqObjTyp = sq_gettype(m_Bot->GetVMCore()->getVM(), -1);
		switch(sqObjTyp)
		{
		case OT_FLOAT:
		case OT_INTEGER:
			{
				SQFloat sqFloat = 0;
				sq_getfloat(m_Bot->GetVMCore()->getVM(), -1, &sqFloat);
				var->SetValue(wxString::Format(wxT("%f"), sqFloat));
			}
			break;
		case OT_BOOL:
			{
				SQBool sqBool;
				sq_getbool(m_Bot->GetVMCore()->getVM(), -1, &sqBool);
				if(sqBool)
					var->SetValue(wxT("true"));
				else
					var->SetValue(wxT("false"));
			}
			break;
		case OT_STRING:
			{
				const SQChar *sqChar;
				sq_getstring(m_Bot->GetVMCore()->getVM(), -1, &sqChar);
				var->SetValue(wxString(sqChar));
			}
			break;
		}
		varnode = varnode->GetNext();
	}
	sq_settop(m_Bot->GetVMCore()->getVM(), top);

	wxGetApp().GetScriptMutex()->Lock();
	er->AddMessage(ap->GetLngRes(wxT("lr_BotStopMsg")), CError::eMsgType_BotMsg, strGUIDBot);
	wxGetApp().GetScriptMutex()->Unlock();
	//Завершим все процессы виртуальной машины...
	m_Bot->SetStatusBot(STATUS_STOP);
	m_Bot->SetDebugState(CBot::Debug_No);
	wxDELETE(trigger);

	return NULL;
}
//**************************************************
//Реализаций узла дерева Ботов
CNodeBot::CNodeBot()
{
	CoCreateGuid(&m_GUIDNode); //Создаем ГУИД ветки дерева
}
//**************************************************
CBot *CNodeBot::GetBot(const GUID guid)
{
	if(GetTotalBot() <= 0)
		return NULL;
	wxList::compatibility_iterator node = GetListBot().GetFirst();
	while (node)
	{
		CBot *Bot = (CBot *)node->GetData();
		if(guid == Bot->GetGUIDBot())
		{
			return Bot;
		}
		node = node->GetNext();
	}
	return NULL;
}
//**************************************************
CBot *CNodeBot::AddBot(const wxString nameBot, GUID* BotGUID, int* num)
{
	CBot* newBot = new CBot(true);
	*BotGUID = newBot->GetGUIDBot();
	*num = GetTotalBot();
	wxString text;
	text.Printf(wxT("%s%d"), nameBot,*num);
	newBot->SetNameBot(text);
	newBot->SetChangedBot(true);

	GetListBot().Append(newBot);

	return newBot;
}
//**************************************************
bool CNodeBot::DeleteBot(GUID guid)
{
	if(GetTotalBot() <= 0)
		return false;
	CBot *bot = GetBot(guid);
	if(bot)
	{
		if(bot->GetStatusBot() == STATUS_PLAY)
		{
			CBotThread *thread = bot->GetThread();
			if(thread != NULL)
			{
				thread->Delete();
				while(bot->GetStatusBot() == STATUS_PLAY)
				{
					::Sleep(100);
				}
			}
			bot->SetThread(NULL);
		}
		GetListBot().DeleteContents(true);
		GetListBot().DeleteObject(bot);
		return true;
	}
	return false;
}
//**************************************************
bool CNodeBot::ClearNode()
{
	if(GetTotalBot() > 0)
	{
		GetListBot().DeleteContents(true);
		GetListBot().Clear();
	}
	
	if(GetTotalNode() > 0)
	{
		wxList::compatibility_iterator ciNode = GetListNode().GetFirst();
		while (ciNode)
		{
			CNodeBot *node = (CNodeBot *)ciNode->GetData();
			node->ClearNode();
			ciNode = ciNode->GetNext();
		}
		GetListNode().DeleteContents(true);
		GetListNode().Clear();
	}
	
	return true;
}
//**************************************************
bool CNodeBot::LoadBot(GUID guid, CBot *bot)
{
	if(!bot)
		return false;
	SqliteDatabaseLayer *DB = new SqliteDatabaseLayer();
	bool bCreate = !wxFileExists(m_DbNodePath);
	if(bCreate)
	{
		wxDELETE(DB);
		wxGetApp().GetError()->SetError(true, wxT("Database does not exist!"), wxGetApp().GetError()->eErrorType_DB_Open, false);
		return false;
	}
	
	try
	{
		DB->Open(m_DbNodePath);
	}
	catch(DatabaseLayerException & e)
	{
		wxDELETE(DB);
		wxGetApp().GetError()->SetError(true, e.GetErrorMessage(), wxGetApp().GetError()->eErrorType_DB_Open, false);
		return false;
	}

	CAppProperties *app = wxGetApp().GetAppProperties();
	wxString guidBot = app->GuidToStr(bot->GetGUIDBot());
	SqliteResultSet *dbResSet(NULL);

	try
	{
		dbResSet = (SqliteResultSet *)DB->ExecuteQuery(wxString::Format(wxT("SELECT * FROM _Bot WHERE _Id='%s'"), guidBot));
		while(dbResSet->Next())
		{
			wxMemoryBuffer memBuf;
			dbResSet->GetResultBlob(7, memBuf);
			double _Bytes = memBuf.GetBufSize();
			if(_Bytes == 0)
				continue;
			wxMemoryInputStream memIn((const void *)memBuf.GetData(), _Bytes - 1024);
			bot->GetShapeDiagram()->Load(memIn);
		}
		dbResSet->Close();

		dbResSet = (SqliteResultSet *)DB->ExecuteQuery(wxString::Format(wxT("SELECT * FROM _Rule WHERE _ParentId='%s'"), guidBot));
		while(dbResSet->Next())
		{
			wxString _Id = dbResSet->GetResultString(1);
			if(_Id.Length() == 36)
			{
				CRule *rule = new CRule();
				rule->SetGUIDRule(app->StrToGuid(_Id));
				rule->SetComponent(app->GetComponent(app->StrToGuid(dbResSet->GetResultString(3))));
				rule->SetExecute((bool)dbResSet->GetResultInt(4));
				rule->UnPackValues(dbResSet->GetResultString(5));
				bot->GetListRule().Append(rule);
			}
		}
		dbResSet->Close();

		dbResSet = (SqliteResultSet *)DB->ExecuteQuery(wxString::Format(wxT("SELECT * FROM _Variables WHERE _ParentId='%s'"), guidBot));
		while(dbResSet->Next())
		{
			CVar *var = new CVar();
			var->SetId(dbResSet->GetResultDouble(1));
			var->SetDescription(dbResSet->GetResultString(3));
			var->SetType(eVar(dbResSet->GetResultInt(4)));
			var->SetValue(dbResSet->GetResultString(5));
			bot->GetListVariables().Append(var);
		}
		dbResSet->Close();
	}
	catch(DatabaseLayerException & e)
	{
		wxUnusedVar(e);
		wxFAIL_MSG(e.GetErrorMessage());
		dbResSet->Close();
		DB->Close();
		wxDELETE(DB);
		return false;
	}
	DB->Close();
	wxDELETE(DB);

	bot->SetChangedBot(true);
	return true;
}
//**************************************************
bool CNodeBot::SaveBot(CBot *bot, SqliteDatabaseLayer *pDataBase)
{
	if((!pDataBase) || (!bot))
		return false;
	PreparedStatement * pStatement(NULL);
	if(bot->GetListRule().GetCount() > 0)
	{
		pStatement = pDataBase->PrepareStatement( wxT("DELETE FROM _Rule WHERE _ParentId = ?") );
		if (pStatement)
		{
			pStatement->SetParamString(1, wxGetApp().GetAppProperties()->GuidToStr(bot->GetGUIDBot()));
			pStatement->RunQuery();
			pDataBase->CloseStatement(pStatement);
			pStatement = NULL;
		}

		wxList::compatibility_iterator ciRule = bot->GetListRule().GetFirst();
		while(ciRule)
		{
			CRule *rule = (CRule *)ciRule->GetData();
			pStatement = pDataBase->PrepareStatement(
				wxT("INSERT INTO _Rule(_Id, _ParentId, _ComponentId, _Execute, _Value) VALUES (?, ?, ?, ?, ?);"));
			if (pStatement)
			{
				pStatement->SetParamString(1, wxGetApp().GetAppProperties()->GuidToStr(rule->GetGUIDRule()));
				pStatement->SetParamString(2, wxGetApp().GetAppProperties()->GuidToStr(bot->GetGUIDBot()));
				pStatement->SetParamString(3, wxGetApp().GetAppProperties()->GuidToStr(rule->GetComponent()->GetGUIDComponent()));
				pStatement->SetParamInt(4, rule->GetExecute());
				pStatement->SetParamString(5, rule->PackValues());
				pStatement->RunQuery();
				pDataBase->CloseStatement(pStatement);
				pStatement = NULL;
			}
			ciRule = ciRule->GetNext();
		}
	}

	if(bot->GetListVariables().GetCount() > 0)
	{
		pStatement = pDataBase->PrepareStatement( wxT("DELETE FROM _Variables WHERE _ParentId = ?") );
		if (pStatement)
		{
			pStatement->SetParamString(1, wxGetApp().GetAppProperties()->GuidToStr(bot->GetGUIDBot()));
			pStatement->RunQuery();
			pDataBase->CloseStatement(pStatement);
			pStatement = NULL;
		}

		wxList::compatibility_iterator ciVar = bot->GetListVariables().GetFirst();
		while(ciVar)
		{
			CVar *var = (CVar *)ciVar->GetData();
			pStatement = pDataBase->PrepareStatement(
				wxT("INSERT INTO _Variables(_Id, _ParentId, _Description, _Type, _Value) VALUES (?, ?, ?, ?, ?);"));
			if (pStatement)
			{
				pStatement->SetParamDouble(1, var->GetId());
				pStatement->SetParamString(2, wxGetApp().GetAppProperties()->GuidToStr(bot->GetGUIDBot()));
				pStatement->SetParamString(3, var->GetDescription());
				pStatement->SetParamInt(4, var->GetType());
				pStatement->SetParamString(5, var->GetValue());
				pStatement->RunQuery();
				pDataBase->CloseStatement(pStatement);
				pStatement = NULL;
			}
			ciVar = ciVar->GetNext();
		}
	}
	return true;
}
//**************************************************
CNodeBot::~CNodeBot()
{
	m_ListNode.DeleteContents(true);
	m_ListNode.Clear();
	m_ListBot.DeleteContents(true);
	m_ListBot.Clear();
}
//**************************************************
// Метод класса узла дерева. Заполняет узел дерева 
// структурой узла включающую, подузлы и боты в дереве.
// Метод не загружает структуру ботов, и правила ботов.
bool CNodeBot::AttachBase(wxString path)
{
	if(path.IsEmpty())
		return false;

	SqliteDatabaseLayer *DB = new SqliteDatabaseLayer();
	bool bCreate = !wxFileExists(path);
	if(bCreate)
	{
		wxDELETE(DB);
		wxGetApp().GetError()->AddMessage(wxT("Database does not exist!"), CError::eMsgType_AppErr);
		return false;
	}
	
	try
	{
		DB->Open(path);
	}
	catch(DatabaseLayerException & e)
	{
		wxDELETE(DB);
		wxGetApp().GetError()->AddMessage(wxT("Attach error: ") + e.GetErrorMessage(), CError::eMsgType_AppErr);
		return false;
	}

	CAppProperties *app = wxGetApp().GetAppProperties();
	PreparedStatement *pStatement(NULL);
	SqliteResultSet *dbResSet(NULL);

	wxString _Id;
	wxString _ParentId;
	wxString _ActiveRule;
	wxString _NameBot;
	wxString _DescrBot;
	int		 _StatusWork = NULL;
	long	 _LastNumVar = NULL;

	// Загружаем подузлы в дереве ботов
	dbResSet = (SqliteResultSet *)DB->ExecuteQuery(wxT("SELECT * FROM _Node ORDER BY _NameNode;"));
	while(dbResSet->Next())
	{
		_Id = dbResSet->GetResultString(1);
		if(_Id.Length() == 36)
		{
			CNodeBot *node = new CNodeBot();
			node->SetGUIDNode(app->StrToGuid(_Id));
			node->SetNodeName(dbResSet->GetResultString(3));
			node->SetNodeType(eNodeType_Folder);
			GetListNode().Append(node);
		}
	}
	dbResSet->Close();

	// Загружаем боты в дерево ботов
	dbResSet = (SqliteResultSet *)DB->ExecuteQuery(wxT("SELECT * FROM _Bot ORDER BY _NameBot;"));
	while(dbResSet->Next())
	{
		_Id = dbResSet->GetResultString(1);
		if(_Id.Length() == 36)
		{
			CBot *bot = new CBot(false);
			bot->SetGUIDBot(app->StrToGuid(_Id));
			bot->SetNameBot(dbResSet->GetResultString(3));
			bot->SetDescrBot(dbResSet->GetResultString(4));
			bot->SetStatusBot(dbResSet->GetResultInt(5));
			bot->SetLastNumVar(dbResSet->GetResultLong(6));
			bot->SetChangedBot(false);
			
			GUID guid = app->StrToGuid(dbResSet->GetResultString(2));
			if(GetGUIDNode() == guid)
			{
				GetListBot().Append(bot);
			}
			else
			{
				CNodeBot *nodebot = wxGetApp().GetTreeBot()->GetNode(guid);
				if(nodebot)
					nodebot->GetListBot().Append(bot);
			}
		}
	}
	dbResSet->Close();

	DB->Close();
	wxDELETE(DB);
	return true;
}
//**************************************************
bool CNodeBot::DetachBase()
{
	CAppProperties *app = wxGetApp().GetAppProperties();
	SqliteDatabaseLayer *DB = new SqliteDatabaseLayer();
	PreparedStatement * pStatement(NULL);
	wxArrayString asNODE;
	wxArrayString asBOT;
	
	if(m_DbNodePath.IsEmpty())
		return false;

	bool bCreate = !wxFileExists(m_DbNodePath);
	if(bCreate)
	{
		wxGetApp().GetError()->AddMessage(app->GetLngRes(wxT("lr_DataBaseNoExist")), CError::eMsgType_Default);
		try
		{
			DB->Open(m_DbNodePath);
		}
		catch(DatabaseLayerException & e)
		{
			wxDELETE(DB);
			wxGetApp().GetError()->AddMessage(wxT("Detach error:") + e.GetErrorMessage(), CError::eMsgType_AppErr);
			return false;
		}

		try
		{
			DB->RunQuery(wxT("CREATE TABLE _Bot(_Id varchar(36), _ParentId varchar(36), _NameBot varchar(50),\
							 _DescrBot text, _StatusWork integer, _LastNumVar real, _XML text);"), true);
			DB->RunQuery(wxT("CREATE TABLE _Node(_Id varchar(36), _ParentId varchar(36), _NameNode varchar(50),\
							 _DescNode text);"), true);
			DB->RunQuery(wxT("CREATE TABLE _Rule(_Id varchar(36), _ParentId varchar(36), _ComponentId varchar(36),\
							 _Execute integer, _Value text);"), true);
			DB->RunQuery(wxT("CREATE TABLE _Variables(_Id double, _ParentId varchar(36), _Description varchar(50),\
							 _Type integer, _Value text);"), true);
		}
		catch(DatabaseLayerException & e) 
		{
			wxDELETE(DB);
			wxGetApp().GetError()->AddMessage(wxT("Detach error:") + e.GetErrorMessage(), CError::eMsgType_AppErr);
			return false;
		}
	}
	else
	{
		try
		{
			DB->Open(m_DbNodePath);
		}
		catch(DatabaseLayerException & e)
		{
			wxDELETE(DB);
			wxGetApp().GetError()->AddMessage(wxT("Detach error:") + e.GetErrorMessage(), CError::eMsgType_AppErr);
			return false;
		}
	}

	try
	{
		DB->BeginTransaction();
		DB->RunQuery(wxT("DELETE FROM _Node;"), true);

		if(GetListNode().GetCount() > 0)
		{
			wxList::compatibility_iterator thisnode = GetListNode().GetFirst();
			while(thisnode)
			{
				CNodeBot *thisNodeBot = (CNodeBot *)thisnode->GetData();
				asNODE.Add(app->GuidToStr(thisNodeBot->GetGUIDNode()));
				pStatement = DB->PrepareStatement(
					wxT("INSERT INTO _Node(_Id, _ParentId, _NameNode, _DescNode) VALUES (?, ?, ?, ?);"));
				if (pStatement)
				{
					pStatement->SetParamString(1, wxGetApp().GetAppProperties()->GuidToStr(thisNodeBot->GetGUIDNode()));
					pStatement->SetParamString(2, wxGetApp().GetAppProperties()->GuidToStr(GetGUIDNode()));
					pStatement->SetParamString(3, thisNodeBot->GetNodeName());
					pStatement->SetParamString(4, wxEmptyString);
					pStatement->RunQuery();
					DB->CloseStatement(pStatement);
					pStatement = NULL;
				}
				
				if(thisNodeBot->GetListBot().GetCount() > 0)
				{
					wxList::compatibility_iterator subBot = thisNodeBot->GetListBot().GetFirst();
					while(subBot)
					{
						CBot *bot = (CBot *)subBot->GetData();
						asBOT.Add(app->GuidToStr(bot->GetGUIDBot()));
						if(bot->IsChangedBot())
						{
							pStatement = DB->PrepareStatement( wxT("DELETE FROM _Bot WHERE _Id = ?") );
							if (pStatement)
							{
								pStatement->SetParamString(1, wxGetApp().GetAppProperties()->GuidToStr(bot->GetGUIDBot()));
								pStatement->RunQuery();
								DB->CloseStatement(pStatement);
								pStatement = NULL;
							}

							pStatement = DB->PrepareStatement(
								wxT("INSERT INTO _Bot(_Id, _ParentId, _NameBot, _DescrBot, _StatusWork, _LastNumVar, _XML) VALUES (?,?,?,?,?,?,?);"));
							if (pStatement)
							{
								pStatement->SetParamString(1, wxGetApp().GetAppProperties()->GuidToStr(bot->GetGUIDBot()));
								pStatement->SetParamString(2, wxGetApp().GetAppProperties()->GuidToStr(thisNodeBot->GetGUIDNode()));
								pStatement->SetParamString(3, bot->GetNameBot());
								pStatement->SetParamBlob(4, bot->GetDescrBot().wc_str(), bot->GetDescrBot().Length());
								pStatement->SetParamInt(5, bot->GetStatusBot());
								pStatement->SetParamInt(6, bot->GetLastNumVar());
								char *ch = NULL;
								if(bot->GetShapeDiagram())
								{
									wxMemoryOutputStream wos;
									bot->GetShapeDiagram()->Save(wos);
									long len = wos.GetLength();
									if(len)
									{
										wos.SeekO(0);
										ch = new char[len];
										size_t st = wos.CopyTo(ch, len);
										wos.Close();
										pStatement->SetParamBlob(7, ch, len);
									}
								}
								pStatement->RunQuery();
								DB->CloseStatement(pStatement);
								pStatement = NULL;
								wxDELETEA(ch);
							}
							SaveBot(bot, DB);
						}
						subBot = subBot->GetNext();
					}
				}
				thisnode = thisnode->GetNext();
			}
		}
		if(GetListBot().GetCount() > 0)
		{
			wxList::compatibility_iterator subBot = GetListBot().GetFirst();
			while(subBot)
			{
				CBot *bot = (CBot *)subBot->GetData();
				asBOT.Add(app->GuidToStr(bot->GetGUIDBot()));
				if(bot->IsChangedBot())
				{
					pStatement = DB->PrepareStatement( wxT("DELETE FROM _Bot WHERE _Id = ?") );
					if (pStatement)
					{
						pStatement->SetParamString(1, wxGetApp().GetAppProperties()->GuidToStr(bot->GetGUIDBot()));
						pStatement->RunQuery();
						DB->CloseStatement(pStatement);
						pStatement = NULL;
					}

					pStatement = DB->PrepareStatement(
						wxT("INSERT INTO _Bot(_Id, _ParentId, _NameBot, _DescrBot, _StatusWork, _LastNumVar, _XML) VALUES (?,?,?,?,?,?,?);"));
					if (pStatement)
					{
						pStatement->SetParamString(1, wxGetApp().GetAppProperties()->GuidToStr(bot->GetGUIDBot()));
						pStatement->SetParamString(2, wxGetApp().GetAppProperties()->GuidToStr(GetGUIDNode()));
						pStatement->SetParamString(3, bot->GetNameBot());
						pStatement->SetParamBlob(4, bot->GetDescrBot().wc_str(), bot->GetDescrBot().Length());
						pStatement->SetParamInt(5, bot->GetStatusBot());
						pStatement->SetParamInt(6, bot->GetLastNumVar());
						char *ch = NULL;
						if(bot->GetShapeDiagram())
						{
							wxMemoryOutputStream wos;
							bot->GetShapeDiagram()->Save(wos);
							long len = wos.GetLength();
							if(len)
							{
								wos.SeekO(0);
								ch = new char[len];
								size_t st = wos.CopyTo(ch, len);
								wos.Close();
								pStatement->SetParamBlob(7, ch, len);
							}
						}
						pStatement->RunQuery();
						DB->CloseStatement(pStatement);
						pStatement = NULL;
						wxDELETEA(ch);
					}
					SaveBot(bot, DB);
				}
				subBot = subBot->GetNext();
			}
		}

		wxArrayString asNothing;
		DatabaseResultSet *drs;
		pStatement = DB->PrepareStatement( wxT("SELECT _Id FROM _Bot") );
		if(pStatement)
		{
			drs = pStatement->RunQueryWithResults();
			while(drs->Next())
			{
				bool bHave = false;
				wxString str = drs->GetResultString(1);
				for(size_t i = 0; i < asBOT.Count(); i++)
				{
					if(asBOT.Item(i).Upper() == str.Upper())
					{
						bHave = true;
						break;
					}
				}
				
				if(!bHave)
				{
					asNothing.Add(str);
				}
			}
			drs->Close();
			DB->CloseStatement(pStatement);
			pStatement = NULL;
		}

		for(size_t i = 0; i < asNothing.Count(); i++)
		{
			pStatement = DB->PrepareStatement( wxT("DELETE FROM _Rule WHERE _ParentId = ?") );
			if (pStatement)
			{
				pStatement->SetParamString(1, asNothing.Item(i));
				pStatement->RunQuery();
				DB->CloseStatement(pStatement);
				pStatement = NULL;
			}

			pStatement = DB->PrepareStatement( wxT("DELETE FROM _Bot WHERE _Id = ?") );
			if (pStatement)
			{
				pStatement->SetParamString(1, asNothing.Item(i));
				pStatement->RunQuery();
				DB->CloseStatement(pStatement);
				pStatement = NULL;
			}
		}

		asNothing.Clear();
		pStatement = DB->PrepareStatement( wxT("SELECT _Id FROM _Node") );
		if(pStatement)
		{
			drs = pStatement->RunQueryWithResults();
			while(drs->Next())
			{
				bool bHave = false;
				wxString str = drs->GetResultString(1);
				for(size_t i = 0; i < asNODE.Count(); i++)
				{
					if(asNODE.Item(i).Upper() == str.Upper())
					{
						bHave = true;
						break;
					}
				}
				
				if(!bHave)
				{
					asNothing.Add(str);
				}
			}
			drs->Close();
			DB->CloseStatement(pStatement);
			pStatement = NULL;
		}
		
		for(size_t i = 0; i < asNothing.Count(); i++)
		{
			pStatement = DB->PrepareStatement( wxT("DELETE FROM _Node WHERE _Id = ?") );
			if (pStatement)
			{
				pStatement->SetParamString(1, asNothing.Item(i));
				pStatement->RunQuery();
				DB->CloseStatement(pStatement);
				pStatement = NULL;
			}
		}
	}
	catch(DatabaseLayerException & e)
	{
		wxUnusedVar(e);
		if(pStatement)
		{
			DB->CloseStatement(pStatement);
			pStatement = NULL;
		}
		wxGetApp().GetError()->AddMessage(wxT("Detach error:") + e.GetErrorMessage(), CError::eMsgType_AppErr);
		DB->RollBack();
		wxDELETE(DB);
		return false;
	}
	DB->Commit();
	DB->Close();
	wxDELETE(DB);
	return true;
}
//**************************************************
//Реализация CBot
CBot::CBot(bool createGuid = true)
{
	if(createGuid)
	{
		CoCreateGuid(&m_GUIDBot);
	}
	m_listRule = (wxList)NULL;
	m_listVariables = (wxList)NULL;
	m_statusWork = STATUS_STOP;
	m_lastNumVar = NULL;
	m_CurrThread = NULL;
	m_DebugLine = 0;
	m_shapeDiagram = new wxXmlDocument();

	m_ChangedBot = false;
	m_LoadedBot = false;
	m_DebugState = Debug_No;
	m_VMCore = (script::VMCore*)NULL;
}
//**************************************************
CBot::~CBot(void)
{
	m_listRule.DeleteContents(true);
	m_listRule.Clear();
	m_listVariables.DeleteContents(true);
	m_listVariables.Clear();
	wxDELETE(m_shapeDiagram)
	wxDELETE(m_VMCore);
}
//**************************************************
CRule *CBot::AddRule(GUID componentGuid, GUID *guidRule)
{
	long TotalRule = GetTotalRule();

	CRule* newRule = new CRule();
	GUID guid;
	HRESULT hr = CoCreateGuid(&guid);
	if (!FAILED(hr))
	{
		SetChangedBot(true);
		newRule->SetGUIDRule(guid);
		newRule->SetComponent(wxGetApp().GetAppProperties()->GetComponent(componentGuid));
		GetListRule().Append(newRule);
	}
	else
	{
		return (CRule *)NULL;
	}
	*guidRule = newRule->GetGUIDRule();
	SetChangedBot(true);
	return newRule;
}
//**************************************************
CVar* CBot::GetVariables(long varId)
{
	if(GetTotalVariables() <= 0)
		return (CVar*)NULL;	//Нет вообще ботов в массиве
	wxList::compatibility_iterator node = GetListVariables().GetFirst();
	while (node)
	{
		CVar *var = (CVar *)node->GetData();
		if(varId == var->GetId())
		{
			return var;
		}
		node = node->GetNext();
	}
	return (CVar*)NULL;
}
//**************************************************
CRule *CBot::GetRule(const GUID guid)
{
	if(GetTotalRule() <= 0)
		return (CRule *)NULL;
	wxList::compatibility_iterator node = GetListRule().GetFirst();
	while (node)
	{
		CRule *Rule = (CRule *)node->GetData();
		if(guid == Rule->GetGUIDRule())
		{
			return Rule;
		}
		node = node->GetNext();
	}
	return (CRule *)NULL;
}
//**************************************************
bool CBot::DeleteRule(const GUID guidRule)
{
	//добавить остановку выполнения правил
	if(GetTotalRule() <= 0)
		return false;

	CRule *rule = GetRule(guidRule);
	if(rule == NULL)
		return false;

	wxGetApp().GetTreeBot()->UnActiveRule();
	GetListRule().DeleteContents(true);
	SetChangedBot(true);
	return GetListRule().DeleteObject(rule);
}
//**************************************************
long CBot::IfGet(ArrayOfIF *ArrIf, wxString guid)
{
	for(size_t i = 0; i < ArrIf->Count(); i++)
	{
		if(guid == ArrIf->Item(i)._IF)
			return i;
	}
	
	return -1;
}
//**************************************************
long CBot::IndexLast(ArrayOfIF *ArrIf, wxArrayString *ArrScr)
{
	int iCountIf = ArrIf->GetCount();
	int _CONDITION;
	//bool _IfFound = false;
	wxString _IF;
	
	//Посмотрим есть ли блок If
	if(iCountIf != 0)
	{
		//Блок If имеется, теперь запрашиваем гуид условия и ветку условия
		_IF = ArrIf->Item(iCountIf - 1)._IF;
		_CONDITION = ArrIf->Item(iCountIf - 1)._CONDITION;

		//Начинаем перебор текста алгоритма
		for(size_t i = 0; i < ArrScr->Count(); i++)
		{
			//Посмотрим найдено ли нужное нам условие
			if(_CONDITION == -1)
			{
				if(ArrScr->Item(i) == (wxT("} else { //") + _IF))
					return i;
			}
			else if(_CONDITION == 3)
			{
				if(ArrScr->Item(i) == (wxT("} //") + _IF))
					return i;
			}
			else
			{
				if(ArrScr->Item(i) == (wxT("} //") + _IF))
					return i;
			}
		}
	}
	return -1;
}
//**************************************************
void CBot::ExpandXMLNode(wxXmlNode *object, ArrayOfUNIT &ArrUnit, ArrayOfREL &ArrRel, int parent)
{
	wxString type = object->GetPropVal(wxT("type"), wxEmptyString);
	double _id = 0;
	double _condition = 0;
	double _source = 0;
	double _target = 0;
	wxString _guid = wxEmptyString;

	wxXmlNode *subparam = object->GetChildren();
	while(subparam)
	{
		wxString subtype = subparam->GetPropVal(wxT("name"), wxEmptyString);
		if(subparam->GetName() == wxT("property"))
		{
			if(subtype == wxT("id"))
			{
				subparam->GetChildren()->GetContent().ToDouble(&_id);
			}
			else if(subtype == wxT("GUID"))
			{
				_guid = subparam->GetChildren()->GetContent();
			}
			else if(subtype == wxT("condition"))
			{
				subparam->GetChildren()->GetContent().ToDouble(&_condition);
			}
			else if(subtype == wxT("source"))
			{
				subparam->GetChildren()->GetContent().ToDouble(&_source);
			}
			else if(subtype == wxT("target"))
			{
				subparam->GetChildren()->GetContent().ToDouble(&_target);
			}
		}
		else if(subparam->GetName() == wxT("object"))
		{
			wxString subtype = subparam->GetPropVal(wxT("type"), wxEmptyString);
			if(subtype == wxT("wxSFComponent")) {}
			else if(subtype == wxT("wxSFTrigger")) {}
			else if(subtype == wxT("wxSFCycle")) {}
			else if(subtype == wxT("wxSFIfShape")) {}
			else if(subtype == wxT("wxSFBotLineShape")) {}
			else
			{
				subparam = subparam->GetNext();
				continue;
			}
			ExpandXMLNode(subparam, ArrUnit, ArrRel, _id);
		}
		subparam = subparam->GetNext();
	}

	if(type == wxT("wxSFBotLineShape"))
	{
		REL rel;
		rel._CONDITION = (int)_condition;
		rel._SOURCE = (int)_source;
		rel._TARGET = (int)_target;
		ArrRel.Add(rel);
	}
	else
	{
		UNIT unit;
		unit._ID = (int)_id;
		unit._TYPE = type;
		unit._UNIT = _guid;
		unit._PARENT = parent;
		unit._CONDITION = -1;
		ArrUnit.Add(unit);
	}
}
//**************************************************
void CBot::ExpandXMLRule(wxXmlNode *GetChildren, CRule *rule, wxArrayString *script, size_t *index, wxString objectName, wxString sCompile)
{
	if( GetChildren )
	{
		while( GetChildren )
		{
			if( GetChildren->GetName() == wxT("category") )
			{
				wxString criterion = GetChildren->GetPropVal(wxT("criterion"), wxEmptyString);
				if(rule->CriterionOK(criterion))
				{
					ExpandXMLRule(GetChildren->GetChildren(), rule, script, index, objectName, sCompile);
				}
			}
			else if( GetChildren->GetName() == wxT("parameter") )
			{
				wxString name		= GetChildren->GetPropVal(wxT("name"), wxEmptyString);
				wxString compile	= GetChildren->GetPropVal(wxT("compile"), wxEmptyString);
				wxString criterion	= GetChildren->GetPropVal(wxT("criterion"), wxEmptyString);
				if( (!rule->CriterionOK(criterion)) || (compile != sCompile) )
				{
					GetChildren = GetChildren->GetNext();
					continue;
				}

				wxString value;
				wxString typevalue;

				wxXmlNode *subparam = GetChildren->GetChildren();
				while(subparam)
				{
					if(subparam->GetName() == wxT("value"))
					{
						typevalue	= subparam->GetPropVal(wxT("type"), wxEmptyString);
						value		= subparam->GetChildren()->GetContent();
					}
					else if(subparam->GetName() == wxT("enumvalue"))
					{
						wxXmlNode *enumparam = subparam->GetChildren();
						while(enumparam)
						{
							if(enumparam->GetName() == wxT("value"))
							{
								typevalue	= enumparam->GetPropVal(wxT("type"), wxEmptyString);
								value		= enumparam->GetChildren()->GetContent();
							}
							enumparam = enumparam->GetNext();
						}
					}
					subparam = subparam->GetNext();
				}
				value = rule->GetValue(value);

				if(typevalue == wxT("bool"))
				{
					if((value == wxT("false")) || (value == wxT("0")) || (value == wxEmptyString))
						value = wxT("false");
					else
						value = wxT("true");
				}

				wxString matchStr = value;
				wxString out;
				size_t start, len;
				wxRegEx reVars(wxT("(\\$[[:digit:]]+)"), wxRE_ADVANCED);
				while( reVars.Matches(matchStr) )
				{
					out = reVars.GetMatch(matchStr);
					wxList::compatibility_iterator ciVar = GetListVariables().GetFirst();
					while (ciVar)
					{
						CVar *var = (CVar *)ciVar->GetData();
						if(wxString::Format(wxT("$%d"), var->GetId()) == out)
						{
							value.Replace(out, wxString::Format(wxT("var_%d"), var->GetId()));
							break;
						}
						ciVar = ciVar->GetNext();
					}
					reVars.GetMatch(&start, &len);
					matchStr = matchStr.Right(matchStr.Len() - (start + len));
				}

				if(!value.IsEmpty())
				{
					if( sCompile == wxT("1") )
					{
						name.Replace(wxT("m_"), wxT("set_"));
						script->Insert(objectName + wxT(".") + name + wxT("(") + value + wxT(");"), ++(*index));
						//script->Insert(objectName + wxT(".") + name + wxT(" = ") + value + wxT(";"), ++(*index));
					}
					else
					{
						name.Replace(wxT("m_"), wxT("get_"));
						script->Insert(value + wxT(" = ") + objectName + wxT(".") + name + wxT("();"), ++(*index));
						//script->Insert(value + wxT(" = ") + objectName + wxT(".") + name + wxT(";"), ++(*index));
					}
				}
			}
			GetChildren = GetChildren->GetNext();
		}
	}
}
//**************************************************
bool CBot::Compile(wxString *sCode, wxString *strTrigger)
{
	CAppProperties *ap = wxGetApp().GetAppProperties();
	CError *er = wxGetApp().GetError();
	wxString strGUIDBot = ap->GuidToStr(GetGUIDBot());

	er->AddMessage(ap->GetLngRes(wxT("lr_BotCompileBeginMsg")), CError::eMsgType_BotMsg, strGUIDBot);

	//добавим все переменные в компилируемый скрипт
	//важный момент index не совпадает с varID переменной, поэтому далее по тексту 
	//необходимо производить сопоставления
	er->AddMessage(ap->GetLngRes(wxT("lr_BotVarLinkMsg")), CError::eMsgType_BotMsg, strGUIDBot);

	SQInteger top = sq_gettop(GetVMCore()->getVM());
	sq_pushroottable(GetVMCore()->getVM());
	wxList::compatibility_iterator varnode = GetListVariables().GetFirst();
	while (varnode)
	{
		CVar *var = (CVar *)varnode->GetData();
		sq_pushstring(GetVMCore()->getVM(), wxString::Format(wxT("var_%d"), var->GetId()), -1);
		switch(var->GetType())
		{
		case Var_bool:
			{
				if(var->GetValue() == wxT("true"))
					sq_pushbool(GetVMCore()->getVM(), true);
				else
					sq_pushbool(GetVMCore()->getVM(), false);
			}
			break;
		case Var_number:
			{
				double val = 0;
				var->GetValue().ToDouble(&val);
				sq_pushfloat(GetVMCore()->getVM(), (SQFloat)val);
			}
			break;
		case Var_string:
			sq_pushstring(GetVMCore()->getVM(), var->GetValue().c_str(), var->GetValue().Len());
			break;
		}
		sq_createslot(GetVMCore()->getVM(), -3);
		varnode = varnode->GetNext();
	}
	sq_settop(GetVMCore()->getVM(), top);

	//Прочитаем схему и заполним массивы компонент и связей
	ArrayOfIF ArrIf;
	ArrayOfUNIT ArrUnit;
	ArrayOfREL ArrRel;

	wxXmlDocument *doc = GetShapeDiagram();
	if(!doc->IsOk())
	{
		er->AddMessage(ap->GetLngRes(wxT("lr_BotCantReadSchemeMsg")), CError::eMsgType_BotErr, strGUIDBot);
		return false;
	}
	if (doc->GetRoot()->GetName() != wxT("canvas"))
	{
		er->AddMessage(ap->GetLngRes(wxT("lr_BotCantReadSchemeMsg")), CError::eMsgType_BotErr, strGUIDBot);
		return false;
	}
	wxXmlNode *chart = doc->GetRoot()->GetChildren();
	while(chart)
	{
		if(chart->GetName() == wxT("chart"))
		{
			wxXmlNode *object = chart->GetChildren();
			while(object)
			{
				if(object->GetName() == wxT("object"))
				{
					wxString type = object->GetPropVal(wxT("type"), wxEmptyString);
					if(type == wxT("wxSFComponent")) {}
					else if(type == wxT("wxSFTrigger")) {}
					else if(type == wxT("wxSFCycle")) {}
					else if(type == wxT("wxSFIfShape")) {}
					else if(type == wxT("wxSFBotLineShape")) {}
					else
					{
						object = object->GetNext();
						continue;
					}
					ExpandXMLNode(object, ArrUnit, ArrRel, 0);
				}
				object =  object->GetNext();
			}
		}
		chart = chart->GetNext();
	}

	//Найдем на схеме Триггер
	bool haveTrigger = false;
	__int32 stIndexTrigger = 0;
	for(size_t i = 0; i < ArrUnit.Count(); i++)
	{
		if(ArrUnit.Item(i)._TYPE == wxT("wxSFTrigger"))
		{
			haveTrigger = true;
			stIndexTrigger = (__int32)i;
			*strTrigger = ArrUnit.Item(i)._UNIT;
			break;
		}
	}

	if(haveTrigger == false)
	{
		er->AddMessage(ap->GetLngRes(wxT("lr_BotNoTriggerSchemeMsg")), CError::eMsgType_BotErr, strGUIDBot);
		return false;
	}

	//Самый сложный процесс построение алгоритма в виде скрипта
	er->AddMessage(ap->GetLngRes(wxT("lr_BotBuildBeginMsg")), CError::eMsgType_BotMsg, strGUIDBot);
	//Количество циклов, если превысит максимальное число то прервём компиляцию, это значит схема составлена неправильно
	//либо слишком большой алгоритм
	int iCountLoops = 0;
	//Флаг отвечающий за прерывания цикла сборки, в случае опроса всех ветвей цикл прекращается
	bool breakCycle = true;
	bool currentCondition = true;
	wxArrayString script;	//Массив строк заполняется алгоритмом
	__int32 stBegin = stIndexTrigger;//Фигура в начале соединения
	__int32 stEnd;	//Фигура на конце соединения

	//Начинам разбирать схему алгоритма
	do
	{
		stEnd = -1;
		if(ArrUnit.Item(stBegin)._TYPE == wxT("wxSFIfShape"))
		{
			//Проверим по какой ветке условия следует пойти (TRUE) или (FALSE)
			long index = IndexLast(&ArrIf, &script);
			long ifIndex = IfGet(&ArrIf, ArrUnit.Item(stBegin)._UNIT);
			if(ifIndex == (-1))
			{
				IF _if;
				_if._CONDITION = 0;
				_if._IF = ArrUnit.Item(stBegin)._UNIT;
				ArrIf.Add(_if);
				ifIndex = ArrIf.GetCount() - 1;
			}

			if(ArrIf.Item(ifIndex)._CONDITION == 1)
			{
				for(size_t i = 0; i < ArrRel.Count(); i++)
				{
					if( (ArrUnit.Item(stBegin)._ID == ArrRel.Item(i)._SOURCE) && (ArrRel.Item(i)._CONDITION == 1) )
					{
						for(size_t ii = 0; ii < ArrUnit.Count(); ii++)
						{
							if(ArrUnit.Item(ii)._ID == ArrRel.Item(i)._TARGET)
							{
								stEnd = ii;
								break;
							}
						}
						break;
					}
				}
				ArrIf.Item(ifIndex)._CONDITION = -1;
			}
			else if(ArrIf.Item(ifIndex)._CONDITION == -1)
			{
				ArrIf.RemoveAt(ifIndex);
				stEnd = -1;
			}
			else
			{
				ArrIf.Item(ifIndex)._CONDITION = 1;
				wxString strGuid = ArrUnit.Item(stBegin)._UNIT;
				if(index == -1)
				{
					script.Add(strGuid + wxT("; "));
					script.Add(wxT("{ //") + strGuid);
					script.Add(wxT("} else { //") + strGuid);
					script.Add(wxT("} //") + strGuid);
				}
				else
				{
					script.Insert(strGuid + wxT("; "), index++);
					script.Insert(wxT("{ //") + strGuid, index++);
					script.Insert(wxT("} else { //") + strGuid, index++);
					script.Insert(wxT("} //") + strGuid, index++);
				}

				currentCondition = false;
				
				for(size_t i = 0; i < ArrRel.Count(); i++)
				{
					if( (ArrUnit.Item(stBegin)._ID == ArrRel.Item(i)._SOURCE) && (ArrRel.Item(i)._CONDITION != 1) )
					{
						ArrUnit.Item(stBegin)._CONDITION = 0;
						for(size_t ii = 0; ii < ArrUnit.Count(); ii++)
						{
							if(ArrUnit.Item(ii)._ID == ArrRel.Item(i)._TARGET)
							{
								stEnd = ii;
								break;
							}
						}
						break;
					}
				}
			}
		}
		else if(ArrUnit.Item(stBegin)._TYPE == wxT("wxSFCycle"))
		{
			long index = IndexLast(&ArrIf, &script);
			wxString strGuid = ArrUnit.Item(stBegin)._UNIT;
			long ifIndex = IfGet(&ArrIf, strGuid);
			if(ifIndex == (-1))
			{
				IF _if;
				_if._CONDITION = 0;
				_if._IF = strGuid;
				ArrIf.Add(_if);
				ifIndex = ArrIf.GetCount() - 1;
			}

			if(ArrIf.Item(ifIndex)._CONDITION != 3)
			{
				ArrIf.Item(ifIndex)._CONDITION = 3;
				if(index == -1)
				{
					script.Add(wxT("while()//") + strGuid + wxT("; "));
					script.Add(wxT("{ //") + strGuid);
					script.Add(wxT("} //") + strGuid);
				}
				else
				{
					script.Insert(wxT("while()//") + strGuid + wxT("; "), index++);
					script.Insert(wxT("{ //") + strGuid, index++);
					script.Insert(wxT("} //") + strGuid, index++);
				}

				for(size_t i = 0; i < ArrRel.Count(); i++)
				{
					if( ArrUnit.Item(stBegin)._ID == ArrRel.Item(i)._SOURCE )
					{
						for(size_t ii = 0; ii < ArrUnit.Count(); ii++)
						{
							if( (ArrUnit.Item(ii)._ID == ArrRel.Item(i)._TARGET) && 
								(ArrUnit.Item(ii)._PARENT == ArrUnit.Item(stBegin)._ID) )
							{
								stEnd = ii;
								break;
							}
						}
						if(stEnd != -1)
							break;
					}
				}
			}

			if(stEnd == -1)
			{
				for(size_t i = 0; i < ArrRel.Count(); i++)
				{
					if( ArrUnit.Item(stBegin)._ID == ArrRel.Item(i)._SOURCE )
					{
						for(size_t ii = 0; ii < ArrUnit.Count(); ii++)
						{
							if( (ArrUnit.Item(ii)._ID == ArrRel.Item(i)._TARGET) &&
								(ArrUnit.Item(ii)._PARENT != ArrUnit.Item(stBegin)._ID) )
							{
								ArrIf.RemoveAt(ifIndex);
								stEnd = ii;
								break;
							}
						}
						if(stEnd != -1)
							break;
					}
				}

				if(stEnd == -1)
				{
					ArrIf.RemoveAt(ifIndex);
				}
			}
		}
		else
		{
			long index = IndexLast(&ArrIf, &script);
			if(index == -1)
				script.Add(ArrUnit.Item(stBegin)._UNIT + wxT("; "));
			else
				script.Insert(ArrUnit.Item(stBegin)._UNIT + wxT("; "), index);
			currentCondition = true;

			for(size_t i = 0; i < ArrRel.Count(); i++)
			{
				if(ArrUnit.Item(stBegin)._ID == ArrRel.Item(i)._SOURCE)
				{
					for(size_t ii = 0; ii < ArrUnit.Count(); ii++)
					{
						if(ArrUnit.Item(ii)._ID == ArrRel.Item(i)._TARGET)
						{
							stEnd = ii;
							break;
						}
					}
					break;
				}
			}
		}
		
		//Посмотрим не равно ли текущее правило 0, и если равно то мы наткнулись на конец ветки
		//попытаемся вернуться назад, до ближайшего if и получить значение другой ветки
		if(stEnd == -1)
		{
			if(ArrIf.GetCount() != 0)
			{
				for(size_t i = 0; i < ArrUnit.Count(); i++)
				{
					if(ArrUnit.Item(i)._UNIT == ArrIf.Item(ArrIf.GetCount() - 1)._IF)
					{
						stEnd = i;
						break;
					}
				}
			}
		}

		stBegin = stEnd;
		//Проверим закончился ли опрос всех ветвей алгоритма, если закончился, то заканчиваем строить
		if((stEnd == -1) && (ArrIf.Count() == 0))
			breakCycle = false;
		iCountLoops++;
		if(iCountLoops > 35000)
		{
			breakCycle = false;
			er->AddMessage(ap->GetLngRes(wxT("lr_BotScriptOverflowMsg")), CError::eMsgType_BotErr, strGUIDBot);
		}
	} while(breakCycle);

	wxRegEx reVars(wxT("(\\$[[:digit:]]+)"), wxRE_ADVANCED);
	wxList::compatibility_iterator ciRule = GetListRule().GetFirst();
	while(ciRule)
	{
		CRule *rule = (CRule *)ciRule->GetData();
		wxString stGUIDRule = ap->GuidToStr(rule->GetGUIDRule());
		if(rule->GetComponent()->GetComponentType() == Trigger)
		{
			for(size_t i = 0; i < script.Count(); i++)
			{
				int iRes = script.Item(i).Find(stGUIDRule);
				if(iRes == 0)
				{
					script.RemoveAt(i);
					break;
				}
			}
			ciRule = ciRule->GetNext();
			continue;
		}
		else if(rule->GetComponent()->GetComponentType() == Script)
		{
			for(size_t i = 0; i < script.Count(); i++)
			{
				int iRes = script.Item(i).Find(stGUIDRule);
				if(iRes == 0)
				{
					wxString newStr = rule->GetValue(wxT("m4"));
					wxString matchStr = newStr;
					wxString out;
					size_t start, len;
					while( reVars.Matches(matchStr) )
					{
						out = reVars.GetMatch(matchStr);
						wxList::compatibility_iterator ciVar = GetListVariables().GetFirst();
						while (ciVar)
						{
							CVar *var = (CVar *)ciVar->GetData();
							if(wxString::Format(wxT("$%d"), var->GetId()) == out)
							{
								newStr.Replace(out, wxString::Format(wxT("var_%d"), var->GetId()));
								break;
							}
							ciVar = ciVar->GetNext();
						}
						reVars.GetMatch(&start, &len);
						matchStr = matchStr.Right(matchStr.Len() - (start + len));
					}
					script.Item(i) = newStr;
					break;
				}
			}
		}
		else if((rule->GetComponent()->GetComponentType() == Condition) ||
				(rule->GetComponent()->GetComponentType() == Cycle) )
		{
			for(size_t i = 0; i < script.Count(); i++)
			{
				int iRes = script.Item(i).Find(stGUIDRule);
				if( (iRes == 0) ||
					((rule->GetComponent()->GetComponentType() == Cycle)&&(iRes == 9)) )
				{
					wxString newStr = rule->GetValue(wxT("m3"));
					wxString matchStr = newStr;
					wxString out;
					size_t start, len;
					while( reVars.Matches(matchStr) )
					{
						out = reVars.GetMatch(matchStr);
						wxList::compatibility_iterator ciVar = GetListVariables().GetFirst();
						while (ciVar)
						{
							CVar *var = (CVar *)ciVar->GetData();
							if(wxString::Format(wxT("$%d"), var->GetId()) == out)
							{
								newStr.Replace(out, wxString::Format(wxT("var_%d"), var->GetId()));
								break;
							}
							ciVar = ciVar->GetNext();
						}
						reVars.GetMatch(&start, &len);
						matchStr = matchStr.Right(matchStr.Len() - (start + len));
					}
					if( rule->GetComponent()->GetComponentType() == Condition )
					{
						if(newStr.IsEmpty())
							script.Item(i) = wxT("if(true)");
						else
							script.Item(i) = wxT("if(") + newStr + wxT(")");
					}
					else
					{
						if(newStr.IsEmpty())
							script.Item(i) = wxT("while(true)");
						else
							script.Item(i) = wxT("while(") + newStr + wxT(")");
					}
				}
			}
		}
		else
		{
			for(size_t i = 0; i < script.Count(); i++)
			{
				int iRes = script.Item(i).Find(stGUIDRule);
				if( iRes == 0 )
				{
					wxXmlDocument *doc = rule->GetComponent()->GetXMLStruct();
					if(!doc->IsOk())
					{
						er->AddMessage(ap->GetLngRes(wxT("lr_BotCantReadComponentSchemeMsg")) + wxT(" ") + ap->GuidToStr(rule->GetComponent()->GetGUIDComponent()), CError::eMsgType_BotErr, strGUIDBot);
						return false;
					}
					if (doc->GetRoot()->GetName() != wxT("componentStructure"))
					{
						er->AddMessage(ap->GetLngRes(wxT("lr_BotCantReadComponentSchemeMsg")) + wxT(" ") + ap->GuidToStr(rule->GetComponent()->GetGUIDComponent()), CError::eMsgType_BotErr, strGUIDBot);
						return false;
					}
					wxXmlNode *object = doc->GetRoot()->GetChildren();
					if(object)
					{
						if(object->GetName() == wxT("object"))
						{
							wxString objectClass = object->GetPropVal(wxT("class"), wxEmptyString);
							wxString objectName = objectClass.Right(objectClass.Len() - 1) + wxString::Format(wxT("_%d"), i+1);
							script.Item(i) = objectName + wxT(" <- ") + objectClass + wxT(".create(); ");
							ExpandXMLRule(object->GetChildren(), rule, &script, &i, objectName, wxT("1"));
							script.Insert(objectName + wxT(".Execute(); "), ++i);
							ExpandXMLRule(object->GetChildren(), rule, &script, &i, objectName, wxT("2"));
						}
					}
				}
			}
		}
		ciRule = ciRule->GetNext();
	}

	for(size_t i = 0; i < script.Count(); i++)
	{
		///if(GetDebugState() != CBot::Debug_No)
			wxLogMessage(wxT("%d - %s"), i + 1, script.Item(i));
		sCode->Append(script.Item(i) + wxT("\n"));
	}
	ArrIf.Clear();
	ArrUnit.Clear();
	ArrRel.Clear();
	script.Clear();
	return true;
}
//**************************************************
void CBot::InitVMCore()
{
	wxDELETE(m_VMCore);
	m_VMCore = new script::VMCore(1024);
	sq_setprintfunc(m_VMCore->getVM(), printfunc);
	sq_pushroottable(m_VMCore->getVM());
	sqstd_register_bloblib(m_VMCore->getVM());
	sqstd_register_iolib(m_VMCore->getVM());
	sqstd_register_systemlib(m_VMCore->getVM());
	sqstd_register_mathlib(m_VMCore->getVM());
	sqstd_register_stringlib(m_VMCore->getVM());
	sqstd_seterrorhandlers(m_VMCore->getVM());
	sq_pop(m_VMCore->getVM(), 1);
}
//**************************************************
//Реализация CRule
//**************************************************
CRule::~CRule()
{
}
//**************************************************
CRule::CRule()
{
	SetExecute(false);
	SetComponent((CComponent *)NULL);
}
//**************************************************
wxString CRule::GetValue(wxString valueID)
{
	long valueIndex = 0;
	valueID.Remove(0,1);
	if(valueID.ToLong(&valueIndex))
	{
		if(valueIndex+1 > m_Values.Count())
		{
			return wxEmptyString;
		}
		else
		{
			return m_Values[valueIndex];
		}
	}

	return wxEmptyString;
}
//**************************************************
wxString CRule::PackValues()
{
	wxString returnString = wxEmptyString;
	for(size_t i = 0; i < m_Values.GetCount(); i++)
	{
		returnString.Append(wxString::Format(wxT("m%d:"), i));
		returnString.Append(m_Values.Item(i));
	}
	return returnString;
}
//**************************************************
void CRule::UnPackValues(wxString values)
{
	if(values.IsEmpty())
		return;
	int i = 0;
	size_t startpos = 0;
	size_t endpos = 0;
	size_t strlen = values.Length();
	wxString valStr;
	while(true)
	{
		size_t len = wxString::Format(wxT("m%d:"),i).Length();
		startpos = values.find(wxString::Format(wxT("m%d:"),i), startpos);
		if(startpos > strlen)
			break;
		endpos	 = values.find(wxString::Format(wxT("m%d:"),i+1), startpos);
		if(endpos > strlen)
			endpos = strlen;
		valStr	 = values.Mid(startpos+len, endpos-startpos-len);
		if(valStr == wxT("NULL"))
			valStr = wxEmptyString;
		m_Values.Add(valStr);
		i++;
	}
}
//**************************************************
void CRule::SetValue(wxString valueID, wxString value)
{
	long valueIndex = 0;
	valueID.Remove(0,1);
	if(valueID.ToLong(&valueIndex))
	{
		if(valueIndex+1 > m_Values.Count())
		{
			for(size_t i = m_Values.Count(); i < valueIndex; i++)
			{
				m_Values.Add(wxEmptyString);
			}
			m_Values.Add(value);
		}
		else
			m_Values[valueIndex] = value;
	}
}
//**************************************************
bool CRule::CriterionOK(wxString criterion)
{
	if(criterion == wxEmptyString)
		return true;
	if(criterion.Length() < 3)
		return false;
	size_t startpos = criterion.find(':');
	wxString valStr = GetValue(criterion.Mid(0, startpos));
	wxString valCri = criterion.Mid(startpos+1, criterion.Length());
	if(valStr.Lower() == valCri.Lower())
		return true;
	else if((valCri == wxT("0")) && (valStr.IsEmpty()))
		return true;
	else
		return false;
}
//**************************************************
//Реализация CTrigger
CTrigger::CTrigger()
{
	m_FlagStartRun = false;
	m_StartDateTime = wxDateTime::Now();
	m_PeriodDateTime = m_StartDateTime;
}
//**************************************************
bool CTrigger::Check(CBot *bot, size_t *delay)
{
	*delay = 0;
	//Изначально проверим условие запуска с определённого времени-даты
	if( GetStartRun() && !m_FlagStartRun)
	{
		if(GetDateStart().GetDateOnly() < wxDateTime::Now().GetDateOnly())
		{
			*delay = 3000;
			return false;
		}
		if(GetDateStart().GetDateOnly() == wxDateTime::Now().GetDateOnly())
		{
			wxDateTime dt(GetDateStart().GetDay(), GetDateStart().GetMonth(), GetDateStart().GetYear(),
						GetTimeStart().GetHour(), GetTimeStart().GetMinute(), GetTimeStart().GetSecond());
			wxTimeSpan tSpan(0, 0, 10);
			wxDateTime dt2 = dt + tSpan;

			if( wxDateTime::Now() > dt2 )
			{
				bot->SetStatusBot(STATUS_STOP);
				return false;
			}

			if( !wxDateTime::Now().IsBetween(dt, dt2) )
			{
				*delay = 3000;
				return false;
			}
		}
		m_FlagStartRun = true;
	}
	
	if( (!GetOnTimetable()) && (GetTriggerType() == 0) )
	{
		bot->SetStatusBot(STATUS_STOP);
		return true;
	}

	if(GetOnTimetable())
	{
		switch(GetPeriodicity())
		{
		case 0:
			{
				if( !wxDateTime::Now().IsBetween(m_DailyFreqBegin, m_DailyFreqEnd) )
				{
					*delay = 3000;
					return false;
				}
				wxTimeSpan tsTime = wxDateTime::Now() - GetStartDateTime();
				if( GetOccursDaily() > 1 )
					if( (tsTime.GetDays() % GetOccursDaily()) != 0 )
					{
						*delay = 3000;
						return false;
					}
				//TimeUnit = 0 Hour
				if( GetTimeUnit() == 0 )
				{
					if( GetEvryN() > 1 )
						if( (tsTime.GetHours() % GetEvryN()) != 0 )
						{
							*delay = 3000;
							return false;
						}
				}
				else if( GetTimeUnit() == 1 )
				{
					if( GetEvryN() > 1 )
						if( (tsTime.GetMinutes() % GetEvryN()) != 0 )
						{
							*delay = 1000;
							return false;
						}
				}
			}
			break;
		case 1:
			{
				if( !wxDateTime::Now().IsBetween(m_DailyFreqBegin, m_DailyFreqEnd) )
				{
					*delay = 3000;
					return false;
				}
				wxTimeSpan tsTime = wxDateTime::Now() - GetStartDateTime();

				if( (1<<wxDateTime::Now().GetWeekDay()) & GetDayOfWeek() )
				{
				}
				else
				{
					*delay = 3000;
					return false;
				}
				//TimeUnit = 0 Hour
				if( GetTimeUnit() == 0 )
				{
					if( GetEvryN() > 1 )
						if( (tsTime.GetHours() % GetEvryN()) != 0 )
						{
							*delay = 3000;
							return false;
						}
				}
				else if( GetTimeUnit() == 1 )
				{
					if( GetEvryN() > 1 )
						if( (tsTime.GetMinutes() % GetEvryN()) != 0 )
						{
							*delay = 1000;
							return false;
						}
				}
			}
			break;
		case 2:
			{
				if( !wxDateTime::Now().IsBetween(m_DailyFreqBegin, m_DailyFreqEnd) )
				{
					*delay = 3000;
					return false;
				}
				wxTimeSpan tsTime = wxDateTime::Now() - GetStartDateTime();

				if( (1<<(int)wxDateTime::Now().GetWeekOfMonth()) & GetWeekly() )
				{
				}
				else
				{
					*delay = 3000;
					return false;
				}

				if( (1<<(wxDateTime::Now().GetMonth()+1)) & GetMonth() )
				{
				}
				else
				{
					*delay = 3000;
					return false;
				}

				if( (1<<wxDateTime::Now().GetWeekDay()) & GetDayOfWeek() )
				{
				}
				else
				{
					*delay = 3000;
					return false;
				}
				//TimeUnit = 0 Hour
				if( GetTimeUnit() == 0 )
				{
					if( GetEvryN() > 1 )
						if( (tsTime.GetHours() % GetEvryN()) != 0 )
						{
							*delay = 3000;
							return false;
						}
				}
				else if( GetTimeUnit() == 1 )
				{
					if( GetEvryN() > 1 )
						if( (tsTime.GetMinutes() % GetEvryN()) != 0 )
						{
							*delay = 1000;
							return false;
						}
				}
			}
			break;
		}
	}

	//TriggerType = 1 (TIMER)
	if(GetTriggerType() == 1)
	{
		wxTimeSpan tSpan(0, 0, GetPeriodSec());
		wxDateTime dt2 = GetPeriodDateTime() + tSpan;

		if( wxDateTime::Now() > dt2 )
		{
			SetPeriodDateTime(wxDateTime::Now());
			return true;
		}
		*delay = 1000;
		return false;
	}

	return true;
}
//**************************************************
bool CRule::ConvStrToDate(const wxString value, wxDateTime *ConvDate)
{
	wxString oldDate = value;
	if(value.Length() != 10)
		return false;
		
	wxString sMonth(value, 0, 2), sDay(value, 3, 2), sYear(value, 6, value.Length()-6);
	double dYear, dMonth, dDay;
	if((!sYear.ToDouble(&dYear)) || (!sMonth.ToDouble(&dMonth)) || (!sDay.ToDouble(&dDay)))
		return false;

	if((dMonth <= 0)||(dMonth > 12))
		return false;

	unsigned short numberDay = wxDateTime::GetNumberOfDays((wxDateTime::Month)((int)dMonth-1),(int)dYear);
	if(((unsigned short)dDay > numberDay) || ((unsigned short)dDay <= 0))
		return false;

	wxDateTime dt((unsigned short)dDay,(wxDateTime::Month)((int)dMonth-1),(int)dYear);
	if(dt.IsValid())
	{
		*ConvDate = dt;
		return true;
	}
	else
		return false;
}
//**************************************************
bool CRule::ConvStrToTime(const wxString value, wxDateTime *ConvTime)
{
	wxString oldDate = value;
	if(value.Length() != 8)
		return false;
		
	wxString sHour(value, 0, 2), sMinute(value, 3, 2), sSecond(value, 6, 2);
	double dHour, dMinute, dSecond;
	if((!sHour.ToDouble(&dHour)) || (!sMinute.ToDouble(&dMinute)) || (!sSecond.ToDouble(&dSecond)))
		return false;

	if(((dHour < 0)||(dHour > 24)) || ((dMinute < 0)||(dMinute > 59)) || ((dSecond < 0)||(dSecond > 59)))
		return false;

	wxDateTime dt((unsigned short)dHour,(unsigned short)dMinute,(unsigned short)dSecond,0);
	if(dt.IsValid())
	{
		*ConvTime = dt;
		return true;
	}
	else
		return false;
}
//**************************************************
wxString CBot::GetValueFromMember(wxString value)
{
	if(value != wxEmptyString)
	{//Если строка с заголовком не пустая
		if(value.Left(1) == wxT("$"))
		{//Если строка с заголовком ссылка на стек переменных
			value.Remove(0,1);
			long lvalue;
			if(value.ToLong(&lvalue))
			{
				CVar *var = (CVar *)NULL;
				var = GetVariables(lvalue);
				if(var != NULL)
				{
					value = var->GetValue();
				}
				else
				{
					value = wxEmptyString;
				}
			}
		}
	}
	return value;
}
//**************************************************
void CBot::SetValueFromMember(wxString VarToSet, wxString value)
{
	if(VarToSet != wxEmptyString)
	{//Если строка с заголовком не пустая
		if(VarToSet.Left(1) == wxT("$"))
		{//Если строка с заголовком ссылка на стек переменных
			VarToSet.Remove(0,1);
			long lvalue;
			if(VarToSet.ToLong(&lvalue))
			{
				CVar *var = (CVar *)NULL;
				var = GetVariables(lvalue);
				if(var != NULL)
				{
					if((value == wxT("true")) || (value == wxT("false")))
					{
						var->SetType(Var_bool);
						if(value == wxT("true"))
							var->SetValue(wxT("1"));
						else
							var->SetValue(wxT(""));
					}
					else
						var->SetValue(value);
				}
			}
		}
	}
}
//**************************************************
void CProgramProcess::OnTerminate(int WXUNUSED(pid), int WXUNUSED(status))
{
	delete this;
}
//**************************************************
void CComponent::Link(script::VMCore *ptCore)
{
	if(GetGUIDComponent() == wxGetApp().GetAppProperties()->StrToGuid(wxT("767FB19B-5F8A-4561-8DFA-4B53547736AE")))
	{
		script::ClassTraits<CAutoBotAction>::bind(ptCore);
	}
	
	if(GetComponentType() == Action)
	{
		if(m_Library)
		{
			typedef void (*InitClassPtr) (script::VMCore *);
			/* пытаемся получить адрес функции getSum */
			InitClassPtr pProc = (InitClassPtr)GetProcAddress(m_Library, "InitClass"); 
			/* проверяем успешность получения адреса */
			_ASSERT(pProc != NULL);
			(pProc)(ptCore);
		}
	}
}
//**************************************************