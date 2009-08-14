//**************************************************
#include "TreeFrame.h"
//**************************************************
//Таблица сообщений
BEGIN_EVENT_TABLE(CTreeFrame, wxTreeCtrl)
	EVT_TREE_BEGIN_DRAG(Tree_Ctrl, CTreeFrame::OnBeginDrag)
	EVT_TREE_END_DRAG(Tree_Ctrl, CTreeFrame::OnEndDrag)
	EVT_TREE_BEGIN_LABEL_EDIT(Tree_Ctrl, CTreeFrame::OnBeginLabelEdit)
	EVT_TREE_END_LABEL_EDIT(Tree_Ctrl, CTreeFrame::OnEndLabelEdit)
	//EVT_TREE_DELETE_ITEM(Tree_Ctrl, CTreeFrame::OnDeleteItem)
	EVT_TREE_ITEM_ACTIVATED(Tree_Ctrl, CTreeFrame::OnItemActivated)
	// so many differents ways to handle right mouse button clicks...
	EVT_CONTEXT_MENU(CTreeFrame::OnContextMenu)
	// EVT_TREE_ITEM_MENU is the preferred event for creating context menus
	// on a tree control, because it includes the point of the click or item,
	// meaning that no additional placement calculations are required.
	EVT_TREE_ITEM_MENU(Tree_Ctrl, CTreeFrame::OnItemMenu)
	EVT_MENU(Tree_Add, CTreeFrame::OnAddItem)
	EVT_MENU(Tree_AddFolder, CTreeFrame::OnAddSubItem)
	EVT_MENU(Tree_Start, CTreeFrame::OnStartItem)
	EVT_MENU(Tree_Stop, CTreeFrame::OnStopItem)
	EVT_MENU(Tree_Pause, CTreeFrame::OnPauseItem)
	EVT_MENU(Tree_Delete,CTreeFrame::OnDeleteItem)
	//EVT_TREE_BEGIN_RDRAG(Tree_Ctrl, CTreeFrame::OnBeginRDrag)
	//EVT_TREE_ITEM_EXPANDED(Tree_Ctrl, CTreeFrame::OnItemExpanded)
	//EVT_TREE_ITEM_EXPANDING(Tree_Ctrl, CTreeFrame::OnItemExpanding)
	//EVT_TREE_ITEM_COLLAPSED(Tree_Ctrl, CTreeFrame::OnItemCollapsed)
	//EVT_TREE_ITEM_COLLAPSING(Tree_Ctrl, CTreeFrame::OnItemCollapsing)
	EVT_TREE_SEL_CHANGED(Tree_Ctrl, CTreeFrame::OnSelChanged)
	EVT_TREE_SEL_CHANGING(Tree_Ctrl, CTreeFrame::OnSelChanging)
	EVT_TREE_KEY_DOWN(Tree_Ctrl, CTreeFrame::OnTreeKeyDown)
	EVT_TREE_ITEM_RIGHT_CLICK(Tree_Ctrl, CTreeFrame::OnItemRClick)
	EVT_RIGHT_DOWN(CTreeFrame::OnRMouseDown)
	EVT_RIGHT_UP(CTreeFrame::OnRMouseUp)
	//EVT_RIGHT_DCLICK(CTreeFrame::OnRMouseDClick)
END_EVENT_TABLE()
//**************************************************
//Реализация класса CTreeFrame implementation
IMPLEMENT_DYNAMIC_CLASS(CTreeFrame, wxTreeCtrl)
//**************************************************
CTreeFrame::CTreeFrame(wxWindow *parent, const wxWindowID id,
						const wxPoint& pos, const wxSize& size, long style)
:wxTreeCtrl(parent, id, pos, size, style)
{
	m_reverseSort = false;

	CreateImageList();
}
//**************************************************
void CTreeFrame::CreateImageList(int size)
{
	if ( size == -1 )
	{
		SetImageList(NULL);
		return;
	}
	if ( size == 0 )
		size = m_imageSize;
	else
		m_imageSize = size;

	// Сделать image list содержащий маленькие иконки
	m_images = new wxImageList(size, size, true);

	CAppProperties *ap = wxGetApp().GetAppProperties();
	// должно соответствовать CTreeFrameIcon_xxx enum
	wxBusyCursor wait;
	wxIcon icons[5];
	icons[0] = wxIcon(ap->GetIco(icr_treeitem1));
	icons[1] = wxIcon(ap->GetIco(icr_treeitem2));
	icons[2] = wxIcon(ap->GetIco(icr_treeitem3));
	icons[3] = wxIcon(ap->GetIco(icr_treeitem4));
	icons[4] = wxIcon(ap->GetIco(icr_treeitem5));

	int sizeOrig = icons[0].GetWidth();
	for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
	{
		if ( size == sizeOrig )
			m_images->Add(icons[i]);
		else
			m_images->Add(wxBitmap(wxBitmap(icons[i]).ConvertToImage().Rescale(size, size)));
	}
	AssignImageList(m_images);
}
//**************************************************
#if USE_GENERIC_TREECTRL || !defined(__WXMSW__)
void CTreeFrame::CreateButtonsImageList(int size)
{
    if ( size == -1 )
    {
        SetButtonsImageList(NULL);
        return;
    }

    // Make an image list containing small icons
    wxImageList *images = new wxImageList(size, size, true);

    // should correspond to CTreeFrameIcon_xxx enum
    wxBusyCursor wait;
    wxIcon icons[4];
    icons[0] = wxIcon(icon3_xpm);   // closed
    icons[1] = wxIcon(icon3_xpm);   // closed, selected
    icons[2] = wxIcon(icon5_xpm);   // open
    icons[3] = wxIcon(icon5_xpm);   // open, selected

    for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
    {
        int sizeOrig = icons[i].GetWidth();
        if ( size == sizeOrig )
        {
            images->Add(icons[i]);
        }
        else
        {
            images->Add(wxBitmap(wxBitmap(icons[i]).ConvertToImage().Rescale(size, size)));
        }
    }

    AssignButtonsImageList(images);
#else
void CTreeFrame::CreateButtonsImageList(int WXUNUSED(size))
{
#endif
}
//**************************************************
int CTreeFrame::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
	if (m_reverseSort)
		return wxTreeCtrl::OnCompareItems(item2, item1);
	else
		return wxTreeCtrl::OnCompareItems(item1, item2);
}
//**************************************************
// Обработка макроса сообщения по умолчанию
#define TREE_EVENT_HANDLER(name)                                 \
void CTreeFrame::name(wxTreeEvent& event)                        \
{                                                                \
    SetLastItem(wxTreeItemId());                                 \
    event.Skip();                                                \
}
//**************************************************
//TREE_EVENT_HANDLER(OnBeginRDrag)
//TREE_EVENT_HANDLER(OnGetInfo)
//TREE_EVENT_HANDLER(OnSetInfo)
//TREE_EVENT_HANDLER(OnItemExpanded)
//TREE_EVENT_HANDLER(OnItemExpanding)
//TREE_EVENT_HANDLER(OnItemCollapsed)
//TREE_EVENT_HANDLER(OnSelChanged)
TREE_EVENT_HANDLER(OnSelChanging)
//**************************************************
#undef TREE_EVENT_HANDLER
//**************************************************
void CTreeFrame::OnDeleteItem(wxCommandEvent& WXUNUSED(event))
{
	if(!wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_DeleteTreeItem, this, (wxEvent *)NULL))
		return;
}
//**************************************************
void CTreeFrame::OnSelChanged(wxTreeEvent& event)
{
	if(wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_SelectTreeNode, this, &event))
		event.Skip();
}
//**************************************************
void CTreeFrame::OnBeginDrag(wxTreeEvent& event)
{
	if(wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_BeginDragTreeItem, this, &event))
		event.Allow();
	else
		event.Veto();
}
//**************************************************
void CTreeFrame::OnEndDrag(wxTreeEvent& event)
{
	if(!wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_EndDragTreeItem, this, &event))
		return;
}
//**************************************************
void CTreeFrame::OnTreeKeyDown(wxTreeEvent& event)
{
}
//**************************************************
void CTreeFrame::OnBeginLabelEdit(wxTreeEvent& event)
{
	wxTreeItemId itemId = event.GetItem();
	TreeItemData *item = (TreeItemData *)GetItemData(itemId);
	if(item->isNode())
	{
		if(!wxGetApp().GetTreeBot()->CanChangeTree(item->GetGUID())) 
		{
			event.Veto();
			return;
		}
	}
	event.Skip();
}
//**************************************************
void CTreeFrame::OnEndLabelEdit(wxTreeEvent& event)
{
	TreeItemData *item = (TreeItemData *)GetItemData(event.GetItem());
	if(event.GetLabel().IsEmpty())
	{
		event.Veto();
		return;
	}
	if(item->isNode())
	{
		CNodeBot *nodebot = wxGetApp().GetTreeBot()->GetActiveNode();
		nodebot->SetNodeName(event.GetLabel());
		wxGetApp().GetPropertyFrame()->RefreshPropertyGrid(BotNode);
	}
	else
	{
		CBot *bot = wxGetApp().GetTreeBot()->GetActiveBot();
		bot->SetNameBot(event.GetLabel());
		wxGetApp().GetPropertyFrame()->RefreshPropertyGrid(Bot);
	}
	event.Skip();
}
//**************************************************
void CTreeFrame::OnItemActivated(wxTreeEvent& event)
{
	event.Skip();
}
//**************************************************
void CTreeFrame::OnItemMenu(wxTreeEvent& event)
{
	wxTreeItemId itemId = event.GetItem();
	wxPoint clientpt = event.GetPoint();
	wxPoint screenpt = ClientToScreen(clientpt);
	ShowMenu(itemId, clientpt);
	event.Skip();
}
//**************************************************
void CTreeFrame::OnContextMenu(wxContextMenuEvent& WXUNUSED(event))
{
}
//**************************************************
void CTreeFrame::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
	wxString title;
	if (id.IsOk())
		title << wxT("Menu for ") << GetItemText(id);
	else
		title = wxT("Menu for no particular item");

#if wxUSE_MENUS
	wxMenu menu(title);
	menu.Append(Tree_Start, wxT("&Запустить"));
	menu.Append(Tree_Pause, wxT("&Приостановить"));
	menu.Append(Tree_Stop, wxT("&Остановить"));
	menu.AppendSeparator();
	menu.Append(Tree_Add, wxT("&Добавить бота"));
	menu.Append(Tree_AddFolder, wxT("&Добавить папку"));
	menu.Append(Tree_Delete, wxT("&Удалить"));

	PopupMenu(&menu, pt);
#endif // wxUSE_MENUS
}
//**************************************************
void CTreeFrame::OnRMouseDown(wxMouseEvent& event)
{
	event.Skip();
}
//**************************************************
void CTreeFrame::OnRMouseUp(wxMouseEvent& event)
{
	event.Skip();
}
//**************************************************
void CTreeFrame::OnItemRClick(wxTreeEvent& event)
{
	event.Skip();
}
//**************************************************
//Добавление элемента к дереву
void CTreeFrame::OnAddItem(wxCommandEvent& WXUNUSED(event))
{
	if(!wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_AddTreeItem, this, (wxEvent *)NULL))
		return;
}
//**************************************************
void CTreeFrame::OnAddSubItem(wxCommandEvent& WXUNUSED(event))
{
	if(!wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_AddTreeSubItem, this, (wxEvent *)NULL))
		return;
}
//**************************************************
void CTreeFrame::OnSelectSubItem()
{
}
//**************************************************
void CTreeFrame::OnStartItem(wxCommandEvent& WXUNUSED(event))
{
	if(!wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_StartItem, this, (wxEvent *)NULL))
		return;
}
//**************************************************
void CTreeFrame::OnStopItem(wxCommandEvent& WXUNUSED(event))
{
	if(!wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_StopItem, this, (wxEvent *)NULL))
		return;
}
//**************************************************
void CTreeFrame::OnPauseItem(wxCommandEvent& WXUNUSED(event))
{
	if(!wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_PauseItem, this, (wxEvent *)NULL))
		return;
}
//**************************************************
// Перестроить содержание окна дерева ботов
void CTreeFrame::RefreshTree()
{
	//Предварительно удалим все ветки дерева
	this->DeleteAllItems();

	//Получим дерево ботов и создадим корень дерева
	CTreeBot *treeBot = wxGetApp().GetTreeBot();
	if(!treeBot)
		return;

	wxTreeItemId root = AddRoot(treeBot->GetRootNode()->GetNodeName(), TreeFrameIcon_FolderOpened, 
								TreeFrameIcon_FolderOpened, 
								new TreeItemData(treeBot->GetRootNode()->GetGUIDNode(), true));
	wxArrayTreeItemIds items;

	//Начнем читать список ботов в дереве
	wxList::compatibility_iterator node = treeBot->GetRootNode()->GetListNode().GetFirst();
	while(node)
	{
		//Добавляем основные узлы 1го уровня
		CNodeBot *nodeBot = (CNodeBot *)node->GetData();
		wxTreeItemId itId = AppendItem(root, nodeBot->GetNodeName(), TreeFrameIcon_Folder, 
							TreeFrameIcon_FolderSelected, new TreeItemData(nodeBot->GetGUIDNode(), true));
		if(treeBot->GetActiveNode() == nodeBot)
			SelectItem(itId, true);
		//Проверим на наличие (под узлов) 2го уровня, если есть заполним
		if(nodeBot->GetTotalNode() > 0)
		{
			wxList::compatibility_iterator node2 = nodeBot->GetListNode().GetFirst();
			while(node2)
			{
				CNodeBot *nodeBot2 = (CNodeBot *)node2->GetData();
				wxTreeItemId itId2 = AppendItem(itId, nodeBot2->GetNodeName(), TreeFrameIcon_Folder, 
												TreeFrameIcon_FolderSelected, new TreeItemData(nodeBot2->GetGUIDNode(), true));
				if(treeBot->GetActiveNode() == nodeBot2)
					SelectItem(itId2, true);
				
				if(nodeBot2->GetTotalNode() > 0)
				{
					wxList::compatibility_iterator node3 = nodeBot2->GetListNode().GetFirst();
					while(node3)
					{
						CNodeBot *nodeBot3 = (CNodeBot *)node3->GetData();
						wxTreeItemId itId3 = AppendItem(itId2, nodeBot3->GetNodeName(), TreeFrameIcon_Folder, 
														TreeFrameIcon_FolderSelected, new TreeItemData(nodeBot3->GetGUIDNode(), true));
						if(treeBot->GetActiveNode() == nodeBot3)
							SelectItem(itId3, true);
						if(nodeBot3->GetTotalBot() > 0)
						{
							wxList::compatibility_iterator bots3 = nodeBot3->GetListBot().GetFirst();
							while(bots3)
							{
								CBot *bot3 = (CBot *)bots3->GetData();
								AppendItem(itId3, bot3->GetNameBot(), TreeFrameIcon_File, TreeFrameIcon_FileSelected, new TreeItemData(bot3->GetGUIDBot(), false));
								bots3 = bots3->GetNext();
							}
						}
						node3 = node3->GetNext();
					}
				}
				if(nodeBot2->GetTotalBot() > 0)
				{
					wxList::compatibility_iterator bots2 = nodeBot2->GetListBot().GetFirst();
					while(bots2)
					{
						CBot *bot2 = (CBot *)bots2->GetData();
						AppendItem(itId2, bot2->GetNameBot(), TreeFrameIcon_File, TreeFrameIcon_FileSelected, new TreeItemData(bot2->GetGUIDBot(), false));
						bots2 = bots2->GetNext();
					}
				}
				node2 = node2->GetNext();
			}
		}
		//Проверим на наличие ботов в узлах 1го уровня, если есть заполним
		if(nodeBot->GetTotalBot() > 0)
		{
			wxList::compatibility_iterator bot1 = nodeBot->GetListBot().GetFirst();
			while(bot1)
			{
				CBot *bot = (CBot *)bot1->GetData();
				AppendItem(itId, bot->GetNameBot(), TreeFrameIcon_File, TreeFrameIcon_FileSelected, new TreeItemData(bot->GetGUIDBot(), false));
				bot1 = bot1->GetNext();
			}
		}
		node = node->GetNext();
	}
	Expand(root);
}