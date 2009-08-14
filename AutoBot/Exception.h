#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include "Autobot.h"
//**************************************************
///> Macro for throwing exception.
#define Except(type, func, desc) throw(CException(CException::type, desc, _CRT_WIDE(__FILE__), func, __LINE__ ))
///> Macro for asserting.
#define Assert(cond, type, func) if(!(cond)) Except(type, func, wxT("Assertion error: ") _CRT_WIDE(#cond))
//**************************************************
class CException/* : public SquirrelError*/
{
public:
	/** Definitions of error codes. */
	enum ExceptionType
	{
		/** Unknown internal error (mostly occured by another library). */
		INTERNAL_ERROR,
		/** Nullpointer error. */
		NULL_POINTER_ERROR,
		/** Invalid name error. */
		INVALID_NAME_ERROR,
		/** Invalid enumerated value error. */
		INVALID_ENUM_ERROR,
		/** Invalid value error. */
		INVALID_VALUE_ERROR,
		/** Invalid object error. */
		INVALID_OBJECT_ERROR,
		/** Invalid class error (not proper derived class). */
		INVALID_CLASS_ERROR,
		/** Invalid operation error. */
		INVALID_OPERATION_ERROR,
		/** Operation is not supported on this platform. */
		OPERATION_NOT_SUPPORTED_ERROR,
		/** Out of memory error. */
		OUT_OF_MEMORY_ERROR,
		/** File I/O error. */
		FILE_IO_ERROR,
		/** Invalid file format error. */
		FILE_FORMAT_ERROR,
		/** The user stopped the application. */
		USER_BREAK_ERROR,
		/** Error in a script file. */
		SCRIPT_ERROR,
		/** Xlib error. */
		XLIB_ERROR
	};
protected:
	/** Pointer to the last raised exception.
	 @remarks
	 This attribute references an attribute.
	*/
	static CException *mLastException;
protected:
	/** Type of the exception.
	 @remarks
	 This is own attribute of this class.
	*/
	ExceptionType mType;

	/** Textual description of the exception.
	 @remarks
	 This is own attribute of this class.
	*/
	wxString mDescription;

	/** Name of the file where the exception was thrown.
	 @remarks
	 This is own attribute of this class.
	*/
	wxString mFileName;

	/** Name of the function where the exception was thrown.
	 @remarks
	 This is own attribute of this class.
	*/
	wxString mFunctionName;

	/** Number of the line where the exception was thrown.
	 @remarks
	 This is own attribute of this class.
	*/
	u32 mLineNumber;
public:
	/**
	 Default constructor.
	 @param[in] type Type of exception.
	 @param[in] description Textual description of the exception.
	 @param[in] fileName Name of the file where the exception was thrown.
	 @param[in] functionName Name of the function where the exception was thrown.
	 @param[in] lineNumber Number of the line where the exception was thrown.
	 */
	//Exception(const ExceptionType &type = INTERNAL_ERROR, const wxString &description =
	//	 wxT("unknown"), const wxString &fileName = wxT("unknown"), const wxString &functionName =
	//	 wxT("unknown"), const u32 &lineNumber = 0);
	CException(const ExceptionType &type = INTERNAL_ERROR, const wxString &description =
		 wxT("unknown"), const wxString &fileName = wxT("unknown"), const wxString &functionName =
		 wxT("unknown"), const u32 &lineNumber = 0);
public:
	/**
	 Getter of mType. Returns value of mType.
	 @return The value of mType
	*/
	const CException::ExceptionType &getType() const;

	/**
	 Getter of mDescription. Returns value of mDescription.
	 @return The value of mDescription
	*/
	const wxString &getDescription() const;

	/**
	 Getter of mFileName. Returns value of mFileName.
	 @return The value of mFileName
	*/
	const wxString & getFileName() const;

	/**
	 Getter of mFunctionName. Returns value of mFunctionName.
	 @return The value of mFunctionName
	*/
	const wxString & getFunctionName() const;

	/**
	 Getter of mLineNumber. Returns value of mLineNumber.
	 @return The value of mLineNumber
	*/
	const u32 & getLineNumber() const;

	/**
	 Getter of mLastException. Returns value of mLastException.
	 @return The value of mLastException
	*/
	static CException *getLastException();
public:
	/**
	 Translate type to human readable format.
	 @param[in] type Enum exception value.
	 @return
	 */
	static wxString translateType(const CException::ExceptionType & type);
};
//**************************************************
inline CException::CException(const CException::ExceptionType &type, const wxString &description,
			 const wxString &fileName, const wxString &functionName, const u32 &lineNumber)
	// You have to initialize the following attributes:
	// - mType
	// - mDescription
	// - mFileName
	// - mFunctionName
	// - mLineNumber
	// - mLastException
{
	mType = type;
	mDescription = description;
	mFileName = fileName;
	mFunctionName = functionName;
	mLineNumber = lineNumber;
	mLastException = this;
}
//**************************************************
inline const CException::ExceptionType & CException::getType() const
{
	return mType;
}
//**************************************************
inline const wxString &CException::getDescription() const
{
	return mDescription;
}
//**************************************************
inline const wxString &CException::getFileName() const
{
	return mFileName;
}
//**************************************************
inline const wxString &CException::getFunctionName() const
{
	return mFunctionName;
}
//**************************************************
inline const u32 &CException::getLineNumber() const
{
	return mLineNumber;
}
//**************************************************
inline CException *CException::getLastException()
{
	return mLastException;
}
//**************************************************
#endif