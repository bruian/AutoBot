/**************************************************
 * Name:      BotShape.cpp
 * Purpose:   Implements Bot shape class
 * Author:    Bruian Blake (bruianio@gmail.com)
 * Created:   2008-12-30
 * Copyright: Bruian Blake
 * License:   AutoBot license (www.bruian.com)
 * Notes:
 **************************************************/
//#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include "BotShape.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/CommonFcn.h"
#include "wxsf/RectShape.h"

#include "res/NoSource.xpm"
//**************************************************
XS_IMPLEMENT_CLONABLE_CLASS(wxSFIfShape, wxSFDiamondShape);
XS_IMPLEMENT_CLONABLE_CLASS(wxSFBotLineShape, wxSFLineShape);
XS_IMPLEMENT_CLONABLE_CLASS(wxSFTrigger, wxSFRectShape);
XS_IMPLEMENT_CLONABLE_CLASS(wxSFComponent, wxSFRectShape);
XS_IMPLEMENT_CLONABLE_CLASS(wxSFCycle, wxSFRectShape);
//**************************************************
//********************Figure IF*********************
//**************************************************
wxSFIfShape::wxSFIfShape(void)
: wxSFDiamondShape()
{
	m_fRescaleInProgress = false;
	m_fCanScale = sfdvIFSHAPE_SCALEIMAGE;
	CreateFromXPM(NoSource_xpm);

	// mark serialized properties
	MarkSerializableDataMembers();
}
//**************************************************
wxSFIfShape::wxSFIfShape(const wxRealPoint& pos, const wxString& imagePath, wxSFDiagramManager* manager)
: wxSFDiamondShape(pos, manager)
{
	m_fRescaleInProgress = false;
	m_fCanScale = sfdvIFSHAPE_SCALEIMAGE;

	// mark serialized properties
	MarkSerializableDataMembers();
}
//**************************************************
wxSFIfShape::wxSFIfShape(const wxSFIfShape& obj)
: wxSFDiamondShape(obj)
{
	m_fRescaleInProgress = false;
	m_fCanScale = obj.m_fCanScale;

	m_shapeGUID = obj.m_shapeGUID;
	// create real bitmap copy
	m_Image = obj.m_Image.GetSubImage(wxRect(0, 0, obj.m_Image.GetWidth(), obj.m_Image.GetHeight()));
	m_OriginalImage = m_Image;

	// mark serialized properties
	MarkSerializableDataMembers();
}
//**************************************************
wxSFIfShape::~wxSFIfShape(void)
{
}
//**************************************************
void wxSFIfShape::MarkSerializableDataMembers()
{
	XS_SERIALIZE(m_shapeGUID, wxT("GUID"));
	XS_SERIALIZE_EX(m_fCanScale, wxT("scale_image"), sfdvIFSHAPE_SCALEIMAGE);
}
//**************************************************
// public functions
//**************************************************
bool wxSFIfShape::CreateFromXPM(const char* const* bits)
{
	bool fSuccess = false;

	// create bitmap from XPM
	m_Image = wxImage(bits);
	fSuccess = m_Image.IsOk();

	if(!fSuccess)
	{
		m_Image = wxImage(NoSource_xpm);
	}

	m_OriginalImage = m_Image;

	m_nRectSize.x = m_Image.GetWidth();
	m_nRectSize.y = m_Image.GetHeight();

	wxRealPoint diamond[4]={wxRealPoint(0,						m_Image.GetHeight()/2), 
							wxRealPoint(m_Image.GetWidth()/2,	0), 
							wxRealPoint(m_Image.GetWidth(),		m_Image.GetHeight()/2), 
							wxRealPoint(m_Image.GetWidth()/2,	m_Image.GetHeight())};
	SetVertices(4, diamond);

	if(m_fCanScale)
	{
		AddStyle(sfsSIZE_CHANGE);
	}
	else
		RemoveStyle(sfsSIZE_CHANGE);

	return fSuccess;
}
//**************************************************
// public virtual functions
//**************************************************
void wxSFIfShape::Scale(double x, double y, bool children)
{
	if(m_fCanScale)
	{
		m_nRectSize.x *= x;
		m_nRectSize.y *= y;

		if(!m_fRescaleInProgress) RescaleImage(m_nRectSize);
		// call default function implementation (needed for scaling of shape's children)
		wxSFShapeBase::Scale(x, y, children);
	}
}
//**************************************************
void wxSFIfShape::OnBeginHandle(wxSFShapeHandle& WXUNUSED(handle))
{
	if(m_fCanScale)
	{
		m_fRescaleInProgress = true;
		m_nPrevPos = GetAbsolutePosition();
	}
}
//**************************************************
void wxSFIfShape::OnHandle(wxSFShapeHandle& handle)
{
	if(m_fCanScale)
	{
		wxSFDiamondShape::OnHandle(handle);
	}
	else
		RemoveStyle(sfsSIZE_CHANGE);
}
//**************************************************
void wxSFIfShape::OnEndHandle(wxSFShapeHandle& WXUNUSED(handle))
{
	if(m_fCanScale)
	{
		m_fRescaleInProgress = false;
		RescaleImage(m_nRectSize);
	}
}
//**************************************************
// protected functions
//**************************************************
void wxSFIfShape::RescaleImage(const wxRealPoint& size)
{
	if( GetParentCanvas() )
	{
		wxImage image = m_OriginalImage;

		if( wxSFShapeCanvas::IsGCEnabled() )
		{
			image.Rescale(int(size.x), int(size.y), wxIMAGE_QUALITY_NORMAL);
		}
		else
		{
			image.Rescale(int(size.x * GetParentCanvas()->GetScale()), int(size.y * GetParentCanvas()->GetScale()), wxIMAGE_QUALITY_NORMAL);
		}

		m_Image = image;
	}
}
//**************************************************
// protected virtual functions
//**************************************************
void wxSFIfShape::DrawNormal(wxDC& dc)
{
	// HINT: overload it for custom actions...
	if(m_fRescaleInProgress)
	{
		dc.DrawBitmap(wxBitmap(m_Image), wxSFCommonFcn::Conv2Point(m_nPrevPos));

		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.SetPen(wxPen(wxColour(100, 100, 100), 1, wxDOT));
		dc.DrawRectangle(wxSFCommonFcn::Conv2Point(GetAbsolutePosition()), wxSFCommonFcn::Conv2Size(m_nRectSize));
		dc.SetPen(wxNullPen);
		dc.SetBrush(wxNullBrush);
	}
	else
	{
		dc.DrawBitmap(wxBitmap(m_Image), wxSFCommonFcn::Conv2Point(GetAbsolutePosition()));
	}
}
//**************************************************
void wxSFIfShape::DrawHover(wxDC& dc)
{
	// HINT: overload it for custom actions...

	wxRealPoint pos = GetAbsolutePosition();
	dc.DrawBitmap(wxBitmap(m_Image), wxSFCommonFcn::Conv2Point(pos));

	dc.SetPen(wxPen(m_nHoverColor, 1));
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.DrawRectangle(wxSFCommonFcn::Conv2Point(pos), wxSFCommonFcn::Conv2Size(m_nRectSize));
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}
//**************************************************
void wxSFIfShape::DrawHighlighted(wxDC& dc)
{
	// HINT: overload it for custom actions...

	wxRealPoint pos = GetAbsolutePosition();
	dc.DrawBitmap(wxBitmap(m_Image), wxSFCommonFcn::Conv2Point(pos));

	dc.SetPen(wxPen(m_nHoverColor, 2));
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.DrawRectangle(wxSFCommonFcn::Conv2Point(pos), wxSFCommonFcn::Conv2Size(m_nRectSize));
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}
//**************************************************
void wxSFIfShape::OnLeftClick(const wxPoint &pos)
{
	wxPoint pon = pos;
}
//**************************************************
wxRealPoint wxSFIfShape::GetBorderPoint(bool condition)
{
	// HINT: override it for custom actions ...
	size_t ptsCnt = m_arrVertices.Count();
	if(ptsCnt == 0) return GetCenter();

	wxRealPoint intersection;
	wxRealPoint *pts = new wxRealPoint[ptsCnt];
	GetTranslatedVerices(pts);

	if(condition)
	{
		intersection = pts[0];
	}
	else
	{
		intersection = pts[2];
	}

	delete [] pts;
	return intersection;
}
//**************************************************
bool wxSFIfShape::CreateFromImage(wxImage *image)
{
	bool fSuccess = true;

	if(image)
	{
		m_Image = *image;
		fSuccess = true;
	}
	else
		fSuccess = false;

	if(!fSuccess)
	{
		m_Image = wxImage(NoSource_xpm);
	}

	m_OriginalImage = m_Image;

	m_nRectSize.x = m_Image.GetWidth();
	m_nRectSize.y = m_Image.GetHeight();

	wxRealPoint diamond[4]={wxRealPoint(0,						m_Image.GetHeight()/2), 
							wxRealPoint(m_Image.GetWidth()/2,	0), 
							wxRealPoint(m_Image.GetWidth(),		m_Image.GetHeight()/2), 
							wxRealPoint(m_Image.GetWidth()/2,	m_Image.GetHeight())};
	SetVertices(4, diamond);

	if(m_fCanScale)
	{
		AddStyle(sfsSIZE_CHANGE);
	}
	else
		RemoveStyle(sfsSIZE_CHANGE);

	return fSuccess;
}
//**************************************************
//*****************Figure LINE**********************
//**************************************************
void wxSFBotLineShape::GetDirectLine(wxRealPoint& src, wxRealPoint& trg)
{
	wxSFShapeBase* pSrcShape = GetShapeManager()->FindShape(m_nSrcShapeId);
	wxSFShapeBase* pTrgShape = GetShapeManager()->FindShape(m_nTrgShapeId);

	if(pSrcShape && pTrgShape)
	{
		wxRealPoint trgCenter = GetModTrgPoint();
		wxRealPoint srcCenter = GetModSrcPoint();

		if( (pSrcShape->GetParent() == pTrgShape) || (pTrgShape->GetParent() == pSrcShape) )
		{
			wxRect trgBB = pTrgShape->GetBoundingBox();
			wxRect srcBB = pSrcShape->GetBoundingBox();

			if( trgBB.Contains((int)srcCenter.x, (int)srcCenter.y) )
			{
				src = wxRealPoint(srcCenter.x, srcBB.GetTop());
				trg = wxRealPoint(trgBB.x + trgBB.GetWidth() / 2, trgBB.GetTop());
				return;
			}
			else if( srcBB.Contains((int)trgCenter.x, (int)trgCenter.y) )
			{
				src = wxRealPoint(srcCenter.x, srcBB.GetTop());
				trg = wxRealPoint(trgBB.x + trgBB.GetWidth() / 2, trgBB.GetTop());
				return;
			}
		}
		
		if(pSrcShape->IsKindOf(CLASSINFO(wxSFIfShape)))
		{
			wxSFIfShape *ifShape = (wxSFIfShape *)pSrcShape;
			if(GetCondition() == 1)
				src = ifShape->GetBorderPoint(true);
			else
				src = ifShape->GetBorderPoint(false);
		}
		else
			src = wxRealPoint(pSrcShape->GetBoundingBox().x + pSrcShape->GetBoundingBox().GetWidth() / 2, pSrcShape->GetBoundingBox().GetBottom());

		if(pTrgShape->IsKindOf(CLASSINFO(wxSFIfShape)))
			trg = wxRealPoint(trgCenter.x, pTrgShape->GetBoundingBox().GetTop());
		else
			trg = wxRealPoint(pTrgShape->GetBoundingBox().x + pTrgShape->GetBoundingBox().GetWidth() / 2, pTrgShape->GetBoundingBox().GetTop());
	}	
}
//**************************************************
void wxSFBotLineShape::DrawCompleteLine(wxDC& dc)
{
	if(!m_pParentManager)return;

	size_t i;
	wxRealPoint src, trg;
	wxSFShapeBase* pSrcShape = GetShapeManager()->FindShape(m_nSrcShapeId);

	switch(m_nMode)
	{
	case modeREADY:
		{
			// draw basic line parts
			for(i = 0; i <= m_lstPoints.GetCount(); i++)
			{
				GetLineSegment( i, src, trg );
				if((i == 0) && ( pSrcShape ))
					if(pSrcShape->IsKindOf(CLASSINFO(wxSFIfShape)))
					{
						wxSFIfShape *ifShape = (wxSFIfShape *)pSrcShape;
						if(GetCondition() == 1)
							src = ifShape->GetBorderPoint(true);
						else
							src = ifShape->GetBorderPoint(false);
					}
					dc.DrawLine(wxSFCommonFcn::Conv2Point(src), wxSFCommonFcn::Conv2Point(trg));
			}

			// draw target arrow
			if(m_pTrgArrow)m_pTrgArrow->Draw(src, trg, dc);
			// draw source arrow
			GetLineSegment( 0, src, trg );
			if(m_pSrcArrow)m_pSrcArrow->Draw(trg, src, dc);
		}
		break;
	case modeUNDERCONSTRUCTION:
		{
			// draw basic line parts
			for(i = 0; i < m_lstPoints.GetCount(); i++)
			{
				GetLineSegment( i, src, trg );
				if((i == 0) && ( pSrcShape ))
					if(pSrcShape->IsKindOf(CLASSINFO(wxSFIfShape)))
					{
						wxSFIfShape *ifShape = (wxSFIfShape *)pSrcShape;
						if(GetCondition() == 1)
							src = ifShape->GetBorderPoint(true);
						else
							src = ifShape->GetBorderPoint(false);
					}
					dc.DrawLine(wxSFCommonFcn::Conv2Point(src), wxSFCommonFcn::Conv2Point(trg));
			}
			// draw unfinished line segment if any (for interactive line creation)
			dc.SetPen( wxPen(*wxBLACK, 1, wxDOT) );

			if( i )
			{
				dc.DrawLine( wxSFCommonFcn::Conv2Point(trg), m_nUnfinishedPoint );
			}
			else
			{
				if( pSrcShape )
					if(pSrcShape->IsKindOf(CLASSINFO(wxSFIfShape)))
					{
						wxSFIfShape *ifShape = (wxSFIfShape *)pSrcShape;
						if(GetCondition() == 1)
							dc.DrawLine( wxSFCommonFcn::Conv2Point(ifShape->GetBorderPoint(true)), m_nUnfinishedPoint );
						else
							dc.DrawLine( wxSFCommonFcn::Conv2Point(ifShape->GetBorderPoint(false)), m_nUnfinishedPoint );
					}
					else
						dc.DrawLine( wxSFCommonFcn::Conv2Point(wxRealPoint(pSrcShape->GetBoundingBox().x + pSrcShape->GetBoundingBox().GetWidth() / 2, pSrcShape->GetBoundingBox().GetBottom())), m_nUnfinishedPoint );
			}
			dc.SetPen(wxNullPen);
		}
		break;
	case modeSRCCHANGE:
		{
			// draw basic line parts
			for(i = 1; i <= m_lstPoints.GetCount(); i++)
			{
				GetLineSegment( i, src, trg );
				dc.DrawLine(wxSFCommonFcn::Conv2Point(src), wxSFCommonFcn::Conv2Point(trg));
			}
			// draw linesegment being updated
			GetLineSegment( 0, src, trg );
			if( pSrcShape )
				if(pSrcShape->IsKindOf(CLASSINFO(wxSFIfShape)))
				{
					wxSFIfShape *ifShape = (wxSFIfShape *)pSrcShape;
					if(GetCondition() == 1)
						src = ifShape->GetBorderPoint(true);
					else
						src = ifShape->GetBorderPoint(false);
				}
			
			dc.SetPen(wxPen(*wxBLACK, 1, wxDOT));
			dc.DrawLine(m_nUnfinishedPoint, wxSFCommonFcn::Conv2Point(trg));
			dc.SetPen(wxNullPen);
		}
		break;
	case modeTRGCHANGE:
		{
			// draw basic line parts
			if( !m_lstPoints.IsEmpty() )
			{
				for(i = 0; i < m_lstPoints.GetCount(); i++)
				{
					GetLineSegment( i, src, trg );
					if((i == 0) && ( pSrcShape ))
						if(pSrcShape->IsKindOf(CLASSINFO(wxSFIfShape)))
						{
							wxSFIfShape *ifShape = (wxSFIfShape *)pSrcShape;
							if(GetCondition() == 1)
								src = ifShape->GetBorderPoint(true);
							else
								src = ifShape->GetBorderPoint(false);
						}
						dc.DrawLine(wxSFCommonFcn::Conv2Point(src), wxSFCommonFcn::Conv2Point(trg));
				}
			}
			else
				trg = GetSrcPoint();
			// draw linesegment being updated
			dc.SetPen(wxPen(*wxBLACK, 1, wxDOT));
			dc.DrawLine( wxSFCommonFcn::Conv2Point(trg), m_nUnfinishedPoint);
			dc.SetPen(wxNullPen);
		}
		break;
	}
}
//**************************************************
void wxSFBotLineShape::MarkSerializableDataMembers()
{
	XS_SERIALIZE(m_condition, wxT("condition"));
}
//**************************************************
wxRealPoint wxSFBotLineShape::GetTrgPoint()
{
	wxRealPoint pt1, pt2;
	
	wxSFShapeBase* pTrgShape = GetShapeManager()->FindShape(m_nTrgShapeId);
	
	if( pTrgShape && !m_lstPoints.IsEmpty() )
	{
		RealPointList::compatibility_iterator node = m_lstPoints.GetLast();
		if( node )
		{	
			pt2 = *node->GetData();
			wxRect trgBB = pTrgShape->GetBoundingBox();
			return wxRealPoint(trgBB.x + trgBB.GetWidth() / 2, trgBB.GetTop());
		}
	}
	else
	{
		if( m_nMode != modeUNDERCONSTRUCTION )
			GetDirectLine( pt1, pt2 );
		else
			pt2 = wxSFCommonFcn::Conv2RealPoint( m_nUnfinishedPoint );
			
		return pt2;
	}

	return wxRealPoint();
}
//**************************************************
wxRealPoint wxSFBotLineShape::GetSrcPoint()
{
	wxRealPoint pt1, pt2;
	
	wxSFShapeBase* pSrcShape = GetShapeManager()->FindShape(m_nSrcShapeId);
	
	if( pSrcShape && !m_lstPoints.IsEmpty() )
	{
		RealPointList::compatibility_iterator node = m_lstPoints.GetFirst();
		if( node )
		{	
			pt1 = *node->GetData();
			if(pSrcShape->IsKindOf(CLASSINFO(wxSFIfShape)))
			{
				wxSFIfShape *ifShape = (wxSFIfShape *)pSrcShape;
				if(GetCondition() == 1)
					return ifShape->GetBorderPoint(true);
				else
					return ifShape->GetBorderPoint(false);
			}
			else
				return wxRealPoint(pSrcShape->GetBoundingBox().x + pSrcShape->GetBoundingBox().GetWidth() / 2, pSrcShape->GetBoundingBox().GetBottom());
		}
	}
	else
	{
		if( m_nMode != modeUNDERCONSTRUCTION )
			GetDirectLine( pt1, pt2 );
		else
			pt1 = wxRealPoint(pSrcShape->GetBoundingBox().x + pSrcShape->GetBoundingBox().GetWidth() / 2, pSrcShape->GetBoundingBox().GetBottom());
		return pt1;
	}

	return wxRealPoint();
}
//**************************************************
void wxSFBotLineShape::OnEndHandle(wxSFShapeHandle& handle)
{
	wxSFShapeBase *pParent = GetParentCanvas()->GetShapeUnderCursor();
	if( pParent )
	{
		wxPoint lpos = /*wxSFShapeCanvas::DP2LP(*/handle.GetPosition()/*)*/;
		wxRect bbRect = pParent->GetBoundingBox();
		int centr = bbRect.GetPosition().x + (bbRect.GetWidth()/2);

		switch( handle.GetType() )
		{
			case wxSFShapeHandle::hndLINESTART:
				if( pParent->GetId() == m_nSrcShapeId )
				{
					if(pParent->IsKindOf(CLASSINFO(wxSFIfShape)))
					{
						wxSFIfShape *ifShape = (wxSFIfShape*)pParent;

						wxSFBotLineShape* pLine;
						ShapeList m_lstLines;
						GetShapeManager()->GetShapes(CLASSINFO(wxSFBotLineShape), m_lstLines);
						if(!m_lstLines.IsEmpty())
						{
							ShapeList::compatibility_iterator node = m_lstLines.GetFirst();
							while(node)
							{
								pLine = (wxSFBotLineShape*)node->GetData();
								if(lpos.x <= centr) //True
								{
									if( (ifShape->GetId() == pLine->GetSrcShapeId()) &&
										(pLine->GetCondition() == 1) )	return;
								}
								else if(lpos.x > centr) //False
								{
									if( (ifShape->GetId() == pLine->GetSrcShapeId()) &&
										(pLine->GetCondition() == 0) )	return;
								}
								node = node->GetNext();
							}
						}

						if(lpos.x <= centr) //True
						{
							SetCondition(1);
						}
						else if(lpos.x > centr) //False
						{
							SetCondition(0);
						}
					}
				}
				break;
			case wxSFShapeHandle::hndLINEEND:
				if( pParent->GetId() == m_nTrgShapeId )
				{
					if(pParent->IsKindOf(CLASSINFO(wxSFIfShape)))
					{
						wxSFIfShape *ifShape = (wxSFIfShape*)pParent;

						wxSFBotLineShape* pLine;
						ShapeList m_lstLines;
						GetShapeManager()->GetShapes(CLASSINFO(wxSFBotLineShape), m_lstLines);
						
						if(!m_lstLines.IsEmpty())
						{
							ShapeList::compatibility_iterator node = m_lstLines.GetFirst();
							while(node)
							{
								pLine = (wxSFBotLineShape*)node->GetData();
								if( pLine->GetTrgShapeId() == pParent->GetId() )
									return;
								node = node->GetNext();
							}
						}
					}

					wxSFShapeBase* pSourceShape = GetShapeManager()->FindShape(m_nSrcShapeId);
					if(pSourceShape->IsKindOf(CLASSINFO(wxSFCycle)))
					{
						//if(pSourceShape = pShapeUnder->GetParentShape())
						//	return;
					}
					else
					{
						if(!pSourceShape->GetParentShape())
							if(pParent->GetParentShape())
								return;
						if(pSourceShape->GetParentShape())
							if(!pParent->GetParentShape())
								return;
						if(pSourceShape->GetParentShape())
							if(pParent->GetParentShape())
								if(pParent->GetParentShape() != pSourceShape->GetParentShape())
									return;
					}

					if( !LineCanConnect(pParent) )
						return;
				}
				break;
			default:
				break;
		}
	}
	wxSFLineShape::OnEndHandle(handle);
}
//**************************************************
bool wxSFBotLineShape::LineCanConnect(wxSFShapeBase* pShapeUnder)
{
	ShapeList m_lstLines;
	GetShapeManager()->GetShapes(CLASSINFO(wxSFBotLineShape), m_lstLines);
	if(m_lstLines.IsEmpty())
		return true;

	wxSFShapeBase *pSrc = GetShapeManager()->FindShape(GetSrcShapeId());
	wxSFShapeBase *pTrg = pShapeUnder;

	wxSFBotLineShape* pLine = (wxSFBotLineShape*)NULL;
	ShapeList::compatibility_iterator ciLine;
	
	//Если фигура источник, хочет присоедениться к фигуре источнику
	if(pSrc == pTrg) return false;
	
	//Если фигура источник - цикл, при этом соединяется с другой фигурой в составе цикла
	//то проверим что бы фигура с которой соединяемся была в составе фигуры источника
	if( pSrc->GetParentShape() )
	{
		//if( pSrc->IsKindOf(CLASSINFO(wxSFCycle)) )
			if( pTrg->GetParentShape() )
			{
				if( pTrg->GetParent() != pSrc )
					if( pSrc->GetParentShape() != pTrg->GetParentShape() ) return false;
			}
			else return false;
	}
	else
		if( pSrc->IsKindOf(CLASSINFO(wxSFCycle)) )
			if( pTrg->GetParentShape() )
				if( pSrc != pTrg->GetParentShape() ) return false;
	//Если фигура назначения в составе другой фигуры, то проверим что бы фигура контейнер была на том же уровне
	//что и фигура источник
	if( pTrg->GetParentShape() )
		if( pSrc->GetParentShape() )
		{
			if( pSrc->GetParentShape() != pTrg->GetParentShape() )
				if( pSrc != pTrg->GetParentShape() ) return false;
		}
		else
			if( pSrc != pTrg->GetParentShape() ) return false;

	ciLine = m_lstLines.GetFirst();
	while(ciLine)
	{
		pLine = (wxSFBotLineShape*)ciLine->GetData();
		//Если фигура источник имеет тип - цикл, то может иметь 2 исходящих соединения,
		//все остальные фигуры только по одному
		if( pTrg->IsKindOf(CLASSINFO(wxSFCycle)) )
		{
			if( (pLine != this) &&
				(pLine->GetTrgShapeId() == pTrg->GetId()) ) return false;
		}
		else
		{
			//Если фигура источник имеет уже исходящие соединения, то соединять не будем
			if( (pLine != this) && //Удостоверимся что проверяем все линии кроме, той которой хотим соединить
				(pLine->GetTrgShapeId() == pTrg->GetId()) ) return false;
		}

		ciLine = ciLine->GetNext();
	}
	
	long currentID = GetSrcShapeId();
	long trgID = pShapeUnder->GetId();
	bool broke = true;

	while(broke)
	{
		broke = false;

		ShapeList::compatibility_iterator node = m_lstLines.GetFirst();
		while(node)
		{
			if(currentID == trgID)
				return false;
			pLine = (wxSFBotLineShape*)node->GetData();
			if(pLine->GetTrgShapeId() == currentID)
			{
				if(pLine != this)
				{
					currentID = pLine->GetSrcShapeId();
					broke = true;
				}
			}
			
			node = node->GetNext();
		}
	}

	return true;
}

//**************************************************
//*****************Figure TRIGGER*******************
//**************************************************
void wxSFTrigger::MarkSerializableDataMembers()
{
	XS_SERIALIZE(m_shapeGUID, wxT("GUID"));
	XS_SERIALIZE_EX(m_fCanScale, wxT("scale_image"), sfdvTRIGGERSHAPE_SCALEIMAGE);
}
//**************************************************
wxSFTrigger::wxSFTrigger(void)
: wxSFRectShape()
{
	m_fRescaleInProgress = false;
	m_fCanScale = sfdvTRIGGERSHAPE_SCALEIMAGE;
	CreateFromXPM(NoSource_xpm);

	// mark serialized properties
	MarkSerializableDataMembers();
}
//**************************************************
wxSFTrigger::wxSFTrigger(const wxRealPoint& pos, const wxRealPoint& size, const wxString& imagePath, wxSFDiagramManager* manager)
: wxSFRectShape(pos, size, manager)
{
	m_fRescaleInProgress = false;
	m_fCanScale = sfdvTRIGGERSHAPE_SCALEIMAGE;

	// mark serialized properties
	MarkSerializableDataMembers();
}
//**************************************************
wxSFTrigger::wxSFTrigger(const wxSFTrigger& obj)
: wxSFRectShape(obj)
{
	m_fRescaleInProgress = false;
	m_fCanScale = obj.m_fCanScale;

	m_shapeGUID = obj.m_shapeGUID;
	// create real bitmap copy
	m_Image = obj.m_Image.GetSubImage(wxRect(0, 0, obj.m_Image.GetWidth(), obj.m_Image.GetHeight()));
	m_OriginalImage = m_Image;

	// mark serialized properties
	MarkSerializableDataMembers();
}
//**************************************************
wxSFTrigger::~wxSFTrigger(void)
{
}
//**************************************************
bool wxSFTrigger::CreateFromImage(wxImage *image)
{
	bool fSuccess = true;

	if(image)
	{
		m_Image = *image;
		fSuccess = true;
	}
	else
		fSuccess = false;

	if(!fSuccess)
	{
		m_Image = wxImage(NoSource_xpm);
	}

	m_OriginalImage = m_Image;

	m_nRectSize.x = m_Image.GetWidth();
	m_nRectSize.y = m_Image.GetHeight();

	if(m_fCanScale)
	{
		AddStyle(sfsSIZE_CHANGE);
	}
	else
		RemoveStyle(sfsSIZE_CHANGE);

	return fSuccess;
}
//**************************************************
void wxSFTrigger::Scale(double x, double y, bool children)
{
	if(m_fCanScale)
	{
		m_nRectSize.x *= x;
		m_nRectSize.y *= y;

		if(!m_fRescaleInProgress) RescaleImage(m_nRectSize);
		// call default function implementation (needed for scaling of shape's children)
		wxSFShapeBase::Scale(x, y, children);
	}
}
//**************************************************
bool wxSFTrigger::CreateFromXPM(const char* const* bits)
{
	bool fSuccess = false;

	// create bitmap from XPM
	m_Image = wxImage(bits);
	fSuccess = m_Image.IsOk();

	if(!fSuccess)
	{
		m_Image = wxImage(NoSource_xpm);
	}

	m_OriginalImage = m_Image;

	m_nRectSize.x = m_Image.GetWidth();
	m_nRectSize.y = m_Image.GetHeight();

	if(m_fCanScale)
	{
		AddStyle(sfsSIZE_CHANGE);
	}
	else
		RemoveStyle(sfsSIZE_CHANGE);

	return fSuccess;
}
//**************************************************
void wxSFTrigger::RescaleImage(const wxRealPoint& size)
{
	if( GetParentCanvas() )
	{
		wxImage image = m_OriginalImage;

		if( wxSFShapeCanvas::IsGCEnabled() )
		{
			image.Rescale(int(size.x), int(size.y), wxIMAGE_QUALITY_NORMAL);
		}
		else
		{
			image.Rescale(int(size.x * GetParentCanvas()->GetScale()), int(size.y * GetParentCanvas()->GetScale()), wxIMAGE_QUALITY_NORMAL);
		}

		m_Image = image;
	}
}
//**************************************************
// protected virtual functions
//**************************************************
void wxSFTrigger::DrawNormal(wxDC& dc)
{
	// HINT: overload it for custom actions...
	if(m_fRescaleInProgress)
	{
		dc.DrawBitmap(wxBitmap(m_Image), wxSFCommonFcn::Conv2Point(m_nPrevPos));

		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.SetPen(wxPen(wxColour(100, 100, 100), 1, wxDOT));
		dc.DrawRectangle(wxSFCommonFcn::Conv2Point(GetAbsolutePosition()), wxSFCommonFcn::Conv2Size(m_nRectSize));
		dc.SetPen(wxNullPen);
		dc.SetBrush(wxNullBrush);
	}
	else
		dc.DrawBitmap(wxBitmap(m_Image), wxSFCommonFcn::Conv2Point(GetAbsolutePosition()));
}
//**************************************************
void wxSFTrigger::DrawHover(wxDC& dc)
{
	// HINT: overload it for custom actions...

	wxRealPoint pos = GetAbsolutePosition();
	dc.DrawBitmap(wxBitmap(m_Image), wxSFCommonFcn::Conv2Point(pos));

	dc.SetPen(wxPen(m_nHoverColor, 1));
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.DrawRectangle(wxSFCommonFcn::Conv2Point(pos), wxSFCommonFcn::Conv2Size(m_nRectSize));
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}
//**************************************************
void wxSFTrigger::DrawHighlighted(wxDC& dc)
{
	// HINT: overload it for custom actions...

	wxRealPoint pos = GetAbsolutePosition();
	dc.DrawBitmap(wxBitmap(m_Image), wxSFCommonFcn::Conv2Point(pos));

	dc.SetPen(wxPen(m_nHoverColor, 2));
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.DrawRectangle(wxSFCommonFcn::Conv2Point(pos), wxSFCommonFcn::Conv2Size(m_nRectSize));
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}
//**************************************************
//***************Figure COMPONENT*******************
//**************************************************
void wxSFComponent::MarkSerializableDataMembers()
{
	XS_SERIALIZE(m_shapeGUID, wxT("GUID"));
	XS_SERIALIZE_EX(m_fCanScale, wxT("scale_image"), sfdvCOMPONENTSHAPE_SCALEIMAGE);
}
//**************************************************
wxSFComponent::wxSFComponent(void)
: wxSFRectShape()
{
	m_fRescaleInProgress = false;
	m_fCanScale = sfdvCOMPONENTSHAPE_SCALEIMAGE;
	CreateFromXPM(NoSource_xpm);

	// mark serialized properties
	MarkSerializableDataMembers();
}
//**************************************************
wxSFComponent::wxSFComponent(const wxRealPoint& pos, const wxRealPoint& size, const wxString& imagePath, wxSFDiagramManager* manager)
: wxSFRectShape(pos, size, manager)
{
	m_fRescaleInProgress = false;
	m_fCanScale = sfdvCOMPONENTSHAPE_SCALEIMAGE;

	// mark serialized properties
	MarkSerializableDataMembers();
}
//**************************************************
wxSFComponent::wxSFComponent(const wxSFComponent& obj)
: wxSFRectShape(obj)
{
	m_fRescaleInProgress = false;
	m_fCanScale = obj.m_fCanScale;

	m_shapeGUID = obj.m_shapeGUID;
	// create real bitmap copy
	m_Image = obj.m_Image.GetSubImage(wxRect(0, 0, obj.m_Image.GetWidth(), obj.m_Image.GetHeight()));
	m_OriginalImage = m_Image;

	// mark serialized properties
	MarkSerializableDataMembers();
}
//**************************************************
wxSFComponent::~wxSFComponent(void)
{
}
//**************************************************
void wxSFComponent::Scale(double x, double y, bool children)
{
	if(m_fCanScale)
	{
		m_nRectSize.x *= x;
		m_nRectSize.y *= y;

		if(!m_fRescaleInProgress) RescaleImage(m_nRectSize);
		// call default function implementation (needed for scaling of shape's children)
		wxSFShapeBase::Scale(x, y, children);
	}
}
//**************************************************
bool wxSFComponent::CreateFromXPM(const char* const* bits)
{
	bool fSuccess = false;

	// create bitmap from XPM
	m_Image = wxImage(bits);
	fSuccess = m_Image.IsOk();

	if(!fSuccess)
	{
		m_Image = wxImage(NoSource_xpm);
	}

	m_OriginalImage = m_Image;

	m_nRectSize.x = m_Image.GetWidth();
	m_nRectSize.y = m_Image.GetHeight();

	if(m_fCanScale)
	{
		AddStyle(sfsSIZE_CHANGE);
	}
	else
		RemoveStyle(sfsSIZE_CHANGE);

	return fSuccess;
}
//**************************************************
void wxSFComponent::RescaleImage(const wxRealPoint& size)
{
	if( GetParentCanvas() )
	{
		wxImage image = m_OriginalImage;

		if( wxSFShapeCanvas::IsGCEnabled() )
		{
			image.Rescale(int(size.x), int(size.y), wxIMAGE_QUALITY_NORMAL);
		}
		else
		{
			image.Rescale(int(size.x * GetParentCanvas()->GetScale()), int(size.y * GetParentCanvas()->GetScale()), wxIMAGE_QUALITY_NORMAL);
		}

		m_Image = image;
	}
}
//**************************************************
// protected virtual functions
//**************************************************
void wxSFComponent::DrawNormal(wxDC& dc)
{
	// HINT: overload it for custom actions...
	if(m_fRescaleInProgress)
	{
		dc.DrawBitmap(wxBitmap(m_Image), wxSFCommonFcn::Conv2Point(m_nPrevPos));

		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.SetPen(wxPen(wxColour(100, 100, 100), 1, wxDOT));
		dc.DrawRectangle(wxSFCommonFcn::Conv2Point(GetAbsolutePosition()), wxSFCommonFcn::Conv2Size(m_nRectSize));
		dc.SetPen(wxNullPen);
		dc.SetBrush(wxNullBrush);
	}
	else
		dc.DrawBitmap(wxBitmap(m_Image), wxSFCommonFcn::Conv2Point(GetAbsolutePosition()));
}
//**************************************************
void wxSFComponent::DrawHover(wxDC& dc)
{
	// HINT: overload it for custom actions...

	wxRealPoint pos = GetAbsolutePosition();
	dc.DrawBitmap(wxBitmap(m_Image), wxSFCommonFcn::Conv2Point(pos));

	dc.SetPen(wxPen(m_nHoverColor, 1));
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.DrawRectangle(wxSFCommonFcn::Conv2Point(pos), wxSFCommonFcn::Conv2Size(m_nRectSize));
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}
//**************************************************
void wxSFComponent::DrawHighlighted(wxDC& dc)
{
	// HINT: overload it for custom actions...

	wxRealPoint pos = GetAbsolutePosition();
	dc.DrawBitmap(wxBitmap(m_Image), wxSFCommonFcn::Conv2Point(pos));

	dc.SetPen(wxPen(m_nHoverColor, 2));
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.DrawRectangle(wxSFCommonFcn::Conv2Point(pos), wxSFCommonFcn::Conv2Size(m_nRectSize));
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}
//**************************************************
bool wxSFComponent::CreateFromImage(wxImage *image)
{
	bool fSuccess = true;

	if(image)
	{
		m_Image = *image;
		fSuccess = true;
	}
	else
		fSuccess = false;

	if(!fSuccess)
	{
		m_Image = wxImage(NoSource_xpm);
	}

	m_OriginalImage = m_Image;

	m_nRectSize.x = m_Image.GetWidth();
	m_nRectSize.y = m_Image.GetHeight();

	if(m_fCanScale)
	{
		AddStyle(sfsSIZE_CHANGE);
	}
	else
		RemoveStyle(sfsSIZE_CHANGE);

	return fSuccess;
}
//**************************************************
//******************Figure CYCLE********************
//**************************************************
void wxSFCycle::MarkSerializableDataMembers()
{
	XS_SERIALIZE(m_shapeGUID, wxT("GUID"));
}
//**************************************************
wxSFCycle::wxSFCycle(void)
: wxSFRectShape()
{
	MarkSerializableDataMembers();
}
//**************************************************
wxSFCycle::wxSFCycle(const wxRealPoint& pos, const wxRealPoint& size, wxSFDiagramManager* manager)
: wxSFRectShape(pos, size, manager)
{
	MarkSerializableDataMembers();
}
//**************************************************
wxSFCycle::wxSFCycle(const wxSFCycle& obj)
: wxSFRectShape(obj)
{
	MarkSerializableDataMembers();
}
//**************************************************
wxSFCycle::~wxSFCycle(void)
{
}
//**************************************************
// protected virtual functions
//**************************************************
//void wxSFCycle::DrawNormal(wxDC& dc)
//{
//	// HINT: overload it for custom actions...
//}
////**************************************************
//void wxSFCycle::DrawHover(wxDC& dc)
//{
//	// HINT: overload it for custom actions...
//}
////**************************************************
//void wxSFCycle::DrawHighlighted(wxDC& dc)
//{
//	// HINT: overload it for custom actions...
//}
//**************************************************