/*
 * This application demonstrate a simple DICOM SCP.
 *
 * The application listen for incoming ACSE association and parses
 * DIMSE streams once the association is negotiated.
 * The application can process several concurrent connections.
 *
 * Each connection is processed in a separate thread, and for each
 * received command the header and payload datasets are logged to
 * the standard output.
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <functional>
#include <list>
#include <set>

#include <imebra/imebra.h>

#include <fstream>
#include <cstdlib>

#include <sstream>
#include  <unistd.h>
#include "scpDefine.h"
///
/// \brief Output to the standard output all the tags contained in a dataset
///
/// \param dataset the dataset to output
/// \param prefix  a string to display before each tag (used for indentation
///                in recursive calls of this method)
///
//////////////////////////////////////////////////////////////////////////////////////
void outputDatasetTags(const imebra::DataSet &dataset, const std::wstring &prefix) {
    // Get all the tags
    imebra::tagsIds_t tags = dataset.getTags();

    // Output all the tags
    for (const imebra::TagId &tagId: tags) {
        try {
            std::wstring tagName = imebra::DicomDictionary::getUnicodeTagDescription(tagId);
            std::wcout << prefix << L"Tag " << tagId.getGroupId() << L"," << tagId.getTagId() << L" (" << tagName
                       << L")" << std::endl;
        }
        catch (const imebra::DictionaryUnknownTagError &) {
            std::wcout << prefix << L"Tag " << tagId.getGroupId() << L"," << tagId.getTagId() << L" (Unknown tag)"
                       << std::endl;
        }

        imebra::Tag tag(dataset.getTag(tagId));

        for (size_t itemId(0);; ++itemId) {
            try {
                imebra::DataSet sequence = tag.getSequenceItem(itemId);
                std::wcout << prefix << L"  SEQUENCE " << itemId << std::endl;
                outputDatasetTags(sequence, prefix + L"    ");
            }
            catch (const imebra::MissingDataElementError &) {
                break;
            }
        }

        for (size_t bufferId(0); bufferId != tag.getBuffersCount(); ++bufferId) {
            imebra::ReadingDataHandler handler = tag.getReadingDataHandler(bufferId);
            if (handler.getDataType() != imebra::tagVR_t::OW && handler.getDataType() != imebra::tagVR_t::OB) {
                for (size_t scanHandler(0); scanHandler != handler.getSize(); ++scanHandler) {
                    std::wcout << prefix << L"  buffer " << bufferId << L", position " << scanHandler << ":"
                               << handler.getUnicodeString(scanHandler) << std::endl;
                }
            } else {
                std::wcout << prefix << L"  Not shown: size " << handler.getSize() << " elements" << std::endl;
            }

        }
    }
}


///
/// \brief Calls outputDatasetTags to display both the command dataset and the
///        payload dataset.
///
/// \param title   title to display before the datasets
/// \param command DIMSE command containing the command and payload datasets
///
//////////////////////////////////////////////////////////////////////////////////////
void outputCommandTags(const std::string &title, const imebra::DimseCommand &command) {
    // 不显示命令
//    if (!title.empty()) {
//        std::wcout << std::endl;
//        std::wcout << std::endl;
//        std::wcout << title.c_str() << std::endl;
//        std::wcout << std::wstring(title.size(), L'*') << std::endl;
//        std::wcout << std::endl;
//    }
//
//    try {
//        // Get the header dataset
//        imebra::DataSet header = command.getCommandDataSet();
//        std::wcout << std::endl;
//        std::wcout << L"    HEADER:" << std::endl;
//        std::wcout << L"    -------" << std::endl;
//        outputDatasetTags(header, L"    ");
//
//        // Get the payload dataset
//        imebra::DataSet payload = command.getPayloadDataSet();
//        std::wcout << std::endl;
//        std::wcout << L"    PAYLOAD:" << std::endl;
//        std::wcout << L"    --------" << std::endl;
//        outputDatasetTags(payload, L"    ");
//    }
//    catch (const imebra::MissingItemError &) {
//        // We arrive here if the payload we request above does not exist.
//    }
}


// When an association is created then its address is inserted
// in the set below, when it is destroyed it is removed from the set.
// When the app must terminate then we abort all the active associations.
static std::set<imebra::AssociationBase *> activeAssociations;
static std::mutex lockActiveAssociations; // Lock the access to the associations set.
static imebra::PresentationContexts presentationContexts;

void initDicomProcessor() {

    for (const std::string &abstractSyntax: abstractSyntaxes) {
        imebra::PresentationContext context(abstractSyntax);
        for (const std::string &transferSyntax: transferSyntaxes) {
            context.addTransferSyntax(transferSyntax);
        }
        presentationContexts.addPresentationContext(context);
    }
}


void saveToDisk(imebra::DataSet &payload, std::string &dcmStoreDir) {
    std::string patientId = payload.getString(imebra::TagId(imebra::tagId_t::PatientID_0010_0020),
                                              0, "");
    std::string studyUid = payload.getString(
            imebra::TagId(imebra::tagId_t::StudyInstanceUID_0020_000D),
            0, "");

    std::string seriesUid = payload.getString(
            imebra::TagId(imebra::tagId_t::SeriesInstanceUID_0020_000E),
            0, "");
    std::string sopInstUid = payload.getString(
            imebra::TagId(imebra::tagId_t::SOPInstanceUID_0008_0018), 0, "");

    if (patientId.empty() || studyUid.empty() || seriesUid.empty() || sopInstUid.empty()) {
        return  ;

    }
    std::ostringstream saveTo;
    saveTo << dcmStoreDir << patientId << "/" << studyUid << "/" << seriesUid << "/";

    std::stringstream commandText;
    commandText << "mkdir -p  \"" << saveTo.str() << "\"";
    std::string command = commandText.str();
    commandText.clear();
    std::system(command.c_str());
    saveTo << sopInstUid << ".dcm";
    std::string savePath = saveTo.str();
    saveTo.clear();
    imebra::CodecFactory::save(payload, savePath, imebra::codecType_t::dicom);
    std::wcout << L"Save DicomFile To:" << savePath.c_str() << std::endl;

}



///
/// \brief When a DIMSE connection is received then this method is executed in a
///        separate thread and continues until the connection is closed, either
///        on the remote or on the local side.
///
/// \param tcpStream the tcp stream for the TCP connection
/// \param aet       the SCP aet to communicate during the ACSE negotiation
///
//////////////////////////////////////////////////////////////////////////////////////
void processDimseCommands(imebra::TCPStream tcpStream, std::string  aet ,std::string dcmSaveDirectory ) {

    std::list<imebra::DataSet> cPayLoadQueue;

    try {
        // tcpStream represents the connected socket. Allocate a stream reader and a writer
        // to read and write on the connected socket
        imebra::StreamReader readSCU(tcpStream.getStreamInput());
        imebra::StreamWriter writeSCU(tcpStream.getStreamOutput());

        // The AssociationSCP constructor will negotiate the assocation
        imebra::AssociationSCP scp(aet, 1, 1, presentationContexts, readSCU, writeSCU, 0, 10);

        {
            std::lock_guard<std::mutex> lock(lockActiveAssociations);
            activeAssociations.insert(&scp);
        }

        try {
            // Receive commands via the dimse service, which uses the scp association
            imebra::DimseService dimse(scp);

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
                        outputCommandTags("**** Received CSTORE command from " + scp.getOtherAET(), command);
                        imebra::CStoreCommand cstore = command.getAsCStoreCommand(); // Convert to cstore to retrieve cstore-specific data
                        imebra::DataSet payload = cstore.getPayloadDataSet();
                        cPayLoadQueue.push_back(payload);
                        dimse.sendCommandOrResponse(imebra::CStoreResponse(cstore, imebra::dimseStatusCode_t::success));
                    }
                        break;
                    case imebra::dimseCommandType_t::cGet:
                        // Received a CGET command
                        ////////////////////////////
                    {
                        outputCommandTags("**** Received CGET command from " + scp.getOtherAET(), command);

                        imebra::CGetCommand cget = command.getAsCGetCommand(); // Convert to cget to retrieve cget-specific data

                        // Respond with an error
                        dimse.sendCommandOrResponse(
                                imebra::CGetResponse(cget, imebra::dimseStatusCode_t::unappropriateActionForInstance, 0,
                                                     0, 0, 0));
                    }
                        break;

                    case imebra::dimseCommandType_t::cFind:
                        // Received a CFIND command
                        ////////////////////////////
                    {
                        outputCommandTags("**** Received CFIND command from " + scp.getOtherAET(), command);

                        imebra::CFindCommand cfind = command.getAsCFindCommand(); // Convert to cfind to retrieve cfind-specific data

                        // CFIND processing example
                        //-------------------------
                        const imebra::DataSet payload = cfind.getPayloadDataSet();

                        // Find the root of the query
                        std::string abstractSyntax = cfind.getAbstractSyntax();
                        if (abstractSyntax == imebra::dicom2018e::uidPatientRootQueryRetrieveInformationModelFIND) {
                            // Patient root
                            // ....
                        } else if (abstractSyntax ==
                                   imebra::dicom2018e::uidStudyRootQueryRetrieveInformationModelFIND) {
                            // Study root
                            // ....
                        }

                        // Find out what we should retrieve (e.g. "PATIENT", "STUDY")
                        std::string queryLevel = payload.getString(
                                imebra::TagId(imebra::tagId_t::QueryRetrieveLevel_0008_0052), 0);

                        // Find out the query parameters
                        std::string patientId = payload.getString(imebra::TagId(imebra::tagId_t::PatientID_0010_0020),
                                                                  0, "");
                        std::string patientIdIssuer = payload.getString(
                                imebra::TagId(imebra::tagId_t::IssuerOfPatientID_0010_0021), 0, "");
                        std::string patientName = payload.getString(
                                imebra::TagId(imebra::tagId_t::PatientName_0010_0010), 0, "");

                        // Search the database for results
                        // .....

                        // For each result:
                        imebra::charsetsList_t charsets;
                        charsets.push_back("ISO_IR 192");
                        imebra::MutableDataSet responseDataSet(scp.getTransferSyntax(abstractSyntax), charsets);

                        responseDataSet.setString(imebra::TagId(imebra::tagId_t::QueryRetrieveLevel_0008_0052),
                                                  queryLevel);

                        //...Set the key attributes for the found record
                        responseDataSet.setString(imebra::TagId(imebra::tagId_t::PatientID_0010_0020),
                                                  "FoundPatientId");
                        //...
                        responseDataSet.setString(imebra::TagId(imebra::tagId_t::RetrieveAETitle_0008_0054),
                                                  "AET where the file can be retrieved with c-get or c-move");
                        responseDataSet.setString(imebra::TagId(imebra::tagId_t::SOPInstanceUID_0008_0018),
                                                  "SOP.INSTANCE.FOUND.RECORD");

                        // Send the response
                        imebra::CFindResponse response(cfind, responseDataSet);
                        dimse.sendCommandOrResponse(response);

                        // At the end, send the final confirmation for c-find
                        imebra::CFindResponse finalResponse(cfind, imebra::dimseStatusCode_t::success);
                        dimse.sendCommandOrResponse(finalResponse);

                    }
                        break;
                    case imebra::dimseCommandType_t::cMove:
                        // Received a CMOVE command
                        ////////////////////////////
                    {
                        outputCommandTags("**** Received CMOVE command from " + scp.getOtherAET(), command);

                        imebra::CMoveCommand cmove = command.getAsCMoveCommand(); // Convert to cmove to retrieve cmove-specific data

                        // Respond with an error
                        dimse.sendCommandOrResponse(
                                imebra::CMoveResponse(cmove, imebra::dimseStatusCode_t::unappropriateActionForInstance,
                                                      0, 0, 0, 0));
                    }
                        break;
                    case imebra::dimseCommandType_t::cCancel:
                        // Received a CMOVE command
                        ////////////////////////////
                    {
                        outputCommandTags("**** Received CCANCEL command from " + scp.getOtherAET(), command);
                    }
                        break;
                    case imebra::dimseCommandType_t::cEcho:
                        // Received a CECHO command
                        ////////////////////////////
                    {
                        outputCommandTags("**** Received CECHO command from " + scp.getOtherAET(), command);

                        imebra::CEchoCommand cecho = command.getAsCEchoCommand(); // Convert to cmove to retrieve cecho-specific data

                        // Respond with an error
                        dimse.sendCommandOrResponse(imebra::CEchoResponse(cecho, imebra::dimseStatusCode_t::success));
                    }
                        break;
                    case imebra::dimseCommandType_t::nEventReport:
                        // Received a NEVENTREPORT command
                        ///////////////////////////////////
                    {
                        outputCommandTags("**** Received NEVENTREPORT command from " + scp.getOtherAET(), command);

                        imebra::NEventReportCommand neventreport = command.getAsNEventReportCommand(); // Convert to neventreport to retrieve neventreport-specific data

                        // Respond with an error
                        dimse.sendCommandOrResponse(imebra::NEventReportResponse(neventreport,
                                                                                 imebra::dimseStatusCode_t::unappropriateActionForInstance));
                    }
                        break;
                    case imebra::dimseCommandType_t::nGet:
                        // Received a NGET command
                        ///////////////////////////////////
                    {
                        outputCommandTags("**** Received NGET command from " + scp.getOtherAET(), command);

                        imebra::NGetCommand nget = command.getAsNGetCommand(); // Convert to nget to retrieve nget-specific data

                        // Respond with an error
                        dimse.sendCommandOrResponse(
                                imebra::NGetResponse(nget, imebra::dimseStatusCode_t::unappropriateActionForInstance));
                    }
                        break;
                    case imebra::dimseCommandType_t::nSet:
                        // Received a NSET command
                        ///////////////////////////////////
                    {
                        outputCommandTags("**** Received NSET command from " + scp.getOtherAET(), command);

                        imebra::NSetCommand nset = command.getAsNSetCommand(); // Convert to nset to retrieve nset-specific data

                        // Respond with an error
                        dimse.sendCommandOrResponse(
                                imebra::NSetResponse(nset, imebra::dimseStatusCode_t::unappropriateActionForInstance));
                    }
                        break;
                    case imebra::dimseCommandType_t::nAction:
                        // Received a NACTION command
                        ///////////////////////////////////
                    {
                        outputCommandTags("**** Received NACTION command from " + scp.getOtherAET(), command);

                        imebra::NActionCommand naction = command.getAsNActionCommand(); // Convert to naction to retrieve naction-specific data

                        // Respond with an error
                        dimse.sendCommandOrResponse(imebra::NActionResponse(naction,
                                                                            imebra::dimseStatusCode_t::unappropriateActionForInstance));
                    }
                        break;
                    case imebra::dimseCommandType_t::nCreate:
                        // Received a NCREATE command
                        ///////////////////////////////////
                    {
                        outputCommandTags("**** Received NCREATE command from " + scp.getOtherAET(), command);

                        imebra::NCreateCommand ncreate = command.getAsNCreateCommand(); // Convert to ncreate to retrieve ncreate-specific data

                        // Respond with an error
                        dimse.sendCommandOrResponse(imebra::NCreateResponse(ncreate,
                                                                            imebra::dimseStatusCode_t::unappropriateActionForInstance));
                    }
                        break;
                    case imebra::dimseCommandType_t::nDelete:
                        // Received a NDELETE command
                        ///////////////////////////////////
                    {
                        outputCommandTags("**** Received NDELETE command from " + scp.getOtherAET(), command);

                        imebra::NDeleteCommand ndelete = command.getAsNDeleteCommand(); // Convert to ndelete to retrieve ndelete-specific data

                        // Respond with an error
                        dimse.sendCommandOrResponse(imebra::NDeleteResponse(ndelete,
                                                                            imebra::dimseStatusCode_t::unappropriateActionForInstance));
                    }
                        break;
                    default:
                        std::wcout << L"Wrong command received" << std::endl;
                }
            }
        }
        catch (const imebra::StreamEOFError &) {
            // The association has been closed during the association
        }
        catch (const std::exception &e) {
            std::wcout << e.what() << std::endl;
        }

        {
            std::lock_guard<std::mutex> lock(lockActiveAssociations);
            activeAssociations.erase(&scp);
        }

    }
    catch (const imebra::StreamEOFError &) {

    }
    catch (const std::exception &e) {
        std::wcout << e.what() << std::endl;
    }
    while(! cPayLoadQueue.empty()){
        imebra::DataSet payloadDs = cPayLoadQueue.front();
        saveToDisk(payloadDs, dcmSaveDirectory);
        cPayLoadQueue.pop_front();
    }
    cPayLoadQueue.clear();
}


///
/// \brief main
///
/// \param argc number of arguments
/// \param argv arguments
/// \return 0
///
//////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
    std::wcout << L"scpExample" << std::endl;

    try {
        // Check the number of received arguments
        if (argc < 4) {
            std::wcout << L"Usage: scpExample port AET  dcmSaveDirectory" << std::endl;
            return 1;
        }

        std::string savedDirectory(argv[3]);
        unsigned long length = savedDirectory.length();
        if (savedDirectory[length - 1] != '/') {
            savedDirectory.append("/");
        }

        if (access(savedDirectory.c_str(), F_OK) != 0) {
            //--- 目录不存在
            std::string createDirectory("mkdir -p ");
            createDirectory.append(savedDirectory);
            std::system(createDirectory.c_str());
        }


        if (access(savedDirectory.c_str(), F_OK | R_OK | W_OK) != 0) {
            std::wcout << "DICOM 文件存放目录： " << argv[3] << " 不存在或是无操作权限" << std::endl;
            return 0;
        }
        std::string port(argv[1]);
        std::string aet(argv[2]);
        initDicomProcessor();

        // Create a listening socket bound to the port in the first argument
        imebra::TCPPassiveAddress listeningAddress("", port);
        imebra::TCPListener listenForConnections(listeningAddress);

        // Listen in a lambda execute in another thread
        std::thread listeningThread(
                [&]() {
                    try {
                        for (;;) {
                            // Blocks until the TCPListener is terminated (throws EOF) or until a connection arrives
                            imebra::TCPStream newTCPStream = listenForConnections.waitForConnection();

                            // Launch a thread that processes the dimse commands on the new connection
                            std::thread dimseCommandsThread(processDimseCommands, newTCPStream, aet , savedDirectory);

                            // We detach the thread, so we can forget about it.
                            dimseCommandsThread.detach();
                        }
                    }
                    catch (const imebra::StreamEOFError &) {
                    }
                    catch (const std::exception &e) {
                        // An error occurred. Print it out.
                        std::wcout << e.what() << std::endl;
                    }

                    // Abort all open associations
                    {
                        std::lock_guard<std::mutex> lock(lockActiveAssociations);
                        for (imebra::AssociationBase *pAssociation: activeAssociations) {
                            pAssociation->abort();
                        }

                    }

                });

        std::wcout << "SCP is listening on port " << port.c_str()
                   << " with AET " << aet.c_str()
                   << " And  DicomFile Saved To :" << savedDirectory.c_str()
                   << std::endl;
        std::wcout << "Press ENTER to exit..." << argv[1] << std::endl;
        getchar();
        // Terminate the listening socket: will cause the listening thread to exit
        listenForConnections.terminate();
        listeningThread.join();
        return 0;

    }
    catch (const std::exception &e) {
        std::wcout << std::endl << "Error: " << std::endl << e.what() << std::endl;
        return 1;
    }
}