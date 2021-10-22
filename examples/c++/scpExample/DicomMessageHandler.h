//
// Created by dhz on 2021/10/19.
//

#ifndef IMEBRA_DICOMMESSAGEHANDLER_H
#define IMEBRA_DICOMMESSAGEHANDLER_H

#include <amqpcpp.h>
#include <amqpcpp/linux_tcp.h>
#include <amqpcpp/libuv.h>

class DicomMessageHandler : public AMQP::LibUvHandler
{
private:
    /**
     *  Method that is called when a connection error occurs
     *  @param  connection
     *  @param  message
     */
    virtual void onError(AMQP::TcpConnection *connection, const char *message) override
    {
        std::wcout <<  L"Connect To RabbitMQ Server Error："  << message << std::endl;
    }

    /**
     *  Method that is called when the TCP connection ends up in a connected state
     *  @param  connection  The TCP connection
     */
    virtual void onConnected(AMQP::TcpConnection *connection) override
    {
        std::wcout << L"Connect To RabbitMQ Server  Sucess " << std::endl;
    }



public:
    /**
     *  Constructor
     *  @param  uv_loop
     */
    DicomMessageHandler(uv_loop_t *loop) : AMQP::LibUvHandler(loop) {}

    /**
     *  Destructor
     */
    virtual ~DicomMessageHandler() = default;
};


#endif //IMEBRA_DICOMMESSAGEHANDLER_H
