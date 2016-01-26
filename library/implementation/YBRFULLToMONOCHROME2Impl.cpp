/*
$fileHeader$
*/

/*! \file YBRFULLToMONOCHROME2.cpp
    \brief Implementation of the class YBRFULLToMONOCHROME2.

*/

#include "exceptionImpl.h"
#include "YBRFULLToMONOCHROME2Impl.h"
#include "dataHandlerImpl.h"
#include "dataSetImpl.h"
#include "imageImpl.h"

namespace puntoexe
{

namespace imebra
{

namespace transforms
{

namespace colorTransforms
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the initial color space
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::string YBRFULLToMONOCHROME2::getInitialColorSpace()
{
    return "YBR_FULL";
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the final color space
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::string YBRFULLToMONOCHROME2::getFinalColorSpace()
{
    return "MONOCHROME2";
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Create a new YBRFULLToMONOCHROME2 object
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::shared_ptr<colorTransform> YBRFULLToMONOCHROME2::createColorTransform()
{
    return std::shared_ptr<colorTransform>(std::make_shared<YBRFULLToMONOCHROME2>());
}


} // namespace colorTransforms

} // namespace transforms

} // namespace imebra

} // namespace puntoexe

