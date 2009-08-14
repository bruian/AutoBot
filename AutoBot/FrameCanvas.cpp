//**************************************************
#include "FrameCanvas.h"
#include "AutoBot.h"

#include <wx/textdlg.h>
#include <wx/filename.h>

BEGIN_EVENT_TABLE(FrameCanvas, wxSFShapeCanvas)
	EVT_KEY_UP(FrameCanvas::OnKeyUp)
END_EVENT_TABLE()
//**************************************************
FrameCanvas::FrameCanvas(wxSFDiagramManager* manager, wxWindow* parent, wxWindowID id)
: wxSFShapeCanvas(manager, parent, id, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL | wxSTATIC_BORDER)
{
	// get pointer to main application frame
	m_pParentFrame = (wxAutoBotFrame*)parent;/*(wxAutoBotFrame*)wxGetApp().GetTopWindow()*/;

	// initialize grid
	AddStyle(sfsGRID_USE);
	AddStyle(sfsGRID_SHOW);

	// the canvas background can be filled with a solid colour ...
	AddStyle(sfsGRADIENT_BACKGROUND);
	SetGradientFrom(sfdvSHAPECANVAS_GRADIENT_FROM);
	SetGradientTo(sfdvSHAPECANVAS_GRADIENT_TO);

	// also shadows style can be set here:
	//SetShadowFill(wxBrush(wxColour(100, 100, 100), wxCROSSDIAG_HATCH)); // standard values can be sfdvSHAPECANVAS_SHADOWBRUSH or sfdvSHAPECANVAS_SHADOWCOLOR
	//SetShadowOffset(wxRealPoint(7, 7));

	// now you can use also these styles...

	// RemoveStyle(sfsHOVERING);
	// RemoveStyle(sfsHIGHLIGHTING);
	// RemoveStyle(sfsUNDOREDO);
	// RemoveStyle(sfsDND);
	// RemoveStyle(sfsCLIPBOARD);
	// RemoveStyle(sfsMULTI_SIZE_CHANGE);
	// RemoveStyle(sfsMULTI_SELECTION);

	// a style flag presence can be tested like this:
	// if( ContainsStyle(sfsGRID_USE) ) DoSomething();

	// multiple styles can be set in this way:
	// SetStyle( sfsGRID_USE | sfsGRID_SHOW ) ... or ...
	// SetStyle( sfsDEFAULT_CANVAS_STYLE )

	// set accepted shapes
	GetDiagramManager()->ClearAcceptedShapes();
	GetDiagramManager()->AcceptShape(wxT("All"));

	// you can set also the canvas history manager working mode:
	// 1) FASTER, but requires implementation of xsSerializable::Clone() virtual function
	// in all classes derived from the xsSerializable class
	GetHistoryManager().SetMode(wxSFCanvasHistory::histUSE_CLONING);
	// 2) SLOWER, but no other programming overhead is required (except implementation
	// of standard serialization functionality). This working mode is default.
	// GetHistoryManager().SetMode(wxSFCanvasHistory::histUSE_SERIALIZATION);

	// change of the canvas history manager working mode clears the stored states
	// so we should save initial canvas state manually
	SaveCanvasState();
}
//**************************************************
FrameCanvas::~FrameCanvas(void)
{
}
//**************************************************
void FrameCanvas::OnLeftDown(wxMouseEvent& event)
{
	wxSFShapeBase *pShape = NULL;

	switch(m_pParentFrame->GetToolMode())
	{
	case wxAutoBotFrame::modeIF:
		{
			if(!wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_AddRule, this, (wxEvent *)NULL))
				return;
			
			CComponent *comp = wxGetApp().GetTreeBot()->GetActiveRule()->GetComponent();
			if(!comp)
				return;

			pShape = GetDiagramManager()->AddShape(CLASSINFO(wxSFIfShape), event.GetPosition(), sfDONT_SAVE_STATE);
			if(pShape)
			{
				((wxSFIfShape *)pShape)->SetShapeGUID(wxGetApp().GetAppProperties()->GuidToStr(wxGetApp().GetTreeBot()->GetActiveRule()->GetGUIDRule()));
				// create image from BMP file
				((wxSFIfShape*)pShape)->CreateFromImage( &comp->GetImage() );
				// set shape policy
				pShape->AcceptConnection(wxT("All"));
				pShape->AcceptSrcNeighbour(wxT("All"));
				pShape->AcceptTrgNeighbour(wxT("All"));
				pShape->RemoveStyle(wxSFShapeBase::sfsSIZE_CHANGE);
				pShape->RemoveStyle(wxSFShapeBase::sfsSHOW_HANDLES);
			}
		}
		break;
	case wxAutoBotFrame::modeTEXT:
	case wxAutoBotFrame::modeEDITTEXT:
		{
			wxTextEntryDialog dlg(this, wxT(""), wxT("Enter text"), wxT("Hello World!"));

			if(dlg.ShowModal() == wxID_OK)
			{
				if(m_pParentFrame->GetToolMode() == wxAutoBotFrame::modeTEXT)
					pShape = GetDiagramManager()->AddShape(CLASSINFO(wxSFTextShape), event.GetPosition(), sfDONT_SAVE_STATE);
				else
					pShape = GetDiagramManager()->AddShape(CLASSINFO(wxSFEditTextShape), event.GetPosition(), sfDONT_SAVE_STATE);

				if(pShape)
				{
					((wxSFTextShape*)pShape)->SetText(dlg.GetValue());

					// set alignment
					pShape->SetVAlign(wxSFShapeBase::valignTOP);
					pShape->SetHAlign(wxSFShapeBase::halignCENTER);
					pShape->SetVBorder(10);

					// set shapes policy
					pShape->AcceptConnection(wxT("All"));
					pShape->AcceptSrcNeighbour(wxT("All"));
					pShape->AcceptTrgNeighbour(wxT("wxSFTextShape"));
					pShape->AcceptTrgNeighbour(wxT("wxSFEditTextShape"));
				}
			}
		}
		break;
	case wxAutoBotFrame::modeTRIGGER:
		{
			if(!wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_AddRule, this, (wxEvent *)NULL))
				return;
			
			CComponent *comp = wxGetApp().GetTreeBot()->GetActiveRule()->GetComponent();
			if(!comp)
				return;

			pShape = GetDiagramManager()->AddShape(CLASSINFO(wxSFTrigger), event.GetPosition(), sfDONT_SAVE_STATE);
			if(pShape)
			{
				((wxSFTrigger*)pShape)->SetShapeGUID(wxGetApp().GetAppProperties()->GuidToStr(wxGetApp().GetTreeBot()->GetActiveRule()->GetGUIDRule()));
				((wxSFTrigger*)pShape)->CreateFromImage(&comp->GetImage());

				// set shape policy
				pShape->AcceptConnection(wxT("All"));
				pShape->AcceptTrgNeighbour(wxT("All"));
				pShape->RemoveStyle(wxSFShapeBase::sfsSIZE_CHANGE);
				pShape->RemoveStyle(wxSFShapeBase::sfsSHOW_HANDLES);
			}
		}
		break;
	case wxAutoBotFrame::modeCOMPONENT:
		{
			if(!wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_AddRule, this, (wxEvent *)NULL))
				return;

			CComponent *comp = wxGetApp().GetTreeBot()->GetActiveRule()->GetComponent();
			if(!comp)
				return;

			pShape = GetDiagramManager()->AddShape(CLASSINFO(wxSFComponent), event.GetPosition(), sfDONT_SAVE_STATE);
			if(pShape)
			{
				((wxSFComponent *)pShape)->SetShapeGUID(wxGetApp().GetAppProperties()->GuidToStr(wxGetApp().GetTreeBot()->GetActiveRule()->GetGUIDRule()));
				// create image from BMP file
				((wxSFComponent*)pShape)->CreateFromImage( &comp->GetImage() );

				// set shape policy
				pShape->AcceptConnection(wxT("All"));
				pShape->AcceptSrcNeighbour(wxT("All"));
				pShape->AcceptTrgNeighbour(wxT("All"));
				pShape->RemoveStyle(wxSFShapeBase::sfsSIZE_CHANGE);
				pShape->RemoveStyle(wxSFShapeBase::sfsSHOW_HANDLES);
			}
		}
		break;
	case wxAutoBotFrame::modeCYCLE:
		{
			if(!wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_AddRule, this, (wxEvent *)NULL))
				return;
			pShape = GetDiagramManager()->AddShape(CLASSINFO(wxSFCycle), event.GetPosition(), sfDONT_SAVE_STATE);
			if(pShape)
			{
				((wxSFCycle *)pShape)->SetShapeGUID(wxGetApp().GetAppProperties()->GuidToStr(wxGetApp().GetTreeBot()->GetActiveRule()->GetGUIDRule()));
				// set shape policy
				pShape->AcceptChild(wxT("wxSFCycle"));
				pShape->AcceptChild(wxT("wxSFComponent"));
				pShape->AcceptChild(wxT("wxSFBotLineShape"));
				pShape->AcceptChild(wxT("wxSFTextShape"));
				pShape->AcceptChild(wxT("wxSFEditTextShape"));
				pShape->AcceptChild(wxT("wxSFIfShape"));

				pShape->AcceptConnection(wxT("All"));
				pShape->AcceptSrcNeighbour(wxT("All"));
				pShape->AcceptTrgNeighbour(wxT("All"));

				wxSFShapeBase *pInnerShape = GetDiagramManager()->AddShape(CLASSINFO(wxSFTextShape), sfDONT_SAVE_STATE);
				((wxSFTextShape *)pInnerShape)->SetText(wxT("while()"));
				((wxSFTextShape *)pInnerShape)->SetBorder(wxPen(*wxBLACK, 1, wxDOT));
				pInnerShape->SetVAlign(wxSFShapeBase::valignTOP);
				pInnerShape->SetHAlign(wxSFShapeBase::halignCENTER);
				pInnerShape->RemoveStyle(wxSFShapeBase::sfsSIZE_CHANGE);
				pInnerShape->RemoveStyle(wxSFShapeBase::sfsPOSITION_CHANGE);
				pInnerShape->RemoveStyle(wxSFShapeBase::sfsPARENT_CHANGE);
				pInnerShape->RemoveStyle(wxSFShapeBase::sfsSHOW_HANDLES);
				pInnerShape->RemoveStyle(wxSFShapeBase::sfsHOVERING);
				pInnerShape->RemoveStyle(wxSFShapeBase::sfsHIGHLIGHTING);
				pInnerShape->Reparent( (xsSerializable*)pShape );

				wxRect shapeBB = pShape->GetBoundingBox();
				wxRect innerBB = pInnerShape->GetBoundingBox();
				int x = (shapeBB.GetWidth()-innerBB.GetWidth())/2;

				pInnerShape->SetRelativePosition( x, 1 );
			}
		}
		break;
	case wxAutoBotFrame::modeLINE:
		{
			if(GetMode() == modeREADY)
			{
				StartInteractiveConnection(CLASSINFO(wxSFBotLineShape), event.GetPosition());
			}
			else
			{
				wxPoint lpos = DP2LP(event.GetPosition());
				wxSFShapeBase* pShapeUnder = GetShapeAtPosition(lpos);
				if( pShapeUnder )
				{
					if( pShapeUnder->IsKindOf(CLASSINFO(wxSFBotLineShape)) )
						return;
					wxSFBotLineShape* pCurentLine = (wxSFBotLineShape*)NULL;
					ShapeList m_lstLines;
					if(GetDiagramManager()->GetShapes(CLASSINFO(wxSFBotLineShape), m_lstLines))
					{
						ShapeList::compatibility_iterator ciLines = m_lstLines.GetLast();
						if( ciLines )
							pCurentLine = (wxSFBotLineShape *)ciLines->GetData();
						if( !pCurentLine )
							return;
						if( !pCurentLine->LineCanConnect(pShapeUnder) )
							return;
					}
				}
				wxSFShapeCanvas::OnLeftDown(event);
			}
		}
		break;
	default:
		// do default actions
		wxSFShapeCanvas::OnLeftDown(event);
	}

	if( pShape )
	{
		ShowShadows(m_pParentFrame->m_fShowShadows, shadowALL);
		SaveCanvasState();
		if(!event.ControlDown())
			m_pParentFrame->SetToolMode(wxAutoBotFrame::modeDESIGN);

		pShape->Refresh();
	}

	if(!event.ControlDown())
		wxGetApp().GetListFrame()->Deselect();
}
//**************************************************
void FrameCanvas::OnRightDown(wxMouseEvent& event)
{
	wxSFShapeBase* pShape = GetShapeUnderCursor();
	// eventualy you can use:
	//wxSFShapeBase *pShape = GetShapeAtPosition(DP2LP(event.GetPosition()), 1, searchBOTH);

	// print out information about the shape (if found)
	//if(pShape)
	//{
	//	ShapeList lstShapes;
	//	wxString msg;
	//	wxSFShapeBase *pChild;
	//	int counter;

	//	// show basic info
	//	msg.Printf(wxT("Class name: %s, ID: %d\n"), pShape->GetClassInfo()->GetClassName(), pShape->GetId());

	//	// show info about shape's children
	//	counter = 1;
	//	pShape->GetChildShapes(lstShapes, sfRECURSIVE);
	//	if( !lstShapes.IsEmpty() )
	//	{
	//		msg += wxT("\nChildren:\n");
	//		ShapeList::compatibility_iterator node = lstShapes.GetFirst();
	//		while(node)
	//		{
	//			pChild = node->GetData();

	//			msg += wxString::Format(wxT("%d. Class name: %s, ID: %d\n"), counter, pChild->GetClassInfo()->GetClassName(), pChild->GetId());
	//			counter++;

	//			node = node->GetNext();
	//		}
	//	}

	//	// show info about shape's neighbours
	//	counter = 1;
	//	lstShapes.Clear();
	//	pShape->GetNeighbours(lstShapes, CLASSINFO(wxSFBotLineShape), wxSFShapeBase::lineBOTH, sfINDIRECT);
	//	if( !lstShapes.IsEmpty() )
	//	{
	//		msg += wxT("\nNeighbours:\n");
	//		ShapeList::compatibility_iterator node = lstShapes.GetFirst();
	//		while(node)
	//		{
	//			pChild = node->GetData();
	//			msg += wxString::Format(wxT("%d. Class name: %s, ID: %d\n"), counter, pChild->GetClassInfo()->GetClassName(), pChild->GetId());
	//			counter++;

	//			node = node->GetNext();
	//		}
	//	}

	//	// show message
	//	wxMessageBox(msg, wxT("wxShapeFramework"), wxOK | wxICON_INFORMATION);
	//}
	//else
	//	wxMessageBox(wxT("No shape found on this position."), wxT("wxShapeFramework"), wxOK | wxICON_INFORMATION);

	// call default handler
	wxSFShapeCanvas::OnRightDown(event);
}
//**************************************************
void FrameCanvas::OnKeyUp(wxKeyEvent& event)
{
	switch(event.GetKeyCode())
	{
	case WXK_ESCAPE:
		m_pParentFrame->SetToolMode(wxAutoBotFrame::modeDESIGN);
		break;
	case WXK_DELETE:
		{
			wxASSERT(GetDiagramManager());
			if(!GetDiagramManager())return;

			wxSFShapeBase *pShape;
			ShapeList m_lstSelection;
			GetSelectedShapes(m_lstSelection);

			ShapeList::compatibility_iterator node = m_lstSelection.GetFirst();
			while(node)
			{
				pShape = node->GetData();
				wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_DeleteRule, pShape, (wxEvent*)NULL);
				node = node->GetNext();
			}
		}
		break;
	}

	// do default actions
	wxSFShapeCanvas::OnKeyDown(event);
}
//**************************************************
void FrameCanvas::OnConnectionFinished(wxSFLineShape* connection)
{
	if(connection)
	{
		//connection->SetSrcArrow(CLASSINFO(wxSFOpenArrow));
		connection->SetTrgArrow(CLASSINFO(wxSFSolidArrow));

		connection->AcceptChild(wxT("wxSFTextShape"));
		connection->AcceptChild(wxT("wxSFEditTextShape"));

		connection->AcceptConnection(wxT("All"));
		connection->AcceptSrcNeighbour(wxT("All"));
		connection->AcceptTrgNeighbour(wxT("All"));

		m_pParentFrame->SetToolMode(wxAutoBotFrame::modeDESIGN);
	}
}
//**************************************************
void FrameCanvas::SetScale(double scale)
{
	wxASSERT(GetDiagramManager());
	if(!GetDiagramManager())return;

	wxSFShapeCanvas::SetScale(scale);
	// rescale all bitmap shapes if neccessary
	//if( !m_fEnableGC )
	{
		ShapeList lsShape;
		GetDiagramManager()->GetShapes(CLASSINFO(wxSFIfShape), lsShape);

		ShapeList::compatibility_iterator node = lsShape.GetFirst();
		while(node)
		{
			node->GetData()->Scale(1, 1);
			node = node->GetNext();
		}

		GetDiagramManager()->GetShapes(CLASSINFO(wxSFTrigger), lsShape);
		node = lsShape.GetFirst();
		while(node)
		{
			node->GetData()->Scale(1, 1);
			node = node->GetNext();
		}

		GetDiagramManager()->GetShapes(CLASSINFO(wxSFComponent), lsShape);
		node = lsShape.GetFirst();
		while(node)
		{
			node->GetData()->Scale(1, 1);
			node = node->GetNext();
		}
	}
	UpdateVirtualSize();
}
//**************************************************
void FrameCanvas::StartInteractiveConnection(wxClassInfo* shapeInfo, const wxPoint& pos)
{
	wxASSERT(GetDiagramManager());
	if(!GetDiagramManager())return;

	wxSFShapeCanvas::StartInteractiveConnection(shapeInfo, pos);

	wxPoint lpos = DP2LP(pos);

	if( shapeInfo->IsKindOf(CLASSINFO(wxSFBotLineShape)) )
	{
		wxSFShapeBase* pShapeUnder = GetShapeAtPosition(lpos);
		if( !pShapeUnder )
			return;
		if( pShapeUnder->IsKindOf(CLASSINFO(wxSFBotLineShape)) )
		{
			AbortInteractiveConnection();
			return;
		}
		wxSFBotLineShape* pCurentLine = (wxSFBotLineShape*)NULL;

		wxSFBotLineShape* pLine;
		wxSFIfShape *ifShape = (wxSFIfShape*)pShapeUnder;
		wxRect shpRct = pShapeUnder->GetBoundingBox();
		int centr = shpRct.GetPosition().x + (shpRct.GetWidth()/2);
		short int siCycle = 0;

		ShapeList m_lstLines;
		if( GetDiagramManager()->GetShapes(shapeInfo, m_lstLines) )
		{
			//Получим линию которая в данный момент коннектиться к фигуре
			ShapeList::compatibility_iterator ciLines = m_lstLines.GetLast();
			if( ciLines )
				pCurentLine = (wxSFBotLineShape *)ciLines->GetData();
			if( !pCurentLine )
			{
				AbortInteractiveConnection();
				return;
			}

			//Проверим все соединения и попытаемся определить, есть ли подключенные к этому объекту
			ciLines = m_lstLines.GetFirst();
			while( ciLines )
			{
				pLine = (wxSFBotLineShape*)ciLines->GetData();
				
				//Текущую линию не учитываем
				if( pLine == pCurentLine )
				{
					ciLines = ciLines->GetNext();
					continue;
				}
				
				if(pShapeUnder->IsKindOf( CLASSINFO(wxSFIfShape)) )
				{
					if(lpos.x <= centr) //True
					{
						if( (ifShape->GetId() == pLine->GetSrcShapeId()) &&
							(pLine->GetCondition() == 1) )
						{
							AbortInteractiveConnection();
							return;
						}
					}
					else if(lpos.x > centr) //False
					{
						if( (ifShape->GetId() == pLine->GetSrcShapeId()) &&
							(pLine->GetCondition() == 0) )
						{
							AbortInteractiveConnection();
							return;
						}
					}
				}
				else
				{
					if( pShapeUnder->IsKindOf(CLASSINFO(wxSFCycle)) )
					{
						if( pShapeUnder->GetId() == pLine->GetSrcShapeId() )
							siCycle++;
						if(siCycle > 1)
						{
							AbortInteractiveConnection();
							return;
						}
					}
					else
						if( pShapeUnder->GetId() == pLine->GetSrcShapeId() )
						{
							AbortInteractiveConnection();
							return;
						}
				}
				ciLines = ciLines->GetNext();
			}
		}

		if(pShapeUnder->IsKindOf( CLASSINFO(wxSFIfShape)) )
			if(lpos.x <= centr) //True
			{
				pCurentLine->SetCondition(1);
			}
			else if(lpos.x > centr) //False
			{
				pCurentLine->SetCondition(0);
			}
	}
}
//**************************************************
void FrameCanvas::OnLeftUp(wxMouseEvent &event)
{
	//SetFocus();
	wxPoint lpos = DP2LP(event.GetPosition());

	switch(m_nWorkingMode)
	{
	case modeREADY:
		{
			ShapeList m_lstLines;
			if(!GetSelectedShapes(m_lstLines))
			{
				wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_UnSelectRule, this, (wxEvent *)NULL);
			}
		}
		break;
	case modeMULTISELECTION:
		wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_UnSelectRule, this, (wxEvent *)NULL);
		break;
	case modeMULTIHANDLEMOVE:
	case modeHANDLEMOVE:
		{
			wxSFShapeBase* pShapeUnder = GetShapeUnderCursor();
			if( pShapeUnder )
			{
				if( pShapeUnder->IsKindOf(CLASSINFO(wxSFBotLineShape)) )
				{
					event.StopPropagation();
					return;
				}

				wxSFBotLineShape* pLine;
				wxSFBotLineShape* pCurentLine;
				ShapeList m_lstLines;
				
				GetSelectedShapes(m_lstLines);
				ShapeList::compatibility_iterator ciLines = m_lstLines.GetLast();
				pCurentLine = (wxSFBotLineShape *)ciLines->GetData();
				if( !pCurentLine )
				{
					event.StopPropagation();
					return;
				}

				if(GetSelectedHandle()->GetType() == wxSFShapeHandle::hndLINESTART)
				{
					wxSFBotLineShape* pLine;
					wxSFIfShape *ifShape = (wxSFIfShape*)pShapeUnder;
					wxRect shpRct = pShapeUnder->GetBoundingBox();
					int centr = shpRct.GetPosition().x + (shpRct.GetWidth()/2);
					short int siCycle = 0;

					ShapeList m_lstLines;
					if( GetDiagramManager()->GetShapes(CLASSINFO(wxSFBotLineShape), m_lstLines) )
					{
						//Проверим все соединения и попытаемся определить, есть ли подключенные к этому объекту
						ciLines = m_lstLines.GetFirst();
						while( ciLines )
						{
							pLine = (wxSFBotLineShape*)ciLines->GetData();
							
							//Текущую линию не учитываем
							if( pLine == pCurentLine )
							{
								ciLines = ciLines->GetNext();
								continue;
							}
							
							if(pShapeUnder->IsKindOf( CLASSINFO(wxSFIfShape)) )
							{
								if(lpos.x <= centr) //True
								{
									if( (ifShape->GetId() == pLine->GetSrcShapeId()) &&
										(pLine->GetCondition() == 1) )
									{
										event.StopPropagation();
										return;
									}
								}
								else if(lpos.x > centr) //False
								{
									if( (ifShape->GetId() == pLine->GetSrcShapeId()) &&
										(pLine->GetCondition() == 0) )
									{
										event.StopPropagation();
										return;
									}
								}
							}
							else
							{
								if( pShapeUnder->IsKindOf(CLASSINFO(wxSFCycle)) )
								{
									if( pShapeUnder->GetId() == pLine->GetSrcShapeId() )
										siCycle++;
									if(siCycle > 1)
									{
										event.StopPropagation();
										return;
									}
								}
								else
									if( pShapeUnder->GetId() == pLine->GetSrcShapeId() )
									{
										event.StopPropagation();
										return;
									}
							}
							ciLines = ciLines->GetNext();
						}
					}

					if(pShapeUnder->IsKindOf( CLASSINFO(wxSFIfShape)) )
						if(lpos.x <= centr) //True
						{
							pCurentLine->SetCondition(1);
						}
						else if(lpos.x > centr) //False
						{
							pCurentLine->SetCondition(0);
						}
				}
				else if (GetSelectedHandle()->GetType() == wxSFShapeHandle::hndLINEEND)
				{
					if( !pCurentLine->LineCanConnect(pShapeUnder) )
					{
						event.StopPropagation();
						// update canvas
						m_nWorkingMode = modeREADY;
						UpdateMultieditSize();
						UpdateVirtualSize();
						Refresh();
						return;
					}
				}
			}
		}
		break;
	case modeSHAPEMOVE:
		{
			ShapeList lstSelection, lstLines;
			ShapeList::compatibility_iterator nodeIn;
			bool _delete;
			bool _firstFig = true;

			wxSFShapeBase* pShapeUnder = GetShapeUnderCursor(searchUNSELECTED);
			
			wxSFShapeBase* pShapeSelect = GetShapeUnderCursor();
			if(pShapeSelect)
			{
				CBot *bot = wxGetApp().GetTreeBot()->GetActiveBot();
				if(!bot)
					return;

				CRule *rule = (CRule *)NULL;
				if(pShapeSelect->IsKindOf(CLASSINFO(wxSFComponent)))
				{
					rule = bot->GetRule(wxGetApp().GetAppProperties()->StrToGuid(((wxSFComponent *)pShapeSelect)->GetShapeGUID()));
				}
				else if(pShapeSelect->IsKindOf(CLASSINFO(wxSFIfShape)))
				{
					rule = bot->GetRule(wxGetApp().GetAppProperties()->StrToGuid(((wxSFIfShape *)pShapeSelect)->GetShapeGUID()));
				}
				else if(pShapeSelect->IsKindOf(CLASSINFO(wxSFCycle)))
				{
					rule = bot->GetRule(wxGetApp().GetAppProperties()->StrToGuid(((wxSFCycle *)pShapeSelect)->GetShapeGUID()));
				}
				else if(pShapeSelect->IsKindOf(CLASSINFO(wxSFTrigger)))
				{
					rule = bot->GetRule(wxGetApp().GetAppProperties()->StrToGuid(((wxSFTrigger *)pShapeSelect)->GetShapeGUID()));
				}

				if(rule)
				{
					wxGetApp().GetAppProperties()->OnProcess(CAppProperties::Event_SelectRule, rule, (wxEvent *)NULL);
				}
			}

			if(GetSelectedShapes(lstSelection))
			{
				GetDiagramManager()->GetShapes(CLASSINFO(wxSFBotLineShape), lstLines);
				ShapeList::compatibility_iterator node = lstLines.GetFirst();
				while(node)
				{
					wxSFBotLineShape* pShapeLine = (wxSFBotLineShape*)node->GetData();
					_delete = false;

					nodeIn = lstSelection.GetFirst();
					while(nodeIn)
					{
						wxSFShapeBase* pShape = nodeIn->GetData();
						if(_firstFig)
						{
							if(pShapeUnder == pShape)
								break;

							if(pShapeUnder == pShape->GetParentShape())
								break;
							_firstFig = false;
						}

						if(pShapeLine == pShape)
						{
							_delete = false;
							break;
						}
						
						if((pShape->GetId() == pShapeLine->GetSrcShapeId()) ||
							(pShape->GetId() == pShapeLine->GetTrgShapeId()) )
						{
							_delete = true;
						}
						nodeIn = nodeIn->GetNext();
					}

					if(_delete)
						this->GetDiagramManager()->RemoveShape(pShapeLine);

					node = node->GetNext();
				}
			}
		}
		break;
	default:
		break;
	}
	wxSFShapeCanvas::OnLeftUp(event);
}
//**************************************************
void FrameCanvas::SaveCanvasToDoc(wxXmlDocument *xmlDoc)
{
	wxASSERT(GetDiagramManager());
	if(!GetDiagramManager())return;

	// create root node and save canvas properties
	wxXmlNode *root = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("canvas"));
	// initialize settings
	m_Settings.m_arrAcceptedShapes.Clear();
	WX_APPEND_ARRAY(m_Settings.m_arrAcceptedShapes, GetDiagramManager()->GetAcceptedShapes());

	// serialize settings
	wxXmlNode* settings = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("settings"));
	settings->AddChild(m_Settings.SerializeObject(settings));
	root->AddChild(settings);

	// create chart node
	wxXmlNode* chart = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("chart"));
	GetDiagramManager()->SerializeObjects(GetDiagramManager()->GetRootItem(), chart, false);
	root->AddChild(chart);

	xmlDoc->SetRoot(root);
}
//**************************************************
void FrameCanvas::LoadCanvasFromDoc(wxXmlDocument *xmlDoc)
{
	wxASSERT(GetDiagramManager());
	if(!GetDiagramManager())return;

	bool fChartLoaded = false;

	wxXmlNode* root = xmlDoc->GetRoot();
	if(root)
	{
		GetDiagramManager()->Clear();
		ClearCanvasHistory();

		if(root->GetName() == wxT("chart"))
		{
			// the file contains only chart without any additional info
			GetDiagramManager()->DeserializeObjects(NULL, root);
		}
		else if(root->GetName() == wxT("canvas"))
		{
			// the file contains chart and additional canvas info
			wxXmlNode* child = root->GetChildren();
			while(child)
			{
				if(child->GetName() == wxT("settings"))
				{
					m_Settings.DeserializeObject(child->GetChildren());
					GetDiagramManager()->GetAcceptedShapes().Clear();
					WX_APPEND_ARRAY(GetDiagramManager()->GetAcceptedShapes(), m_Settings.m_arrAcceptedShapes);
				}
				else if(child->GetName() == wxT("chart"))
				{
					// only one chart can be loaded at once
					if(!fChartLoaded)
					{
						GetDiagramManager()->DeserializeObjects(NULL, child);
						fChartLoaded = true;
					}
				}
				child = child->GetNext();
			}
		}
		else
			wxMessageBox(wxT("Unknown file format."), wxT("wxShapeFramework"), wxOK | wxICON_WARNING);

		SetScale(m_Settings.m_nScale);
		SaveCanvasState();
		UpdateVirtualSize();
		Refresh(false);
	}
}
//**************************************************
void FrameCanvas::OnPaste(const ShapeList& pasted)
{
	if(!ContainsStyle(sfsCLIPBOARD)) return;
	CBot *bot = wxGetApp().GetTreeBot()->GetActiveBot();
	if(!bot) return;

	if( !pasted.IsEmpty() )
	{
		ShapeList::compatibility_iterator ciShape = pasted.GetFirst();
		while(ciShape)
		{
			wxSFShapeBase* pShape = (wxSFShapeBase*)ciShape->GetData();
			wxString guid;
			if(pShape->IsKindOf(CLASSINFO(wxSFIfShape)))
				guid = ((wxSFIfShape *)pShape)->GetShapeGUID();
			else if(pShape->IsKindOf(CLASSINFO(wxSFTrigger)))
				guid = ((wxSFTrigger *)pShape)->GetShapeGUID();
			else if(pShape->IsKindOf(CLASSINFO(wxSFComponent)))
				guid = ((wxSFComponent *)pShape)->GetShapeGUID();
			else if(pShape->IsKindOf(CLASSINFO(wxSFCycle)))
				guid = ((wxSFCycle *)pShape)->GetShapeGUID();
			if(!guid.IsEmpty())
			{
				CRule *rule = wxGetApp().GetTreeBot()->GetClipboardRule(wxGetApp().GetAppProperties()->StrToGuid(guid));
				if(rule)
				{
					GUID itemGUID;
					CRule *newRule = bot->AddRule(rule->GetComponent()->GetGUIDComponent(), &itemGUID);
					if(newRule)
					{
						newRule->SetExecute(rule->GetExecute());
						newRule->SetValues(rule->GetValues());

						if(pShape->IsKindOf(CLASSINFO(wxSFIfShape)))
						{
							((wxSFIfShape *)pShape)->SetShapeGUID(wxGetApp().GetAppProperties()->GuidToStr(itemGUID));
							((wxSFIfShape *)pShape)->CreateFromImage(&newRule->GetComponent()->GetImage());
						}
						else if(pShape->IsKindOf(CLASSINFO(wxSFTrigger)))
						{
							((wxSFTrigger *)pShape)->SetShapeGUID(wxGetApp().GetAppProperties()->GuidToStr(itemGUID));
							((wxSFTrigger *)pShape)->CreateFromImage(&newRule->GetComponent()->GetImage());
						}
						else if(pShape->IsKindOf(CLASSINFO(wxSFComponent)))
						{
							((wxSFComponent *)pShape)->SetShapeGUID(wxGetApp().GetAppProperties()->GuidToStr(itemGUID));
							((wxSFComponent *)pShape)->CreateFromImage(&newRule->GetComponent()->GetImage());
						}
						else if(pShape->IsKindOf(CLASSINFO(wxSFCycle)))
						{
							((wxSFCycle *)pShape)->SetShapeGUID(wxGetApp().GetAppProperties()->GuidToStr(itemGUID));
						}
					}
				}
			}
			ciShape = ciShape->GetNext();
		}
	}
}
//**************************************************
void FrameCanvas::Copy()
{
	if( !ContainsStyle(sfsCLIPBOARD) )return;

	wxASSERT(GetDiagramManager());
	if(!GetDiagramManager())return;

	ShapeList lstSelection;
	GetSelectedShapes(lstSelection);

	if(!lstSelection.IsEmpty())
	{
		wxGetApp().GetTreeBot()->GetListClipboardRule()->DeleteContents(wxGetApp().GetTreeBot()->m_isCut);
		wxGetApp().GetTreeBot()->m_isCut = false;
		wxGetApp().GetTreeBot()->GetListClipboardRule()->Clear();
		ShapeList::compatibility_iterator ciShape = lstSelection.GetFirst();
		while(ciShape)
		{
			wxSFShapeBase* pShape = (wxSFShapeBase*)ciShape->GetData();
			wxString guid;
			if(pShape->IsKindOf(CLASSINFO(wxSFIfShape)))
				guid = ((wxSFIfShape *)pShape)->GetShapeGUID();
			else if(pShape->IsKindOf(CLASSINFO(wxSFTrigger)))
				guid = ((wxSFTrigger *)pShape)->GetShapeGUID();
			else if(pShape->IsKindOf(CLASSINFO(wxSFComponent)))
				guid = ((wxSFComponent *)pShape)->GetShapeGUID();
			else if(pShape->IsKindOf(CLASSINFO(wxSFCycle)))
				guid = ((wxSFCycle *)pShape)->GetShapeGUID();
			if(!guid.IsEmpty())
			{
				CBot *bot = wxGetApp().GetTreeBot()->GetActiveBot();
				if(bot)
				{
					CRule *rule = bot->GetRule(wxGetApp().GetAppProperties()->StrToGuid(guid));
					wxGetApp().GetTreeBot()->GetListClipboardRule()->Append(rule);
				}
			}
			ciShape = ciShape->GetNext();
		}
	}
	wxSFShapeCanvas::Copy();
}
//**************************************************
void FrameCanvas::Cut()
{
	if(!ContainsStyle(sfsCLIPBOARD))return;

	wxASSERT(GetDiagramManager());
	if(!GetDiagramManager())return;
	// remove selected shapes
	ShapeList lstSelection;
	GetSelectedShapes(lstSelection);

	if(!lstSelection.IsEmpty())
	{
		CBot *bot = wxGetApp().GetTreeBot()->GetActiveBot();
		if(bot)
		{
			wxGetApp().GetTreeBot()->GetListClipboardRule()->DeleteContents(wxGetApp().GetTreeBot()->m_isCut);
			wxGetApp().GetTreeBot()->m_isCut = true;
			wxGetApp().GetTreeBot()->GetListClipboardRule()->Clear();
			ShapeList::compatibility_iterator ciShape = lstSelection.GetFirst();
			while(ciShape)
			{
				wxSFShapeBase* pShape = (wxSFShapeBase*)ciShape->GetData();
				wxString guid;
				if(pShape->IsKindOf(CLASSINFO(wxSFIfShape)))
					guid = ((wxSFIfShape *)pShape)->GetShapeGUID();
				else if(pShape->IsKindOf(CLASSINFO(wxSFTrigger)))
					guid = ((wxSFTrigger *)pShape)->GetShapeGUID();
				else if(pShape->IsKindOf(CLASSINFO(wxSFComponent)))
					guid = ((wxSFComponent *)pShape)->GetShapeGUID();
				else if(pShape->IsKindOf(CLASSINFO(wxSFCycle)))
					guid = ((wxSFCycle *)pShape)->GetShapeGUID();
				if(!guid.IsEmpty())
				{
					CRule *rule = bot->GetRule(wxGetApp().GetAppProperties()->StrToGuid(guid));
					bot->GetListRule().DeleteContents(false);
					bot->GetListRule().DeleteObject(rule);
					wxGetApp().GetTreeBot()->GetListClipboardRule()->Append(rule);
				}
				ciShape = ciShape->GetNext();
			}
		}
	}
	wxSFShapeCanvas::Cut();
}