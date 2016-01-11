/*
$fileHeader$
*/

/*! \file buffer.h
    \brief Declaration of the buffer class.

*/

#if !defined(imebraBuffer_DE3F98A9_664E_47c0_A29B_B681F9AEB118__INCLUDED_)
#define imebraBuffer_DE3F98A9_664E_47c0_A29B_B681F9AEB118__INCLUDED_

#include "streamControllerImpl.h"
#include "memoryImpl.h"
#include "dataHandlerNumericImpl.h"

#include "charsetsListImpl.h"

///////////////////////////////////////////////////////////
//
// Everything is in the namespace puntoexe::imebra
//
///////////////////////////////////////////////////////////
namespace puntoexe
{
	class streamReader;
	class streamWriter;
    class memory;

namespace imebra
{

/// \addtogroup group_dataset
///
/// @{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief This class manages a memory area containing
///         data in dicom format.
///
/// A buffer also knows the data type of the
///  elements it stores.
/// The data type is in Dicom format (two upper case
///  chars).
///
/// The memory can be accessed through a 
///  \ref handlers::dataHandler derived object
///  obtained by calling the function getDataHandler().
///  
/// Data handlers work on a copy of the buffer, so most
///  of the problem related to the multithreading
///  enviroments are avoided.
///
/// The data handlers supply several functions that
///  allow to access to the data in several formats
///  (strings, numeric, time, and so on).
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class buffer: public std::enable_shared_from_this<buffer>
{

public:
	///////////////////////////////////////////////////////////
	/// \name Constructor
	///
	///////////////////////////////////////////////////////////
	//@{

	/// \brief Constructor. Initialize the buffer object and
	///        set the default data type.
	///
	/// If no data type is specified, then the Dicom data
	///  type "OB" is used.
	///
	/// @param defaultType  a string with the buffer's type.
	///                     The buffer's type must be one of
	///                      the Dicom data types.
	///                     A dicom's data type is formed by
	///                      two uppercase chars
	///
	///////////////////////////////////////////////////////////
    buffer(const std::string& defaultType="");

	/// \brief Constructor. Initialize the buffer object and
	///         declare the buffer's content on demand.
	///
	/// On demand content is loaded from the original stream
	///  when the application requires the access to the
	///  buffer.
	///
	/// @param defaultType  a string with the buffer's type.
	///                     The buffer's type must be one of
	///                      the Dicom data types.
	///                     A dicom's data type is formed by
	///                      two uppercase chars
	/// @param originalStream the stream from which the content
	///                      can be read
	/// @param bufferPosition the first stream's byte that 
	///                      contains the buffer's content
	/// @param bufferLength the buffer's content length, in
	///                      bytes
	/// @param wordLength   the size of a buffer's element,
	///                      in bytes
	/// @param endianType   the stream's endian type
	///
	///////////////////////////////////////////////////////////
    buffer(
		const std::string& defaultType,
        const std::shared_ptr<baseStreamInput>& originalStream,
		std::uint32_t bufferPosition,
		std::uint32_t bufferLength,
		std::uint32_t wordLength,
		streamController::tByteOrdering endianType);

	//@}

	///////////////////////////////////////////////////////////
	/// \name Data handlers
	///
	///////////////////////////////////////////////////////////
	//@{
public:
	/// \brief Retrieve a data handler that can be used to
	///         read, write and resize the memory controlled by 
	///         the buffer.
	///
	/// The data handler will have access to a local copy
	///  of the buffer, then it will not have to worry about
	///  multithreading related problems.
	/// If a writing handler is requested, then the handler's
	///  local buffer will be copied back into the buffer when
	///  the handler will be destroyed.
	///
	/// @param bWrite set to true if you want to write into
	///                the buffer
	/// @param size   this parameter is used only when the
	///                parameter bWrite is set to true and the
	///                buffer is empty: in this case, the
	///                returned buffer will be resized to the
	///                number of elements (NOT bytes) declared
	///                in this parameter
	/// @return a pointer to a dataHandler object
	///
	///////////////////////////////////////////////////////////
    std::shared_ptr<handlers::readingDataHandler> getReadingDataHandler() const;

    std::shared_ptr<handlers::writingDataHandler> getWritingDataHandler(std::uint32_t size = 0);

	/// \brief Retrieve a raw data handler that can be used to
	///         read, write and resize the memory controlled by 
	///         the buffer.
	///
	/// Raw data handlers always see a collection of bytes,
	///  regardless of the original buffer's type.
	///
	/// The data handler will have access to a local copy
	///  of the buffer, then it will not have to worry about
	///  multithreading related problems.
	/// If a writing handler is requested, then the handler's
	///  local buffer will be copied back into the buffer when
	///  the handler will be destroyed.
	///
	/// @param bWrite set to true if you want to write into
	///                the buffer
	/// @param size   this parameter is used only when the
	///                parameter bWrite is set to true and the
	///                buffer is empty: in this case, the
	///                returned buffer will be resized to the
	///                number of bytes declared in this
	///                parameter
	/// @return a pointer to a dataHandler object
	///
	///////////////////////////////////////////////////////////
    std::shared_ptr<handlers::readingDataHandlerRaw> getReadingDataHandlerRaw() const;

    std::shared_ptr<handlers::writingDataHandlerRaw> getWritingDataHandlerRaw(std::uint32_t size = 0);

	//@}


	///////////////////////////////////////////////////////////
	/// \name Stream
	///
	///////////////////////////////////////////////////////////
	//@{
	
	/// \brief Return the current buffer's size in bytes
	///
	/// If the buffer is currently loaded then return the
	///  memory's size, otherwise return the size that the
	///  buffer would have when it is loaded.
	///
	/// @return the buffer's size, in bytes
	///////////////////////////////////////////////////////////
    std::uint32_t getBufferSizeBytes() const;

	//@}


	///////////////////////////////////////////////////////////
	/// \name Stream
	///
	///////////////////////////////////////////////////////////
	//@{

	/// \brief Return a stream reader connected to the 
	///         buffer's content.
	///
	/// @return          a pointer to a stream reader
	///
	///////////////////////////////////////////////////////////
	std::shared_ptr<streamReader> getStreamReader();

	/// \brief Return a stream writer connected to the 
	///         buffer's content.
	///
	/// The stream works on a local copy of the buffer's data,
	///  then it doesn't have to worry about multithreading
	///  related problems.
	///
	/// @return          a pointer to a stream writer
	///
	///////////////////////////////////////////////////////////
	std::shared_ptr<streamWriter> getStreamWriter();

	//@}


	///////////////////////////////////////////////////////////
	/// \name Buffer's data type
	///
	///////////////////////////////////////////////////////////
	//@{

	/// \brief Returns the buffer's data type.
	///
	/// Return a string with the buffer's data type.
	///
	/// @return a string with the buffer's data type in Dicom
	///          format.
	//
	///////////////////////////////////////////////////////////
    std::string getDataType() const;

	//@}

    void commit(std::shared_ptr<memory> newMemory, const std::string& newBufferType, const charsetsList::tCharsetsList& newCharsetsList);

    void commit(std::shared_ptr<memory> newMemory, const std::string& newBufferType);

	///////////////////////////////////////////////////////////
	/// \name Charsets
	///
	///////////////////////////////////////////////////////////
	//@{

	/// \brief Defines the charsets that should be used by
	///         the object.
	///
	/// The valid charsets are:
	/// - ""
    /// - "ISO_IR 6"
	/// - "ISO_IR 100"
	/// - "ISO_IR 101"
	/// - "ISO_IR 109"
	/// - "ISO_IR 110"
	/// - "ISO_IR 144"
	/// - "ISO_IR 127"
	/// - "ISO_IR 126"
	/// - "ISO_IR 138"
	/// - "ISO_IR 148"
	/// - "ISO_IR 13"
	/// - "ISO_IR 166"
	/// - "ISO 2022 IR 6"
	/// - "ISO 2022 IR 100"
	/// - "ISO 2022 IR 101"
	/// - "ISO 2022 IR 109"
	/// - "ISO 2022 IR 110"
	/// - "ISO 2022 IR 144"
	/// - "ISO 2022 IR 127"
	/// - "ISO 2022 IR 126"
	/// - "ISO 2022 IR 138"
	/// - "ISO 2022 IR 148"
	/// - "ISO 2022 IR 13"
	/// - "ISO 2022 IR 166"
	/// - "ISO 2022 IR 87"
	/// - "ISO 2022 IR 159"
	/// - "ISO 2022 IR 149"
	/// - "ISO_IR 192" (UTF-8)
	/// - "GB18030"
	///
	/// @param pCharsetsList  a list of charsets that can be
	///                        used by the dicom object.
	///                       The default charsets must be 
	///                        the first item in the list
	///
	///////////////////////////////////////////////////////////
    virtual void setCharsetsList(const charsetsList::tCharsetsList& charsets);
	
	/// \brief Retrieve the charsets used by the dicom object.
	///
	/// If during the operation an error is detected (diffetent
	///  objects use different default charsets) then
	///  the exception charsetListExceptionDiffDefault is 
	///  thrown.
	///
	/// @param pCharsetsList  a pointer to a list that will
	///                        be filled with the used 
	///                        charsets
	///
	///////////////////////////////////////////////////////////
    virtual void getCharsetsList(charsetsList::tCharsetsList* pCharsetsList) const;

	//@}

protected:

    std::shared_ptr<memory> getLocalMemory() const;

	//
	// Attributes
	//
	///////////////////////////////////////////////////////////
private:
	// The memory buffer
	///////////////////////////////////////////////////////////
	std::shared_ptr<memory> m_memory;

protected:
	// The buffer's type, in Dicom standard
	///////////////////////////////////////////////////////////
	std::string m_bufferType;

protected:
	// The following variables are used to reread the buffer
	//  from the stream.
	///////////////////////////////////////////////////////////
    std::shared_ptr<baseStreamInput> m_originalStream;    // < Original stream
	std::uint32_t m_originalBufferPosition; // < Original buffer's position
	std::uint32_t m_originalBufferLength;   // < Original buffer's length
	std::uint32_t m_originalWordLength;     // < Original word's length (for low/high endian adjustment)
	streamController::tByteOrdering m_originalEndianType; // < Original endian type
	
private:
	// Charset list
	///////////////////////////////////////////////////////////
	charsetsList::tCharsetsList m_charsetsList;

};


/// @}

} // End of namespace imebra

} // End of namespace puntoexe

#endif // !defined(imebraBuffer_DE3F98A9_664E_47c0_A29B_B681F9AEB118__INCLUDED_)
