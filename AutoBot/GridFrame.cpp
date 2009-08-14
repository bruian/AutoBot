#include "GridFrame.h"
//**************************************************
CGridCellAttrProvider::CGridCellAttrProvider()
{
	m_attrForOddRows = new wxGridCellAttr;
	m_attrForOddRows->SetBackgroundColour(*wxLIGHT_GREY);
}
//**************************************************
CGridCellAttrProvider::~CGridCellAttrProvider()
{
	m_attrForOddRows->DecRef();
}
//**************************************************
wxGridCellAttr *CGridCellAttrProvider::GetAttr(int row, int col, wxGridCellAttr::wxAttrKind kind) const
{
	wxGridCellAttr *attr = wxGridCellAttrProvider::GetAttr(row, col, kind);

	if ( row % 2 )
	{
		if ( !attr )
		{
			attr = m_attrForOddRows;
			attr->IncRef();
		}
		else
		{
			if ( !attr->HasBackgroundColour() )
			{
				wxGridCellAttr *attrNew = attr->Clone();
				attr->DecRef();
				attr = attrNew;
				attr->SetBackgroundColour(*wxLIGHT_GREY);
			}
		}
	}

	return attr;
}
//**************************************************
CGrid::CGrid(wxWindow *parent, const wxWindowID id,
			 const wxPoint& pos, const wxSize& size, long style)
			 :wxGrid(parent, id, pos, size, style)
{
}
//**************************************************
BEGIN_EVENT_TABLE(CGrid, wxGrid)
	EVT_GRID_CELL_CHANGE( CGrid::OnCellValueChanged )
	EVT_GRID_SELECT_CELL( CGrid::OnCellValueSelect )
END_EVENT_TABLE()
//**************************************************
IMPLEMENT_DYNAMIC_CLASS(CGrid, wxGrid)
//**************************************************
void CGrid::OnCellValueSelect( wxGridEvent& ev )
{
	int row = ev.GetRow();
	
	if(GetCellValue(row, Col_type) == variables[0])
	{
		//SetCellEditor(row, Col_value, new wxGridCellChoiceEditor(WXSIZEOF(booleans), booleans));
		SetCellRenderer( row, Col_value, new wxGridCellBoolRenderer );
		SetCellEditor( row, Col_value, new wxGridCellBoolEditor );
	}
	else if (GetCellValue(row, Col_type) == variables[1])
	{
		SetCellRenderer(row, Col_value, new wxGridCellFloatRenderer);
		SetCellEditor(row, Col_value, new wxGridCellFloatEditor);
	}
	else if (GetCellValue(row, Col_type) == variables[2])
	{
		SetCellEditor(row, Col_value, new wxGridCellTextEditor());
	}
	ev.Skip();
}
//**************************************************
void CGrid::OnCellValueChanged( wxGridEvent& ev )
{
	int row = ev.GetRow(),
		col = ev.GetCol();
	
	wxString str = GetCellValue(row, Col_id);
	str.Remove(0,1);
	long val = NULL; 
	if(!str.ToLong(&val))
		return;
	CBot *bot = wxGetApp().GetTreeBot()->GetActiveBot();
	if(bot == NULL)
		return;
	CVar *var = bot->GetVariables(val);
	if(var == NULL)
		return;

	if (col == Col_type)
	{
		if(GetCellValue(row, col) == variables[0])
		{
			var->SetType(Var_bool);
			var->SetValue(GetCellValue(row, Col_value));
			SetCellRenderer( row, Col_value, new wxGridCellBoolRenderer );
			SetCellEditor( row, Col_value, new wxGridCellBoolEditor );
			//SetCellEditor(row, Col_value, new wxGridCellChoiceEditor(WXSIZEOF(booleans), booleans));
		}
		else if (GetCellValue(row, col) == variables[1])
		{
			var->SetType(Var_number);
			var->SetValue(GetCellValue(row, Col_value));
			SetCellRenderer(row, Col_value, new wxGridCellFloatRenderer);
			SetCellEditor(row, Col_value, new wxGridCellFloatEditor);
			//SetCellEditor(row, Col_value, new wxGridCellNumberEditor());
		}
		else if (GetCellValue(row, col) == variables[2])
		{
			var->SetType(Var_string);
			var->SetValue(GetCellValue(row, Col_value));
			SetCellEditor(row, Col_value, new wxGridCellTextEditor());
		}
	}
	else if(col == Col_value)
	{
		var->SetValue(GetCellValue(row, Col_value));
	}
	else if(col == Col_dscr)
	{
		var->SetDescription(GetCellValue(row, Col_dscr));
	}

	ev.Skip();
}
//**************************************************
void CGrid::RefreshTable()
{
	ClearGrid();
	if(GetRows() > 0)
		DeleteRows(0,GetRows());
	CBot *bot = wxGetApp().GetTreeBot()->GetActiveBot();
	if(bot == NULL)
		return;
	if(bot->GetTotalVariables() <= 0)	
		return;

	AppendRows(bot->GetTotalVariables());
	long l = 0;
	wxList::compatibility_iterator node = bot->GetListVariables().GetFirst();
	while (node)
	{
		CVar *var = (CVar *)node->GetData();

		SetCellValue( l, Col_id, wxString().Format(wxT("$%u"),var->GetId()));
		SetReadOnly( l, Col_id );
		SetCellValue( l, Col_dscr, var->GetDescription());
		SetCellEditor(l, Col_type, new wxGridCellChoiceEditor(WXSIZEOF(variables), variables) );
		SetCellValue( l, Col_type, variables[var->GetType()] );

		if(var->GetType() == Var_bool)
		{
			SetCellRenderer( l, Col_value, new wxGridCellBoolRenderer );
			SetCellEditor( l, Col_value, new wxGridCellBoolEditor );
			SetCellValue( l, Col_value, var->GetValue() );
		}
		else if(var->GetType() == Var_number)
		{
			SetCellRenderer( l, Col_value, new wxGridCellFloatRenderer );
			SetCellEditor( l, Col_value, new wxGridCellFloatEditor );
			SetCellValue( l, Col_value, var->GetValue() );
		}
		else if(var->GetType() == Var_string)
		{
			SetCellEditor( l, Col_value, new wxGridCellTextEditor );
			SetCellValue( l, Col_value, var->GetValue() );
		}

		node = node->GetNext();
		l++;
	}
}