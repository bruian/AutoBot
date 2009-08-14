#pragma once
#ifndef __SCINTILLAFRAME_H__
#define __SCINTILLAFRAME_H__
#include "AutoBot.h"
//**************************************************
#define STYLE_TYPES_COUNT 32
//! general style types
#define TOKEN_DEFAULT 0

#define TOKEN_WORD1 1
#define TOKEN_WORD2 2
#define TOKEN_WORD3 3
#define TOKEN_WORD4 4
#define TOKEN_WORD5 5
#define TOKEN_WORD6 6

#define TOKEN_COMMENT 7
#define TOKEN_COMMENT_DOC 8
#define TOKEN_COMMENT_LINE 9
#define TOKEN_COMMENT_SPECIAL 10

#define TOKEN_CHARACTER 11
#define TOKEN_CHARACTER_EOL 12
#define TOKEN_STRING 13
#define TOKEN_STRING_EOL 14

#define TOKEN_DELIMITER 15

#define TOKEN_PUNCTUATION 16

#define TOKEN_OPERATOR 17

#define TOKEN_BRACE 18

#define TOKEN_COMMAND 19
#define TOKEN_IDENTIFIER 20
#define TOKEN_LABEL 21
#define TOKEN_NUMBER 22
#define TOKEN_PARAMETER 23
#define TOKEN_REGEX 24
#define TOKEN_UUID 25
#define TOKEN_VALUE 26

#define TOKEN_PREPROCESSOR 27
#define TOKEN_SCRIPT 28

#define TOKEN_ERROR 29
//**************************************************
//! style bits types
#define TOKEN_STYLE_BOLD 1
#define TOKEN_STYLE_ITALIC 2
#define TOKEN_STYLE_UNDERL 4
#define TOKEN_STYLE_HIDDEN 8
//**************************************************
//! general folding types
#define FOLD_TYPE_COMMENT 1
#define FOLD_TYPE_COMPACT 2
#define FOLD_TYPE_PREPROC 4

#define FOLD_TYPE_HTML 16
#define FOLD_TYPE_HTMLPREP 32

#define FOLD_TYPE_COMMENTPY 64
#define FOLD_TYPE_QUOTESPY 128
//**************************************************
struct LanguageInfo
{
	wxChar *name;
	wxChar *filepattern;
	int lexer;
	struct
	{
		int type;
		const wxChar *words;
	} styles [32];
	int folds;
};
//**************************************************
extern const LanguageInfo g_LanguagePrefs[];
extern const int g_LanguagePrefsSize;
//**************************************************
struct CommonInfo {
	// editor functionality prefs
	bool syntaxEnable;
	bool foldEnable;
	bool indentEnable;
	// display defaults prefs
	bool readOnlyInitial;
	bool overTypeInitial;
	bool wrapModeInitial;
	bool displayEOLEnable;
	bool indentGuideEnable;
	bool lineNumberEnable;
	bool longLineOnEnable;
	bool whiteSpaceEnable;
};
extern const CommonInfo g_CommonPrefs;
//**************************************************
// StyleInfo
struct StyleInfo {
	wxChar *name;
	wxChar *foreground;
	wxChar *background;
	wxChar *fontname;
	int fontsize;
	int fontstyle;
	int lettercase;
};

extern const StyleInfo g_StylePrefs[];
extern const int g_StylePrefsSize;
//**************************************************
class CScintillaFrame : public wxScintilla
{
	friend class EditProperties;
	friend class EditPrint;
public:
	CScintillaFrame(wxWindow *parent, wxWindowID id = -1,
			const wxPoint &pos = wxDefaultPosition,
			const wxSize &size = wxDefaultSize,
			long style = wxSUNKEN_BORDER|wxVSCROLL );

	~CScintillaFrame();
	void OnScintillaUndo(wxCommandEvent &event);
	void OnScintillaRedo(wxCommandEvent &event);
	void OnScintillaCut(wxCommandEvent &event);
	void OnScintillaCopy(wxCommandEvent &event);
	void OnScintillaPaste(wxCommandEvent &event);
	void OnScintillaDelete(wxCommandEvent &event);
	void OnScintillaFind(wxCommandEvent &event);
	bool InitializePrefs(const wxString &name);
private:
	// lanugage properties
	LanguageInfo const* m_language;

	// margin variables
	int m_LineNrID;
	int m_LineNrMargin;
	int m_FoldingID;
	int m_FoldingMargin;
	int m_DividerID;
	
	DECLARE_EVENT_TABLE()
};
//**************************************************
#endif //SCINTILLAFRAME