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

	void OnBeginDrag(wxTreeEvent& event);			//Начало перетаскивания левой клавишей мыши
	void OnEndDrag(wxTreeEvent& event);				//Конец перетаскивания
	void OnBeginLabelEdit(wxTreeEvent& event);		//Начало редактирования надписи элемента
	void OnEndLabelEdit(wxTreeEvent& event);		//Конец редактирования надписи элемента
	void OnDeleteItem(wxCommandEvent& event);		//Удаление элемента
	void OnContextMenu(wxContextMenuEvent& event);	//Вызов контекстного меню
	void OnItemMenu(wxTreeEvent& event);			//Выбор элемента контекстного меню
	void OnItemActivated(wxTreeEvent& event);		//Пункт активирован, двойной щелчок мышью или клавиатурой
	void OnItemRClick(wxTreeEvent& event);			//Нажатие на пункте правой кнопкой мыши
	void OnSelChanged(wxTreeEvent& event);			//Выделенное изменилось
	void OnSelChanging(wxTreeEvent& event);			//Выделенное изменяется
	void OnRMouseDown(wxMouseEvent& event);			//Нажатие кнопкой мыши на элементе
	void OnRMouseUp(wxMouseEvent& event);			//Отпускание кнопки мыши на элементе

	void OnAddItem(wxCommandEvent& event);
	void OnStartItem(wxCommandEvent& event);
	void OnStopItem(wxCommandEvent& event);
	void OnPauseItem(wxCommandEvent& event);
	void OnAddSubItem(wxCommandEvent& event);
	void OnSelectSubItem();
	//void OnItemExpanded(wxTreeEvent& event);		//Пункт расширен
	//void OnItemExpanding(wxTreeEvent& event);		//Пункт расширяется, можно запретить используя метот Veto()
	//void OnItemCollapsed(wxTreeEvent& event);		//Пункт свернут
	//void OnItemCollapsing(wxTreeEvent& event);	//Пункт сворачивается
	void OnTreeKeyDown(wxTreeEvent& event);		//Нажатие клавиши клавиатуры на дереве
	//void OnBeginRDrag(wxTreeEvent& event);		//Начало перетаскивания правой клавишей мыши
	//void OnRMouseDClick(wxMouseEvent& event);		//Двойное нажатие кнопки мыши на элементе

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