//**************************************************
#include "ListFrame.h"
//**************************************************
BEGIN_EVENT_TABLE(CListFrame, wxListCtrl)
	EVT_LIST_ITEM_SELECTED	(List_Ctrl, CListFrame::OnSelected)
	//EVT_LIST_ITEM_DESELECTED(List_Ctrl, CListFrame::OnDeselected)
	EVT_LIST_ITEM_ACTIVATED	(List_Ctrl, CListFrame::OnActivated)
	EVT_CONTEXT_MENU		(CListFrame::OnContextMenu)
	EVT_MOUSE_EVENTS		(CListFrame::OnMouseEvent)
END_EVENT_TABLE()
//**************************************************
IMPLEMENT_DYNAMIC_CLASS(CListFrame, wxListCtrl)
//**************************************************
CListFrame::CListFrame(wxWindow *parent, const wxWindowID id,
             const wxPoint& pos, const wxSize& size, long style)
			 :wxListCtrl(parent, id, pos, size, style),
			 m_attr(*wxBLUE, *wxLIGHT_GREY, wxNullFont)
{
	m_imageListNormal = ((wxImageList *)NULL);
}
//**************************************************
void CListFrame::OnMouseEvent(wxMouseEvent& event)
{
	event.Skip();
}
//**************************************************
void CListFrame::SetColumnImage(int col, int image)
{
	wxListItem item;
	item.SetMask(wxLIST_MASK_IMAGE);
	item.SetImage(image);
	SetColumn(col, item);
}
//**************************************************
void CListFrame::OnSelected(wxListEvent& event)
{
	if(wxGetApp().GetFrameCanvas()->GetMode() == FrameCanvas::modeCREATECONNECTION)wxGetApp().GetFrameCanvas()->AbortInteractiveConnection();

	///wxGetApp().GetAppProperties()->SetSelectComponent(event.GetIndex());
	switch(event.GetIndex())
	{
		case 0:
			((wxAutoBotFrame *)wxGetApp().GetTopWindow())->SetToolMode(wxAutoBotFrame::modeLINE);
			break;
		case 1:
			((wxAutoBotFrame *)wxGetApp().GetTopWindow())->SetToolMode(wxAutoBotFrame::modeCYCLE);
			break;
		case 2:
			((wxAutoBotFrame *)wxGetApp().GetTopWindow())->SetToolMode(wxAutoBotFrame::modeTRIGGER);
			break;
		case 3:
			((wxAutoBotFrame *)wxGetApp().GetTopWindow())->SetToolMode(wxAutoBotFrame::modeIF);
			break;
		default:
			((wxAutoBotFrame *)wxGetApp().GetTopWindow())->SetToolMode(wxAutoBotFrame::modeCOMPONENT);
			break;
	}
}
////**************************************************
//void CListFrame::OnSelected(wxListEvent& event)
//{
//}
//**************************************************
void CListFrame::Deselect()
{
	int n = GetItemCount();
	for (int i = 0; i < n; i++)
		SetItemState(i, 0, wxLIST_STATE_SELECTED);
}
//**************************************************
void CListFrame::OnActivated(wxListEvent& event)
{
	event.Skip();
}
//**************************************************
wxString CListFrame::OnGetItemText(long item, long column) const
{
    //if ( GetItemCount() == WXSIZEOF(SMALL_VIRTUAL_VIEW_ITEMS) )
    //{
    //    return SMALL_VIRTUAL_VIEW_ITEMS[item][column];
    //}
    //else
    //{
	return wxString::Format(_T("Column %ld of item %ld"), column, item);
    //}
}
//**************************************************
int CListFrame::OnGetItemColumnImage(long item, long column) const
{
	if (!column)
		return 0;
	if (!(item %3) && column == 1)
		return 0;
	return -1;
}
//**************************************************
void CListFrame::ShowContextMenu(const wxPoint& pos)
{
	wxMenu menu;

	menu.Append(wxID_ABOUT, _T("&About"));
	menu.AppendSeparator();
	menu.Append(wxID_EXIT, _T("E&xit"));

	PopupMenu(&menu, pos.x, pos.y);
}
//**************************************************
void CListFrame::OnContextMenu(wxContextMenuEvent& event)
{
	wxPoint point = event.GetPosition();
	// If from keyboard
	if (point.x == -1 && point.y == -1) {
		wxSize size = GetSize();
		point.x = size.x / 2;
		point.y = size.y / 2;
	}
	else
		point = ScreenToClient(point);
	ShowContextMenu(point);
}
//**************************************************
//
//**************************************************
//void CListFrame::OnCacheHint(wxListEvent& event)
//{
//    wxLogMessage( wxT("OnCacheHint: cache items %ld..%ld"),
//                  event.GetCacheFrom(), event.GetCacheTo() );
//}
////**************************************************
//void CListFrame::OnColClick(wxListEvent& event)
//{
//    int col = event.GetColumn();
//
//    // set or unset image
//    static bool x = false;
//    x = !x;
//    SetColumnImage(col, x ? 0 : -1);
//
//    wxLogMessage( wxT("OnColumnClick at %d."), col );
//}
////**************************************************
//void CListFrame::OnColRightClick(wxListEvent& event)
//{
//    int col = event.GetColumn();
//    if ( col != -1 )
//    {
//        SetColumnImage(col, -1);
//    }
//
//    // Show popupmenu at position
//    wxMenu menu(wxT("Test"));
//    menu.Append(LIST_ABOUT, _T("&About"));
//    PopupMenu(&menu, event.GetPoint());
//
//    wxLogMessage( wxT("OnColumnRightClick at %d."), event.GetColumn() );
//}
////**************************************************
//void CListFrame::OnColBeginDrag(wxListEvent& event)
//{
//    LogColEvent( event, wxT("OnColBeginDrag") );
//
//    if ( event.GetColumn() == 0 )
//    {
//        wxLogMessage(_T("Resizing this column shouldn't work."));
//
//        event.Veto();
//    }
//}
////**************************************************
//void CListFrame::OnColDragging(wxListEvent& event)
//{
//    LogColEvent( event, wxT("OnColDragging") );
//}
////**************************************************
//void CListFrame::OnColEndDrag(wxListEvent& event)
//{
//    LogColEvent( event, wxT("OnColEndDrag") );
//}
////**************************************************
//void CListFrame::OnBeginRDrag(wxListEvent& event)
//{
//    wxLogMessage( wxT("OnBeginRDrag at %d,%d."),
//                  event.m_pointDrag.x, event.m_pointDrag.y );
//}
////**************************************************
//void CListFrame::OnBeginLabelEdit(wxListEvent& event)
//{
//    wxLogMessage( wxT("OnBeginLabelEdit: %s"), event.m_item.m_text.c_str());
//}
////**************************************************
//void CListFrame::OnEndLabelEdit(wxListEvent& event)
//{
//    wxLogMessage( wxT("OnEndLabelEdit: %s"),
//                  event.IsEditCancelled() ? _T("[cancelled]")
//                                          : event.m_item.m_text.c_str());
//}
////**************************************************
//void CListFrame::OnDeleteItem(wxListEvent& event)
//{
//    LogEvent(event, _T("OnDeleteItem"));
//    wxLogMessage( wxT("Number of items when delete event is sent: %d"), GetItemCount() );
//}
////**************************************************
//void CListFrame::OnDeleteAllItems(wxListEvent& event)
//{
//    LogEvent(event, _T("OnDeleteAllItems"));
//}
////**************************************************
//void CListFrame::OnFocused(wxListEvent& event)
//{
//    LogEvent(event, _T("OnFocused"));
//
//    event.Skip();
//}
////**************************************************
//void CListFrame::OnListKeyDown(wxListEvent& event)
//{
//    long item;
//
//    switch ( event.GetKeyCode() )
//    {
//        case 'c': // colorize
//        case 'C':
//            {
//                wxListItem info;
//                info.m_itemId = event.GetIndex();
//                if ( info.m_itemId == -1 )
//                {
//                    // no item
//                    break;
//                }
//
//                GetItem(info);
//
//                wxListItemAttr *attr = info.GetAttributes();
//                if ( !attr || !attr->HasTextColour() )
//                {
//                    info.SetTextColour(*wxCYAN);
//
//                    SetItem(info);
//
//                    RefreshItem(info.m_itemId);
//                }
//            }
//            break;
//
//        case 'n': // next
//        case 'N':
//            item = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
//            if ( item++ == GetItemCount() - 1 )
//            {
//                item = 0;
//            }
//
//            wxLogMessage(_T("Focusing item %ld"), item);
//
//            SetItemState(item, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
//            EnsureVisible(item);
//            break;
//
//        case 'r': // show bounding Rect
//        case 'R':
//            {
//                item = event.GetIndex();
//                wxRect r;
//                if ( !GetItemRect(item, r) )
//                {
//                    wxLogError(_T("Failed to retrieve rect of item %ld"), item);
//                    break;
//                }
//
//                wxLogMessage(_T("Bounding rect of item %ld is (%d, %d)-(%d, %d)"),
//                             item, r.x, r.y, r.x + r.width, r.y + r.height);
//            }
//            break;
//
//        case WXK_DELETE:
//            item = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
//            while ( item != -1 )
//            {
//                DeleteItem(item);
//
//                wxLogMessage(_T("Item %ld deleted"), item);
//
//                // -1 because the indices were shifted by DeleteItem()
//                item = GetNextItem(item - 1,
//                                   wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
//            }
//            break;
//
//        case WXK_INSERT:
//            if ( GetWindowStyle() & wxLC_REPORT )
//            {
//                if ( GetWindowStyle() & wxLC_VIRTUAL )
//                {
//                    SetItemCount(GetItemCount() + 1);
//                }
//                else // !virtual
//                {
//                    InsertItemInReportView(event.GetIndex());
//                }
//            }
//            //else: fall through
//
//        default:
//            LogEvent(event, _T("OnListKeyDown"));
//
//            event.Skip();
//    }
//}
////**************************************************
//void CListFrame::OnChar(wxKeyEvent& event)
//{
//    wxLogMessage(_T("Got char event."));
//
//    switch ( event.GetKeyCode() )
//    {
//        case 'n':
//        case 'N':
//        case 'c':
//        case 'C':
//            // these are the keys we process ourselves
//            break;
//
//        default:
//            event.Skip();
//    }
//}
////**************************************************
//void CListFrame::OnRightClick(wxMouseEvent& event)
//{
//    if ( !event.ControlDown() )
//    {
//        event.Skip();
//        return;
//    }
//
//    int flags;
//    long subitem;
//    long item = HitTest(event.GetPosition(), flags, &subitem);
//
//    wxString where;
//    switch ( flags )
//    {
//        case wxLIST_HITTEST_ABOVE: where = _T("above"); break;
//        case wxLIST_HITTEST_BELOW: where = _T("below"); break;
//        case wxLIST_HITTEST_NOWHERE: where = _T("nowhere near"); break;
//        case wxLIST_HITTEST_ONITEMICON: where = _T("on icon of"); break;
//        case wxLIST_HITTEST_ONITEMLABEL: where = _T("on label of"); break;
//        case wxLIST_HITTEST_ONITEMRIGHT: where = _T("right on"); break;
//        case wxLIST_HITTEST_TOLEFT: where = _T("to the left of"); break;
//        case wxLIST_HITTEST_TORIGHT: where = _T("to the right of"); break;
//        default: where = _T("not clear exactly where on"); break;
//    }
//
//    wxLogMessage(_T("Right double click %s item %ld, subitem %ld"),
//                 where.c_str(), item, subitem);
//}
////**************************************************
//void CListFrame::LogEvent(const wxListEvent& event, const wxChar *eventName)
//{
//    wxLogMessage(_T("Item %ld: %s (item text = %s, data = %ld)"),
//                 event.GetIndex(), eventName,
//                 event.GetText().c_str(), event.GetData());
//}
//**************************************************
//void CListFrame::OnBeginDrag(wxListEvent& event)
//{
//    const wxPoint& pt = event.m_pointDrag;
//
//    int flags;
//	event.Skip();
//    //wxLogMessage( wxT("OnBeginDrag at (%d, %d), item %ld."),
//    //              pt.x, pt.y, HitTest(pt, flags) );
//}