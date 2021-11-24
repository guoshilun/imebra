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
#include <list>

#include <imebra/imebra.h>

#include <cstdlib>

#include  <unistd.h>
#include <condition_variable>
#include "scpDefine.h"
#include "RuntimeConfig.h"
#include <yaml-cpp/node/parse.h>
#include <fstream>
// When an association is created then its address is inserted
// in the set below, when it is destroyed it is removed from the set.
// When the app must terminate then we abort all the active associations.
#ifndef  SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE//必须定义这个宏,才能输出文件名和行号
#endif

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
    std::locale lc("zh_CN.UTF-8");
    std::locale::global(lc);
    std::wcout.imbue(lc);

    try {
        // Check the number of received arguments
        if (argc < 6) {
            std::wcout << L"Usage: scpExample port AET  dcmSaveDirectory  dcmLogDir  dcmConfigYamlFilePath" << std::endl;
            return 1;
        }

        std::string savedDirectory(argv[3]);
        std::string logDirectory(argv[4]);
        std::string configPath (argv[5]);

        unsigned long length = savedDirectory.length();
        if (savedDirectory[length - 1] != '/') {
            savedDirectory.append("/");
        }

        if (access(savedDirectory.c_str(), F_OK | R_OK | W_OK) != 0) {
            std::wcout << "DICOM Saved Directory： " << argv[3] << " Not Exists  or  Deny Access !" << std::endl;
            return 0;
        }

        if (access(logDirectory.c_str(), F_OK | R_OK | W_OK) != 0) {
            std::wcout << "DICOM Log  Directory： " << argv[4] << " Not Exists  or  Deny Access !" << std::endl;
            return 0;
        }

        if (access(configPath.c_str(), F_OK | R_OK ) != 0) {
            std::wcout << "DICOM Config.yaml Directory： " << argv[5] << " Not Exists  or  Deny Access !" << std::endl;
            return 0;
        }


        RuntimeConfig  runtimeConfig(savedDirectory , logDirectory , configPath);
        std::shared_ptr<RuntimeConfig>  configPtr = std::make_shared<RuntimeConfig>( runtimeConfig);


        bool createLog = configPtr.get()->setupSpdlogRuntime();
        if (!createLog) {
            return 0;
        }
        configPtr.get()->setupRabbitDispatcher();

        std::mutex cv_m;
        std::unique_lock<std::mutex> lk(cv_m);
        std::condition_variable cv;
        std::string port(argv[1]);
        std::string aet(argv[2]);


        setupDicomContexts(presentationContexts);


        // Create a listening socket bound to the port in the first argument
        imebra::TCPPassiveAddress listeningAddress("", port);
        imebra::TCPListener listenForConnections(listeningAddress);



        // Listen in a lambda execute in another thread
        std::thread listeningThread(
                [&listenForConnections, &aet, &savedDirectory,&configPtr]() {
                    std::once_flag  connectionFlag;
                    try {
                        for (;;) {



                            // Blocks until the TCPListener is terminated (throws EOF) or until a connection arrives
                            imebra::TCPStream newTCPStream = listenForConnections.waitForConnection();
                            // Launch a thread that processes the dimse commands on the new connection
                            std::thread commandThread(dimseCommands, newTCPStream, aet, savedDirectory, configPtr.get());

                            // We detach the thread, so we can forget about it.
                            commandThread.detach();
                        }
                    }
                    catch (const imebra::StreamEOFError &) {
                    }
                    catch (const std::exception &e) {
                        // An error occurred. Print it out.
                        spdlog::error( "listeningThread 运行时错误:{}" ,e.what());
                        std::wcout << e.what() << std::endl;
                    }

                    std::call_once(connectionFlag,[&]() {   // Abort all open associations
                        std::lock_guard<std::mutex> lock(lockActiveAssociations);
                        for (imebra::AssociationBase *pAssociation: activeAssociations) {
                            pAssociation->abort();
                        }
                    });

                });


        //  loopThrea.join();
        spdlog::info("DicomCStoreSCP Service  is listening on {}@{}, File Save To:{},Log Save To:{},Config  From:{}",
                     port, aet,
                     savedDirectory,logDirectory, configPath);

        cv.wait(lk, [] { return false; });

        // Terminate the listening socket: will cause the listening thread to exit
        listenForConnections.terminate();
        listeningThread.join();
        spdlog::drop_all();
        return 0;

    }
    catch (const std::exception &e) {
        spdlog::error(e.what());
        return 1;
    }

}