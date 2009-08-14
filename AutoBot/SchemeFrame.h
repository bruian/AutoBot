#ifndef __SCHEMEFRAME_H__
#define __SCHEMEFRAME_H__
#include "AutoBot.h"

class CRelationship;
class MyDragImage;
class CDragShape;
//**************************************************
//Описание класса Окна-Схем
class CSchemeFrame : public wxScrolledWindow
{
public:
	CSchemeFrame(){}
	CSchemeFrame(wxWindow *parent);
	~CSchemeFrame();

	void OnPaint(wxPaintEvent &event);
	void OnMouseEvent(wxMouseEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void OnScroll(wxScrollWinEvent& event);

	void ClearShapes();
	void ClearLines();
	void DrawShapes(wxDC& dc);
	void EraseShape(CDragShape* shape, wxDC& dc);
	void DrawLines( wxDC &dc );

	bool AddShapeOnFrame(GUID componentGuid, GUID shapeGUID, wxString nameComponent, wxPoint pos);
	void RefreshShapeOnFrame();

	CDragShape* FindShape(const wxPoint& pt) const;
	CDragShape* FindShape(const GUID value);
	CRelationship* FindRelationship(CDragShape *value);
	CRelationship* FindRelationship(const wxPoint& pt) const;
	wxList& GetDisplayList() { return m_displayList; }
	wxList& GetLineList() { return m_lineList; }
	CRelationship *GetSelectRelationship() { return m_selectRelationship; }
private:
	wxWindow *m_owner;
	wxPoint m_BegPoint;
	wxPoint m_CurPoint;
	wxPoint m_EndPoint;
	bool m_Line;
	long m_CountShape;

	wxList			m_displayList; // A list of DragShapes
	wxList			m_lineList;
	int				m_dragMode;
	CDragShape*		m_draggedShape;
	CRelationship*	m_draggedRelationship;
	CRelationship*	m_selectRelationship;
	CDragShape*		m_currentlyHighlighted; // The shape that's being highlighted
	wxPoint			m_dragStartPos;
	MyDragImage*	m_dragImage;

	DECLARE_DYNAMIC_CLASS(CSchemeFrame);
	DECLARE_EVENT_TABLE();
};
//**************************************************
//Описание класса элемент-схемы
class CDragShape: public wxObject
{
public:
	CDragShape(const wxBitmap& bitmap);
	~CDragShape(){};

	bool HitTest(const wxCoord x, const wxCoord y) const;
	bool Draw(wxDC& dc, int x, int y, wxInt16 highlight = 0);

	wxPoint GetPosition() const { return m_pos; }
	void SetPosition(const wxPoint& pos) { m_pos = pos; }

	wxRect GetRect() const { return wxRect(m_pos.x, m_pos.y, m_bitmap.GetWidth(), m_bitmap.GetHeight()); }

	wxBitmap& GetBitmap() const { return (wxBitmap&) m_bitmap; }
	void SetBitmap(const wxBitmap& bitmap) { m_bitmap = bitmap; }

	int GetDragMethod() const { return m_dragMethod; }
	void SetDragMethod(int method) { m_dragMethod = method; }

	bool IsShown() const { return m_show; }
	void SetShow(bool show) { m_show = show; }
	
	GUID		m_guidShape;
	eRuleType	m_typeShape;
protected:
	wxPoint		m_pos;
	wxBitmap	m_bitmap;
	int			m_dragMethod;
	bool		m_show;
};
//**************************************************
//Описание класса связи-схемы
class CRelationship: public wxObject
{
public:
	CRelationship();
	~CRelationship()
	{
		m_Regions.DeleteContents(true);
		m_Regions.Clear();

		m_Regions2.DeleteContents(true);
		m_Regions2.Clear();
	};

	bool Draw(wxDC& dc);
	bool HitTest(const wxCoord x, const wxCoord y);

	CDragShape *m_StartShape;
	CDragShape *m_EndShape;
	CDragShape *m_EndShape2;
	CDragShape *m_Select;
	wxList		m_Regions;
	wxList		m_Regions2;

	wxPoint		m_segmentPoint;
	bool		m_dragging;
};
//**************************************************
class MyDragImage: public wxGenericDragImage
{
public:
	MyDragImage(CSchemeFrame* canvas): m_canvas(canvas) {}

	MyDragImage(CSchemeFrame* canvas, const wxBitmap& image, const wxCursor& cursor = wxNullCursor):
				wxGenericDragImage(image, cursor), m_canvas(canvas)
	{
	}
	// On some platforms, notably Mac OS X with Core Graphics, we can't blit from
	// a window, so we need to draw the background explicitly.
	virtual bool UpdateBackingFromWindow(wxDC& windowDC, wxMemoryDC& destDC, const wxRect& sourceRect,
										 const wxRect& destRect) const;
	wxPoint GetOffset(void) { return m_offset; };
protected:
	CSchemeFrame*	m_canvas;
};
//**************************************************
#endif //SCHEMEFRAME