#ifndef __TREEFRAME_H__
#define __TREEFRAME_H__
//**************************************************
#include "AutoBot.h"
//**************************************************
class TreeItemData : public wxTreeItemData
{
private:
	GUID m_GUID;
	bool m_isNode;	//True - node, False - bot
public:
	TreeItemData(GUID guid, bool isNode)
	{
		m_GUID = guid;
		m_isNode = isNode;
	}

	GUID GetGUID() { return m_GUID; }
	bool isNode() { return m_isNode; }
};
//**************************************************
class CTreeFrame : public wxTreeCtrl
{
public:
	CTreeFrame() {}
	CTreeFrame(wxWindow *parent, const wxWindowID id,
			   const wxPoint& pos, const wxSize& size, long style);
	~CTreeFrame() {};

	void OnBeginDrag(wxTreeEvent& event);			//������ �������������� ����� �������� ����
	void OnEndDrag(wxTreeEvent& event);				//����� ��������������
	void OnBeginLabelEdit(wxTreeEvent& event);		//������ �������������� ������� ��������
	void OnEndLabelEdit(wxTreeEvent& event);		//����� �������������� ������� ��������
	void OnDeleteItem(wxCommandEvent& event);		//�������� ��������
	void OnContextMenu(wxContextMenuEvent& event);	//����� ������������ ����
	void OnItemMenu(wxTreeEvent& event);			//����� �������� ������������ ����
	void OnItemActivated(wxTreeEvent& event);		//����� �����������, ������� ������ ����� ��� �����������
	void OnItemRClick(wxTreeEvent& event);			//������� �� ������ ������ ������� ����
	void OnSelChanged(wxTreeEvent& event);			//���������� ����������
	void OnSelChanging(wxTreeEvent& event);			//���������� ����������
	void OnRMouseDown(wxMouseEvent& event);			//������� ������� ���� �� ��������
	void OnRMouseUp(wxMouseEvent& event);			//���������� ������ ���� �� ��������

	void OnAddItem(wxCommandEvent& event);
	void OnStartItem(wxCommandEvent& event);
	void OnStopItem(wxCommandEvent& event);
	void OnPauseItem(wxCommandEvent& event);
	void OnAddSubItem(wxCommandEvent& event);
	void OnSelectSubItem();
	//void OnItemExpanded(wxTreeEvent& event);		//����� ��������
	//void OnItemExpanding(wxTreeEvent& event);		//����� �����������, ����� ��������� ��������� ����� Veto()
	//void OnItemCollapsed(wxTreeEvent& event);		//����� �������
	//void OnItemCollapsing(wxTreeEvent& event);	//����� �������������
	void OnTreeKeyDown(wxTreeEvent& event);		//������� ������� ���������� �� ������
	//void OnBeginRDrag(wxTreeEvent& event);		//������ �������������� ������ �������� ����
	//void OnRMouseDClick(wxMouseEvent& event);		//������� ������� ������ ���� �� ��������

	void CreateImageList(int size = 16);
	void CreateButtonsImageList(int size = 11);

	void DoSortChildren(const wxTreeItemId& item, bool reverse = false)
		{ m_reverseSort = reverse; wxTreeCtrl::SortChildren(item); }
	void DoEnsureVisible() { if (m_lastItem.IsOk()) EnsureVisible(m_lastItem); }
	void ShowMenu(wxTreeItemId id, const wxPoint& pt);
	int ImageSize(void) const { return m_imageSize; }
	void RefreshTree();
	void SetLastItem(wxTreeItemId id) { m_lastItem = id; }
	void SetDraggedItem(wxTreeItemId value) { m_draggedItem = value; }
	wxTreeItemId GetDraggedItem() { return m_draggedItem; }
protected:
	virtual int OnCompareItems(const wxTreeItemId& i1, const wxTreeItemId& i2);
private:
	int			 m_imageSize;			// current size of images
	bool		 m_reverseSort;			// flag for OnCompareItems
	wxTreeItemId m_lastItem,			// for OnEnsureVisible()
				 m_draggedItem;			// item being dragged right now
	wxImageList	 *m_images;

	DECLARE_DYNAMIC_CLASS(CTreeFrame)
	DECLARE_EVENT_TABLE()
};
//**************************************************
#endif	//__TREEFRAME_H__