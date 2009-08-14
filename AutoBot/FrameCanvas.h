#ifndef _FRAMECANVAS_H
#define _FRAMECANVAS_H
//**************************************************
#include "wxsf/wxShapeFramework.h"
#include "AutoBot.h"
//**************************************************
class wxAutoBotFrame;
//**************************************************
class FrameCanvas : public wxSFShapeCanvas
{
public:
	FrameCanvas(){;}
	FrameCanvas(wxSFDiagramManager* manager, wxWindow* parent, wxWindowID id);
	virtual ~FrameCanvas(void);

	// public virtual functions
	virtual void OnLeftDown(wxMouseEvent& event);
	virtual void OnRightDown(wxMouseEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	virtual void OnPaste(const ShapeList& pasted);
	void Copy();
	void Cut();
	void OnConnectionFinished(wxSFLineShape* connection);
	virtual void SetScale(double scale);
	void OnLeftUp(wxMouseEvent &event);
	void SaveCanvasToDoc(wxXmlDocument *xmlDoc);
	void LoadCanvasFromDoc(wxXmlDocument *xmlDoc);

	void StartInteractiveConnection(wxClassInfo* shapeInfo, const wxPoint& pos);
protected:
	// protected data members
	wxAutoBotFrame *m_pParentFrame;
private:
	DECLARE_EVENT_TABLE();
};
//**************************************************
#endif //_FRAMECANVAS_H