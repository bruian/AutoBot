#include "CScriptAction.h"
//**************************************************
//CScriptAction::CScriptAction() 
//{
//	//SetScriptType(eScriptType_Squirrel);
//	//SetScript(wxEmptyString);
//}
////**************************************************
//wxString CScriptAction::PackValuesToString() 
//{
//	wxString retString = wxEmptyString;
//	//retString += wxString::Format(wxT("m1:%d"), GetScriptType());
//	//wxString outArray = wxEmptyString;
//	//for(size_t xi = 0; xi < GetVarList().size(); xi++)
//	//{
//	//	outArray.Append(GetVarList().Item(xi));
//	//	outArray.Append(wxT("%:%"));
//	//}
//	//if(outArray == wxEmptyString) retString += wxT("m2:NULL");
//	//	else retString += wxT("m2:") + outArray;
//	return retString; 
//}
////**************************************************
//void CScriptAction::UnPackValuesFromString(wxString value)
//{
//	size_t startpos = 0;
//	size_t endpos = 0;
//	wxString valStr;
//	for(int i = 1; i < 15; i++)
//	{
//		size_t len = wxString::Format(wxT("m%d:"),i).Length();
//		startpos = value.find(wxString::Format(wxT("m%d:"),i), startpos);
//		if(startpos > value.Len())
//			break;
//		endpos	 = value.find(wxString::Format(wxT("m%d:"),i+1), startpos);
//		valStr	 = value.Mid(startpos+len, endpos-startpos-len);
//		if(valStr == wxT("NULL"))
//			valStr = wxEmptyString;
//
//		switch(i)
//		{
//		case 1:
//			{
//				ULONG val = 0; valStr.ToULong(&val);
//				SetScriptType(eScriptType((int)val));
//			}
//			break;
//		case 2:
//			{
//				wxArrayString tmpArr;
//				wxStringTokenizer tkz(valStr, wxT("%:%"));
//				while ( tkz.HasMoreTokens() )
//				{
//					tmpArr.Add(tkz.GetNextToken());
//				}
//				if(tmpArr.size() > 0)
//					SetVarList(tmpArr);
//			}
//			break;
//		}
//	}
//}
////**************************************************
void CScriptAction::Execute() 
{
	//Проверим все ли данные доступны
	//if(GetScript() == wxEmptyString)
	//	return false;

	//switch(GetScriptType())//Выбор исполняемой операции
	//{
	//	//Поиск письма по критериям
	//case eScriptType_Squirrel:
	//	{
	//		return RunSquirrelBuffer(bot);
	//	}
	//	break;
	//}
	//return false;
}
////**************************************************
//bool CScriptAction::RunSquirrelBuffer(CBot *bot)
//{
//	//SQInteger retval	= 0;
//
//	//SQInteger oldtop = sq_gettop(bot->GetSquirrel());
//	//sq_pushroottable(bot->GetSquirrel());
//
//	//wxString tmpStr, matchstring;
//	//wxRegEx reFile;
//	//reFile.Compile(wxT("([\$]+[[:digit:]]+)"));
//	//wxArrayString tmpArr = GetVarList();
//	//for(size_t xi = 0; xi < tmpArr.size(); xi++)
//	//{
//	//	tmpStr = tmpArr.Item(xi);
//	//	if ( reFile.Matches(tmpStr) )
//	//	{
//	//		matchstring = reFile.GetMatch(tmpStr, 1);
//	//		matchstring.Remove(0,1);
//	//		tmpStr.Replace(wxT("$"), wxT("var"));
//	//		long lmatchstring;
//	//		if(matchstring.ToLong(&lmatchstring))
//	//		{
//	//			CVar *var = (CVar *)NULL;
//	//			var = bot->GetVariables(lmatchstring);
//	//			if(var != NULL)
//	//			{
//	//				if(var->GetType() == Var_number)
//	//				{
//	//					long lvalue;
//	//					var->GetValue().ToLong(&lvalue);
//	//					sq_pushstring(bot->GetSquirrel(), tmpStr.c_str(), -1);
//	//					sq_pushfloat(bot->GetSquirrel(), lvalue);
//	//					sq_createslot(bot->GetSquirrel(),-3);
//	//				}
//	//				else if(var->GetType() == Var_string)
//	//				{
//	//					sq_pushstring(bot->GetSquirrel(), tmpStr.c_str(), -1);
//	//					sq_pushstring(bot->GetSquirrel(), var->GetValue(), -1);
//	//					sq_createslot(bot->GetSquirrel(),-3);
//	//				}
//	//				else if(var->GetType() == Var_bool)
//	//				{
//	//					sq_pushstring(bot->GetSquirrel(), tmpStr.c_str(), -1);
//	//					if(var->GetValue().Upper() == wxT("TRUE"))
//	//					{
//	//						sq_pushbool(bot->GetSquirrel(), true);
//	//					}
//	//					else
//	//					{
//	//						sq_pushbool(bot->GetSquirrel(), false);
//	//					}
//	//					sq_createslot(bot->GetSquirrel(),-3);
//	//				}
//	//			}
//	//			else
//	//			{
//	//				sq_pushstring(bot->GetSquirrel(), tmpStr.c_str(), -1);
//	//				sq_pushstring(bot->GetSquirrel(), _SC(""), -1);
//	//				sq_createslot(bot->GetSquirrel(),-3);
//	//			}
//	//		}
//	//	}
//	//}
//
//	//sq_pop(bot->GetSquirrel(),1);
//	//if(SQ_SUCCEEDED(sq_compilebuffer(bot->GetSquirrel(), GetScript().c_str(), GetScript().Len(), _SC("script compiler"), SQTrue)))
//	//{
//	//	sq_pushroottable(bot->GetSquirrel());
//	//	if(SQ_SUCCEEDED(sq_call(bot->GetSquirrel(), 1, retval, SQTrue)) &&	retval)
//	//	{
//	//		scprintf(_SC("\n"));
//	//		sq_pushroottable(bot->GetSquirrel());
//	//		sq_pushstring(bot->GetSquirrel(), _SC("print"), -1);
//	//		sq_get(bot->GetSquirrel(), -2);
//	//		sq_pushroottable(bot->GetSquirrel());
//	//		sq_push(bot->GetSquirrel(), -4);
//	//		sq_call(bot->GetSquirrel(), 2, SQFalse, SQTrue);
//	//		retval = 0;
//	//		scprintf(_SC("\n"));
//	//	}
//	//}
//
//	//SQBool sqBool = false;
//	//SQInteger sqInt = 0;
//	//SQFloat sqFlt	= 0;
//	//const SQChar *sqChr;
//	//for(size_t xi = 0; xi < tmpArr.size(); xi++)
//	//{
//	//	tmpStr = tmpArr.Item(xi);
//	//	if ( reFile.Matches(tmpStr) )
//	//	{
//	//		matchstring = reFile.GetMatch(tmpStr, 1);
//	//		matchstring.Remove(0,1);
//	//		tmpStr.Replace(wxT("$"), wxT("var"));
//	//		long lmatchstring;
//	//		if(matchstring.ToLong(&lmatchstring))
//	//		{
//	//			CVar *var = (CVar *)NULL;
//	//			var = bot->GetVariables(lmatchstring);
//	//			if(var != NULL)
//	//			{
//	//				sq_pushstring( bot->GetSquirrel(), tmpStr.c_str(), -1 );
//	//				if( SQ_FAILED(sq_get(bot->GetSquirrel(),-3)) ) 
//	//				{
//	//					sq_poptop( bot->GetSquirrel() );
//	//					return false;
//	//				}
//	//				SQObjectType sqot = sq_gettype(bot->GetSquirrel(), -1);
//	//				switch(sqot)
//	//				{
//	//				case OT_INTEGER:
//	//					{
//	//						sq_getinteger(bot->GetSquirrel(), -1, &sqInt);
//	//						var->SetType(Var_number);
//	//						var->SetValue(wxString::Format(wxT("%d"), sqInt));
//	//					}
//	//					break;
//	//				case OT_FLOAT:
//	//					{
//	//						sq_getfloat(bot->GetSquirrel(), -1, &sqFlt);
//	//						var->SetType(Var_number);
//	//						var->SetValue(wxString::Format(wxT("%f"), sqFlt));
//	//					}
//	//					break;
//	//				case OT_NULL:
//	//				case OT_STRING:
//	//					{
//	//						sq_getstring(bot->GetSquirrel(), -1, &sqChr);
//	//						var->SetType(Var_string);
//	//						var->SetValue(wxString(sqChr));
//	//					}
//	//					break;
//	//				case OT_BOOL:
//	//					{
//	//						sq_getbool(bot->GetSquirrel(), -1, &sqBool);
//	//						var->SetType(Var_bool);
//	//						if(sqBool)
//	//						{
//	//							var->SetValue(wxT("true"));
//	//						}
//	//						else
//	//						{
//	//							var->SetValue(wxT("false"));
//	//						}
//	//					}
//	//					break;
//	//				}
//	//				sq_pop(bot->GetSquirrel(), 1);
//	//			}
//	//		}
//	//	}
//	//}
//	//sq_settop(bot->GetSquirrel(),oldtop);
//
//	return true;
//}