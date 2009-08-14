//**************************************************
#include "PropertyFrame.h"
//**************************************************
BEGIN_EVENT_TABLE(CPropertyFrame, wxPropertyGrid)
	EVT_PG_CHANGED(Prop_Ctrl, CPropertyFrame::OnPropertyGridChange)
	//EVT_TEXT(Prop_Ctrl, CPropertyFrame::OnPropertyGridTextUpdate )
END_EVENT_TABLE()
//**************************************************
class wxTimeValidator : public wxValidator
{
public:
	wxTimeValidator() : wxValidator()
	{
	}

	virtual wxObject* Clone() const
	{
		return new wxTimeValidator();
	}

	virtual bool Validate(wxWindow* WXUNUSED(parent))
	{
		wxTextCtrl* tc = wxDynamicCast(GetWindow(), wxTextCtrl);
		wxCHECK_MSG(tc, true, wxT("validator window must be wxTextCtrl"));

		wxString val = tc->GetValue();

		if((val.Length() < 8) || (val.Length() > 8))
			return false;

		wxString sHour(val, 0, 2), sMinute(val, 3, 2), sSecond(val, 6, 2), sDiv1(val, 2, 1), sDiv2(val, 5, 1);
		double dHour, dMinute, dSecond;
		if((!sHour.ToDouble(&dHour)) || (!sMinute.ToDouble(&dMinute)) || (!sSecond.ToDouble(&dSecond)))
			return false;

		if(((dHour < 0)||(dHour > 24)) || ((dMinute < 0)||(dMinute > 59)) || ((dSecond < 0)||(dSecond > 59)))
			return false;

		if((sDiv1 != wxT(":")) || (sDiv2 != wxT(":")))
			return false;

		return true;
	}
};
//**************************************************
void CPropertyFrame::ExpandXMLNode(wxXmlNode *GetChildren, wxPropertyGrid *propgrid, CRule *rule, wxPGId *pgid = (wxPGId *)NULL)
{
	CAppProperties *ap = wxGetApp().GetAppProperties();
	if(GetChildren)
	{
		while(GetChildren)
		{
			if(GetChildren->GetName() == wxT("category"))
			{
				wxString label = GetChildren->GetPropVal(wxT("label"), wxEmptyString);
				wxString criterion = GetChildren->GetPropVal(wxT("criterion"), wxEmptyString);
				wxPGId *newPgid = (wxPGId *)NULL;
				if(rule->CriterionOK(criterion))
				{
					if(pgid == NULL)
					{
						newPgid = &propgrid->AppendCategory(ap->GetLngRes(label), label);
						propgrid->SetPropertyHelpString(*newPgid, ap->GetLngRes(label));
						ExpandXMLNode(GetChildren->GetChildren(), propgrid, rule, newPgid);
					}
					else
					{
						newPgid = &propgrid->AppendIn(*pgid, wxPropertyCategory(ap->GetLngRes(label), label));
						propgrid->SetPropertyHelpString(*newPgid, ap->GetLngRes(label));
						ExpandXMLNode(GetChildren->GetChildren(), propgrid, rule, newPgid);
					}
				}
			}
			else if(GetChildren->GetName() == wxT("parameter"))
			{
				wxString name		= GetChildren->GetPropVal(wxT("name"), wxEmptyString);
				wxString label		= GetChildren->GetPropVal(wxT("label"), wxEmptyString);
				wxString criterion	= GetChildren->GetPropVal(wxT("criterion"), wxEmptyString);
				if(!rule->CriterionOK(criterion))
				{
					GetChildren = GetChildren->GetNext();
					continue;
				}
				wxArrayString enums;
				wxArrayInt enums_values;
				wxString style;
				wxString value;
				wxString typevalue;

				wxXmlNode *subparam = GetChildren->GetChildren();
				while(subparam)
				{
					if(subparam->GetName() == wxT("style"))
						style = subparam->GetChildren()->GetContent();
					else if(subparam->GetName() == wxT("value"))
					{
						typevalue	= subparam->GetPropVal(wxT("type"), wxEmptyString);
						value		= subparam->GetChildren()->GetContent();
					}
					else if(subparam->GetName() == wxT("enumvalue"))
					{
						wxXmlNode *enumparam = subparam->GetChildren();
						while(enumparam)
						{
							if(enumparam->GetName() == wxT("enum"))
							{
								enums.Add(ap->GetLngRes(enumparam->GetPropVal(wxT("label"), wxEmptyString)));
								if(style ==  wxT("wxFlagsProperty"))
								{
									double dVal = 0;
									enumparam->GetChildren()->GetContent().ToDouble(&dVal);
									enums_values.Add(dVal);
								}
							}
							else if(enumparam->GetName() == wxT("value"))
							{
								typevalue	= enumparam->GetPropVal(wxT("type"), wxEmptyString);
								value		= enumparam->GetChildren()->GetContent();
							}
							enumparam = enumparam->GetNext();
						}
					}
					subparam = subparam->GetNext();
				}

				if(name == wxT("m_GUID"))
					value = ap->GuidToStr(rule->GetGUIDRule());
				else
					value = rule->GetValue(value);

				bool valueBool = true;
				long valueNumb = NULL;
				if(typevalue == wxT("bool"))
				{
					if((value == wxT("false")) || (value == wxT("0")) || (value == wxEmptyString))
						valueBool = false;
				}
				else if(typevalue == wxT("number"))
				{
					value.ToLong(&valueNumb);
				}

				if(style == wxT("wxBoolProperty"))
				{
					wxPGId pgi = propgrid->AppendIn(*pgid, wxBoolProperty(ap->GetLngRes(label), name, valueBool));
					propgrid->SetPropertyHelpString(pgi, ap->GetLngRes(label));
				}
				else if(style == wxT("wxEnumProperty"))
				{
					wxPGId pgi = propgrid->AppendIn(*pgid, wxEnumProperty(ap->GetLngRes(label), name, enums, valueNumb));
					propgrid->SetPropertyHelpString(pgi, ap->GetLngRes(label));
				}
				else if(style ==  wxT("wxFlagsProperty"))
				{
					wxPGId pgi = propgrid->AppendIn(*pgid, wxFlagsProperty(ap->GetLngRes(label), name, enums, enums_values, valueNumb));
					propgrid->SetPropertyHelpString(pgi, ap->GetLngRes(label));
				}
				else
				{
					wxPGId pgi;
					if(typevalue == wxT("Date"))
					{
						wxDateTime dt;
						if(!rule->ConvStrToDate(value, &dt))
							dt = wxDateTime::Now();
						pgi = propgrid->AppendIn(*pgid, wxDateProperty(ap->GetLngRes(label), name, dt));
						propgrid->SetPropertyAttribute(pgi, wxPG_DATE_PICKER_STYLE, (long)(wxDP_DROPDOWN | wxDP_SHOWCENTURY) );
						propgrid->SetPropertyAttribute(pgi, wxPG_DATE_FORMAT, (wxString)(wxT("%d.%m.%Y")) );
					}
					else if(typevalue == wxT("Time"))
					{
						//wxDateTime dt(10,10,10,0);
						//dt.Format(wxDefaultTimeSpanFormat);
						//wxPGProperty *dprop = wxDateProperty(ap->GetLngRes(label), name, dt);
						//(wxDateProperty*)dprop->SetFormat(wxDefaultTimeSpanFormat);
						//propgrid->SetPropertyAttribute(pgi, wxPG_DATE_PICKER_STYLE, (long)(wxDP_SPIN) );
						//propgrid->SetPropertyAttribute(pgi, wxPG_DATE_FORMAT, (wxString)(wxT("%H:%M:%S")/*wxDefaultTimeSpanFormat*/) );
						//wxTimeValidator TValidator();
						wxDateTime dt;
						if(!rule->ConvStrToTime(value, &dt))
							dt = wxDateTime::Now();
						wxString sHour, sMinute, sSecond;
						sHour.Printf(wxT("%d"), dt.GetHour());
						sMinute.Printf(wxT("%d"), dt.GetMinute());
						sSecond.Printf(wxT("%d"), dt.GetSecond());
						if(dt.GetHour() == 0)
							sHour.Append(wxT("0"));
						if(dt.GetMinute() == 0)
							sMinute.Append(wxT("0"));
						if(dt.GetSecond() == 0)
							sSecond.Append(wxT("0"));
						wxPGProperty *dprop = wxStringProperty(ap->GetLngRes(label), name, wxString::Format(wxT("%s:%s:%s"), sHour, sMinute, sSecond));
						pgi = propgrid->AppendIn(*pgid, dprop);
						propgrid->SetPropertyValidator(pgi, wxTimeValidator());
						propgrid->SetPropertyEditor(pgi, wxPG_EDITOR(TextCtrl));
					}
					else
						pgi = propgrid->AppendIn(*pgid, wxStringProperty(ap->GetLngRes(label), name, value));
					propgrid->SetPropertyHelpString(pgi, ap->GetLngRes(label));
				}
			}
			GetChildren = GetChildren->GetNext();
		}
	}
}
//**************************************************
void CPropertyFrame::SetValueXMLNode(wxXmlNode *GetChildren, CRule *rule, wxString NameValue, wxPropertyGridEvent *event, bool *Refresh)
{
	if(GetChildren)
	{
		while(GetChildren)
		{
			if(GetChildren->GetName() == wxT("category"))
			{
				wxString criterion = GetChildren->GetPropVal(wxT("criterion"), wxEmptyString);
				if(rule->CriterionOK(criterion))
				{
					SetValueXMLNode(GetChildren->GetChildren(), rule, NameValue, event, Refresh);
				}
			}
			else if(GetChildren->GetName() == wxT("parameter"))
			{
				wxString name		= GetChildren->GetPropVal(wxT("name"), wxEmptyString);
				if(name != NameValue)
				{
					GetChildren = GetChildren->GetNext();
					continue;
				}
				wxString criterion	= GetChildren->GetPropVal(wxT("criterion"), wxEmptyString);
				if(!rule->CriterionOK(criterion))
				{
					GetChildren = GetChildren->GetNext();
					continue;
				}
				wxString style;
				wxString value;
				wxString typevalue;

				wxXmlNode *subparam = GetChildren->GetChildren();
				while(subparam)
				{
					if(subparam->GetName() == wxT("style"))
						style = subparam->GetChildren()->GetContent();
					else if(subparam->GetName() == wxT("value"))
					{
						typevalue = subparam->GetPropVal(wxT("type"), wxEmptyString);
						value = subparam->GetChildren()->GetContent();
					}
					else if(subparam->GetName() == wxT("enumvalue"))
					{
						wxXmlNode *subEnum = subparam->GetChildren();
						while(subEnum)
						{
							if(subEnum->GetName() == wxT("value"))
							{
								typevalue = subEnum->GetPropVal(wxT("type"), wxEmptyString);
								value = subEnum->GetChildren()->GetContent();
							}
							subEnum = subEnum->GetNext();
						}
					}
					subparam = subparam->GetNext();
				}

				if(style == wxT("wxBoolProperty"))
				{
					rule->SetValue(value, event->GetPropertyValueAsString());
					*Refresh = true;
				}
				else if(style == wxT("wxEnumProperty"))
				{
					wxString str; str.Printf(wxT("%d"), event->GetPropertyValueAsInt());
					rule->SetValue(value, str);
					*Refresh = true;
				}
				else if(style == wxT("wxFlagsProperty"))
				{
					wxString str; str.Printf(wxT("%d"), event->GetPropertyValueAsInt());
					rule->SetValue(value, str);
					*Refresh = false;
				}
				else
				{
					*Refresh = false;
					rule->SetValue(value, event->GetPropertyValueAsString());
				}
				
				event->Skip();
				return;
			}
			GetChildren = GetChildren->GetNext();
		}
	}
}
//**************************************************
//Обновить панель свойств
void CPropertyFrame::RefreshPropertyGrid(ePropertyType propertytype)
{
	wxPropertyGrid* myProp = wxGetApp().GetPropertyFrame();
	if(myProp == NULL)
		return;
	myProp->GetGrid()->Clear();

	CAppProperties *ap = wxGetApp().GetAppProperties();
	switch (propertytype)
	{
	case BotTree:
		break;
	case BotNode:
		{
			CNodeBot *nodebot = wxGetApp().GetTreeBot()->GetActiveNode();
			if(!nodebot)
				break;
			myProp->Append(wxPropertyCategory(ap->GetLngRes(wxT("lr_NodeParametr")), wxT("Node_Property")));
			myProp->Append(wxStringProperty(wxT("GUID"), wxPG_LABEL, ap->GuidToStr(nodebot->GetGUIDNode())));
			myProp->SetPropertyReadOnly(wxT("Node_Property.GUID"), true);
			myProp->Append(wxStringProperty(ap->GetLngRes(wxT("lr_name")), wxT("Node_Name"), nodebot->GetNodeName()));
			switch(nodebot->GetNodeType())
			{
			case eNodeType_Main:
			case eNodeType_Import:
			case eNodeType_Net:
			case eNodeType_FolderUnderRoot:
			case eNodeType_Templates:
				myProp->Append(wxFileProperty(ap->GetLngRes(wxT("lr_NodePath")), wxT("Node_Path"), nodebot->GetNodePath()));
				break;
			}
		}
		break;
	case Bot:
		{
			CBot* myBot = wxGetApp().GetTreeBot()->GetActiveBot();
			if(myBot == NULL)
				break;
			myProp->Append(wxPropertyCategory(ap->GetLngRes(wxT("lr_prpgridBotPrp")),wxT("Bot_Property")));
			myProp->Append(wxStringProperty(wxT("GUID"), wxPG_LABEL, ap->GuidToStr(myBot->GetGUIDBot())));
			myProp->SetPropertyReadOnly(wxT("Bot_Property.GUID"), true );
			myProp->Append(wxStringProperty(ap->GetLngRes(wxT("lr_name")), wxT("Bot_Name"), myBot->GetNameBot()));
			static const wxChar* statusMode[] = {wxT("STOP"),
												 wxT("PLAY"),
												 wxT("PAUSE"),
												 wxT("ERROR"),
												 (const wxChar*) NULL};
			myProp->Append(wxStringProperty(ap->GetLngRes(wxT("lr_status")),wxT("Bot_StatusMode"),statusMode[myBot->GetStatusBot()]));
			myProp->SetPropertyReadOnly(wxT("Bot_Property.Bot_StatusMode"), true );
			myProp->Append(wxStringProperty(ap->GetLngRes(wxT("lr_description")), wxT("Bot_Description"), myBot->GetDescrBot()));
		}
		break;
	case Rule:
		{
			CRule* myRule = wxGetApp().GetTreeBot()->GetActiveRule();
			if(myRule == NULL)
				break;

			wxXmlDocument *doc = myRule->GetComponent()->GetXMLStruct();
			if(!doc->IsOk())
				return;
			if (doc->GetRoot()->GetName() != wxT("componentStructure"))
				return;
			wxXmlNode *object = doc->GetRoot()->GetChildren();
			if(object)
			{
				if(object->GetName() == wxT("object"))
				{
					wxString content		= object->GetNodeContent();
					wxString objectClass	= object->GetPropVal(wxT("class"), wxEmptyString);
					wxString objectName		= object->GetPropVal(wxT("name"), wxEmptyString);
					wxString objectGuid		= object->GetPropVal(wxT("guid"), wxEmptyString);
					ExpandXMLNode(object->GetChildren(), myProp, myRule);
				}
			}
		}
		break;
	default:
		break;
	}
}
//**************************************************
void CPropertyFrame::OnPropertyGridChange(wxPropertyGridEvent& event)
{
	if(!this->AcceptsFocus())
		return;
	const wxString &name = event.GetPropertyName();
	//wxPGId id = event.GetProperty();
	//if(name.g)
	//	wxLogDebug(wxT("Changed value"));
	//wxVariant value = event.GetPropertyValue();
	//   wxLogDebug( wxT("Changed value of '%s' to '%s'"),
	//               name.c_str(),
	//               event.GetPropertyValueAsString().c_str() );

	CBot *bot = wxGetApp().GetTreeBot()->GetActiveBot();
	if(bot)
	{
		if (wxT("Bot_Name") == name)
		{
			wxString val = event.GetPropertyValueAsString();
			bot->SetNameBot(val);
			wxGetApp().GetTreeFrame()->SetItemText(wxGetApp().GetTreeFrame()->GetSelection(), val);
			event.Skip();
			return;
		}
		else if (wxT("Bot_Description") == name)
		{
			bot->SetDescrBot(event.GetPropertyValueAsString());
			event.Skip();
			return;
		}
	}
	
	if(wxT("Node_Name") == name)
	{
		CNodeBot *nodebot = wxGetApp().GetTreeBot()->GetActiveNode();
		nodebot->SetNodeName(event.GetPropertyValueAsString());
	}
	else if(wxT("Node_Path") == name)
	{
		CNodeBot *nodebot = wxGetApp().GetTreeBot()->GetActiveNode();
		nodebot->SetNodePath(event.GetPropertyValueAsString());
	}
	
	CRule* rule = wxGetApp().GetTreeBot()->GetActiveRule();
	if(rule == NULL)
		return;

	if(rule->GetComponent() != NULL)
	{
		wxXmlDocument *doc = rule->GetComponent()->GetXMLStruct();
		if(!doc->IsOk())
			return;
		if (doc->GetRoot()->GetName() != wxT("componentStructure"))
			return;
		wxXmlNode *object = doc->GetRoot()->GetChildren();
		if(object)
		{
			if(object->GetName() == wxT("object"))
			{
				wxString content		= object->GetNodeContent();
				wxString objectClass	= object->GetPropVal(wxT("class"), wxEmptyString);
				wxString objectName		= object->GetPropVal(wxT("name"), wxEmptyString);
				wxString objectGuid		= object->GetPropVal(wxT("guid"), wxEmptyString);
				bool Refresh = false;
				SetValueXMLNode(object->GetChildren(), rule, name, &event, &Refresh);
				if(Refresh)
					RefreshPropertyGrid(Rule);
			}
		}
		//event.Skip();
		return;
	}
	event.Skip();
	//else if (wxT("Action_Expression") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->SetExpression(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionDlg_TypeDlg") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	CWindowDialog *wd = rule->GetWindowDialog();
	//	wd->SetDialogType(eDialogType(event.GetPropertyValueAsInt()));
	//	RefreshPropertyGrid(Rule);
	//}
	//else if (wxT("ActionDlg_Style") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowDialog()->SetDialogStyle(event.GetPropertyValueAsLong());
	//}
	//else if (wxT("ActionDlg_Label") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowDialog()->SetDialogTitle(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionDlg_Message") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowDialog()->SetDialogMessage(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionDlg_Answer") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowDialog()->SetAnswerId(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionFndWin_Answer") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowSearch()->SetAnswerId(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionFndWin_CLabel") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowSearch()->SetCWindowTitle(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionFndWin_CName") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowSearch()->SetCWindowText(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionFndWin_CPID") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowSearch()->SetCHandlePID(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionFndWin_CHandleWin") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowSearch()->SetCHandleWindow(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionFndWin_CHandleWinParent") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowSearch()->SetCHandleWindowParent(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionFndWin_RLabel") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowSearch()->SetRWindowTitle(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionFndWin_RName") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowSearch()->SetRWindowText(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionFndWin_RPID") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowSearch()->SetRHandlePID(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionFndWin_RHandleWin") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowSearch()->SetRHandleWindow(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionFndWin_RHandleWinParent") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowSearch()->SetRHandleWindowParent(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionFndWin_CWindowClass") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowSearch()->SetCWindowClass(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionFndWin_RWindowClass") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowSearch()->SetRWindowClass(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionFndWin_FndMethod") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	if(event.GetPropertyValueAsBool())
	//		rule->GetWindowSearch()->SetFindMethod(true);
	//	else
	//		rule->GetWindowSearch()->SetFindMethod(false);
	//}
	//else if (wxT("ActionFndWin_Msg") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	if(event.GetPropertyValueAsBool())
	//		rule->GetWindowSearch()->SetSendMessage(true);
	//	else
	//		rule->GetWindowSearch()->SetSendMessage(false);
	//	RefreshPropertyGrid(Rule);
	//}
	//else if (wxT("ActionFndWin_TypeMsg") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetWindowSearch()->SetMessageType(event.GetPropertyValueAsLong());
	//}
	//else if (wxT("ActionProcess_ProgramName") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetProcessAction()->SetProgramName(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionProcess_ProcessType") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetProcessAction()->SetProcessType(eProcessType(event.GetPropertyValueAsInt()));
	//	RefreshPropertyGrid(Rule);
	//}
	//else if (wxT("ActionProcess_CPID") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetProcessAction()->SetCPID(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionProcess_RPID") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetProcessAction()->SetRPID(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionProcess_Answer") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetProcessAction()->SetFndResult(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionProcess_RHandleWin") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetProcessAction()->SetRHandleWindow(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionFile_OperType") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetFileAction()->SetFileOperations(eFileOperations(event.GetPropertyValueAsInt()));
	//	RefreshPropertyGrid(Rule);
	//}
	//else if (wxT("ActionFile_Overwrite") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	if(event.GetPropertyValueAsBool())
	//		rule->GetFileAction()->SetOverwrite(true);
	//	else
	//		rule->GetFileAction()->SetOverwrite(false);
	//}
	//else if (wxT("ActionFile_To") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetFileAction()->SetTo(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionFile_From") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetFileAction()->SetFrom(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionFile_Result") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetFileAction()->SetOperationResult(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionMail_OperType") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetMailAction()->SetMailOperations(eMailOperations(event.GetPropertyValueAsInt()));
	//	RefreshPropertyGrid(Rule);
	//}
	//else if (wxT("ActionMail_SrvAddress") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetMailAction()->SetMailServer(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionMail_Login") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetMailAction()->SetMailLogin(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionMail_Password") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetMailAction()->SetMailPassword(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionMail_FromLetter") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetMailAction()->SetEMailFrom(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionMail_ThemeLetter") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetMailAction()->SetMailTheme(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionMail_FindResult") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetMailAction()->SetOperationResult(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionMail_MailId") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetMailAction()->SetMailId(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionMail_SrvPort") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetMailAction()->SetMailPort(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionMail_MailAttach") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetMailAction()->SetMailAttach(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionMail_ToLetter") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetMailAction()->SetEMailTo(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionProcess_WaitEndProcess") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	if(event.GetPropertyValueAsBool())
	//		rule->GetProcessAction()->SetWaitEndProcess(true);
	//	else
	//		rule->GetProcessAction()->SetWaitEndProcess(false);
	//}
	//else if (wxT("ActionMail_REMailFrom") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetMailAction()->SetREMailFrom(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionMail_RMailTheme") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetMailAction()->SetRMailTheme(event.GetPropertyValueAsString());
	//}
	//else if (wxT("ActionScript_ScriptType") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	rule->GetScriptAction()->SetScriptType(eScriptType(event.GetPropertyValueAsInt()));
	//}
	//else if (wxT("ActionScript_ScriptVar") == name)
	//{
	//	CRule* rule = bot->GetActiveRule();
	//	if(rule == NULL)
	//		return;
	//	
	//	rule->GetScriptAction()->SetVarList(event.GetPropertyValueAsArrayString());
	//	RefreshPropertyGrid(Rule);
	//}
}