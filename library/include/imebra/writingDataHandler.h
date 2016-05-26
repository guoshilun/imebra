/*
$fileHeader$
*/

/// \file writingDataHandler.h
/// \brief Declaration of the class WritingDataHandler.
///
///////////////////////////////////////////////////////////////////////////////

#if !defined(imebraWritingDataHandler__INCLUDED_)
#define imebraWritingDataHandler__INCLUDED_

#include <string>
#include <memory>
#include "definitions.h"

#ifndef SWIG

namespace imebra
{
namespace implementation
{
namespace handlers
{
class writingDataHandler;
}
}
}

#endif


namespace imebra
{

///
/// \brief The WritingDataHandler class allows to write the content
///        of a Dicom tag's buffer.
///
/// WritingDataHandler is able to write into the buffer's content strings,
/// a numbers, a date/time or an ages.
///
/// In order to obtain a WritingDataHandler object for a specific tag stored
/// in a DataSet, call DataSet::getWritingDataHandler() or
/// Tag::getWritingDataHandler().
///
/// The WritingDataHandler object always works on a new and clean memory area.
/// The buffer's memory is replaced by the WritingDataHandler memory when the
/// data handler is destroyed.
///
///////////////////////////////////////////////////////////////////////////////
class IMEBRA_API WritingDataHandler
{
    WritingDataHandler(const WritingDataHandler&) = delete;
    WritingDataHandler& operator=(const WritingDataHandler&) = delete;

#ifndef SWIG
    friend class DataSet;
    friend class Tag;
    friend class WritingDataHandlerNumeric;

private:
    WritingDataHandler(std::shared_ptr<imebra::implementation::handlers::writingDataHandler> pDataHandler);
#endif

public:
    /// \brief Destructor: replaces the tag buffer's memory with the memory created
    ///        by this WritingDataHandler.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~WritingDataHandler();

    /// \brief Resize the memory to contain the specified number of elements.
    ///
    /// By default the WritingDataHandler buffer allocates an empty memory block
    /// that must be resized in order to be filled with data.
    ///
    /// The type of the contained elements depend on the tag's VR. The VR can be
    /// retrieved with getDataType().
    ///
    /// \param elementsNumber the number of elements to store in the buffer.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setSize(size_t elementsNumber);

    /// \brief Retrieve the number of elements that can be stored in the buffer
    ///        controlled by WritingDataHandler.
    ///
    /// The memory size can be changed with setSize().
    ///
    /// The type of the contained elements depend on the tag's VR. The VR can be
    /// retrieved with getDataType().
    ///
    /// \return the number of elements that can be stored into the buffer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    size_t getSize() const;

    /// \brief Returns the data type (VR) of the data handled by the data handler.
    ///
    /// \return the data type of the handled data
    ///
    ///////////////////////////////////////////////////////////////////////////////
    tagVR_t getDataType() const;

    /// \brief Write a signed long integer (32 bit).
    ///
    /// If the value cannot be converted from a signed long integer
    /// then throws DataHandlerConversionError.
    ///
    /// \param index the element number within the buffer. Must be smaller than
    ///        getSize()
    /// \param value the value to write
    /// \return the new signed 32 bit integer value
    ///
    ///////////////////////////////////////////////////////////////////////////////
    WritingDataHandler& setSignedLong(size_t index, std::int32_t value);

    /// \brief Write an unsigned long integer (32 bit).
    ///
    /// If the value cannot be converted from an unsigned long integer
    /// then throws DataHandlerConversionError.
    ///
    /// \param index the element number within the buffer. Must be smaller than
    ///        getSize()
    /// \param value the value to write
    /// \return the new unsigned 32 bit integer value
    ///
    ///////////////////////////////////////////////////////////////////////////////
    WritingDataHandler& setUnsignedLong(size_t index, std::uint32_t value);

    /// \brief Write a double floating point value (64 bit).
    ///
    /// If the value cannot be converted from a double floating point
    /// then throws DataHandlerConversionError.
    ///
    /// \param index the element number within the buffer. Must be smaller than
    ///        getSize()
    /// \param value the value to write
    /// \return the 64 bit floating point value
    ///
    ///////////////////////////////////////////////////////////////////////////////
    WritingDataHandler& setDouble(size_t index, double value);

    /// \brief Write a string.
    ///
    /// If the value cannot be converted from a string
    /// then throws DataHandlerConversionError.
    ///
    /// \param index the element number within the buffer. Must be smaller than
    ///        getSize()
    /// \param value the value to write
    /// \return the string value
    ///
    ///////////////////////////////////////////////////////////////////////////////
    WritingDataHandler& setString(size_t index, const std::string& value);

    /// \brief Write an Unicode string.
    ///
    /// If the value cannot be converted from an Unicode string
    /// then throws DataHandlerConversionError.
    ///
    /// \param index the element number within the buffer. Must be smaller than
    ///        getSize()
    /// \param value the value to write
    /// \return the Unicode string value
    ///
    ///////////////////////////////////////////////////////////////////////////////
    WritingDataHandler& setUnicodeString(size_t index, const std::wstring& value);

    /// \brief Write a date and/or a time.
    ///
    /// If the value cannot be converted from a Date
    /// then throws DataHandlerConversionError.
    ///
    /// \param index the element number within the buffer. Must be smaller than
    ///        getSize()
    /// \param date  the value to write
    /// \return the Date value
    ///
    ///////////////////////////////////////////////////////////////////////////////
    WritingDataHandler& setDate(size_t index, const Date& date);

    /// \brief Write an Age value.
    ///
    /// If the value cannot be converted from an Age
    /// then throws DataHandlerConversionError.
    ///
    /// \param index the element number within the buffer. Must be smaller than
    ///        getSize()
    /// \param age   the value to write
    /// \return the Age value
    ///
    ///////////////////////////////////////////////////////////////////////////////
    WritingDataHandler& setAge(size_t index, const Age& age);


#ifndef SWIG
protected:
    std::shared_ptr<imebra::implementation::handlers::writingDataHandler> m_pDataHandler;
#endif
};

}

#endif // !defined(imebraWritingDataHandler__INCLUDED_)
