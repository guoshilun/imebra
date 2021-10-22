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
#include <fcntl.h>


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
    std::wcout.imbue( std::locale("") );


    try {
        // Check the number of received arguments
        if (argc < 4) {
            std::wcout << L"用法: scpExample port AET  dcmSaveDirectory" << std::endl;
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
        setupDicomContexts(presentationContexts);


        std::thread loopThrea([&]() {
            uv_loop_t *loop = static_cast<uv_loop_t *>(malloc(sizeof(uv_loop_t)));
            uv_loop_init(loop);
            setupRabbitRuntime(loop);
            uv_loop_close(loop);
            free(loop);
            std::wcout << L"初始化消息队列完毕" << std::endl;
        });

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


        loopThrea.join();
        std::wcout << L"DicomCStoreSCP 服务 is listening on port: " << port.c_str()
                   << L", AE： " << aet.c_str()
                   << L", Save To：" << savedDirectory.c_str()
                   << std::endl;
        cv.wait(lk, [] { return false; });

        // Terminate the listening socket: will cause the listening thread to exit
        listenForConnections.terminate();
        listeningThread.join();
        return 0;

    }
    catch (const std::exception &e) {
        std::wcout << L"出现错误: " << e.what() << std::endl;

        return 1;
    }

}