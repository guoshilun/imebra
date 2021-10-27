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

#include <imebra/imebra.h>

#include <cstdlib>

#include  <unistd.h>
#include <condition_variable>
#include "scpDefine.h"
#include "SetupRabbitRuntime.h"


// When an association is created then its address is inserted
// in the set below, when it is destroyed it is removed from the set.
// When the app must terminate then we abort all the active associations.

using namespace std;

///
/// \brief main
///
/// \param argc number of arguments
/// \param argv arguments
/// \return 0
///
//////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
    std::ios::sync_with_stdio(false);
    std::wcout.imbue(std::locale(""));


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
            std::wcout << "DICOM Saved Directory： " << argv[3] << " Not Exists  or  Deny Access !" << std::endl;
            return 0;
        }



        bool createLog= setupSpdlogRuntime();
        if (!createLog) {
            return 0;
        }

        std::mutex cv_m;
        std::unique_lock<std::mutex> lk(cv_m);
        std::condition_variable cv;
        std::string port(argv[1]);
        std::string aet(argv[2]);
        setupDicomContexts(presentationContexts);


        std::thread loopThrea([&]() {
            setupRabbitRuntime();
            spdlog::debug("Init RabbitMQ  Exchange And  Queue  Over ");
            //std::wcout << L"Init RabbitMQ  Exchange And  Queue  Over !" << std::endl;
        });

        loopThrea.join();
        // Create a listening socket bound to the port in the first argument
        imebra::TCPPassiveAddress listeningAddress("", port);
        imebra::TCPListener listenForConnections(listeningAddress);


        // Listen in a lambda execute in another thread
        std::thread listeningThread(
                [&listenForConnections, &aet, &savedDirectory]() {
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


        //  loopThrea.join();
        spdlog::info("DicomCStoreSCP Service  is listening on {}@{}, Storage Directory is:{}", port, aet,
                     savedDirectory);

        cv.wait(lk, [] { return false; });

        // Terminate the listening socket: will cause the listening thread to exit
        listenForConnections.terminate();
        listeningThread.join();

        return 0;

    }
    catch (const std::exception &e) {
        spdlog::error(e.what());
        return 1;
    }

}