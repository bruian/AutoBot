/***************************************************************
 * Name:      BitmapShapeXml.cpp
 * Purpose:   Implements bitmap shape's serialization capability
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

//#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include "BotShape.h"
#include "wxsf/CommonFcn.h"

//----------------------------------------------------------------------------------//
// Serialization
//----------------------------------------------------------------------------------//

wxXmlNode* wxSFIfShape::Serialize(wxXmlNode* node)
{
	// HINT: overload it for custom actions...

	if(node)
	{
		node = wxSFDiamondShape::Serialize(node);
	}

	return node;
}

void wxSFIfShape::Deserialize(wxXmlNode* node)
{
	// HINT: overload it for custom actions...

	wxSFDiamondShape::Deserialize(node);
	wxRealPoint prevSize = m_nRectSize;

	CRule *myRule = wxGetApp().GetTreeBot()->GetActiveBot()->GetRule(wxGetApp().GetAppProperties()->StrToGuid(m_shapeGUID));
	if(myRule)
	{
		CreateFromImage(&myRule->GetComponent()->GetImage());
	}

	if(m_fCanScale)
	{
		if(m_nRectSize != prevSize)
		{
			m_nRectSize = prevSize;
			RescaleImage(prevSize);
		}
		else
			Scale(1, 1);
	}
}

wxXmlNode* wxSFTrigger::Serialize(wxXmlNode* node)
{
	// HINT: overload it for custom actions...
	if(node)
	{
		node = wxSFRectShape::Serialize(node);
	}
	return node;
}

void wxSFTrigger::Deserialize(wxXmlNode* node)
{
	// HINT: overload it for custom actions...
	wxSFRectShape::Deserialize(node);
	wxRealPoint prevSize = m_nRectSize;

	CRule *myRule = wxGetApp().GetTreeBot()->GetActiveBot()->GetRule(wxGetApp().GetAppProperties()->StrToGuid(m_shapeGUID));
	if(myRule)
	{
		CreateFromImage(&myRule->GetComponent()->GetImage());
	}

	if(m_fCanScale)
	{
		if(m_nRectSize != prevSize)
		{
			m_nRectSize = prevSize;
			RescaleImage(prevSize);
		}
		else
			Scale(1, 1);
	}
}

wxXmlNode* wxSFComponent::Serialize(wxXmlNode* node)
{
	// HINT: overload it for custom actions...
	if(node)
	{
		node = wxSFRectShape::Serialize(node);
	}
	return node;
}

void wxSFComponent::Deserialize(wxXmlNode* node)
{
	// HINT: overload it for custom actions...
	wxSFRectShape::Deserialize(node);
	wxRealPoint prevSize = m_nRectSize;

	CRule *myRule = wxGetApp().GetTreeBot()->GetActiveBot()->GetRule(wxGetApp().GetAppProperties()->StrToGuid(m_shapeGUID));
	if(myRule)
	{
		if(myRule->GetComponent())
		{
			CreateFromImage(&myRule->GetComponent()->GetImage());
		}
		else
		{
			wxGetApp().GetError()->SetError(true, wxT("Error read component resource!"), wxGetApp().GetError()->eErrorType_APP, false);
		}
	}
	else
	{
		wxGetApp().GetError()->SetError(true, wxT("Error read component resource!"), wxGetApp().GetError()->eErrorType_APP, false);
	}

	if(m_fCanScale)
	{
		if(m_nRectSize != prevSize)
		{
			m_nRectSize = prevSize;
			RescaleImage(prevSize);
		}
		else
			Scale(1, 1);
	}
}

wxXmlNode* wxSFCycle::Serialize(wxXmlNode* node)
{
	// HINT: overload it for custom actions...
	if(node)
	{
		node = wxSFRectShape::Serialize(node);
	}
	return node;
}

void wxSFCycle::Deserialize(wxXmlNode* node)
{
	// HINT: overload it for custom actions...
	wxSFRectShape::Deserialize(node);
	wxRealPoint prevSize = m_nRectSize;
}

wxXmlNode* wxSFBotLineShape::Serialize(wxXmlNode* node)
{
	// HINT: overload it for custom actions...
	if(node)
	{
		node = wxSFLineShape::Serialize(node);
	}
	return node;
}

void wxSFBotLineShape::Deserialize(wxXmlNode* node)
{
	// HINT: overload it for custom actions...
	wxSFLineShape::Deserialize(node);
}