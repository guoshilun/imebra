//
// Created by dhz on 2021/10/18.
//

#ifndef IMEBRA_SCPDEFINE_H
#define IMEBRA_SCPDEFINE_H

#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream, std
#include <imebra/imebra.h>
#include <uv.h>
#include "DicomMessageHandler.h"
#include "DcmInfo.h"
#include "RuntimeConfig.h"
#include "unistd.h"
#include <sys/stat.h>
#include <spdlog/spdlog.h>
#include "scpConstant.h"

// List of accepted abstract syntaxes
const std::list<std::string> abstractSyntaxes{
        //  用于支持CEcho SCP 的传输语法
        imebra::dicom2018e::uidVerificationSOPClass,
        imebra::dicom2018e::uidStoredPrintStorageSOPClass,
        imebra::dicom2018e::uidHardcopyGrayscaleImageStorageSOPClass,
        imebra::dicom2018e::uidHardcopyColorImageStorageSOPClass,
        imebra::dicom2018e::uidComputedRadiographyImageStorage,
        imebra::dicom2018e::uidDigitalXRayImageStorageForPresentation,
        imebra::dicom2018e::uidDigitalXRayImageStorageForProcessing,
        imebra::dicom2018e::uidDigitalMammographyXRayImageStorageForPresentation,
        imebra::dicom2018e::uidDigitalMammographyXRayImageStorageForProcessing,
        imebra::dicom2018e::uidDigitalIntraOralXRayImageStorageForPresentation,
        imebra::dicom2018e::uidDigitalIntraOralXRayImageStorageForProcessing,
        imebra::dicom2018e::uidCTImageStorage,
        imebra::dicom2018e::uidEnhancedCTImageStorage,
        imebra::dicom2018e::uidLegacyConvertedEnhancedCTImageStorage,
        imebra::dicom2018e::uidUltrasoundMultiframeImageStorage,
        imebra::dicom2018e::uidMRImageStorage,
        imebra::dicom2018e::uidEnhancedMRImageStorage,
        imebra::dicom2018e::uidMRSpectroscopyStorage,
        imebra::dicom2018e::uidEnhancedMRColorImageStorage,
        imebra::dicom2018e::uidLegacyConvertedEnhancedMRImageStorage,
        imebra::dicom2018e::uidUltrasoundImageStorage,
        imebra::dicom2018e::uidEnhancedUSVolumeStorage,
        imebra::dicom2018e::uidSecondaryCaptureImageStorage,
        imebra::dicom2018e::uidMultiframeSingleBitSecondaryCaptureImageStorage,
        imebra::dicom2018e::uidMultiframeGrayscaleByteSecondaryCaptureImageStorage,
        imebra::dicom2018e::uidMultiframeGrayscaleWordSecondaryCaptureImageStorage,
        imebra::dicom2018e::uidMultiframeTrueColorSecondaryCaptureImageStorage,
        imebra::dicom2018e::uidStandaloneOverlayStorage,
        imebra::dicom2018e::uidStandaloneCurveStorage,
        imebra::dicom2018e::uidWaveformStorageTrial,
        imebra::dicom2018e::uid12leadECGWaveformStorage,
        imebra::dicom2018e::uidGeneralECGWaveformStorage,
        imebra::dicom2018e::uidAmbulatoryECGWaveformStorage,
        imebra::dicom2018e::uidHemodynamicWaveformStorage,
        imebra::dicom2018e::uidCardiacElectrophysiologyWaveformStorage,
        imebra::dicom2018e::uidBasicVoiceAudioWaveformStorage,
        imebra::dicom2018e::uidGeneralAudioWaveformStorage,
        imebra::dicom2018e::uidArterialPulseWaveformStorage,
        imebra::dicom2018e::uidRespiratoryWaveformStorage,
        imebra::dicom2018e::uidStandaloneModalityLUTStorage,
        imebra::dicom2018e::uidStandaloneVOILUTStorage,
        imebra::dicom2018e::uidGrayscaleSoftcopyPresentationStateStorage,
        imebra::dicom2018e::uidColorSoftcopyPresentationStateStorage,
        imebra::dicom2018e::uidPseudoColorSoftcopyPresentationStateStorage,
        imebra::dicom2018e::uidBlendingSoftcopyPresentationStateStorage,
        imebra::dicom2018e::uidXAXRFGrayscaleSoftcopyPresentationStateStorage,
        imebra::dicom2018e::uidGrayscalePlanarMPRVolumetricPresentationStateStorage,
        imebra::dicom2018e::uidCompositingPlanarMPRVolumetricPresentationStateStorage,
        imebra::dicom2018e::uidAdvancedBlendingPresentationStateStorage,
        imebra::dicom2018e::uidVolumeRenderingVolumetricPresentationStateStorage,
        imebra::dicom2018e::uidSegmentedVolumeRenderingVolumetricPresentationStateStorage,
        imebra::dicom2018e::uidMultipleVolumeRenderingVolumetricPresentationStateStorage,
        imebra::dicom2018e::uidXRayAngiographicImageStorage,
        imebra::dicom2018e::uidEnhancedXAImageStorage,
        imebra::dicom2018e::uidXRayRadiofluoroscopicImageStorage,
        imebra::dicom2018e::uidEnhancedXRFImageStorage,
        imebra::dicom2018e::uidXRayAngiographicBiPlaneImageStorage,
        imebra::dicom2018e::uidXRay3DAngiographicImageStorage,
        imebra::dicom2018e::uidXRay3DCraniofacialImageStorage,
        imebra::dicom2018e::uidBreastTomosynthesisImageStorage,
        imebra::dicom2018e::uidNuclearMedicineImageStorageRetired,
        imebra::dicom2018e::uidNuclearMedicineImageStorage,
        imebra::dicom2018e::uidParametricMapStorage,
        imebra::dicom2018e::uidRawDataStorage,
        imebra::dicom2018e::uidSpatialRegistrationStorage,
        imebra::dicom2018e::uidSpatialFiducialsStorage,
        imebra::dicom2018e::uidDeformableSpatialRegistrationStorage,
        imebra::dicom2018e::uidSegmentationStorage,
        imebra::dicom2018e::uidSurfaceSegmentationStorage,
        imebra::dicom2018e::uidTractographyResultsStorage,
        imebra::dicom2018e::uidRealWorldValueMappingStorage,
        imebra::dicom2018e::uidSurfaceScanMeshStorage,
        imebra::dicom2018e::uidSurfaceScanPointCloudStorage,
        imebra::dicom2018e::uidVLImageStorageTrial,
        imebra::dicom2018e::uidVLMultiframeImageStorageTrial,
        imebra::dicom2018e::uidVLEndoscopicImageStorage,
        imebra::dicom2018e::uidVideoEndoscopicImageStorage,
        imebra::dicom2018e::uidVLMicroscopicImageStorage,
        imebra::dicom2018e::uidVideoMicroscopicImageStorage,
        imebra::dicom2018e::uidVLSlideCoordinatesMicroscopicImageStorage,
        imebra::dicom2018e::uidVLPhotographicImageStorage,
        imebra::dicom2018e::uidVideoPhotographicImageStorage,
        imebra::dicom2018e::uidOphthalmicPhotography8BitImageStorage,
        imebra::dicom2018e::uidOphthalmicPhotography16BitImageStorage,
        imebra::dicom2018e::uidStereometricRelationshipStorage,
        imebra::dicom2018e::uidOphthalmicTomographyImageStorage,
        imebra::dicom2018e::uidTextSRStorageTrial,
        imebra::dicom2018e::uidAudioSRStorageTrial,
        imebra::dicom2018e::uidDetailSRStorageTrial,
        imebra::dicom2018e::uidComprehensiveSRStorageTrial,
        imebra::dicom2018e::uidBasicTextSRStorage,
        imebra::dicom2018e::uidEnhancedSRStorage,
        imebra::dicom2018e::uidComprehensiveSRStorage,
        imebra::dicom2018e::uidComprehensive3DSRStorage,
        imebra::dicom2018e::uidExtensibleSRStorage,
        imebra::dicom2018e::uidProcedureLogStorage,
        imebra::dicom2018e::uidMammographyCADSRStorage,
        imebra::dicom2018e::uidKeyObjectSelectionDocumentStorage,
        imebra::dicom2018e::uidChestCADSRStorage,
        imebra::dicom2018e::uidXRayRadiationDoseSRStorage,
        imebra::dicom2018e::uidRadiopharmaceuticalRadiationDoseSRStorage,
        imebra::dicom2018e::uidColonCADSRStorage,
        imebra::dicom2018e::uidImplantationPlanSRStorage,
        imebra::dicom2018e::uidAcquisitionContextSRStorage,
        imebra::dicom2018e::uidSimplifiedAdultEchoSRStorage,
        imebra::dicom2018e::uidPatientRadiationDoseSRStorage,
        imebra::dicom2018e::uidPlannedImagingAgentAdministrationSRStorage,
        imebra::dicom2018e::uidPerformedImagingAgentAdministrationSRStorage,
        imebra::dicom2018e::uidContentAssessmentResultsStorage,
        imebra::dicom2018e::uidEncapsulatedPDFStorage,
        imebra::dicom2018e::uidEncapsulatedCDAStorage,
        imebra::dicom2018e::uidEncapsulatedSTLStorage,
        imebra::dicom2018e::uidPositronEmissionTomographyImageStorage,
        imebra::dicom2018e::uidLegacyConvertedEnhancedPETImageStorage,
        imebra::dicom2018e::uidStandalonePETCurveStorage,
        imebra::dicom2018e::uidEnhancedPETImageStorage,
        imebra::dicom2018e::uidBasicStructuredDisplayStorage,
        imebra::dicom2018e::uidCTDefinedProcedureProtocolStorage,
        imebra::dicom2018e::uidCTPerformedProcedureProtocolStorage,
        imebra::dicom2018e::uidProtocolApprovalStorage,
        imebra::dicom2018e::uidProtocolApprovalInformationModelFIND,
        imebra::dicom2018e::uidProtocolApprovalInformationModelMOVE,
        imebra::dicom2018e::uidProtocolApprovalInformationModelGET,
        imebra::dicom2018e::uidRTImageStorage,
        imebra::dicom2018e::uidRTDoseStorage,
        imebra::dicom2018e::uidRTStructureSetStorage,
        imebra::dicom2018e::uidRTBeamsTreatmentRecordStorage,
        imebra::dicom2018e::uidRTPlanStorage,
        imebra::dicom2018e::uidRTBrachyTreatmentRecordStorage,
        imebra::dicom2018e::uidRTTreatmentSummaryRecordStorage,
        imebra::dicom2018e::uidRTIonPlanStorage,
        imebra::dicom2018e::uidRTIonBeamsTreatmentRecordStorage,
        imebra::dicom2018e::uidRTPhysicianIntentStorage,
        imebra::dicom2018e::uidRTSegmentAnnotationStorage,
        imebra::dicom2018e::uidDICOSCTImageStorage,
        imebra::dicom2018e::uidDICOSDigitalXRayImageStorageForPresentation,
        imebra::dicom2018e::uidDICOSDigitalXRayImageStorageForProcessing,
        imebra::dicom2018e::uidDICOSThreatDetectionReportStorage,
        imebra::dicom2018e::uidDICOS2DAITStorage,
        imebra::dicom2018e::uidDICOS3DAITStorage,
        imebra::dicom2018e::uidDICOSQuadrupoleResonanceQRStorage,
        imebra::dicom2018e::uidEddyCurrentImageStorage,
        imebra::dicom2018e::uidEddyCurrentMultiframeImageStorage,
        imebra::dicom2018e::uidPatientRootQueryRetrieveInformationModelFIND,
        imebra::dicom2018e::uidPatientRootQueryRetrieveInformationModelMOVE,
        imebra::dicom2018e::uidPatientRootQueryRetrieveInformationModelGET,
        imebra::dicom2018e::uidStudyRootQueryRetrieveInformationModelFIND,
        imebra::dicom2018e::uidStudyRootQueryRetrieveInformationModelMOVE,
        imebra::dicom2018e::uidStudyRootQueryRetrieveInformationModelGET,
        imebra::dicom2018e::uidPatientStudyOnlyQueryRetrieveInformationModelFIND,
        imebra::dicom2018e::uidPatientStudyOnlyQueryRetrieveInformationModelMOVE,
        imebra::dicom2018e::uidPatientStudyOnlyQueryRetrieveInformationModelGET,
        imebra::dicom2018e::uidCompositeInstanceRootRetrieveMOVE,
        imebra::dicom2018e::uidCompositeInstanceRootRetrieveGET,
        imebra::dicom2018e::uidCompositeInstanceRetrieveWithoutBulkDataGET,
        imebra::dicom2018e::uidDefinedProcedureProtocolInformationModelFIND,
        imebra::dicom2018e::uidDefinedProcedureProtocolInformationModelMOVE,
        imebra::dicom2018e::uidDefinedProcedureProtocolInformationModelGET,
        imebra::dicom2018e::uidModalityWorklistInformationModelFIND,
        imebra::dicom2018e::uidGeneralPurposeWorklistInformationModelFIND,
        imebra::dicom2018e::uidRTBeamsDeliveryInstructionStorageTrial,
        imebra::dicom2018e::uidRTBeamsDeliveryInstructionStorage,
        imebra::dicom2018e::uidRTBrachyApplicationSetupDeliveryInstructionStorage,
        imebra::dicom2018e::uidGeneralRelevantPatientInformationQuery,
        imebra::dicom2018e::uidBreastImagingRelevantPatientInformationQuery,
        imebra::dicom2018e::uidCardiacRelevantPatientInformationQuery,
        imebra::dicom2018e::uidColorPaletteStorage,
        imebra::dicom2018e::uidColorPaletteQueryRetrieveInformationModelFIND,
        imebra::dicom2018e::uidColorPaletteQueryRetrieveInformationModelMOVE,
        imebra::dicom2018e::uidColorPaletteQueryRetrieveInformationModelGET,
};

// List of accepted transfer syntaxes
const std::list<std::string> transferSyntaxes
        {
                imebra::dicom2018e::uidImplicitVRLittleEndian,
                imebra::dicom2018e::uidExplicitVRLittleEndian,
                imebra::dicom2018e::uidExplicitVRBigEndian,
                imebra::dicom2018e::uidJPEGBaselineProcess1,
                imebra::dicom2018e::uidJPEGExtendedProcess2_4,
                imebra::dicom2018e::uidJPEGExtendedProcess3_5,
                imebra::dicom2018e::uidJPEGSpectralSelectionNonHierarchicalProcess6_8,
                imebra::dicom2018e::uidJPEGSpectralSelectionNonHierarchicalProcess7_9,
                imebra::dicom2018e::uidJPEGFullProgressionNonHierarchicalProcess10_12,
                imebra::dicom2018e::uidJPEGFullProgressionNonHierarchicalProcess11_13,
                imebra::dicom2018e::uidJPEGLosslessNonHierarchicalProcess14,
                imebra::dicom2018e::uidJPEGLosslessNonHierarchicalProcess15,
                imebra::dicom2018e::uidJPEGExtendedHierarchicalProcess16_18,
                imebra::dicom2018e::uidJPEGExtendedHierarchicalProcess17_19,
                imebra::dicom2018e::uidJPEGSpectralSelectionHierarchicalProcess20_22,
                imebra::dicom2018e::uidJPEGSpectralSelectionHierarchicalProcess21_23,
                imebra::dicom2018e::uidJPEGFullProgressionHierarchicalProcess24_26,
                imebra::dicom2018e::uidJPEGFullProgressionHierarchicalProcess25_27,
                imebra::dicom2018e::uidJPEGLosslessHierarchicalProcess28,
                imebra::dicom2018e::uidJPEGLosslessHierarchicalProcess29,
                imebra::dicom2018e::uidJPEGLosslessNonHierarchicalFirstOrderPredictionProcess14SelectionValue1,
                imebra::dicom2018e::uidJPEGLSLosslessImageCompression,
                imebra::dicom2018e::uidJPEGLSLossyNearLosslessImageCompression,
                imebra::dicom2018e::uidJPEG2000ImageCompressionLosslessOnly,
                imebra::dicom2018e::uidJPEG2000ImageCompression,
                imebra::dicom2018e::uidJPEG2000Part2MulticomponentImageCompressionLosslessOnly,
                imebra::dicom2018e::uidJPEG2000Part2MulticomponentImageCompression,
                imebra::dicom2018e::uidJPIPReferenced,
                imebra::dicom2018e::uidJPIPReferencedDeflate,
                imebra::dicom2018e::uidMPEG2MainProfileMainLevel,
                imebra::dicom2018e::uidMPEG2MainProfileHighLevel,
                imebra::dicom2018e::uidMPEG4AVCH264HighProfileLevel41,
                imebra::dicom2018e::uidMPEG4AVCH264BDcompatibleHighProfileLevel41,
                imebra::dicom2018e::uidMPEG4AVCH264HighProfileLevel42For2DVideo,
                imebra::dicom2018e::uidMPEG4AVCH264HighProfileLevel42For3DVideo,
                imebra::dicom2018e::uidMPEG4AVCH264StereoHighProfileLevel42,
                imebra::dicom2018e::uidHEVCH265MainProfileLevel51,
                imebra::dicom2018e::uidHEVCH265Main10ProfileLevel51,
                imebra::dicom2018e::uidRLELossless
        };


////////////////////////////////////////////////////////////////
/////  RabbitMQ  消息队列设置
/////////////////////////////////////////////////////////////

static void setupDicomContexts(imebra::PresentationContexts &presentationContexts) {

    for (const std::string &abstractSyntax: abstractSyntaxes) {


        if (0 == strcasecmp(abstractSyntax.c_str(), "2.25.215425430857432966361489015284054781485")) {
            std::wcout << L"Found :abstractSyntax:" << abstractSyntax.c_str() << std::endl;
        }

        imebra::PresentationContext context(abstractSyntax);
        for (const std::string &transferSyntax: transferSyntaxes) {
            context.addTransferSyntax(transferSyntax);

        }
        presentationContexts.addPresentationContext(context);
    }
}


static imebra::PresentationContexts presentationContexts;
static std::set<imebra::AssociationBase *> activeAssociations;
static std::mutex lockActiveAssociations; // Lock the access to the associations set.

///
/// \brief When a DIMSE connection is received then this method is executed in a
///        separate thread and continues until the connection is closed, either
///        on the remote or on the local side.
///
/// \param tcpStream the tcp stream for the TCP connection
/// \param aet       the SCP aet to communicate during the ACSE negotiation
///
//////////////////////////////////////////////////////////////////////////////////////
void dimseCommands(imebra::TCPStream tcpStream, std::string aet, std::string dcmSaveDirectory, RuntimeConfig *ptr) {


    std::once_flag commandFlagOnlyOnce;
    std::set<DcmInfo> dicomMessages;
    imebra::AssociationSCP*  pScp= nullptr;
    try {
        // tcpStream represents the connected socket. Allocate a stream reader and a writer
        // to read and write on the connected socket
        imebra::StreamReader readSCU(tcpStream.getStreamInput());
        imebra::StreamWriter writeSCU(tcpStream.getStreamOutput());

        // The AssociationSCP constructor will negotiate the assocation
        imebra::AssociationSCP scp(aet, 1, 1, presentationContexts, readSCU, writeSCU, 0, 10);
        pScp = &scp;

        {
            std::lock_guard<std::mutex> lock(lockActiveAssociations);
            activeAssociations.insert(&scp);
        }

        try {
            // Receive commands via the dimse service, which uses the scp association
            imebra::DimseService dimse(scp);

            std::wcout <<L"DimseService构建完毕" << std::endl;
            // Receive commands until the association is closed
            for (;;) {
                // Blocks until a command is received, throws EOF when the connection
                // is closed.
                imebra::DimseCommand command(dimse.getCommand());

                // Act accordingly to the type of the received command
                switch (command.getCommandType()) {
                    case imebra::dimseCommandType_t::cStore:
                        // Received a CSTORE command
                        ////////////////////////////
                    {

                        imebra::CStoreCommand cstore = command.getAsCStoreCommand(); // Convert to cstore to retrieve cstore-specific data

                        try {
                            imebra::DataSet payload = cstore.getPayloadDataSet();
                            ptr->perfermFileStorek(dicomMessages, payload, dcmSaveDirectory);
                            dimse.sendCommandOrResponse(
                                    imebra::CStoreResponse(cstore, imebra::dimseStatusCode_t::success));
                        }
                        catch (const imebra::MissingItemError &itemError) {
                            spdlog::error("getPayloadDataSet and MissingItemError:{}", itemError.what());
                            dimse.sendCommandOrResponse(
                                    imebra::CStoreResponse(cstore, imebra::dimseStatusCode_t::unableToProcess));
                        }


                    }
                        break;

                    case imebra::dimseCommandType_t::cEcho:
                        // Received a CECHO command
                        ////////////////////////////
                    {

                        imebra::CEchoCommand cecho = command.getAsCEchoCommand(); // Convert to cmove to retrieve cecho-specific data

                        // Respond with an error
                        dimse.sendCommandOrResponse(imebra::CEchoResponse(cecho, imebra::dimseStatusCode_t::success));
                    }
                        break;
                    default:
                        // Received other  command, 不处理
                        ////////////////////////////
                    {
                        spdlog::error("Wrong command received ");

                    }
                        break;

                }
            }
        }

        catch (const imebra::StreamEOFError &error) {
            // The association has been closed during the association
            spdlog::info("StreamEOFError: {}", error.what());
        }
        catch (const std::exception &e) {
            spdlog::error("错误：{}", e.what());
        }


    }
    catch (const imebra::AcseCorruptedMessageError &acseCorruptedMessageError) {
        spdlog::error("AcseCorruptedMessageError错误：{}", acseCorruptedMessageError.what());
    }
    catch (const imebra::StreamError &streamError) {
        spdlog::info("streamError {}", streamError.what());
    }
    catch (const std::exception &e) {
        std::wcout << e.what() << std::endl;
    }
    std::call_once( commandFlagOnlyOnce, [&]()
    {
        std::lock_guard<std::mutex> lock(lockActiveAssociations);
        if(pScp != nullptr){
            activeAssociations.erase(pScp);
        }
    });
    if(dicomMessages.empty()){
        return;
    }

    spdlog::info("释放内存，发布消息");
    //----推送相关记录到消息队列
    ptr->publishCStoreMessage(dicomMessages);


}


#endif //IMEBRA_SCPDEFINE_H
