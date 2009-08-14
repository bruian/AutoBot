#ifndef __LISTFRAME_H__
#define __LISTFRAME_H__
//**************************************************
#include "AutoBot.h"
//**************************************************
class CListFrame: public wxListCtrl
{
public:
	CListFrame(){}
	CListFrame(wxWindow *parent, const wxWindowID id,
			   const wxPoint& pos, const wxSize& size, long style);
	~CListFrame()
	{
		if(m_imageListNormal != NULL)
			delete m_imageListNormal;
	};
	void OnBeginDrag(wxListEvent& event);
	void OnSelected(wxListEvent& event);
	void Deselect();
	void OnActivated(wxListEvent& event);
	void OnContextMenu(wxContextMenuEvent& event);
	void OnMouseEvent(wxMouseEvent& event);
	wxImageList* GetImageListNormal() { return m_imageListNormal; }
	void SetImageListNormal(wxImageList *value) { m_imageListNormal = value; }

	//void OnRightClick(wxMouseEvent& event);
	//void OnBeginRDrag(wxListEvent& event);
	//void OnBeginLabelEdit(wxListEvent& event);
	//void OnEndLabelEdit(wxListEvent& event);
	//void OnColClick(wxListEvent& event);
	//void OnColRightClick(wxListEvent& event);
	//void OnColBeginDrag(wxListEvent& event);
	//void OnColDragging(wxListEvent& event);
	//void OnColEndDrag(wxListEvent& event);
	//void OnDeleteItem(wxListEvent& event);
	//void OnDeleteAllItems(wxListEvent& event);
	//void OnListKeyDown(wxListEvent& event);
	//void OnFocused(wxListEvent& event);
	//void OnCacheHint(wxListEvent& event);
	//void OnChar(wxKeyEvent& event);
private:
	void SetColumnImage(int col, int image);
	void ShowContextMenu(const wxPoint& pos);
	virtual wxString OnGetItemText(long item, long column) const;
	virtual int OnGetItemColumnImage(long item, long column) const;

	wxImageList *m_imageListNormal;

	wxListItemAttr m_attr;
	DECLARE_DYNAMIC_CLASS(CListFrame)
	DECLARE_EVENT_TABLE()
};
#endif //__LISTFRAME_H__