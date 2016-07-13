Quick tour
==========

Loading files
-------------

Imebra can read 2 kinds of files:

- DICOM files
- Jpeg files

In order to do this, Imebra supplies a DICOM codec and a JPEG codec.
Both the codes store the parsed content into a DICOM structure represented by the :cpp:class:`imebra::DataSet` class 
(yes, also the Jpeg codec produces a DICOM structure containing an embedded JPEG file).

Imebra chooses the correct codec automatically according to the stream's content.

In order to create an :cpp:class:`imebra::DataSet` from a stream you use the class :cpp:class:`imebra::CodecFactory`.

In C++:

.. code-block:: c++

    std::unique_ptr<imebra::DataSet> loadedDataSet(imebra::CodecFactory::load("DicomFile.dcm"));

In Java:

.. code-block:: java

    com.imebra.DataSet loadedDataSet = com.imebra.CodecFactory.load("DicomFile.dcm");

The previous line loads the file DicomFile.dcm.

Imebra allows to perform a "lazy loading", which leaves the data on the input stream and loads it into memory
only when necessary; large tags that are not needed are loaded only when necessary and then discarded from memory 
until they are needed once again.

This allows processing large DICOM files by loading large tags only when they are needed and is completely transparent
to the client application.

To enable the "lazy loading", specify the maximum size of the tags that must be loaded immediately. The following line
leaves all the tags bigger than 2048 bytes on the stream and loads them only when needed:

Lazy loading in C++:

.. code-block:: c++

    // Load tags in memory only if their size is equal or smaller than 2048 bytes
    std::unique_ptr<imebra::DataSet> loadedDataSet(imebra::CodecFactory::load("DicomFile.dcm", 2048));

and in Java

.. code-block:: java

    // Load tags in memory only if their size is equal or smaller than 2048 bytes
    com.imebra.DataSet loadedDataSet = com.imebra.CodecFactory.load("DicomFile.dcm", 2048);


Reading the tag's values
------------------------

Once the DataSet has been loaded your application can retrieve the tags stored in it.

Tags' values are converted automatically between different data types unless the conversion would cause a loss
of the value (e.g. the conversion of the string "10" to the number 10 succeeds, but the conversion of the string "Hello" to a number
fails).

In order to retrieve a tag's value, use one of the following methods

- :cpp:member:`imebra::DataSet::getSignedLong`
- :cpp:member:`imebra::DataSet::getUnsignedLong`
- :cpp:member:`imebra::DataSet::getDouble`
- :cpp:member:`imebra::DataSet::getString`
- :cpp:member:`imebra::DataSet::getUnicodeString`
- :cpp:member:`imebra::DataSet::getAge`
- :cpp:member:`imebra::DataSet::getDate`

Alternatively, you can retrieve a :cpp:class:`imebra::ReadingDataHandler` (via :cpp:member:`imebra::DataSet::getReadingDataHandler`) and 
call the methods it offers to read the tag's values.

If you are reading a tag containing numeric values then you can retrieve the Tag's :cpp:class:`imebra::ReadingNumericDataHandler`
(via :cpp:member:`imebra::DataSet::getReadingDataHandlerNumeric`) which exposes the raw memory that stores the actual data: in some cases
this allow for faster information processing.

In order to identify the tag to read you must use the class :cpp:class:`imebra::TagId` which as parameters takes the group ID and the tag ID or
an :cpp:enum:`imebra::tagId_t` enumeration (only in C++, not in Java).

This is how you retrieve the patient's name from the DataSet in C++:

.. code-block:: c++

    // A patient's name can contain up to 5 values, representing different interpretations of the same name
    // (e.g. alphabetic representation, ideographic representation and phonetic representation)
    // Here we retrieve the first interpretations (index 0 and 1)
    std::wstring patientNameCharacter = loadedDataSet->getUnicodeString(imebra::TagId(imebra::tagId_t::PatientName_0010_0010), 0);
    std::wstring patientNameIdeographic = loadedDataSet->getUnicodeString(imebra::TagId(imebra::tagId_t::PatientName_0010_0010), 1);

or

.. code-block:: c++

    // A patient's name can contain up to 5 values, representing different interpretations of the same name
    // (e.g. alphabetic representation, ideographic representation and phonetic representation)
    // Here we retrieve the first 2 interpretations (index 0 and 1)
    std::wstring patientNameCharacter = loadedDataSet->getUnicodeString(imebra::TagId(0x10, 0x10), 0);
    std::wstring patientNameIdeographic = loadedDataSet->getUnicodeString(imebra::TagId(0x10, 0x10), 1);

and in Java:

.. code-block:: c++

    // A patient's name can contain up to 5 values, representing different interpretations of the same name
    // (e.g. alphabetic representation, ideographic representation and phonetic representation)
    // Here we retrieve the first 2 interpretations (index 0 and 1)
    String patientNameCharacter = loadedDataSet.getUnicodeString(new com.imebra.TagId(0x10, 0x10), 0);
    String patientNameIdeographic = loadedDataSet.getUnicodeString(new com.imebra.TagId(0x10, 0x10), 0);
    std::wstring patientNameIdeographic = loadedDataSet->getUnicodeString(imebra::TagId(0x10, 0x10), 1);

Note that the following code will throw one of the exceptions derived from :cpp:class:`imebra::MissingDataElementError`
if the desidered patient name component is not present in the :cpp:class:`imebra::DataSet`.

You can specify a return value that is returned when the value is not present in order to avoid throwing an exception when
a tag's value cannot be found in the DataSet :

.. code-block:: c++

    // Return an empty name if the tag is not present
    std::wstring patientNameCharacter = loadedDataSet->getUnicodeString(imebra::TagId(imebra::tagId_t::PatientName_0010_0010), 0, L"");
    std::wstring patientNameIdeographic = loadedDataSet->getUnicodeString(imebra::TagId(imebra::tagId_t::PatientName_0010_0010), 1, L"");

or

.. code-block:: c++

    // Return an empty name if the tag is not present
    std::wstring patientNameCharacter = loadedDataSet->getUnicodeString(imebra::TagId(0x10, 0x10), 0, L"");
    std::wstring patientNameIdeographic = loadedDataSet->getUnicodeString(imebra::TagId(0x10, 0x10), 1, L"");

and in Java:

.. code-block:: c++

    // Return an empty name if the tag is not present
    String patientNameCharacter = loadedDataSet.getUnicodeString(new com.imebra.TagId(0x10, 0x10), 0, "");
    String patientNameIdeographic = loadedDataSet.getUnicodeString(new com.imebra.TagId(0x10, 0x10), 1, "");


Retrieving an image
-------------------

Imebra exposes two methods to retrieve images from a :cpp:class:`imebra::DataSet`:

- :cpp:member:`imebra::DataSet::getImage`
- :cpp:member:`imebra::DataSet::getImageApplyModalityTransform`

The second method applies to the image the :cpp:member:`imebra::DataSet::ModalityVOILUT` transform automatically if present
and is the reccommended method.

The retrieved image will have the color space & bits per channel as defined in the DataSet.

To retrieve an image in C++:

.. code-block:: c++

    // Retrieve the first image (index = 0)
    std::unique_ptr<imebra::Image> image(loadedDataSet->getImageApplyModalityTransform(0));

    // Get the color space
    std::string colorSpace = image->getColorSpace();

    // Get the size in pixels
    std::uint32_t width = image->getWidth();
    std::uint32_t height = image->getHeight();

To retrieve an image in Java:

.. code-block:: java

    // Retrieve the first image (index = 0)
    com.imebra.Image image = loadedDataSet.getImageApplyModalityTransform(0);

    // Get the color space
    String colorSpace = image.getColorSpace();

    // Get the size in pixels
    long width = image.getWidth();
    long height = image.getHeight();

In order to access the image's pixels you can obtain a :cpp:class:`imebra::ReadingDataHandlerNumeric` and then
access the individual pixels via :cpp:member:`imebra::ReadingDataHandler::getSignedLong` or 
:cpp:member:`imebra::ReadingDataHandler::getUnsignedLong`. For faster processing you could also access
the raw memory containing the pixels.

This is how you scan all the pixels in C++, the slow way

.. code-block:: c++

    // let's assume that we already have the image's size in the variables width and height
    // (see previous code snippet)

    // Retrieve the data handler
    std::unique_ptr<imebra::ReadingDataHandlerNumeric> dataHandler(image->getReadingDataHandler());

    for(std::uint32 scanY(0); scanY != height; ++scanY)
    {
        for(std::uint32 scanX(0); scanX != width; ++scanX)
        {
            // For monochrome images
            std::int32_t luminance = dataHandler->getSignedLong(scanY * width + scanX);

            // For RGB images
            std::int32_t r = dataHandler->getSignedLong((scanY * width + scanX) * 3);
            std::int32_t g = dataHandler->getSignedLong((scanY * width + scanX) * 3 + 1);
            std::int32_t b = dataHandler->getSignedLong((scanY * width + scanX) * 3 + 2);
        }
    }

How to access the pixels in Java:

.. code-block:: java

    // let's assume that we already have the image's size in the variables width and height
    // (see previous code snippet)

    // Retrieve the data handler
    com.imebra.ReadingDataHandlerNumeric dataHandler = image->getReadingDataHandler();

    for(long scanY = 0; scanY != height; scanY++)
    {
        for(long scanX = 0; scanX != width; scanX++)
        {
            // For monochrome images
            int luminance = dataHandler.getSignedLong(scanY * width + scanX);

            // For RGB images
            int r = dataHandler.getSignedLong((scanY * width + scanX) * 3);
            int g = dataHandler.getSignedLong((scanY * width + scanX) * 3 + 1);
            int b = dataHandler.getSignedLong((scanY * width + scanX) * 3 + 2);
        }
    }

In order to make things faster you can retrieve the memory containing the data in raw format from the :cpp:class:`imebra::ReadingDataHandlerNumeric`
object:

.. code-block:: c++

    // Retrieve the data handler
    std::unique_ptr<imebra::ReadingDataHandlerNumeric> dataHandler(image->getReadingDataHandler());

    // Get the memory pointer and the size (in bytes)
    size_t dataLength;
    const char* data = dataHandler->data(&dataLength);

    // Get the number of bytes per each value (1, 2, or 4 for images)
    size_t bytesPerValue = dataHandler->getUnitSize(); 

    // Are the values signed?
    bool bIsSigned = dataHandler->isSigned();

    // Do something with the pixels...A template function would come handy


Displaying an image
-------------------

An image may have to undergo several transformations before it can be displayed on a computer (or mobile) screen.
Usually, the computer monitor accepts 8 bit per channel RGB (or RGBA) images, while images retrieved from a DataSet
may have more than 8 bits per channel (up to 32) and may have a different color space (for instance MONOCHROME1, MONOCHROME2,
YBR_FULL, etc).

While the necessary transforms are performed automatically by the :cpp:class:`imebra::DrawBitmap` class, some 
transformations must still be performed by the client application.

In particular, the :cpp:class:`imebra::DrawBitmap` class takes care of:

- converting the color space
- shifting the channels values to 8 bit

The client application must take care of applying the :cpp:class:`imebra::ModalityVOILUT` transform (but this is easily done
by calling :cpp:member:`imebra::DataSet::getImageApplyModalityTransform` instead of :cpp:member:`imebra::DataSet::getImage`)
and the :cpp:member:`imebra::VOILUT` transform.

The :cpp:member:`imebra::VOILUT` can be applied only to monochromatic images and changes the image's contrast to enhance
different portions of the image (for instance just the bones or the tissue).

Usually, the dataSet contains few tags that store some pre-defined settings for the image: the client application should apply
those values to the VOILUT transform.
The pre-defined settings come as pairs of center/width values or as Lookup Tables stored in the DICOM sequence 0028,3010.

To retrieve the pairs center/width use the method :cpp:member:`imebra::DataSet::getVOIs`, while to retrieve the LUTs use
the method :cpp:member:`imebra::DataSet::getLUT`.

in C++

.. code-block:: c++

    // The transforms chain will contain all the transform that we want to 
    // apply to the image before displaying it
    imebra::TransformsChain chain;

    if(imebra::ColorTransformsFactory::isMonochrome(image->getColorSpace())
    {
        // Allocate a VOILUT transform. If the DataSet does not contain any pre-defined
        //  settings then we will find the optimal ones.
        VOILUT voilutTransform;

        // Retrieve the VOIs (center/width pairs)
        imebra::vois_t vois = loadedDataSet->getVOIs();

        // Retrieve the LUTs
        std::list<std::shared_ptr<imebra::LUT> > luts;
        for(size_t scanLUTs(0); ; ++scanLUTs)
        {
            try
            {
                luts.push_back(loadedDataSet->getLUT(scanLUTs));
            }
            catch(const imebra::MissingDataElementError&)
            {
                // Ignore: means end-of-LUTs
            }
        }

        if(!vois.empty())
        {
            voilutTransform.setCenterWidth(vois[0].center, vois[0].width);
        }
        else if(!luts.empty())
        {
            voilutTransform.setLUT(*(luts.front().get()));
        }
        else
        {
            voilutTransform.applyOptimalVOI(image, 0, 0, width, height);
        }
        
        chain.add(voilutTransform);        
    }

    // If the image is monochromatic then now chain contains the VOILUT transform

in Java

.. code-block:: java

    // The transforms chain will contain all the transform that we want to 
    // apply to the image before displaying it
    com.imebra.TransformsChain chain = new com.imebra.TransformsChain();

    if(com.imebra.ColorTransformsFactory.isMonochrome(image.getColorSpace())
    {
        // Allocate a VOILUT transform. If the DataSet does not contain any pre-defined
        //  settings then we will find the optimal ones.
        VOILUT voilutTransform = new VOILUT();

        // Retrieve the VOIs (center/width pairs)
        com.imebra.vois_t vois = loadedDataSet.getVOIs();

        // Retrieve the LUTs
        List<com.imebra.LUT> luts = new ArrayList<com.imebra.LUT>();
        for(int scanLUTs = 0; ; scanLUTs++)
        {
            try
            {
                luts.add(loadedDataSet.getLUT(scanLUTs));
            }
            catch(Exception e)
            {
                // Ignore: means end-of-LUTs
            }
        }

        if(!vois.isEmpty())
        {
            voilutTransform.setCenterWidth(vois.get(0).center, vois.get(0).width);
        }
        else if(!luts.isEmpty())
        {
            voilutTransform.setLUT(luts.get(0));
        }
        else
        {
            voilutTransform.applyOptimalVOI(image, 0, 0, width, height);
        }
        
        chain.add(voilutTransform);        
    }

    // If the image is monochromatic then now chain contains the VOILUT transform

Now we can display the image. We use :cpp:class:`imebra::DrawBitmap` to obtain an RGB image
ready to be displayed.

In C++

.. code-block:: c++

    // We create a DrawBitmap that always apply the chain transform before getting the RGB image
    imebra::DrawBitmap draw(chain);

    // Ask for the size of the buffer (in bytes)
    size_t requestedBufferSize = draw.getBitmap(image, imebra::drawBitmapType_t::drawBitmapRGBA, 4, 0, 0);
    
    // Now we allocate the buffer and then ask DrawBitmap to fill it
    std::string buffer(requestedBufferSize, char(0));
    draw.getBitmap(image, imebra::drawBitmapType_t::drawBitmapRGBA, 4, &(buffer.at(0)), requestedBufferSize);

In Java

.. code-block:: c++

    // We create a DrawBitmap that always apply the chain transform before getting the RGB image
    com.imebra.DrawBitmap draw = new com.imebra.DrawBitmap(chain);

    // Ask for the size of the buffer (in bytes)
    long requestedBufferSize = draw.getBitmap(image, imebra::drawBitmapType_t::drawBitmapRGBA, 4, new byte[0]);
    
    byte buffer[] = new byte[(int)requestedBufferSize]; // Ideally you want to reuse this in subsequent calls to getBitmap()
    ByteBuffer byteBuffer = ByteBuffer.wrap(buffer);

    // Now fill the buffer with the image data and create a bitmap from it
    drawBitmap.getBitmap(image, drawBitmapType_t.drawBitmapRGBA, 4, buffer);
    Bitmap renderBitmap = Bitmap.createBitmap((int)image.getWidth(), (int)image.getHeight(), Bitmap.Config.ARGB_8888);
    renderBitmap.copyPixelsFromBuffer(byteBuffer);

    // The Bitmap can be assigned to an ImageView on Android


Creating an empty DataSet
-------------------------

When creating an empty :cpp:class:`imebra::DataSet` you have to specify the transfer syntax that will be used to encode it.
The transfer syntax specifies also how the embedded images are compressed.

The accepted transfer syntaxes are:

- "1.2.840.10008.1.2" (Implicit VR little endian)
- "1.2.840.10008.1.2.1" (Explicit VR little endian)
- "1.2.840.10008.1.2.2" (Explicit VR big endian)
- "1.2.840.10008.1.2.5" (RLE compression)
- "1.2.840.10008.1.2.4.50" (Jpeg baseline 8 bit lossy)
- "1.2.840.10008.1.2.4.51" (Jpeg extended 12 bit lossy)
- "1.2.840.10008.1.2.4.57" (Jpeg lossless NH)
- "1.2.840.10008.1.2.4.70" (Jpeg lossless NH first order prediction)

To create an empty DataSet in C++:

.. code-block:: c++

    // We specify the transfer syntax and the charset
    imebra::DataSet dataSet("1.2.840.10008.1.2.1", "ISO 2022 IR 6");

In Java:

.. code-block:: c++

    // We specify the transfer syntax and the charset
    com.imebra.DataSet dataSet = new com.imebra.DataSet("1.2.840.10008.1.2.1", "ISO 2022 IR 6");


Modifying the dataset's content
-------------------------------

You can set the tags values by calling the setters on the DataSet or by retrieving a WritingDataHandler for
a specific tag.

WritingDataHandler objects allow modifying several tag's buffers, while the DataSet setters allow setting only
the element 0 of the first tag's buffer.

The available DataSet setters are:

Once the DataSet has been loaded your application can retrieve the tags stored in it.

In order to write a tag's value, use one of the following methods

- :cpp:member:`imebra::DataSet::setSignedLong`
- :cpp:member:`imebra::DataSet::setUnsignedLong`
- :cpp:member:`imebra::DataSet::setDouble`
- :cpp:member:`imebra::DataSet::setString`
- :cpp:member:`imebra::DataSet::setUnicodeString`
- :cpp:member:`imebra::DataSet::setAge`
- :cpp:member:`imebra::DataSet::setDate`

The WritingDataHandler and WritingDataHandlerNumeric contain the same setters but allow to access all the tags' elements, not just
the first one.

This is how you set the patient's name using the DataSet setter:

In C++:

.. code-block:: c++

    dataSet.setUnicodeString(TagId(imebra::tagId_t::PatientName_0010_0010), L"Patient^Name");

In Java:

.. code-block:: java

    dataSet.setUnicodeString(TagId(0x10, 0x10), "Patient^Name");

You can also set tags values by retrieving a WritingDataHandler and populating it: the WritingDataHandler will commit the data
into the DataSet when it is destroyed:

in C++:

.. code-block:: c++
    
    {
        std::unique_ptr<WritingDataHandler> dataHandler(dataSet.getWritingDataHandler(0));
        dataHandler->setUnicodeString(0, L"AlphabeticName");
        dataHandler->setUnicodeString(1, L"IdeographicName");
        dataHandler->setUnicodeString(2, L"PhoneticName");

        // dataHandler will go out of scope and will commit the data into the dataSet
    }

in Java:

.. code-block:: java
    
    {
        com.imebra.WritingDataHandler dataHandler = dataSet.getWritingDataHandler(0);
        dataHandler.setUnicodeString(0, "AlphabeticName");
        dataHandler.setUnicodeString(1, "IdeographicName");
        dataHandler.setUnicodeString(2, "PhoneticName");

        // Force the commit, don't wait for the garbage collector
        dataHandler.finalize();
    }


Embedding images into the dataSet
---------------------------------

When an image is stored in the dataSet then it is compressed according to the dataSet's transfer syntax.

in C++

.. code-block:: c++

    // Create a 300 by 200 pixel image, 15 bits per color channel, RGB
    imebra::Image image(300, 200, imebra::bitDepth_t::depthU16, "RGB", 15);
    
    {
        std::unique_ptr<WritingDataHandlerNumeric> dataHandler(image.getWritingDataHandler());

        // Set all the pixels to red
        for(std::uint32_t scanY(0); scanY != 200; ++scanY)
        {
            for(std::uint32_t scanX(0); scanX != 300; ++scanX)
            {
                dataHandler->setUnsignedLong((scanY * 300 + scanX) * 3, 65535);
                dataHandler->setUnsignedLong((scanY * 300 + scanX) * 3 + 1, 0);
                dataHandler->setUnsignedLong((scanY * 300 + scanX) * 3 + 2, 0);
            }
        }

        // dataHandler will go out of scope and will commit the data into the image
    }

    dataSet.setImage(0, image);

in Java++

.. code-block:: java

    // Create a 300 by 200 pixel image, 15 bits per color channel, RGB
    com.imebra.Image image = new com.imebra.Image(300, 200, com.imebra.bitDepth_t.depthU16, "RGB", 15);
    
    {
        WritingDataHandlerNumeric dataHandler = image.getWritingDataHandler();

        // Set all the pixels to red
        for(long scanY = 0; scanY != 200; scanY++)
        {
            for(long scanX =0; scanX != 300; scanX++)
            {
                dataHandler.setUnsignedLong((scanY * 300 + scanX) * 3, 65535);
                dataHandler.setUnsignedLong((scanY * 300 + scanX) * 3 + 1, 0);
                dataHandler.setUnsignedLong((scanY * 300 + scanX) * 3 + 2, 0);
            }
        }

        // Force the commit, don't wait for the garbage collector
        dataHandler.finalize();
    }

    dataSet.setImage(0, image);


Saving a DataSet
----------------

A DataSet can be saved using the CodecFactory:

in C++

.. code-block:: c++

    imebra::CodecFactory::save(dataSet, "dicomFile.dcm", imebra::codecType_t::dicom);

in Java++

.. code-block:: java

    com.imebra.CodecFactory.save(dataSet, "dicomFile.dcm", com.imebra.codecType_t.dicom);
