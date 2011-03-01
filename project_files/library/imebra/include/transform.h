/*
$fileHeader$
*/

/*! \file transform.h
    \brief Declaration of the base class used by all the transforms.
 
*/

#if !defined(imebraTransform_5DB89BFD_F105_45e7_B9D9_3756AC93C821__INCLUDED_)
#define imebraTransform_5DB89BFD_F105_45e7_B9D9_3756AC93C821__INCLUDED_

#include "../../base/include/baseObject.h"
#include "dataHandlerNumeric.h"
#include "image.h"


#define DEFINE_RUN_TEMPLATE_TRANSFORM \
template <typename inputType>\
void runTemplateTransform1(\
    inputType* inputData, size_t inputDataSize, imbxUint32 inputHandlerWidth, const std::wstring& inputHandlerColorSpace,\
    puntoexe::ptr<puntoexe::imebra::palette> inputPalette,\
    imbxInt32 inputHandlerMinValue, imbxUint32 inputHandlerNumValues,\
    imbxInt32 inputTopLeftX, imbxInt32 inputTopLeftY, imbxInt32 inputWidth, imbxInt32 inputHeight,\
    puntoexe::ptr<puntoexe::imebra::handlers::dataHandlerNumericBase> outputHandler, imbxInt32 outputHandlerWidth, const std::wstring& outputHandlerColorSpace,\
    puntoexe::ptr<puntoexe::imebra::palette> outputPalette,\
    imbxInt32 outputHandlerMinValue, imbxUint32 outputHandlerNumValues,\
    imbxInt32 outputTopLeftX, imbxInt32 outputTopLeftY)\
{\
        HANDLER_CALL_TEMPLATE_FUNCTION_WITH_PARAMS(runTemplateTransform2, outputHandler, \
                        inputData, inputDataSize, inputHandlerWidth, inputHandlerColorSpace,\
			inputPalette,\
                        inputHandlerMinValue, inputHandlerNumValues,\
			inputTopLeftX, inputTopLeftY, inputWidth, inputHeight,\
			outputHandlerWidth, outputHandlerColorSpace,\
                        outputPalette,\
                        outputHandlerMinValue, outputHandlerNumValues,\
			outputTopLeftX, outputTopLeftY);\
}\
\
void runTemplateTransform(\
    puntoexe::ptr<puntoexe::imebra::handlers::dataHandlerNumericBase> inputHandler, imbxUint32 inputHandlerWidth, const std::wstring& inputHandlerColorSpace,\
    puntoexe::ptr<puntoexe::imebra::palette> inputPalette,\
    imbxInt32 inputHandlerMinValue, imbxUint32 inputHandlerNumValues,\
    imbxInt32 inputTopLeftX, imbxInt32 inputTopLeftY, imbxInt32 inputWidth, imbxInt32 inputHeight,\
    puntoexe::ptr<puntoexe::imebra::handlers::dataHandlerNumericBase> outputHandler, imbxInt32 outputHandlerWidth, const std::wstring& outputHandlerColorSpace,\
    puntoexe::ptr<puntoexe::imebra::palette> outputPalette,\
    imbxInt32 outputHandlerMinValue, imbxUint32 outputHandlerNumValues,\
    imbxInt32 outputTopLeftX, imbxInt32 outputTopLeftY)\
{\
        HANDLER_CALL_TEMPLATE_FUNCTION_WITH_PARAMS(runTemplateTransform1, inputHandler, \
                        inputHandlerWidth, inputHandlerColorSpace,\
                        inputPalette,\
			inputHandlerMinValue, inputHandlerNumValues,\
			inputTopLeftX, inputTopLeftY, inputWidth, inputHeight,\
			outputHandler, outputHandlerWidth, outputHandlerColorSpace,\
                        outputPalette,\
                        outputHandlerMinValue, outputHandlerNumValues,\
			outputTopLeftX, outputTopLeftY);\
}\
\
template <typename outputType, typename inputType>\
void runTemplateTransform2(\
    outputType* outputData, size_t outputDataSize, \
    inputType* inputData, size_t inputDataSize, \
    imbxUint32 inputHandlerWidth, const std::wstring& inputHandlerColorSpace,\
    puntoexe::ptr<puntoexe::imebra::palette> inputPalette,\
    imbxInt32 inputHandlerMinValue, imbxUint32 inputHandlerNumValues,\
    imbxInt32 inputTopLeftX, imbxInt32 inputTopLeftY, imbxInt32 inputWidth, imbxInt32 inputHeight,\
    imbxInt32 outputHandlerWidth, const std::wstring& outputHandlerColorSpace,\
    puntoexe::ptr<puntoexe::imebra::palette> outputPalette,\
    imbxInt32 outputHandlerMinValue, imbxUint32 outputHandlerNumValues,\
    imbxInt32 outputTopLeftX, imbxInt32 outputTopLeftY)\
{\
        templateTransform( \
            inputData, inputDataSize, inputHandlerWidth, inputHandlerColorSpace, \
            inputPalette, \
            inputHandlerMinValue, inputHandlerNumValues, \
            inputTopLeftX, inputTopLeftY, inputWidth, inputHeight, \
            outputData, outputDataSize, outputHandlerWidth, outputHandlerColorSpace, \
            outputPalette, \
            outputHandlerMinValue, outputHandlerNumValues, \
            outputTopLeftX, outputTopLeftY);\
}\
\
virtual void runTransformHandlers(\
    puntoexe::ptr<puntoexe::imebra::handlers::dataHandlerNumericBase> inputHandler, imbxUint32 inputHandlerWidth, const std::wstring& inputHandlerColorSpace,\
    puntoexe::ptr<puntoexe::imebra::palette> inputPalette,\
    imbxInt32 inputHandlerMinValue, imbxUint32 inputHandlerNumValues,\
    imbxInt32 inputTopLeftX, imbxInt32 inputTopLeftY, imbxInt32 inputWidth, imbxInt32 inputHeight,\
    puntoexe::ptr<puntoexe::imebra::handlers::dataHandlerNumericBase> outputHandler, imbxInt32 outputHandlerWidth, const std::wstring& outputHandlerColorSpace,\
    puntoexe::ptr<puntoexe::imebra::palette> outputPalette,\
    imbxInt32 outputHandlerMinValue, imbxUint32 outputHandlerNumValues,\
    imbxInt32 outputTopLeftX, imbxInt32 outputTopLeftY)\
{\
    runTemplateTransform(inputHandler, inputHandlerWidth, inputHandlerColorSpace, inputPalette, inputHandlerMinValue, inputHandlerNumValues,\
            inputTopLeftX, inputTopLeftY, inputWidth, inputHeight,\
            outputHandler, outputHandlerWidth, outputHandlerColorSpace, outputPalette, outputHandlerMinValue, outputHandlerNumValues,\
            outputTopLeftX, outputTopLeftY);\
}

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
    class dataHandlerNumericBase;
}

class image;
class dataSet;
class lut;

namespace transforms
{



/// \brief This is the base class for the transforms.
///
/// A transform takes one input and one output image:
///  the output image is modified according to the
///  transform's type, input image's content and
///  transform's parameter.
///
///////////////////////////////////////////////////////////
class transform : public baseObject
{

public:
	/// \brief Returns true if the transform doesn't do
	///         anything.
	///
	/// It always return false, but it is overwritten in the
	///  transformsChain class.
	///
	/// @return false if the transform does something, or true
	///          if the transform doesn't do anything (e.g. an
	///          empty transformsChain object).
	///
	///////////////////////////////////////////////////////////
	virtual bool isEmpty();


	/// \brief Allocate an output image that is compatible with
	///         the transform given the specified input image.
	///
	///////////////////////////////////////////////////////////
	virtual ptr<image> allocateOutputImage(ptr<image> pInputImage, imbxUint32 width, imbxUint32 height) = 0;

	/// \brief Executes the transform.
	///
	/// @param inputImage    the input image for the transform
	/// @param inputTopLeftX the horizontal position of the
	///                       top left corner of the area to
	///                       process
	/// @param inputTopLeftY the vertical position of the top
	///                       left corner of the area to
	///                       process
	/// @param inputWidth    the width of the area to process
	/// @param inputHeight   the height of the area to process
	/// @param outputImage   the output image for the transform
	/// @param outputTopLeftX the horizontal position of the
	///                       top left corner of the output
	///                       area
	/// @param outputTopLeftY the vertical position of the top
	///                        left corner of the output area
	///
	///////////////////////////////////////////////////////////
	virtual void runTransform(
            const ptr<image>& inputImage,
            imbxUint32 inputTopLeftX, imbxUint32 inputTopLeftY, imbxUint32 inputWidth, imbxUint32 inputHeight,
            const ptr<image>& outputImage,
            imbxUint32 outputTopLeftX, imbxUint32 outputTopLeftY) = 0;

};


/// \brief This is the base class for transforms that use
///         templates.
///
/// Transforms derived from transformHandlers
///  have the macro DEFINE_RUN_TEMPLATE_TRANSFORM in
///  their class definition and implement the template
///  function templateTransform().
///
///////////////////////////////////////////////////////////
class transformHandlers: public transform
{
public:
	/// \brief Reimplemented from transform: calls the
	///         templated function templateTransform().
	///
	/// @param inputImage    the input image for the transform
	/// @param inputTopLeftX the horizontal position of the
	///                       top left corner of the area to
	///                       process
	/// @param inputTopLeftY the vertical position of the top
	///                       left corner of the area to
	///                       process
	/// @param inputWidth    the width of the area to process
	/// @param inputHeight   the height of the area to process
	/// @param outputImage   the output image for the transform
	/// @param outputTopLeftX the horizontal position of the
	///                       top left corner of the output
	///                       area
	/// @param outputTopLeftY the vertical position of the top
	///                        left corner of the output area
	///
	///////////////////////////////////////////////////////////
	virtual void runTransform(
            const ptr<image>& inputImage,
            imbxUint32 inputTopLeftX, imbxUint32 inputTopLeftY, imbxUint32 inputWidth, imbxUint32 inputHeight,
            const ptr<image>& outputImage,
            imbxUint32 outputTopLeftX, imbxUint32 outputTopLeftY);

	/// \internal
	virtual void runTransformHandlers(
            ptr<handlers::dataHandlerNumericBase> inputHandler, imbxUint32 inputHandlerWidth, const std::wstring& inputHandlerColorSpace,
            ptr<palette> inputPalette,
            imbxInt32 inputHandlerMinValue, imbxUint32 inputHandlerNumValues,
            imbxInt32 inputTopLeftX, imbxInt32 inputTopLeftY, imbxInt32 inputWidth, imbxInt32 inputHeight,
            ptr<handlers::dataHandlerNumericBase> outputHandler, imbxInt32 outputHandlerWidth, const std::wstring& outputHandlerColorSpace,
            ptr<palette> outputPalette,
            imbxInt32 outputHandlerMinValue, imbxUint32 outputHandlerNumValues,
            imbxInt32 outputTopLeftX, imbxInt32 outputTopLeftY) = 0;

};


/// \brief Base class for the exceptions thrown by the
///         transforms.
///
///////////////////////////////////////////////////////////
class transformException: public std::runtime_error
{
public:
	/// \brief Constructor.
	///
	/// @param message the cause of the exception
	///
	///////////////////////////////////////////////////////////
	transformException(const std::string& message): std::runtime_error(message){}
};



} // namespace transforms

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraTransform_5DB89BFD_F105_45e7_B9D9_3756AC93C821__INCLUDED_)
