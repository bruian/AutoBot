/**************************************************
 * Name:      BotShape.h
 * Purpose:   Defines bot shape class
 * Author:    Bruian Blake (bruianio@gmail.com)
 * Created:   2008-12-30
 * Copyright: Bruian Blake
 * License:   AutoBot license (www.bruian.com)
 * Notes:
 **************************************************/
#ifndef _WXSFBOTSHAPE_H
#define _WXSFBOTSHAPE_H

#include "AutoBot.h"
#include "wxsf/DiamondShape.h"
#include "wxsf/LineShape.h"

// default values
#define sfdvIFSHAPE_SCALEIMAGE true
#define sfdvIFSHAPE_XPMDATA wxT("")
#define sfdvTRIGGERSHAPE_SCALEIMAGE true
#define sfdvTRIGGERSHAPE_XPMDATA wxT("")
#define sfdvCOMPONENTSHAPE_SCALEIMAGE true
#define sfdvCOMPONENTSHAPE_XPMDATA wxT("")
//**************************************************
class WXDLLIMPEXP_SF wxSFIfShape : public wxSFDiamondShape
{
public:
	XS_DECLARE_CLONABLE_CLASS(wxSFIfShape);

	wxSFIfShape(void);
	wxSFIfShape(const wxRealPoint& pos, const wxString& imagePath, wxSFDiagramManager* manager);
	wxSFIfShape(const wxSFIfShape& obj);
	virtual ~wxSFIfShape(void);

	// public member data accessors
	void EnableScale(bool canscale){m_fCanScale = canscale;}
	bool CanScale(){return m_fCanScale;}

	wxString GetShapeGUID()			 { return m_shapeGUID; }
	void	 SetShapeGUID(wxString guid) { m_shapeGUID = guid; }

	// public functions
	bool CreateFromImage(wxImage *image);
	bool CreateFromXPM(const char* const* bits);

	wxRealPoint GetBorderPoint(bool condition);

	// public virtual function
	virtual void Scale(double x, double y, bool children = sfWITHCHILDREN);

	virtual void OnBeginHandle(wxSFShapeHandle &handle);
	virtual void OnHandle(wxSFShapeHandle &handle);
	virtual void OnEndHandle(wxSFShapeHandle &handle);
	virtual void OnLeftClick(const wxPoint &pos);
protected:
	wxImage		m_Image;
	wxImage		m_OriginalImage;
	bool		m_fCanScale;
	bool		m_fRescaleInProgress;
	wxRealPoint m_nPrevPos;
	wxString	m_shapeGUID;

	// protected functions
	void RescaleImage(const wxRealPoint& size);
	// protected virtual functions
	virtual void DrawNormal(wxDC& dc);
	virtual void DrawHover(wxDC& dc);
	virtual void DrawHighlighted(wxDC& dc);

	virtual wxXmlNode* Serialize(wxXmlNode* node);
	virtual void Deserialize(wxXmlNode* node);
private:
	void MarkSerializableDataMembers();
};
//**************************************************
class WXDLLIMPEXP_SF wxSFBotLineShape : public wxSFLineShape
{
public:
	wxSFBotLineShape::wxSFBotLineShape() 
		: wxSFLineShape()
	{
		MarkSerializableDataMembers();
	}
	wxSFBotLineShape::wxSFBotLineShape(long src, long trg, const RealPointList& path, wxSFDiagramManager* manager)
		: wxSFLineShape(src, trg, path, manager)
	{
		MarkSerializableDataMembers();
	}
	wxSFBotLineShape::wxSFBotLineShape(const wxSFBotLineShape& obj)
		: wxSFLineShape(obj)
	{
		m_condition = obj.m_condition;
		MarkSerializableDataMembers();
	}

	XS_DECLARE_CLONABLE_CLASS(wxSFBotLineShape);
	void GetDirectLine(wxRealPoint& src, wxRealPoint& trg);
	wxRealPoint GetSrcPoint();
	wxRealPoint GetTrgPoint();
	void OnEndHandle(wxSFShapeHandle& handle);
	void DrawCompleteLine(wxDC& dc);

	int GetCondition() { return m_condition; }
	void SetCondition(int value) { m_condition = value; }
	bool LineCanConnect(wxSFShapeBase* pShapeUnder);
	virtual wxXmlNode* Serialize(wxXmlNode* node);
	virtual void Deserialize(wxXmlNode* node);
protected:
	//condition 0 - false, 1 - true, -1 or other - other components
	int m_condition;
private:
	// private functions
	void MarkSerializableDataMembers();
};
//**************************************************
class WXDLLIMPEXP_SF wxSFTrigger : public wxSFRectShape
{
public:
	XS_DECLARE_CLONABLE_CLASS(wxSFTrigger);
	wxSFTrigger(void);
	wxSFTrigger(const wxRealPoint& pos, const wxRealPoint& size, const wxString& imagePath, wxSFDiagramManager* manager);
	wxSFTrigger(const wxSFTrigger& obj);
	virtual ~wxSFTrigger(void);

	// public member data accessors
	void EnableScale(bool canscale) { m_fCanScale = canscale; }
	bool CanScale() { return m_fCanScale; }

	virtual void Scale(double x, double y, bool children = sfWITHCHILDREN);

	wxString GetShapeGUID()			 { return m_shapeGUID; }
	void	 SetShapeGUID(wxString guid) { m_shapeGUID = guid; }

	// public functions
	bool CreateFromImage(wxImage *image);
	bool CreateFromXPM(const char* const* bits);
protected:
	wxImage		m_Image;
	wxImage		m_OriginalImage;
	bool		m_fCanScale;
	bool		m_fRescaleInProgress;
	wxRealPoint m_nPrevPos;
	wxString	m_shapeGUID;

	// protected functions
	void RescaleImage(const wxRealPoint& size);
	// protected virtual functions
	virtual void DrawNormal(wxDC& dc);
	virtual void DrawHover(wxDC& dc);
	virtual void DrawHighlighted(wxDC& dc);

	virtual wxXmlNode* Serialize(wxXmlNode* node);
	virtual void Deserialize(wxXmlNode* node);
private:
	void MarkSerializableDataMembers();
};
//**************************************************
class WXDLLIMPEXP_SF wxSFComponent : public wxSFRectShape
{
public:
	XS_DECLARE_CLONABLE_CLASS(wxSFComponent);
	wxSFComponent(void);
	wxSFComponent(const wxRealPoint& pos, const wxRealPoint& size, const wxString& imagePath, wxSFDiagramManager* manager);
	wxSFComponent(const wxSFComponent& obj);
	virtual ~wxSFComponent(void);

	// public member data accessors
	void EnableScale(bool canscale) { m_fCanScale = canscale; }
	bool CanScale() { return m_fCanScale; }
	wxString GetShapeGUID()			 { return m_shapeGUID; }
	void	 SetShapeGUID(wxString guid) { m_shapeGUID = guid; }

	virtual void Scale(double x, double y, bool children = sfWITHCHILDREN);

	// public functions
	bool CreateFromImage(wxImage *image);
	bool CreateFromXPM(const char* const* bits);
protected:
	wxImage		m_Image;
	wxImage		m_OriginalImage;
	bool		m_fCanScale;
	bool		m_fRescaleInProgress;
	wxRealPoint m_nPrevPos;
	wxString	m_shapeGUID;

	// protected functions
	void RescaleImage(const wxRealPoint& size);
	// protected virtual functions
	virtual void DrawNormal(wxDC& dc);
	virtual void DrawHover(wxDC& dc);
	virtual void DrawHighlighted(wxDC& dc);

	virtual wxXmlNode* Serialize(wxXmlNode* node);
	virtual void Deserialize(wxXmlNode* node);
private:
	void MarkSerializableDataMembers();
};
//**************************************************
class WXDLLIMPEXP_SF wxSFCycle : public wxSFRectShape
{
public:
	XS_DECLARE_CLONABLE_CLASS(wxSFCycle);
	wxSFCycle(void);
	wxSFCycle(const wxRealPoint& pos, const wxRealPoint& size, wxSFDiagramManager* manager);
	wxSFCycle(const wxSFCycle& obj);
	wxString GetShapeGUID()			 { return m_shapeGUID; }
	void	 SetShapeGUID(wxString guid) { m_shapeGUID = guid; }

	virtual ~wxSFCycle(void);
protected:
	wxRealPoint m_nPrevPos;
	wxString	m_shapeGUID;

	virtual wxXmlNode* Serialize(wxXmlNode* node);
	virtual void Deserialize(wxXmlNode* node);
private:
	void MarkSerializableDataMembers();
};
//**************************************************
#endif //_WXSFBOTSHAPE_H