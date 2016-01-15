/*
$fileHeader$
*/

/*! \file dataHandlerStringIS.cpp
    \brief Implementation of the class dataHandlerStringIS.

*/

#include "exceptionImpl.h"
#include "dataHandlerStringISImpl.h"

namespace puntoexe
{

namespace imebra
{

namespace handlers
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
//
// dataHandlerStringIS
//
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

readingDataHandlerStringIS::readingDataHandlerStringIS(const memory& parseMemory): readingDataHandlerString(parseMemory, "IS", '\\', 0x20)
{
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get a value as a double.
// Overwritten to use getSignedLong()
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
double readingDataHandlerStringIS::getDouble(const size_t index) const
{
	IMEBRA_FUNCTION_START(L"dataHandlerStringIS::getDouble");

	return (double)getSignedLong(index);

	IMEBRA_FUNCTION_END();
}

writingDataHandlerStringIS::writingDataHandlerStringIS(const std::shared_ptr<buffer> pBuffer):
    writingDataHandlerString(pBuffer, "IS", '\\', 0, 12, 0x20)
{

}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set a value as a double.
// Overwritten to use setSignedLong()
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void writingDataHandlerStringIS::setDouble(const size_t index, const double value)
{
	IMEBRA_FUNCTION_START(L"dataHandlerStringIS::setDouble");

	setSignedLong(index, (std::int32_t)value);

	IMEBRA_FUNCTION_END();
}

} // namespace handlers

} // namespace imebra

} // namespace puntoexe
