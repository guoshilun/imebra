/*
$fileHeader$
*/

/*! \file dataHandlerDateTime.h
    \brief Declaration of the handler for the date and time.

*/

#if !defined(imebraDataHandlerDateTime_85665C7B_8DDF_479e_8CC0_83E95CB625DC__INCLUDED_)
#define imebraDataHandlerDateTime_85665C7B_8DDF_479e_8CC0_83E95CB625DC__INCLUDED_

#include "dataHandlerDateTimeBase.h"


///////////////////////////////////////////////////////////
//
// Everything is in the namespace puntoexe::imebra
//
///////////////////////////////////////////////////////////
namespace puntoexe
{

namespace imebra
{

namespace handlers
{

class dataHandlerDateTime : public dataHandlerDateTimeBase
{

public:
	virtual void getDate(
		imbxInt32* pYear, 
		imbxInt32* pMonth, 
		imbxInt32* pDay, 
		imbxInt32* pHour, 
		imbxInt32* pMinutes,
		imbxInt32* pSeconds,
		imbxInt32* pNanoseconds,
		imbxInt32* pOffsetHours,
		imbxInt32* pOffsetMinutes);

	virtual void setDate(
		imbxInt32 year, 
		imbxInt32 month, 
		imbxInt32 day, 
		imbxInt32 hour, 
		imbxInt32 minutes,
		imbxInt32 seconds,
		imbxInt32 nanoseconds,
		imbxInt32 offsetHours,
		imbxInt32 offsetMinutes);

	/// \brief Return a string representing the date stored in 
	///         the buffer.
	///
	/// The returned string has the format: 
	///  "YYYY-MM-DD HH:MM:SS.FFFFFF"
	///  where:
	///  - YYYY is the year
	///  - MM is the month
	///  - DD is the day of the month
	///  - HH is the hour
	///  - MM are the minutes
	///  - SS are the seconds
	///  - FFFFFF are the nanoseconds
	///
	/// @return a string representing the date stored in the
	///          buffer
	///
	///////////////////////////////////////////////////////////
	virtual std::wstring getUnicodeString();

	/// \brief Set the date from a string.
	///
	/// The string must have the format:
	///  "YYYY-MM-DD HH:MM:SS.FFFFFF"
	///  where:
	///  - YYYY is the year
	///  - MM is the month
	///  - DD is the day
	///  - HH is the hour
	///  - MM are the minutes
	///  - SS are the seconds
	///  - FFFFFF are the nanoseconds
	///
	/// @param value the string representing the date to be set
	///
	///////////////////////////////////////////////////////////
	virtual void setUnicodeString(std::wstring value);

	virtual imbxUint32 getUnitSize();

protected:
	virtual imbxUint32 maxSize();
};

} // namespace handlers

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraDataHandlerDateTime_85665C7B_8DDF_479e_8CC0_83E95CB625DC__INCLUDED_)
