/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

DICOMHero/Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement
 that your application must also be GPL), you may purchase a commercial
 license for DICOMHero/Imebra from the DICOMHero’s website (https://dicomhero.com).
*/

#if !defined(imebraObjcBaseStreamOutput__INCLUDED_)
#define imebraObjcBaseStreamOutput__INCLUDED_

#import <Foundation/Foundation.h>
#include "dicomhero_macros.h"

///
/// \brief This class represents a generic output stream.
///
/// Specialized classes derived from this one can write into files on the
/// computer's disks (DicomheroFileStreamOutput) or to memory
/// (DicomheroMemoryStreamOutput).
///
/// The application can write into the stream by using a DicomheroStreamWriter
/// object.
///
///////////////////////////////////////////////////////////////////////////////
@interface DicomheroBaseStreamOutput: NSObject

{
    @public
    define_dicomhero_object_holder(BaseStreamOutput);
}

    -(id)initWithDicomheroBaseStreamOutput:define_dicomhero_parameter(BaseStreamOutput);

    -(void)dealloc;

@end

#endif // !defined(imebraObjcBaseStreamOutput__INCLUDED_)
