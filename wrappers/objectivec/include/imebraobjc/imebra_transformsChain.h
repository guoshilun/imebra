/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

DICOMHero/Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement
 that your application must also be GPL), you may purchase a commercial
 license for DICOMHero/Imebra from the DICOMHero’s website (https://dicomhero.com).
*/

#if !defined(imebraObjcTransformsChain__INCLUDED_)
#define imebraObjcTransformsChain__INCLUDED_

#import "imebra_transform.h"

///
/// \brief ImebraTransformsChain applies a series of ImebraTransforms to the
///        input image.
///
/// Transforms can be added to the chain by using addTransform().
///
///////////////////////////////////////////////////////////////////////////////
@interface ImebraTransformsChain: ImebraTransform

    -(id)init;

    -(void)addTransform:(ImebraTransform*)pTransform;

@end


#endif // imebraObjcTransformsChain__INCLUDED_
