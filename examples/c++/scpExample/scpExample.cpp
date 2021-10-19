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
#include <condition_variable>
#include "uv.h"
#include "scpDefine.h"


// When an association is created then its address is inserted
// in the set below, when it is destroyed it is removed from the set.
// When the app must terminate then we abort all the active associations.
static std::set<imebra::AssociationBase *> activeAssociations;
static std::mutex lockActiveAssociations; // Lock the access to the associations set.
static imebra::PresentationContexts presentationContexts;

///
/// \brief When a DIMSE connection is received then this method is executed in a
///        separate thread and continues until the connection is closed, either
///        on the remote or on the local side.
///
/// \param tcpStream the tcp stream for the TCP connection
/// \param aet       the SCP aet to communicate during the ACSE negotiation
///
//////////////////////////////////////////////////////////////////////////////////////
void dimseCommands(imebra::TCPStream tcpStream, std::string aet, std::string dcmSaveDirectory) {

    std::list<imebra::DataSet> cPayLoadQueue;
    std::set<std::string> cPayDires;

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

                        imebra::CStoreCommand cstore = command.getAsCStoreCommand(); // Convert to cstore to retrieve cstore-specific data
                        imebra::DataSet payload = cstore.getPayloadDataSet();
                        cPayLoadQueue.push_back(payload);
                        dimse.sendCommandOrResponse(imebra::CStoreResponse(cstore, imebra::dimseStatusCode_t::success));
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
                        std::wcout << L"Wrong command received" << std::endl;
                    }
                        break;

                }
            }
        }
        catch (const imebra::StreamEOFError &) {
            // The association has been closed during the association
        }
        catch (const std::exception &e) {
            std::wcout << L"错误：" << e.what() << std::endl;
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

    while (!cPayLoadQueue.empty()) {
        imebra::DataSet payloadDs = cPayLoadQueue.front();
        onCStoreCallback(payloadDs, dcmSaveDirectory);
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

        std::mutex cv_m;
        std::unique_lock<std::mutex> lk(cv_m);
        std::condition_variable cv;
        std::string port(argv[1]);
        std::string aet(argv[2]);
        initDicomProcessor(presentationContexts);

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
                            std::thread commandThread(dimseCommands, newTCPStream, aet, savedDirectory);

                            // We detach the thread, so we can forget about it.
                            commandThread.detach();
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

        cv.wait(lk, [] { return false; });

        // Terminate the listening socket: will cause the listening thread to exit
        listenForConnections.terminate();
        listeningThread.join();
        return 0;

    }
    catch (const std::exception &e) {
        std::wcout << L"Error: " << e.what() << std::endl;
        return 1;
    }
}