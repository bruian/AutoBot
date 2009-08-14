#include "SchemeFrame.h"
//**************************************************
//Таблица сообщении для Окна-Схем
BEGIN_EVENT_TABLE(CSchemeFrame, wxScrolledWindow)
	EVT_PAINT(CSchemeFrame::OnPaint)
	EVT_ERASE_BACKGROUND(CSchemeFrame::OnEraseBackground)
	EVT_MOUSE_EVENTS(CSchemeFrame::OnMouseEvent)
	EVT_KEY_UP(CSchemeFrame::OnKeyUp)
	EVT_SCROLLWIN(CSchemeFrame::OnScroll)
END_EVENT_TABLE()
//**************************************************
IMPLEMENT_DYNAMIC_CLASS(CSchemeFrame, wxScrolledWindow)
//**************************************************
//**Реализация класса "окно-схемы"
CSchemeFrame::CSchemeFrame(wxWindow *parent)
			: wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
							   wxSUNKEN_BORDER | wxTAB_TRAVERSAL/*wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE*/)
{
	SetBackgroundColour(*wxLIGHT_GREY);
	SetCursor(wxCursor(wxCURSOR_HAND));

	m_dragMode				= DRAG_NONE;
	m_CountShape			= NULL;
	m_dragImage				= (MyDragImage *) NULL;
	m_draggedShape			= (CDragShape *) NULL;
	m_currentlyHighlighted	= (CDragShape *) NULL;
	m_draggedRelationship	= (CRelationship *)NULL;
	m_selectRelationship	= (CRelationship *)NULL;

	m_owner = parent;
	m_Line = false;
}
//**************************************************
CSchemeFrame::~CSchemeFrame()
{
	ClearLines();
	ClearShapes();
	if (m_dragImage)
		delete m_dragImage;
}
//**************************************************
//Добавить фигуру на форму
bool CSchemeFrame::AddShapeOnFrame(GUID componentGuid, GUID shapeGUID, wxString nameComponent, wxPoint pos)
{
	CComponent *comp = wxGetApp().GetAppProperties()->GetComponent(componentGuid);
	if(comp == NULL)
		return false;

	wxImage image(comp->GetImage());
	if (image.IsOk())
	{
		CDragShape* newShape = new CDragShape(wxBitmap(image));
		
		newShape->SetPosition(pos);
		newShape->SetDragMethod(SHAPE_DRAG_BITMAP);
		newShape->m_guidShape = shapeGUID;
		if(componentGuid == wxGetApp().GetTreeBot()->GetGUIDTrigger())
			newShape->m_typeShape = Trigger;
		else if(componentGuid == wxGetApp().GetTreeBot()->GetGUIDCondition())
			newShape->m_typeShape = Condition;
		else
			newShape->m_typeShape = Action;
		
		wxCoord vsX, vsY;
		GetVirtualSize(&vsX, &vsY);
		if(vsX < newShape->GetRect().GetRight())
		{
			vsX = newShape->GetRect().GetRight()+5;
			SetVirtualSize(wxSize(vsX, vsY));
		}
		if(vsY < newShape->GetRect().GetBottom())
		{
			vsY = newShape->GetRect().GetBottom();
			SetVirtualSize(wxSize(vsX, vsY));
		}
		GetDisplayList().Append(newShape);
		m_currentlyHighlighted = newShape;
		m_CountShape++;
	}
	Refresh(true);
	return true;
}
//**************************************************
void CSchemeFrame::RefreshShapeOnFrame()
{
	ClearLines();
	ClearShapes();
	CBot *bot = wxGetApp().GetTreeBot()->GetActiveBot();
	if(bot == NULL)
		return;
	//wxTreeItemId id = wxGetApp().GetTreeFrame()->GetSelection();
	//TreeItemData *item = (TreeItemData *)wxGetApp().GetTreeFrame()->GetItemData(id);
	//if(wxGetApp().GetTreeBot()->GetNode(item->GetGUID()) != NULL)
	//	return;
	wxList::compatibility_iterator node = bot->GetListRule().GetFirst();
	while (node)
	{
		CRule *rule = (CRule *)node->GetData();
		//if(rule->GetTrigger() != NULL)
		//	//AddShapeOnFrame(wxGetApp().GetTreeBot()->GetGUIDTrigger(), rule->GetGUIDRule(), wxT(""), rule->GetPointRule());
		//else
			//AddShapeOnFrame(rule->GetComponent()->GetGUIDComponent(), rule->GetGUIDRule(), wxT(""), rule->GetPointRule());
		node = node->GetNext();
	}
	CRule *activeRule = wxGetApp().GetTreeBot()->GetActiveRule();
	if(activeRule != NULL)
		m_currentlyHighlighted = FindShape(activeRule->GetGUIDRule());
	else
		m_currentlyHighlighted = (CDragShape *)NULL;
		
	//Отрисовка линий
	node = bot->GetListRule().GetFirst();
	while (node)
	{
		CRule *rule = (CRule *)node->GetData();
		CDragShape *shape = FindShape(rule->GetGUIDRule());
		CRelationship* relationship = FindRelationship(shape);
		if(relationship == NULL)
		{
			relationship = new CRelationship();
			relationship->m_StartShape = shape;
			//if(rule->GetFirstRelationship() != NULL)
			//{
			//	CDragShape *shape2 = FindShape(rule->GetFirstRelationship()->GetGUIDRule());
			//	relationship->m_EndShape = shape2;
			//}
			//else
			//{
			//	relationship->m_EndShape = (CDragShape *)NULL;
			//}

			//if(rule->GetSecondRelationship() != NULL)
			//{
			//	CDragShape *shape2 = FindShape(rule->GetSecondRelationship()->GetGUIDRule());
			//	relationship->m_EndShape2 = shape2;
			//}
			//else
			//{
			//	relationship->m_EndShape2 = (CDragShape *)NULL;
			//}
			GetLineList().Append(relationship);
		}
		else
		{
			relationship->m_StartShape = shape;
			//if(rule->GetFirstRelationship() != NULL)
			//{
			//	CDragShape *shape2 = FindShape(rule->GetFirstRelationship()->GetGUIDRule());
			//	relationship->m_EndShape = shape2;
			//}
			//else
			//{
			//	relationship->m_EndShape = (CDragShape *)NULL;
			//}

			//if(rule->GetSecondRelationship() != NULL)
			//{
			//	CDragShape *shape2 = FindShape(rule->GetSecondRelationship()->GetGUIDRule());
			//	relationship->m_EndShape2 = shape2;
			//}
			//else
			//{
			//	relationship->m_EndShape2 = (CDragShape *)NULL;
			//}
		}
		node = node->GetNext();
	}
	Refresh(true);
	Update();
}
//**************************************************
void CSchemeFrame::OnPaint(wxPaintEvent &WXUNUSED(event))
{
	wxPaintDC pdc(this);
	wxDC &dc = pdc;
	m_owner->PrepareDC(dc);

	//if ( m_clip )
	//  dc.SetClippingRegion(100, 100, 100, 100);

	dc.Clear();

	DrawShapes(dc);
	DrawLines(dc);
}
//**************************************************
void CSchemeFrame::OnKeyUp(wxKeyEvent& event)
{
	//switch(event.GetKeyCode())
	//{
	//case WXK_NUMPAD_DELETE:
	//	case WXK_DELETE:
	//		{
	//			if(wxGetApp().GetFrameCanvas()->GetSelectRelationship() != NULL)
	//			{
	//				CRelationship *rel = wxGetApp().GetSchemeFrame()->GetSelectRelationship();
	//				CRule *rule = wxGetApp().GetTreeBot()->GetActiveBot()->GetRule(rel->m_StartShape->m_guidShape);
	//				if(rule != NULL)
	//				{
	//					if(rel->m_EndShape == rel->m_Select)
	//						rule->SetFirstRelationship((CRule *)NULL);
	//					else if(rel->m_EndShape2 == rel->m_Select)
	//						rule->SetSecondRelationship((CRule *)NULL);
	//				}
	//				RefreshShapeOnFrame();
	//			}
	//			else
	//			{
	//				wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, Tree_Delete);
	//				ev.SetInt(1);
	//				wxPostEvent(wxGetApp().GetTreeFrame(), ev);
	//			}
	//		}
	//		break;
	//	default:
	//		event.Skip();
	//}
	event.Skip();
}
//**************************************************
void CSchemeFrame::OnMouseEvent(wxMouseEvent& event)
{
	wxPoint Phys;
	// Здесь нам нужно вычислить координату относительно 
	// действительного положения курсора с поправкой на прокрученную область
	CalcUnscrolledPosition(event.GetPosition().x, event.GetPosition().y, &Phys.x, &Phys.y);
	//wxLogDebug(wxString::Format(wxT("%d, %d"),Phys.x,Phys.y));
	CDragShape* tipshape = FindShape(Phys);
	if (tipshape)
	{
		CRule *tiprule = wxGetApp().GetTreeBot()->GetActiveBot()->GetRule(tipshape->m_guidShape);
		//if(!tiprule->GetDescription().IsEmpty())
		//	this->SetToolTip(tiprule->GetDescription());
	}
	else
		this->SetToolTip(wxT(""));

	if (event.RightDown() == true)
	{
		this->SetFocusFromKbd();
		CDragShape* shape = FindShape(Phys);
		if (shape)
		{
			CRelationship* relationship = FindRelationship(shape);
			if(relationship == NULL)
			{
				relationship = new CRelationship();
				relationship->m_segmentPoint = Phys;
				relationship->m_dragging = true;
				relationship->m_StartShape = shape;
				relationship->m_EndShape = (CDragShape *)NULL;
				relationship->m_EndShape2 = (CDragShape *)NULL;
				GetLineList().Append(relationship);
			}
			else
			{
				relationship->m_segmentPoint = Phys;
				relationship->m_dragging = true;
			}
			m_draggedRelationship = relationship;
		}
		//Refresh(true);
		//Update();
	}
	else if ((event.Dragging() == true) && (event.m_rightDown == true))
	{
		m_CurPoint = event.GetPosition();
		Refresh(true);
	}
	else if (event.RightUp())
	{
		//CDragShape* shape = FindShape(event.GetPosition());
		CDragShape* shape = FindShape(Phys);
		if (shape)
		{
			if(m_draggedRelationship != NULL)
			{
				if(m_draggedRelationship->m_StartShape == shape)
				{
					wxGetApp().GetTextFrame()->SetDefaultStyle(wxTextAttr(*wxRED));
					wxGetApp().GetTextFrame()->AppendText(wxT("Правило не должно ссылаться на себя\n"));
					Refresh(true);
					return;
				}
				CRule *rule = wxGetApp().GetTreeBot()->GetActiveBot()->GetRule(m_draggedRelationship->m_StartShape->m_guidShape);
				CRule *rule2= wxGetApp().GetTreeBot()->GetActiveBot()->GetRule(shape->m_guidShape);
				//if(rule2->GetTrigger() != NULL)
				//{
				//	wxGetApp().GetTextFrame()->SetDefaultStyle(wxTextAttr(*wxRED));
				//	wxGetApp().GetTextFrame()->AppendText(wxT("Правило не должно ссылаться на 'правило-триггер'\n"));
				//	Refresh(true);
				//	return;
				//}

				wxPoint shaPos;
				m_draggedRelationship->m_dragging = false;
				CalcUnscrolledPosition(m_draggedRelationship->m_StartShape->GetPosition().x, m_draggedRelationship->m_StartShape->GetPosition().y, &shaPos.x, &shaPos.y);
				GUID guid;
				if(rule->GetComponent() != NULL)
					guid = rule->GetComponent()->GetGUIDComponent();

				//if((m_draggedRelationship->m_segmentPoint.x <= shaPos.x + m_draggedRelationship->m_StartShape->GetBitmap().GetWidth()/2) || 
				//	(guid != wxGetApp().GetTreeBot()->GetGUIDCondition()))
				//{
				//	rule->SetFirstRelationship(rule2);
				//	m_draggedRelationship->m_EndShape = shape;
				//}
				//else
				//{
				//	rule->SetSecondRelationship(rule2);
				//	m_draggedRelationship->m_EndShape2 = shape;
				//}
			}
			m_draggedRelationship = (CRelationship *)NULL;
			Refresh(true);
			Update();
		}
	}
	else if (event.LeftDown())
	{
		this->SetFocusFromKbd();
		CDragShape* shape = FindShape(Phys);
		if (shape)
		{
			// We tentatively start dragging, but wait for
			// mouse movement before dragging properly.
			CBot* myBot = wxGetApp().GetTreeBot()->GetActiveBot();
			wxGetApp().GetTreeBot()->SetActiveRule(myBot->GetRule(shape->m_guidShape));
			wxGetApp().GetPropertyFrame()->RefreshPropertyGrid(Rule);
			m_dragMode = DRAG_START;
			m_dragStartPos = Phys;//event.GetPosition();
			m_draggedShape = shape;
			m_currentlyHighlighted = shape;
			m_selectRelationship = (CRelationship *)NULL;
		}
		else
		{
			if(wxGetApp().GetTreeBot() != NULL)
			{
				wxGetApp().GetTreeBot()->UnActiveRule();
				wxGetApp().GetPropertyFrame()->RefreshPropertyGrid(Rule);
			}
			m_currentlyHighlighted = (CDragShape*) NULL;

			CRelationship* relationship = FindRelationship(Phys);
			if(relationship)
			{
				m_selectRelationship = relationship;
			}
			else
			{
				m_selectRelationship = (CRelationship *)NULL;
			}
		}
		Refresh(true);
		Update();
	}
	else if (event.LeftUp() && m_dragMode != DRAG_NONE)
	{
		// Finish dragging
		m_dragMode = DRAG_NONE;

		if (!m_draggedShape || !m_dragImage)
			return;
		
		wxPoint nextPoint = Phys;
		if(nextPoint.x - m_dragImage->GetOffset().x < 0) nextPoint.x = m_dragImage->GetOffset().x;
		if(nextPoint.y - m_dragImage->GetOffset().y < 0) nextPoint.y = m_dragImage->GetOffset().y;

		m_draggedShape->SetPosition(m_draggedShape->GetPosition() + nextPoint - m_dragStartPos);

		//wxGetApp().GetTreeBot()->GetActiveRule()->SetPointRule(m_draggedShape->GetPosition());

		m_dragImage->Hide();
		m_dragImage->EndDrag();
		delete m_dragImage;
		m_dragImage = NULL;

		m_draggedShape->SetShow(true);

		m_currentlyHighlighted = m_draggedShape;
		//m_currentlyHighlighted = (CDragShape*) NULL;

		m_draggedShape = (CDragShape*) NULL;

		Refresh(true);
	}
	else if (event.Dragging() && m_dragMode != DRAG_NONE)
	{
		//wxTipWindow(this, wxT("Подсказка"));
		if (m_dragMode == DRAG_START)
		{
			// We will start dragging if we've moved beyond a couple of pixels

			int tolerance = 2;
			int dx = abs(Phys.x/*event.GetPosition().x*/ - m_dragStartPos.x);
			int dy = abs(Phys.y/*event.GetPosition().y*/ - m_dragStartPos.y);
			if (dx <= tolerance && dy <= tolerance)
				return;

			// Start the drag.
			m_dragMode = DRAG_DRAGGING;

			if (m_dragImage)
				delete m_dragImage;

			// Erase the dragged shape from the canvas
			m_draggedShape->SetShow(false);

			// redraw immediately
			Refresh(true);
			Update();
			
			m_dragImage = new MyDragImage(this, m_draggedShape->GetBitmap(), wxCursor(wxCURSOR_HAND));

			// The offset between the top-left of the shape image and the current shape position
			wxPoint beginDragHotSpot = m_dragStartPos - m_draggedShape->GetPosition();

			// Now we do this inside the implementation: always assume
			// coordinates relative to the capture window (client coordinates)

			if (!m_dragImage->BeginDrag(beginDragHotSpot, this, false))
			{
				delete m_dragImage;
				m_dragImage = (MyDragImage*) NULL;
				m_dragMode = DRAG_NONE;
			}
			else
			{
				m_dragImage->Move(Phys/*event.GetPosition()*/);
				m_dragImage->Show();
			}
		}
		else if (m_dragMode == DRAG_DRAGGING)
		{
			// We're currently dragging. See if we're over another shape.
			CDragShape* onShape = FindShape(Phys/*event.GetPosition()*/);

			bool mustUnhighlightOld = false;
			bool mustHighlightNew = false;

			if (m_currentlyHighlighted)
			{
				if ((onShape == (CDragShape*) NULL) || (m_currentlyHighlighted != onShape))
					mustUnhighlightOld = true;
			}

			if (onShape && (onShape != m_currentlyHighlighted) && onShape->IsShown())
				mustHighlightNew = true;

			if (mustUnhighlightOld || mustHighlightNew)
				m_dragImage->Hide();

			// Now with the drag image switched off, we can change the window contents.
			if (mustUnhighlightOld)
				m_currentlyHighlighted = (CDragShape*) NULL;

			if (mustHighlightNew)
				m_currentlyHighlighted = onShape;

			if (mustUnhighlightOld || mustHighlightNew)
			{
				Refresh(mustUnhighlightOld);
				Update();
			}
			
			wxPoint nextPoint = Phys;//event.GetPosition();
			wxCoord PhysX, PhysY;
			wxCoord vsX, vsY;
			GetVirtualSize(&vsX, &vsY);

			CalcScrolledPosition(nextPoint.x, nextPoint.y, &PhysX, &PhysY);
			if (nextPoint.x - m_dragImage->GetOffset().x < 0) 
				nextPoint.x = 0 + m_dragImage->GetOffset().x;
			if (nextPoint.y - m_dragImage->GetOffset().y < 0) 
				nextPoint.y = 0 + m_dragImage->GetOffset().y;
			if (nextPoint.y + m_dragImage->GetOffset().y > vsY)
				vsY = vsY + m_dragImage->GetOffset().y;
			if (nextPoint.x + m_dragImage->GetOffset().x > vsX) 
				vsX = vsX + m_dragImage->GetOffset().x;
			SetVirtualSize(wxSize(vsX,vsY));

			// Move and show the image again
			m_dragImage->Move(wxPoint(PhysX,PhysY));

			//wxString text;
			//text.Printf(wxT("x1-%d, y1-%d, npY1-%d, npY1-%d"),Phys.x,Phys.y,nextPoint.x,nextPoint.y);
			//wxLogDebug(text.wx_str());
			if (mustUnhighlightOld || mustHighlightNew)
				m_dragImage->Show();
		}
	}
}
//**************************************************
void CSchemeFrame::OnEraseBackground(wxEraseEvent& event)
{
	event.Skip(); // The official way of doing it
}
//**************************************************
void CSchemeFrame::DrawLines(wxDC &dc)
{
	wxList::compatibility_iterator node = m_lineList.GetFirst();
	while (node)
	{
		CRelationship* relationship = (CRelationship*) node->GetData();
		if(relationship->m_dragging == false)
		{
			relationship->Draw(dc);
		}
		node = node->GetNext();
	}
}
//**************************************************
void CSchemeFrame::ClearLines()
{
	if(m_lineList.GetCount() == NULL)
		return;
	wxList::compatibility_iterator node = m_lineList.GetFirst();
	while (node)
	{
		CRelationship* relation = (CRelationship*) node->GetData();
		delete relation;
		node = node->GetNext();
	}
	m_lineList.Clear();
	Refresh(true);
	Update();
}
//**************************************************
void CSchemeFrame::ClearShapes()
{
	if(m_CountShape == NULL)
		return;
	wxList::compatibility_iterator node = m_displayList.GetFirst();
	while (node)
	{
		CDragShape* shape = (CDragShape*) node->GetData();
		delete shape;
		node = node->GetNext();
	}
	m_displayList.Clear();
	Refresh(true);
	Update();
}
//**************************************************
void CSchemeFrame::DrawShapes(wxDC& dc)
{
	wxCoord PhysX = 0, PhysY = 0;
	wxList::compatibility_iterator node = m_displayList.GetFirst();
	while (node)
	{
		CDragShape* shape = (CDragShape*) node->GetData();
		if (/*(shape->IsShown() && m_draggedShape != shape) || */(shape->IsShown()))
		{
			// Для того что бы правильно отобразить объект на экране, вычислим его координаты 
			// относительно прокрученного окна, координаты будут относительно видимой области окна
			CalcScrolledPosition(shape->GetPosition().x, shape->GetPosition().y, &PhysX, &PhysY);
			if(m_currentlyHighlighted == shape)
				shape->Draw(dc, PhysX, PhysY, 1);
			else
			{
				//if(wxGetApp().GetTreeBot()->GetActiveBot() != NULL)
				//	if( wxGetApp().GetTreeBot()->GetActiveBot()->GetDebugRule() == 
				//		wxGetApp().GetTreeBot()->GetActiveBot()->GetRule(shape->m_guidShape) )
				//		shape->Draw(dc, PhysX, PhysY, 2);
				//	else
				//		shape->Draw(dc, PhysX, PhysY);
				//else
				//	shape->Draw(dc, PhysX, PhysY);
			}
		}
		node = node->GetNext();
	}
}
//**************************************************
void CSchemeFrame::EraseShape(CDragShape* shape, wxDC& dc)
{
	wxCoord PhysX, PhysY;
	wxRect rect(shape->GetRect());

	CalcScrolledPosition(rect.x, rect.y, &PhysX, &PhysY);;
	dc.SetClippingRegion(PhysX, PhysY, rect.width, rect.height);

	SetBackgroundColour(* wxLIGHT_GREY);//!!!!!!!!!!
	//wxGetApp().TileBitmap(rect, dc, wxGetApp().GetBackgroundBitmap());

	dc.DestroyClippingRegion();
}
//**************************************************
CDragShape* CSchemeFrame::FindShape(const wxPoint& pt) const
{
	wxList::compatibility_iterator node = m_displayList.GetFirst();
	while (node)
	{
		CDragShape* shape = (CDragShape*) node->GetData();
		if (shape->HitTest(pt.x, pt.y))
			return shape;
		node = node->GetNext();
	}
	return (CDragShape*) NULL;
}
//**************************************************
CDragShape* CSchemeFrame::FindShape(const GUID value)
{
	wxList::compatibility_iterator node = m_displayList.GetFirst();
	while (node)
	{
		CDragShape* shape = (CDragShape*) node->GetData();
		if (value == shape->m_guidShape)
			return shape;
		node = node->GetNext();
	}
	return (CDragShape*) NULL;
}
//**************************************************
CRelationship* CSchemeFrame::FindRelationship(const wxPoint& pt) const
{
	wxList::compatibility_iterator node = m_lineList.GetFirst();
	while (node)
	{
		CRelationship* relationship = (CRelationship*) node->GetData();
		if (relationship->HitTest(pt.x, pt.y))
			return relationship;
		node = node->GetNext();
	}
	return (CRelationship*) NULL;
}
//**************************************************
CRelationship* CSchemeFrame::FindRelationship(CDragShape *value)
{
	wxList::compatibility_iterator node = m_lineList.GetFirst();
	while (node)
	{
		CRelationship* relationship = (CRelationship*) node->GetData();
		if (value == relationship->m_StartShape)
			return relationship;
		node = node->GetNext();
	}
	return (CRelationship*) NULL;
}
//**************************************************
void CSchemeFrame::OnScroll(wxScrollWinEvent& event)
{
	Refresh(true);
	Update();
	event.Skip();
}
//**************************************************
//Реализация класса "элемент-схемы"
CDragShape::CDragShape(const wxBitmap& bitmap)
{
	m_bitmap = bitmap;
	m_pos.x = 0;
	m_pos.y = 0;
	m_dragMethod = SHAPE_DRAG_BITMAP;
	m_show = true;
}
//**************************************************
bool CDragShape::HitTest(const wxCoord x, const wxCoord y) const
{
	wxRect rect(GetRect());
	return rect.Contains(x, y);
}
//**************************************************
bool CDragShape::Draw(wxDC& dc, int x, int y, wxInt16 highlight)
{
	if (m_bitmap.Ok())
	{
		wxMemoryDC memDC;
		memDC.SelectObject(m_bitmap);

		dc.Blit(x, y, m_bitmap.GetWidth(), m_bitmap.GetHeight(), & memDC, 0, 0, wxCOPY, true);

		if (highlight == 1)
		{
			dc.SetPen(*wxWHITE_PEN);
			dc.SetBrush(*wxTRANSPARENT_BRUSH);
			dc.DrawRectangle(x, y, m_bitmap.GetWidth(), m_bitmap.GetHeight());
		}
		else if(highlight == 2)
		{
			dc.SetPen(*wxRED_PEN);
			dc.SetBrush(*wxTRANSPARENT_BRUSH);
			dc.DrawRectangle(x, y, m_bitmap.GetWidth(), m_bitmap.GetHeight());
		}

		return true;
	}
	else
		return false;
}
//**************************************************
bool MyDragImage::UpdateBackingFromWindow(wxDC& WXUNUSED(windowDC), wxMemoryDC& destDC, const wxRect& WXUNUSED(sourceRect),
										  const wxRect& destRect) const
{
	destDC.SetClippingRegion(destRect);

    //if (wxGetApp().GetBackgroundBitmap().Ok())
    //    wxGetApp().TileBitmap(destRect, destDC, wxGetApp().GetBackgroundBitmap());
	
	destDC.SetBrush(*wxLIGHT_GREY_BRUSH);
	destDC.DrawRectangle(destRect);

	m_canvas->DrawShapes(destDC);
	return true;
}
//**************************************************
//Реализация класса "связи-схемы"
CRelationship::CRelationship()
{
	m_StartShape = (CDragShape*)NULL;
	m_EndShape	= (CDragShape*)NULL;
	m_EndShape2 = (CDragShape*)NULL;
	m_Select	= (CDragShape*)NULL;
	m_dragging = false;
}
//**************************************************
bool CRelationship::Draw(wxDC& dc)
{
	//Отрисовка связей
	if (m_StartShape != NULL)
	{
		wxCoord xA, yA, xB, yB, xC, yC, coorXA, coorYA;
		wxBitmap m_bitmapE;
		wxPoint	 m_posE;
		wxPoint	 m_posS	   = m_StartShape->GetPosition(); //Позиция первой(Start) фигуры X,Y
		wxBitmap m_bitmapS = m_StartShape->GetBitmap();	  //Битовое изображение фигуры 1

		wxRegion *region;
		if(m_EndShape != NULL)
		{
			m_Regions.DeleteContents(true);
			m_Regions.Clear();
			m_bitmapE = m_EndShape->GetBitmap();	  //Битовое изображение фигуры 2
			if (m_bitmapS.Ok() && m_bitmapE.Ok()) 		//И если мы имеем битовое изображение, значит и связи строить можем
			{
				m_posE = m_EndShape->GetPosition();	  //Позиция второй(End) фигуры X,Y
				if(m_StartShape->m_typeShape == Condition)
				{
					coorXA = 2;
					coorYA = m_bitmapS.GetHeight()/2 - 2;
				}
				else
				{
					coorXA = m_bitmapS.GetWidth()/2;	//Середина битовой фигуры - это ширина пополам X
					coorYA = m_bitmapS.GetHeight();		//Высота битовой фигуры, там где низ Y
				}

				coorXA = dc.DeviceToLogicalX( m_posS.x + coorXA ); //Отсюда начнём рисовать
				coorYA = dc.DeviceToLogicalY( m_posS.y + coorYA ); //первую фигуру
				//wxGetApp().GetSchemeFrame()->CalcScrolledPosition(coorXA, coorYA, &xA, &yA);

				wxCoord coorXB = 0;
				if(m_EndShape->m_typeShape == Condition)	//Вторая фигура
					coorXB = (m_bitmapE.GetWidth()/2);
				else 
					coorXB = m_bitmapE.GetWidth()/2;
				wxCoord coorYB = 0;

				coorXB = dc.DeviceToLogicalX( m_posE.x + coorXB );
				coorYB = dc.DeviceToLogicalX( m_posE.y + coorYB );
				//wxGetApp().GetSchemeFrame()->CalcScrolledPosition(coorXB, coorYB, &xB, &yB);

				//if((wxGetApp().GetSchemeFrame()->GetSelectRelationship() == this) && (m_Select == m_EndShape))
				//	dc.SetPen( wxPen( wxT("white"), 1, wxSOLID) );
				//else
				//	dc.SetPen( wxPen( wxT("black"), 1, wxSOLID) );
				
				//Нарисуем связи в соответствии с положением связанных объектов...
				if(yA < yB)		//Положение второй фигуры ниже, чем первой
				{
					if(xA < xB)	//Положение второй фигуры правее и ниже чем первой
					{
						wxCoord tmpYa = yA + (yB - yA)/2;
						if(m_StartShape->m_typeShape == Condition)	//Если фигура "условие", рисуем от левого отростка
						{
							tmpYa = (yA + m_bitmapS.GetHeight()/2) + (yB - (yA + m_bitmapS.GetHeight()/2))/2;
						}
						dc.DrawLine( xA, yA, xA, tmpYa );		//1 отрезок вниз от фигуры А, до половины расстояния между А и Б
						dc.DrawLine( xA, tmpYa, xB, tmpYa );	//2 отрезок вправо от конца 1 отрезка
						dc.DrawLine( xB, tmpYa, xB, yB );		//3 отрезок вниз от конца отрезка 2
						//4 (стрелка левая сторона) отрезок от середины 2 отрезка, вверх влево
						//5 (стрелка правая сторона)отрезок от середины 2 отрезка, вниз влево
						wxCoord tmpDX = xA + (xB-xA)/2;
						dc.DrawLine( tmpDX, tmpYa, tmpDX - 5, tmpYa - 3 );
						dc.DrawLine( tmpDX, tmpYa, tmpDX - 5, tmpYa + 3 );
						
						region = new wxRegion( wxPoint(xA - 2, yA), wxPoint(xA + 2, tmpYa) ); m_Regions.Append(region);
						region = new wxRegion( wxPoint(xA, tmpYa - 2), wxPoint(xB, tmpYa + 2) ); m_Regions.Append(region);
						region = new wxRegion( wxPoint(xB - 2, tmpYa), wxPoint(xB + 2, yB) ); m_Regions.Append(region);
					}
					else if(xA == xB) //Положение второй фигуры сразу под первой
					{
						dc.DrawLine( xA, yA, xB, yB );	//1 отрезок вниз от А к Б
						//2 (стрелка левая сторона) отрезок от середины 1 отрезка, вниз влево
						//3 (стрелка правая сторона)отрезок от середины 1 отрезка, вниз вправо
						wxCoord tmpDY = yA + (yB-yA)/2;
						dc.DrawLine( xA, tmpDY, xB - 3, tmpDY - 5 );
						dc.DrawLine( xA, tmpDY, xB + 3, tmpDY - 5 );
						
						region = new wxRegion( wxPoint(xA - 2, yA), wxPoint(xA + 2, yB) ); m_Regions.Append(region);
					}
					else	//Положение второй фигуры левее и ниже первой
					{
						wxCoord tmpYa = yA + (yB - yA)/2;
						if(m_StartShape->m_typeShape == Condition)
						{
							tmpYa = (yA + m_bitmapE.GetHeight()/2) + (yB - (yA + m_bitmapE.GetHeight()/2))/2;
						}
						dc.DrawLine( xA, yA, xA, tmpYa );		//1 отрезок вниз от фигуры А, до половины расстояния между А и Б
						dc.DrawLine( xA, tmpYa, xB, tmpYa );	//2 отрезок вправо от конца 1 отрезка
						dc.DrawLine( xB, tmpYa, xB, yB );		//3 отрезок вниз от конца отрезка 2
						//4 (стрелка левая сторона) отрезок от середины 2 отрезка, вверх влево
						//5 (стрелка правая сторона)отрезок от середины 2 отрезка, вниз влево
						wxCoord tmpDX = xB + (xA - xB)/2;
						dc.DrawLine( tmpDX, tmpYa, tmpDX + 5, tmpYa - 3 );
						dc.DrawLine( tmpDX, tmpYa, tmpDX + 5, tmpYa + 3 );
						
						region = new wxRegion( wxPoint(xA - 2, yA), wxPoint(xA + 2, tmpYa) ); m_Regions.Append(region);
						region = new wxRegion( wxPoint(xA, tmpYa - 2), wxPoint(xB, tmpYa + 2) ); m_Regions.Append(region);
						region = new wxRegion( wxPoint(xB - 2, tmpYa), wxPoint(xB + 2, yB) ); m_Regions.Append(region);
					}
				}
				else if (yA > yB)	//Положение второй фигуры выше чем первой
				{
					if(xA < xB)		//Положение второй фигуры правее и выше чем первой
					{
						wxCoord tmpXa = xA + (xB-xA)/2;
						wxCoord tmpYa = yA + 5;
						if(m_StartShape->m_typeShape == Condition)	//Если фигура "условие", рисуем от левого отростка
						{
							tmpXa = (xB - m_bitmapE.GetWidth()/2) - 5;
							if(tmpXa >= xA)
								tmpXa = xA - 5;
							dc.DrawLine( xA,	yA,		tmpXa,	yA );		//1 отрезок влево от фигуры А
							dc.DrawLine( tmpXa, yA,		tmpXa,	yB - 5 );	//2 отрезок вверх от конца первого отрезка
							dc.DrawLine( tmpXa, yB - 5,	xB,		yB - 5 );	//3 отрезок вправо от конца второго отрезка
							//5 (стрелка левая сторона) отрезок от середины 2 отрезка, влево вниз
							//6 (стрелка правая сторона)отрезок от середины 2 отрезка, вправо вниз
							wxCoord tmpDY = yA - (yA-yB)/2 - 5;
							dc.DrawLine( tmpXa, tmpDY, tmpXa - 3, tmpDY+5 );
							dc.DrawLine( tmpXa, tmpDY, tmpXa + 3, tmpDY+5 );
							//Создадим регионы для представления данных на схеме
							region = new wxRegion( wxPoint(tmpXa, yA - 2), wxPoint(xA, yA + 2) ); m_Regions.Append(region);
							region = new wxRegion( wxPoint(tmpXa - 2, yB - 5), wxPoint(tmpXa + 2, yA) ); m_Regions.Append(region);
							region = new wxRegion( wxPoint(xB, yB - 5 - 2), wxPoint(tmpXa, yB - 5 + 2) ); m_Regions.Append(region);
						}
						else //Фигура не "условие" рисуем от нижнего отростка
						{
							//Проверим на промежуток между фигурами
							if((xA + m_bitmapS.GetWidth()/2) < (xB - m_bitmapE.GetWidth()/2))
							{
								dc.DrawLine( xA,	yA,		xA,		tmpYa );	//1 отрезок вниз от фигуры А
								dc.DrawLine( xA,	tmpYa, tmpXa,	tmpYa );	//2 отрезок вправо от конца первого отрезка
								dc.DrawLine( tmpXa, tmpYa, tmpXa,	yB - 5 );	//3 отрезок вверх от конца второго отрезка
								dc.DrawLine( tmpXa, yB - 5, xB,		yB - 5 );	//4 отрезок вправо от конца третьего отрезка

								region = new wxRegion( wxPoint(xA, tmpYa - 2), wxPoint(tmpXa, tmpYa + 2) ); m_Regions.Append(region);
								region = new wxRegion( wxPoint(tmpXa - 2, yB - 5), wxPoint(tmpXa + 2, tmpYa) ); m_Regions.Append(region);
								region = new wxRegion( wxPoint(tmpXa, yB - 5 - 2), wxPoint(xB, yB - 5 + 2) ); m_Regions.Append(region);
							}
							else									//Нет промежутка между фигурами
							{
								tmpXa = xB + m_bitmapE.GetWidth()/2 + 5;
								dc.DrawLine( xA,	yA,		xA,		tmpYa );	//1 отрезок вниз от фигуры А
								dc.DrawLine( xA,	tmpYa, tmpXa,	tmpYa );	//2 отрезок вправо от конца первого отрезка
								dc.DrawLine( tmpXa, tmpYa, tmpXa,	yB - 5 );	//3 отрезок вверх от конца второго отрезка
								dc.DrawLine( tmpXa, yB - 5, xB,		yB - 5 );	//4 отрезок влево от конца третьего отрезка

								region = new wxRegion( wxPoint(xA, tmpYa - 2), wxPoint(tmpXa, tmpYa + 2) ); m_Regions.Append(region);
								region = new wxRegion( wxPoint(tmpXa - 2, yB - 5), wxPoint(tmpXa + 2, tmpYa) ); m_Regions.Append(region);
								region = new wxRegion( wxPoint(xB, yB - 5 - 2), wxPoint(tmpXa, yB - 5 + 2) ); m_Regions.Append(region);
							}
							//6 (стрелка левая сторона) отрезок от середины 3 отрезка, влево вниз
							//7 (стрелка правая сторона)отрезок от середины 3 отрезка, вправо вниз
							wxCoord tmpDY = yA - (yA-yB)/2;
							dc.DrawLine( tmpXa, tmpDY, tmpXa-3, tmpDY+5 );
							dc.DrawLine( tmpXa, tmpDY, tmpXa+3, tmpDY+5 );
						}
					}
					else			//Положение второй фигуры левее и выше чем первой
					{
						wxCoord tmpXa = xA - (xA - xB)/2;
						wxCoord tmpYa = yA + 5;
						if(m_StartShape->m_typeShape == Condition)
						{
							tmpXa = xA-(xA-(xB + m_bitmapE.GetWidth()/2))/2;
							if(tmpXa >= xA)
							{
								tmpXa = (xB - m_bitmapE.GetWidth()/2) - 5;
								region = new wxRegion( wxPoint(tmpXa, yB - 5 - 2), wxPoint(xB, yB - 5 + 2) ); m_Regions.Append(region);//регион 3 отрезка
							}
							else
								region = new wxRegion( wxPoint(xB, yB - 5 - 2), wxPoint(tmpXa, yB - 5 + 2) ); m_Regions.Append(region);//регион 3 отрезка

							dc.DrawLine( xA, yA, tmpXa, yA );			//1 отрезок влево от фигуры А
							dc.DrawLine( tmpXa, yA, tmpXa, yB - 5 );	//2 отрезок вверх от конца отрезка 1
							dc.DrawLine( tmpXa, yB - 5, xB, yB - 5 );	//3 отрезок влево от конца отрезка 2
							//5 (стрелка левая сторона) отрезок от середины 2 отрезка, влево вниз
							//6 (стрелка правая сторона)отрезок от середины 2 отрезка, вправо вниз
							wxCoord tmpDY = yA - (yA-yB)/2 - 5;
							dc.DrawLine( tmpXa, tmpDY, tmpXa - 3, tmpDY+5 );
							dc.DrawLine( tmpXa, tmpDY, tmpXa + 3, tmpDY+5 );
							
							region = new wxRegion( wxPoint(tmpXa, yA - 2), wxPoint(xA, yA + 2) ); m_Regions.Append(region);//регион 1 отрезка 
							region = new wxRegion( wxPoint(tmpXa - 2, yB - 5), wxPoint(tmpXa + 2, yA) ); m_Regions.Append(region);//регион 2 отрезка
						}
						else
						{
							if((xA - m_bitmapS.GetWidth()/2) > (xB + m_bitmapE.GetWidth()/2))
							{
								dc.DrawLine( xA,	yA,		xA,		tmpYa );	//1 отрезок вниз от фигуры А
								dc.DrawLine( xA,	tmpYa,	tmpXa,	tmpYa );	//2 отрезок влево от конца отрезка 1
								dc.DrawLine( tmpXa, tmpYa,	tmpXa,	yB - 5 );	//3 отрезок вверх от конца отрезка 2
								dc.DrawLine( tmpXa, yB - 5, xB,		yB - 5 );	//4 отрезок влево от конца отрезка 3

								region = new wxRegion( wxPoint(tmpXa, tmpYa - 2), wxPoint(xA, tmpYa + 2) ); m_Regions.Append(region);
								region = new wxRegion( wxPoint(tmpXa - 2, yB - 5), wxPoint(tmpXa + 2, tmpYa) ); m_Regions.Append(region);
								region = new wxRegion( wxPoint(xB, yB - 5 - 2), wxPoint(tmpXa, yB - 5 + 2) ); m_Regions.Append(region);
							}
							else
							{
								tmpXa = xB - m_bitmapE.GetWidth()/2 - 5;
								dc.DrawLine( xA,	yA,		xA,		yA + 5 );	//1 отрезок вниз от фигуры А
								dc.DrawLine( xA,	yA + 5, tmpXa,	yA + 5 );	//2 отрезок влево от конца первого отрезка
								dc.DrawLine( tmpXa, yA + 5, tmpXa,	yB - 5 );	//3 отрезок вверх от конца второго отрезка
								dc.DrawLine( tmpXa, yB - 5, xB,		yB - 5 );	//4 отрезок вправо от конца третьего отрезка

								region = new wxRegion( wxPoint(tmpXa, tmpYa - 2), wxPoint(xA, tmpYa + 2) ); m_Regions.Append(region);
								region = new wxRegion( wxPoint(tmpXa - 2, yB - 5), wxPoint(tmpXa + 2, tmpYa) ); m_Regions.Append(region);
								region = new wxRegion( wxPoint(tmpXa, yB - 5 - 2), wxPoint(xB, yB - 5 + 2) ); m_Regions.Append(region);
							}
							//5 (стрелка левая сторона) отрезок от середины 2 отрезка, влево вниз
							//6 (стрелка правая сторона)отрезок от середины 2 отрезка, вправо вниз
							wxCoord tmpDY = yA - (yA-yB)/2;
							dc.DrawLine( tmpXa, tmpDY, tmpXa-3, tmpDY+5 );
							dc.DrawLine( tmpXa, tmpDY, tmpXa+3, tmpDY+5 );
						}
					}
					//Замыкающий отрезок для фигуры B которая выше фигуры A
					dc.DrawLine( xB, yB - 5, xB, yB );
				}
			}
		}
		
		//Второй отросток
		if(m_EndShape2 != NULL)
		{
			m_Regions2.DeleteContents(true);
			m_Regions2.Clear();

			m_bitmapE = m_EndShape2->GetBitmap();	  //Битовое изображение фигуры 3
			if (m_bitmapS.Ok() && m_bitmapE.Ok())
			{
				//Если это условие, то оно может соединиться с третьей фигурой
				m_posE = m_EndShape2->GetPosition();	  //Позиция третьей(End) фигуры X,Y

				if(m_StartShape->m_typeShape == Condition)
				{
					coorXA = m_bitmapS.GetWidth()-4;
					coorYA = m_bitmapS.GetHeight()/2 - 1;
				}
				else
				{
					coorXA = m_bitmapS.GetWidth()/2;	//Середина битовой фигуры - это ширина пополам X
					coorYA = m_bitmapS.GetHeight();		//Высота битовой фигуры, там где низ Y
				}

				coorXA = dc.DeviceToLogicalX( m_posS.x + coorXA ); //Отсюда начнём рисовать
				coorYA = dc.DeviceToLogicalY( m_posS.y + coorYA ); //первую фигуру
				//wxGetApp().GetSchemeFrame()->CalcScrolledPosition(coorXA, coorYA, &xA, &yA);

				wxCoord coorXC = 0;
				if(m_EndShape2->m_typeShape == Condition)	//Вторая фигура
					coorXC = (m_bitmapE.GetWidth()/2);
				else 
					coorXC = m_bitmapE.GetWidth()/2;
				wxCoord coorYC = 0;

				coorXC = dc.DeviceToLogicalX( m_posE.x + coorXC );
				coorYC = dc.DeviceToLogicalX( m_posE.y + coorYC );
				//wxGetApp().GetSchemeFrame()->CalcScrolledPosition(coorXC, coorYC, &xC, &yC);

				//if((wxGetApp().GetSchemeFrame()->GetSelectRelationship() == this) && (m_Select == m_EndShape2))
				//	dc.SetPen( wxPen( wxT("white"), 1, wxSOLID) );
				//else
				//	dc.SetPen( wxPen( wxT("black"), 1, wxSOLID) );

				//Нарисуем связи в соответствии с положением связанных объектов...
				if(yA < yC)		//Положение второй фигуры ниже, чем первой
				{
					if(xA < xC)	//Положение второй фигуры правее и ниже чем первой
					{
						wxCoord tmpYa = (yA + m_bitmapS.GetHeight()/2) + (yC - (yA + m_bitmapS.GetHeight()/2))/2;
						dc.DrawLine( xA, yA, xA, tmpYa );		//1 отрезок вниз от фигуры А, до половины расстояния между А и Б
						dc.DrawLine( xA, tmpYa, xC, tmpYa );	//2 отрезок вправо от конца 1 отрезка
						dc.DrawLine( xC, tmpYa, xC, yC );		//3 отрезок вниз от конца отрезка 2
						//4 (стрелка левая сторона) отрезок от середины 2 отрезка, вверх влево
						//5 (стрелка правая сторона)отрезок от середины 2 отрезка, вниз влево
						wxCoord tmpDX = xA + (xC-xA)/2;
						dc.DrawLine( tmpDX, tmpYa, tmpDX - 5, tmpYa - 3 );
						dc.DrawLine( tmpDX, tmpYa, tmpDX - 5, tmpYa + 3 );
						
						region = new wxRegion( wxPoint(xA - 2, yA), wxPoint(xA + 2, tmpYa) ); m_Regions2.Append(region);
						region = new wxRegion( wxPoint(xA, tmpYa - 2), wxPoint(xC, tmpYa + 2) ); m_Regions2.Append(region);
						region = new wxRegion( wxPoint(xC - 2, tmpYa), wxPoint(xC + 2, yC) ); m_Regions2.Append(region);
					}
					else	//Положение второй фигуры левее и ниже первой
					{
						wxCoord tmpYa = yA + (yC - yA)/2;
						if(m_StartShape->m_typeShape == Condition)
						{
							tmpYa = (yA + m_bitmapE.GetHeight()/2) + (yC - (yA + m_bitmapE.GetHeight()/2))/2;
						}
						dc.DrawLine( xA, yA, xA, tmpYa );		//1 отрезок вниз от фигуры А, до половины расстояния между А и Б
						dc.DrawLine( xA, tmpYa, xC, tmpYa );	//2 отрезок вправо от конца 1 отрезка
						dc.DrawLine( xC, tmpYa, xC, yC );		//3 отрезок вниз от конца отрезка 2
						//4 (стрелка левая сторона) отрезок от середины 2 отрезка, вверх влево
						//5 (стрелка правая сторона)отрезок от середины 2 отрезка, вниз влево
						wxCoord tmpDX = xC + (xA - xC)/2;
						dc.DrawLine( tmpDX, tmpYa, tmpDX + 5, tmpYa - 3 );
						dc.DrawLine( tmpDX, tmpYa, tmpDX + 5, tmpYa + 3 );
						
						region = new wxRegion( wxPoint(xA - 2, yA), wxPoint(xA + 2, tmpYa) ); m_Regions2.Append(region);
						region = new wxRegion( wxPoint(xA, tmpYa - 2), wxPoint(xC, tmpYa + 2) ); m_Regions2.Append(region);
						region = new wxRegion( wxPoint(xC - 2, tmpYa), wxPoint(xC + 2, yC) ); m_Regions2.Append(region);
					}
				}
				else if (yA > yC)	//Положение второй фигуры выше чем первой
				{
					if(xA < xC)		//Положение второй фигуры правее и выше чем первой
					{
						wxCoord tmpXa = xA + (xC - xA)/2;
						if(m_StartShape->m_typeShape == Condition)	//Если фигура "условие", рисуем от левого отростка
						{
							tmpXa = (xC - m_bitmapE.GetWidth()/2) + 5;
							if(tmpXa >= xA)
								tmpXa = xA + 5;
							dc.DrawLine( xA,	yA,		tmpXa,	yA );		//1 отрезок вправо от фигуры А
							dc.DrawLine( tmpXa, yA,		tmpXa,	yC - 5 );	//2 отрезок вверх от конца первого отрезка
							dc.DrawLine( tmpXa, yC - 5,	xC,		yC - 5 );	//3 отрезок вправо от конца второго отрезка
							//5 (стрелка левая сторона) отрезок от середины 2 отрезка, влево вниз
							//6 (стрелка правая сторона)отрезок от середины 2 отрезка, вправо вниз
							wxCoord tmpDY = yA - (yA - yC)/2 - 5;
							dc.DrawLine( tmpXa, tmpDY, tmpXa - 3, tmpDY+5 );
							dc.DrawLine( tmpXa, tmpDY, tmpXa + 3, tmpDY+5 );
							//Создадим регионы для представления данных на схеме
							region = new wxRegion( wxPoint(tmpXa, yA - 2), wxPoint(xA, yA + 2) ); m_Regions2.Append(region);
							region = new wxRegion( wxPoint(tmpXa - 2, yC - 5), wxPoint(tmpXa + 2, yA) ); m_Regions2.Append(region);
							region = new wxRegion( wxPoint(xC, yC - 5 - 2), wxPoint(tmpXa, yC - 5 + 2) ); m_Regions2.Append(region);
						}
					}
					else			//Положение второй фигуры левее и выше чем первой
					{
						wxCoord tmpXa = xC + (m_bitmapE.GetWidth()/2) + 5;
						wxCoord tmpYC = yC - 5;
						if(m_StartShape->m_typeShape == Condition)
						{
							dc.DrawLine( xA, yA, tmpXa, yA );		//1 отрезок вправо от фигуры А
							dc.DrawLine( tmpXa, yA, tmpXa, tmpYC );	//2 отрезок вверх от конца отрезка 1
							dc.DrawLine( tmpXa, tmpYC, xC, tmpYC );	//3 отрезок влево от конца отрезка 2
							//5 (стрелка левая сторона) отрезок от середины 2 отрезка, влево вниз
							//6 (стрелка правая сторона)отрезок от середины 2 отрезка, вправо вниз
							wxCoord tmpDY = yA - (yA - yC)/2 - 5;
							dc.DrawLine( tmpXa, tmpDY, tmpXa - 3, tmpDY + 5 );
							dc.DrawLine( tmpXa, tmpDY, tmpXa + 3, tmpDY + 5 );

							region = new wxRegion( wxPoint(tmpXa, yA - 2), wxPoint(xA, yA + 2) ); m_Regions2.Append(region);//регион 1 отрезка 
							region = new wxRegion( wxPoint(tmpXa - 2, yC - 5), wxPoint(tmpXa + 2, yA) ); m_Regions2.Append(region);//регион 2 отрезка
							region = new wxRegion( wxPoint(tmpXa, tmpYC - 2), wxPoint(xC, tmpYC + 2) ); m_Regions2.Append(region);//регион 3 отрезка
						}
					}
					//Замыкающий отрезок для фигуры B которая выше фигуры A
					dc.DrawLine( xC, yC - 5, xC, yC );
				}
			}
		}
		return true;
	}

	return false;
}
//**************************************************
bool CRelationship::HitTest(const wxCoord xIn, const wxCoord yIn)
{
	//wxCoord x,y;
	//wxGetApp().GetSchemeFrame()->CalcScrolledPosition(xIn, yIn, &x, &y);
	//
	//if(m_Regions.GetCount() > 0)
	//{
	//	wxList::compatibility_iterator node = m_Regions.GetFirst();
	//	while (node)
	//	{
	//		wxRegion *region = (wxRegion *)node->GetData();
	//		if(region->Contains(x, y))
	//		{
	//			m_Select = m_EndShape;
	//			return true;
	//		}
	//		node = node->GetNext();
	//	}
	//}

	//if(m_Regions2.GetCount() > 0)
	//{
	//	wxList::compatibility_iterator node = m_Regions2.GetFirst();
	//	while (node)
	//	{
	//		wxRegion *region = (wxRegion *)node->GetData();
	//		if(region->Contains(x, y))
	//		{
	//			m_Select = m_EndShape2;
	//			return true;
	//		}
	//		node = node->GetNext();
	//	}
	//}

	//m_Select = (CDragShape *)NULL;
	return false;
}