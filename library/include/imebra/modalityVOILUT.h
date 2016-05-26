/*
$fileHeader$
*/

/*! \file modalityVOILUT.h
    \brief Declaration of the class ModalityVOILUT.

*/

#if !defined(imebraModalityVOILUT__INCLUDED_)
#define imebraModalityVOILUT__INCLUDED_

#include "transform.h"
#include "dataSet.h"
#include "definitions.h"

namespace imebra
{


///
/// \brief The ModalityVOILUT transform applies the Modality VOI or LUT
///        to the input image.
///
/// The Modality VOI/LUT applies a rescale intercept and a slope
/// to transform the pixel values of the image into values that are meaningful
/// to the application.
///
/// For instance, the original pixel values could store a device specific
/// value that has a meaning only when used by the device that generated it:
/// applying the rescale slope/intercept to pixel value converts the original
/// values into optical density or other known measurement units
/// (e.g. hounsfield).
///
/// When the transformation is not linear, then a LUT (lookup table) is
/// applied.
///
///////////////////////////////////////////////////////////////////////////////
class IMEBRA_API ModalityVOILUT: public Transform
{
    ModalityVOILUT(const ModalityVOILUT&) = delete;
    ModalityVOILUT& operator=(const ModalityVOILUT&) = delete;

public:
    /// \brief Constructor.
    ///
    /// \param dataset the DataSet from which the modality VOI or LUT data
    ///                is retrieved
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ModalityVOILUT(const DataSet& dataset);

    virtual ~ModalityVOILUT();
};

}

#endif // imebraModalityVOILUT__INCLUDED_
