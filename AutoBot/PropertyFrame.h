#ifndef __PROPERTYFRAME_H__
#define __PROPERTYFRAME_H__
//**************************************************
#include "AutoBot.h"
////**************************************************
//class wxDatePropertyClass : public wxDateTime
//{
//    WX_PG_DECLARE_PROPERTY_CLASS()
//public:
//    wxDatePropertyClass();
//    virtual ~wxDatePropertyClass();
//
//    WX_PG_DECLARE_PARENTAL_TYPE_METHODS()
//    WX_PG_DECLARE_PARENTAL_METHODS()
//    WX_PG_DECLARE_EVENT_METHODS()
//};
//**************************************************
class CPropertyFrame : public wxPropertyGrid
{
public:
    CPropertyFrame();

	CPropertyFrame( wxWindow *parent, wxWindowID id,
					const wxPoint& pos, const wxSize& size,
					long style, const wxChar* name):wxPropertyGrid(parent, id, pos, size, style, name) {}
	~CPropertyFrame() {}
	void RefreshPropertyGrid(ePropertyType propertytype/*, const GUID guid*/);
	void OnPropertyGridChange( wxPropertyGridEvent& event );
	void ExpandXMLNode(wxXmlNode *GetChildren, wxPropertyGrid *propgrid, CRule *rule, wxPGId *pgid);
	void SetValueXMLNode(wxXmlNode *GetChildren, CRule *rule, wxString NameValue, wxPropertyGridEvent *event, bool *Refresh);
	//void OnPropertyGridTextUpdate( wxCommandEvent& event );
	DECLARE_EVENT_TABLE();
};
#endif //__PROPERTYFRAME_H__