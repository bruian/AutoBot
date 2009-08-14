#ifndef __GRIDFRAME_H__
#define __GRIDFRAME_H__
#include "AutoBot.h"
//**************************************************
class CGridCellAttrProvider : public wxGridCellAttrProvider
{
public:
	CGridCellAttrProvider();
	virtual ~CGridCellAttrProvider();
	virtual wxGridCellAttr *GetAttr(int row, int col, wxGridCellAttr::wxAttrKind  kind) const;
private:
	wxGridCellAttr *m_attrForOddRows;
};
//**************************************************
class CGrid : public wxGrid
{
public:
	CGrid(){}
	CGrid(wxWindow *parent, const wxWindowID id,
		  const wxPoint& pos, const wxSize& size, long style);
	void OnCellValueChanged( wxGridEvent& );
	void OnCellValueSelect( wxGridEvent& ev );
	void RefreshTable();
	DECLARE_DYNAMIC_CLASS(CGrid)
	DECLARE_EVENT_TABLE()
};
//**************************************************
#endif //__GRIDFRAME_H__