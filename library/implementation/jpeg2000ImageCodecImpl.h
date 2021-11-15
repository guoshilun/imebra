/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement 
 that your application must also be GPL), you may purchase a commercial 
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/*! \file jpeg2000ImageCodec.h
    \brief Declaration of the class jpeg2000ImageCodec.

*/

#if !defined(imebraJpeg2000ImageCodec_7F63E846_8824_42c6_A048_DD59C657AED4__INCLUDED_)
#define imebraJpeg2000ImageCodec_7F63E846_8824_42c6_A048_DD59C657AED4__INCLUDED_

#ifdef JPEG2000

#include "imageCodecImpl.h"
#include "jpegCodecBaseImpl.h"
#include <map>
#include <list>


namespace imebra
{

namespace implementation
{

namespace codecs
{

/// \addtogroup group_codecs
///
/// @{
    class JPEG2000Internals;
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief The Jpeg 2000 codec.
///
/// This class is used to decode and encode a Jpeg 2000
/// image.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class jpeg2000ImageCodec : public imageCodec
{
public:

    jpeg2000ImageCodec();
    ~jpeg2000ImageCodec() override;

    void SetRate(unsigned int idx, double rate);

     double GetRate(unsigned int idx = 0) const;

    void SetQuality(unsigned int idx, double q);

    double GetQuality(unsigned int idx = 0) const;

    void SetTileSize(unsigned int tx, unsigned int ty);

    void SetNumberOfResolutions(unsigned int nres);

    /// Set Number of threads
    /// @param nThreads : number of threads for decompression codec, if 0 or 1 decompression is done in current thread, if negative value is set determine how many virtual threads are available
    void SetNumberOfThreadsForDecompression(int nThreads);

    void SetReversible(bool res);


    // Retrieve the image from a dataset
	///////////////////////////////////////////////////////////
    virtual std::shared_ptr<image> getImage(const std::string& transferSyntax,
                                            const std::string& colorSpace,
                                            std::uint32_t channelsNumber,
                                            std::uint32_t imageWidth,
                                            std::uint32_t imageHeight,
                                            bool bSubsampledX,
                                            bool bSubsampledY,
                                            bool bInterleaved,
                                            bool b2Complement,
                                            std::uint8_t allocatedBits,
                                            std::uint8_t storedBits,
                                            std::uint8_t highBit,
                                            std::shared_ptr<streamReader> pSourceStream) const override;

	// Insert a jpeg compressed image into a dataset
	///////////////////////////////////////////////////////////
	virtual void setImage(
            std::shared_ptr<streamWriter> pDestStream,
            std::shared_ptr<const image> pSourceImage,
            const std::string& transferSyntax,
            imageQuality_t imageQuality,
            std::uint32_t allocatedBits,
            bool bSubSampledX,
            bool bSubSampledY,
            bool bInterleaved,
            bool b2Complement) const override;

	// Return true if the codec can handle the transfer
	///////////////////////////////////////////////////////////
    virtual bool canHandleTransferSyntax(const std::string& transferSyntax) const override;

	// Returns true if the transfer syntax has to be
	//  encapsulated
	//
	///////////////////////////////////////////////////////////
    virtual bool encapsulated(const std::string& transferSyntax) const override;

    // Returns

    virtual bool defaultInterleaved() const override;
	// Return the suggested allocated bits
	///////////////////////////////////////////////////////////
    virtual std::uint32_t suggestAllocatedBits(const std::string& transferSyntax, std::uint32_t highBit) const override;



private:
    bool
    codeFrameIntoBuffer(char * outdata, size_t outlen, size_t & complen, const char * inputdata, size_t inputlength ,
                        uint32_t image_width,uint32_t image_height,
                        int sample_pixel ,
                        int bitsallocated ,
                        int bitsstored ,
                        int highbit ,
                        int sign,
                        int planarConfiguration
    ) const;


    JPEG2000Internals *Internals;
    bool LossyFlag;


};


/// @}

} // namespace codecs

} // namespace implementation

} // namespace imebra

#endif // JPEG2000

#endif // !defined(imebraJpeg2000ImageCodec_7F63E846_8824_42c6_A048_DD59C657AED4__INCLUDED_)
