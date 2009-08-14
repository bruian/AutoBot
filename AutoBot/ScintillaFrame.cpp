#include "ScintillaFrame.h"
//**************************************************
BEGIN_EVENT_TABLE(CScintillaFrame, wxScintilla)
	EVT_MENU(Menu_UndoScript, CScintillaFrame::OnScintillaUndo)
	EVT_MENU(Menu_RedoScript, CScintillaFrame::OnScintillaRedo)
	EVT_MENU(Menu_CutScript, CScintillaFrame::OnScintillaCut)
	EVT_MENU(Menu_CopyScript, CScintillaFrame::OnScintillaCopy)
	EVT_MENU(Menu_PasteScript, CScintillaFrame::OnScintillaPaste)
	EVT_MENU(Menu_DeleteScript, CScintillaFrame::OnScintillaDelete)
	EVT_MENU(Menu_FindScript, CScintillaFrame::OnScintillaFind)
END_EVENT_TABLE()
//**************************************************
// keywordlists
// C++
wxChar* CppWordlist1 = 
	wxT("break case catch class clone continue \
		extends for function if in local \
		null resume return switch this throw \
		try typeof while parent yield constructor \
		vargc vargv instanceof true false \
		static");
wxChar* CppWordlist2 =
	wxT("dofile loadfile writeclosuretofile file eos flush len readblob readn seek \
		tell writeblob writen blob castf2i casti2f swap2 swap4 swapfloat readblob resize \
		abs acos asin atan atan2 ceil cos exp fabs floor log log10 pow rand sin \
		sqrt srand tan clock date getenv remove rename system time format lstrip regexp \
		rstrip split strip capture match search");
wxChar* CppWordlist3 =
	wxT("stderr stdin stdout { } [ ] . : :: ' ; \" @");
//**************************************************
//! language types
const CommonInfo g_CommonPrefs = {
	// editor functionality prefs
	true,  // syntaxEnable
	true,  // foldEnable
	true,  // indentEnable
	// display defaults prefs
	false, // overTypeInitial
	false, // readOnlyInitial
	false,  // wrapModeInitial
	false, // displayEOLEnable
	false, // IndentGuideEnable
	true,  // lineNumberEnable
	false, // longLineOnEnable
	false, // whiteSpaceEnable
};
//**************************************************
const LanguageInfo g_LanguagePrefs[] = {
	// C++
	{wxT("C++"),
	wxT("*.c;*.cc;*.cpp;*.cxx;*.cs;*.h;*.hh;*.hpp;*.hxx;*.sma"),
	wxSCI_LEX_CPP,
		{{TOKEN_DEFAULT, NULL},
		{TOKEN_COMMENT, NULL},
		{TOKEN_COMMENT_LINE, NULL},
		{TOKEN_COMMENT_DOC, NULL},
		{TOKEN_NUMBER, NULL},
		{TOKEN_WORD1, CppWordlist1}, // KEYWORDS
		{TOKEN_STRING, NULL},
		{TOKEN_CHARACTER, NULL},
		{TOKEN_UUID, NULL},
		{TOKEN_PREPROCESSOR, NULL},
		{TOKEN_OPERATOR, NULL},
		{TOKEN_IDENTIFIER, NULL},
		{TOKEN_STRING_EOL, NULL},
		{TOKEN_DEFAULT, NULL}, // VERBATIM
		{TOKEN_REGEX, NULL},
		{TOKEN_COMMENT_SPECIAL, NULL}, // DOXY
		{TOKEN_WORD2, CppWordlist2}, // EXTRA WORDS
		{TOKEN_WORD3, CppWordlist3}, // DOXY KEYWORDS
		{TOKEN_ERROR, NULL}, // KEYWORDS ERROR
		{-1, NULL},
		{-1, NULL},
		{-1, NULL},
		{-1, NULL},
		{-1, NULL},
		{-1, NULL},
		{-1, NULL},
		{-1, NULL},
		{-1, NULL},
		{-1, NULL},
		{-1, NULL},
		{-1, NULL},
		{-1, NULL}},
		FOLD_TYPE_COMMENT | FOLD_TYPE_COMPACT | FOLD_TYPE_PREPROC}
};
const int g_LanguagePrefsSize = WXSIZEOF(g_LanguagePrefs);
//**************************************************
//! style types
const StyleInfo g_StylePrefs [] = {
	// TOKEN_DEFAULT
	{wxT("Default"),
	wxT("BLACK"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_WORD1
	{wxT("Keyword1"),
	wxT("BLUE"), wxT("WHITE"),
	wxT(""), 10, TOKEN_STYLE_BOLD, 0},
	// TOKEN_WORD2
	{wxT("Keyword2"),
	wxT("DARK BLUE"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_WORD3
	{wxT("Keyword3"),
	wxT("CORNFLOWER BLUE"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_WORD4
	{wxT("Keyword4"),
	wxT("CYAN"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_WORD5
	{wxT("Keyword5"),
	wxT("DARK GREY"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_WORD6
	{wxT("Keyword6"),
	wxT("GREY"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_COMMENT
	{wxT("Comment"),
	wxT("FOREST GREEN"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_COMMENT_DOC
	{wxT("Comment (Doc)"),
	wxT("FOREST GREEN"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_COMMENT_LINE
	{wxT("Comment line"),
	wxT("FOREST GREEN"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_COMMENT_SPECIAL
	{wxT("Special comment"),
	wxT("FOREST GREEN"), wxT("WHITE"),
	wxT(""), 10, TOKEN_STYLE_ITALIC, 0},
	// TOKEN_CHARACTER
	{wxT("Character"),
	wxT("KHAKI"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_CHARACTER_EOL
	{wxT("Character (EOL)"),
	wxT("KHAKI"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_STRING
	{wxT("String"),
	wxT("BROWN"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_STRING_EOL
	{wxT("String (EOL)"),
	wxT("BROWN"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_DELIMITER
	{wxT("Delimiter"),
	wxT("ORANGE"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_PUNCTUATION
	{wxT("Punctuation"),
	wxT("ORANGE"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_OPERATOR
	{wxT("Operator"),
	wxT("BLACK"), wxT("WHITE"),
	wxT(""), 10, TOKEN_STYLE_BOLD, 0},
	// TOKEN_BRACE
	{wxT("Label"),
	wxT("VIOLET"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_COMMAND
	{wxT("Command"),
	wxT("BLUE"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_IDENTIFIER
	{wxT("Identifier"),
	wxT("BLACK"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_LABEL
	{wxT("Label"),
	wxT("VIOLET"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_NUMBER
	{wxT("Number"),
	wxT("SIENNA"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_PARAMETER
	{wxT("Parameter"),
	wxT("VIOLET"), wxT("WHITE"),
	wxT(""), 10, TOKEN_STYLE_ITALIC, 0},
	// TOKEN_REGEX
	{wxT("Regular expression"),
	wxT("ORCHID"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_UUID
	{wxT("UUID"),
	wxT("ORCHID"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_VALUE
	{wxT("Value"),
	wxT("ORCHID"), wxT("WHITE"),
	wxT(""), 10, TOKEN_STYLE_ITALIC, 0},
	// TOKEN_PREPROCESSOR
	{wxT("Preprocessor"),
	wxT("GREY"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_SCRIPT
	{wxT("Script"),
	wxT("DARK GREY"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_ERROR
	{wxT("Error"),
	wxT("RED"), wxT("WHITE"),
	wxT(""), 10, 0, 0},
	// TOKEN_UNDEFINED
	{wxT("Undefined"),
	wxT("ORANGE"), wxT("WHITE"),
	wxT(""), 10, 0, 0}
};
const int g_StylePrefsSize = WXSIZEOF(g_StylePrefs);
//**************************************************
CScintillaFrame::CScintillaFrame(wxWindow *parent, wxWindowID id,
								 const wxPoint &pos,
								 const wxSize &size, long style)
								 : wxScintilla (parent, id, pos, size, style) 
{
	m_language = NULL;
	m_LineNrID = 0;
	m_LineNrMargin = TextWidth(wxSCI_STYLE_LINENUMBER, wxT("_999999"));
	m_FoldingID = 1;
	m_FoldingMargin = 16;
	m_DividerID = 2;

	// default font for all styles
	SetViewEOL(g_CommonPrefs.displayEOLEnable);
	SetIndentationGuides(g_CommonPrefs.indentGuideEnable);
	SetMarginWidth(m_LineNrID, g_CommonPrefs.lineNumberEnable? m_LineNrMargin: 0);
	SetEdgeMode(g_CommonPrefs.longLineOnEnable? wxSCI_EDGE_LINE: wxSCI_EDGE_NONE);
	SetViewWhiteSpace(g_CommonPrefs.whiteSpaceEnable? wxSCI_WS_VISIBLEALWAYS: wxSCI_WS_INVISIBLE);
	SetOvertype(g_CommonPrefs.overTypeInitial);
	SetReadOnly(g_CommonPrefs.readOnlyInitial);
	SetWrapMode(g_CommonPrefs.wrapModeInitial? wxSCI_WRAP_WORD: wxSCI_WRAP_NONE);
	
	wxFont font(10, wxTELETYPE, wxNORMAL, wxNORMAL);
	StyleSetFont(wxSCI_STYLE_DEFAULT, font);
	StyleSetForeground(wxSCI_STYLE_DEFAULT, wxColour(wxT("BLACK")));
	StyleSetBackground(wxSCI_STYLE_DEFAULT, wxColour(wxT("WHITE")));
	StyleSetForeground(wxSCI_STYLE_LINENUMBER, wxColour(wxT("DARK GREY")));
	StyleSetBackground(wxSCI_STYLE_LINENUMBER, wxColour(wxT("WHITE")));
	StyleSetForeground(wxSCI_STYLE_INDENTGUIDE, wxColour(wxT("DARK GREY")));
	InitializePrefs(wxT("C++"));

	// set visibility
	SetVisiblePolicy(wxSCI_VISIBLE_STRICT|wxSCI_VISIBLE_SLOP, 1);
	SetXCaretPolicy(wxSCI_CARET_EVEN|wxSCI_VISIBLE_STRICT|wxSCI_CARET_SLOP, 1);
	SetYCaretPolicy(wxSCI_CARET_EVEN|wxSCI_VISIBLE_STRICT|wxSCI_CARET_SLOP, 1);

	// markers
	MarkerDefine(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_BOXPLUS);
	MarkerSetBackground(wxSCI_MARKNUM_FOLDER, wxColour (wxT("BLACK")));
	MarkerSetForeground(wxSCI_MARKNUM_FOLDER, wxColour (wxT("WHITE")));
	MarkerDefine(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_BOXMINUS);
	MarkerSetBackground(wxSCI_MARKNUM_FOLDEROPEN, wxColour (wxT("BLACK")));
	MarkerSetForeground(wxSCI_MARKNUM_FOLDEROPEN, wxColour (wxT("WHITE")));
	MarkerDefine(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_EMPTY);
	MarkerDefine(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_SHORTARROW);
	MarkerDefine(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_ARROWDOWN);
	MarkerDefine(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_EMPTY);
	MarkerDefine(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_EMPTY);

	// clear wrong default keys
	#if !defined(__WXGTK__)
		CmdKeyClear(wxSCI_KEY_TAB, 0);
		CmdKeyClear(wxSCI_KEY_TAB, wxSCI_SCMOD_SHIFT);
	#endif
	CmdKeyClear('A', wxSCI_SCMOD_CTRL);
	#if !defined(__WXGTK__)
		CmdKeyClear('C', wxSCI_SCMOD_CTRL);
	#endif
	CmdKeyClear('D', wxSCI_SCMOD_CTRL);
	CmdKeyClear('D', wxSCI_SCMOD_SHIFT | wxSCI_SCMOD_CTRL);
	CmdKeyClear('F', wxSCI_SCMOD_ALT | wxSCI_SCMOD_CTRL);
	CmdKeyClear('L', wxSCI_SCMOD_CTRL);
	CmdKeyClear('L', wxSCI_SCMOD_SHIFT | wxSCI_SCMOD_CTRL);
	CmdKeyClear('T', wxSCI_SCMOD_CTRL);
	CmdKeyClear('T', wxSCI_SCMOD_SHIFT | wxSCI_SCMOD_CTRL);
	CmdKeyClear('U', wxSCI_SCMOD_CTRL);
	CmdKeyClear('U', wxSCI_SCMOD_SHIFT | wxSCI_SCMOD_CTRL);
	#if !defined(__WXGTK__)
		CmdKeyClear('V', wxSCI_SCMOD_CTRL);
		CmdKeyClear('X', wxSCI_SCMOD_CTRL);
	#endif
	CmdKeyClear('Y', wxSCI_SCMOD_CTRL);
	#if !defined(__WXGTK__)
		CmdKeyClear('Z', wxSCI_SCMOD_CTRL);
	#endif
	// miscelaneous
	UsePopUp(0);
	SetLayoutCache(wxSCI_CACHE_PAGE);
	SetBufferedDraw(1);
}
//**************************************************
bool CScintillaFrame::InitializePrefs(const wxString &name)
{
	// initialize styles
	StyleClearAll();
	LanguageInfo const* curInfo = NULL;

	// determine language
	bool found = false;
	int languageNr;
	for(languageNr = 0; languageNr < g_LanguagePrefsSize; languageNr++)
	{
		curInfo = &g_LanguagePrefs[languageNr];
		if (curInfo->name == name)
		{
			found = true;
			break;
		}
	}
	if (!found) return false;

	// set lexer and language
	SetLexer (curInfo->lexer);
	m_language = curInfo;

	// set margin for line numbers
	SetMarginType(m_LineNrID, wxSCI_MARGIN_NUMBER);
	StyleSetForeground(wxSCI_STYLE_LINENUMBER, wxColour (wxT("DARK GREY")));
	StyleSetBackground(wxSCI_STYLE_LINENUMBER, wxColour (wxT("WHITE")));
	SetMarginWidth(m_LineNrID, g_CommonPrefs.lineNumberEnable? m_LineNrMargin: 0);

	// set common styles
	StyleSetForeground(wxSCI_STYLE_DEFAULT, wxColour (wxT("DARK GREY")));
	StyleSetForeground(wxSCI_STYLE_INDENTGUIDE, wxColour(wxT("DARK GREY")));

	// initialize settings
	if(g_CommonPrefs.syntaxEnable)
	{
		int keywordnr = 0;
		int Nr;
		for (Nr = 0; Nr < STYLE_TYPES_COUNT; Nr++)
		{
			if(curInfo->styles[Nr].type == -1) continue;
			const StyleInfo &curType = g_StylePrefs[curInfo->styles[Nr].type];
			wxFont font(curType.fontsize, wxTELETYPE, wxNORMAL, wxNORMAL, false, curType.fontname);
			StyleSetFont(Nr, font);
			if(curType.foreground)
			{
				StyleSetForeground(Nr, wxColour(curType.foreground));
			}
			if(curType.background)
			{
				StyleSetBackground(Nr, wxColour(curType.background));
			}
			StyleSetBold(Nr, (curType.fontstyle & TOKEN_STYLE_BOLD) > 0);
			StyleSetItalic(Nr, (curType.fontstyle & TOKEN_STYLE_ITALIC) > 0);
			StyleSetUnderline(Nr, (curType.fontstyle & TOKEN_STYLE_UNDERL) > 0);
			StyleSetVisible(Nr, (curType.fontstyle & TOKEN_STYLE_HIDDEN) == 0);
			StyleSetCase(Nr, curType.lettercase);
			const wxChar *pwords = curInfo->styles[Nr].words;
			if(pwords)
			{
				SetKeyWords(keywordnr, pwords);
				keywordnr += 1;
			}
		}
	}

	// set margin as unused
	SetMarginType(m_DividerID, wxSCI_MARGIN_SYMBOL);
	SetMarginWidth(m_DividerID, 8);
	SetMarginSensitive(m_DividerID, false);

	// folding
	SetMarginType(m_FoldingID, wxSCI_MARGIN_SYMBOL);
	SetMarginMask(m_FoldingID, wxSCI_MASK_FOLDERS);
	StyleSetBackground(m_FoldingID, wxColour(wxT("WHITE")));
	SetMarginWidth(m_FoldingID, 0);
	SetMarginSensitive(m_FoldingID, false);
	if(g_CommonPrefs.foldEnable)
	{
		SetMarginWidth(m_FoldingID, curInfo->folds != 0? m_FoldingMargin: 0);
		SetMarginSensitive(m_FoldingID, curInfo->folds != 0);
		SetProperty(wxT("fold"), curInfo->folds != 0? wxT("1"): wxT("0"));
		SetProperty(wxT("fold.comment"), (curInfo->folds & FOLD_TYPE_COMMENT) > 0? wxT("1"): wxT("0"));
		SetProperty(wxT("fold.compact"), (curInfo->folds & FOLD_TYPE_COMPACT) > 0? wxT("1"): wxT("0"));
		SetProperty(wxT("fold.preprocessor"), (curInfo->folds & FOLD_TYPE_PREPROC) > 0? wxT("1"): wxT("0"));
		SetProperty(wxT("fold.html"), (curInfo->folds & FOLD_TYPE_HTML) > 0? wxT("1"): wxT("0"));
		SetProperty(wxT("fold.html.preprocessor"), (curInfo->folds & FOLD_TYPE_HTMLPREP) > 0? wxT("1"): wxT("0"));
		SetProperty(wxT("fold.comment.python"), (curInfo->folds & FOLD_TYPE_COMMENTPY) > 0? wxT("1"): wxT("0"));
		SetProperty(wxT("fold.quotes.python"), (curInfo->folds & FOLD_TYPE_QUOTESPY) > 0? wxT("1"): wxT("0"));
	}
	SetFoldFlags(wxSCI_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSCI_FOLDFLAG_LINEAFTER_CONTRACTED);

	// set spaces and indention
	SetTabWidth(4);
	SetUseTabs(false);
	SetTabIndents(true);
	SetBackSpaceUnIndents(true);
	SetIndent(g_CommonPrefs.indentEnable? 4: 0);

	// others
	SetViewEOL(g_CommonPrefs.displayEOLEnable);
	SetIndentationGuides(g_CommonPrefs.indentGuideEnable);
	SetEdgeColumn(80);
	SetEdgeMode(g_CommonPrefs.longLineOnEnable? wxSCI_EDGE_LINE: wxSCI_EDGE_NONE);
	SetViewWhiteSpace(g_CommonPrefs.whiteSpaceEnable? wxSCI_WS_VISIBLEALWAYS: wxSCI_WS_INVISIBLE);
	SetOvertype(g_CommonPrefs.overTypeInitial);
	SetReadOnly(g_CommonPrefs.readOnlyInitial);
	SetWrapMode(g_CommonPrefs.wrapModeInitial? wxSCI_WRAP_WORD: wxSCI_WRAP_NONE);

	return true;
}
//**************************************************
CScintillaFrame::~CScintillaFrame()
{
}
//**************************************************
void CScintillaFrame::OnScintillaUndo(wxCommandEvent &WXUNUSED(event))
{
	if(!CanUndo()) return;
	Undo();
}
//**************************************************
void CScintillaFrame::OnScintillaRedo(wxCommandEvent &WXUNUSED(event))
{
	if(!CanRedo()) return;
	Redo();
}
//**************************************************
void CScintillaFrame::OnScintillaCut(wxCommandEvent &WXUNUSED(event))
{
	if(GetReadOnly() || (GetSelectionEnd()-GetSelectionStart() <= 0)) return;
	Cut();
}
//**************************************************
void CScintillaFrame::OnScintillaCopy(wxCommandEvent &WXUNUSED(event))
{
	if(GetSelectionEnd()-GetSelectionStart() <= 0) return;
	Copy();
}
//**************************************************
void CScintillaFrame::OnScintillaPaste(wxCommandEvent &WXUNUSED(event))
{
	if(!CanPaste()) return;
	Paste();
}
//**************************************************
void CScintillaFrame::OnScintillaDelete(wxCommandEvent &WXUNUSED(event))
{
	if(GetReadOnly()) return;
	Clear();
}
//**************************************************
void CScintillaFrame::OnScintillaFind(wxCommandEvent &WXUNUSED(event))
{
}