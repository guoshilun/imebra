/*
$fileHeader$
*/

/*! \file streamWriter_swig.h
    \brief Declaration of the the class used to write the streams for SWIG.

*/

#if !defined(imebraStreamWriter_SWIG_F6221390_BC44_4B83_B5BB_3485222FF1DD__INCLUDED_)
#define imebraStreamWriter_SWIG_F6221390_BC44_4B83_B5BB_3485222FF1DD__INCLUDED_

#ifndef SWIG

#include <memory>

namespace puntoexe
{
class streamWriter;
}
#endif

namespace imebra
{

class BaseStream;

class CodecFactory;

/// \addtogroup group_swig_bindings
/// @{

class StreamWriter
{
	friend class CodecFactory;
public:
    StreamWriter(const BaseStream& stream, int virtualStart = 0, int virtualLength = 0);
    StreamWriter(const StreamWriter& right);
    StreamWriter& operator=(const StreamWriter& right);

#ifndef SWIG
protected:
    std::shared_ptr<puntoexe::streamWriter> m_pWriter;
#endif
};

///@}

}

#endif // !defined(imebraStreamWriter_SWIG_F6221390_BC44_4B83_B5BB_3485222FF1DD__INCLUDED_)