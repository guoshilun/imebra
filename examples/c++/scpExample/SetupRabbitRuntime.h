#include <utility>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
//
// Created by dhz on 2021/10/25.
//

#ifndef IMEBRA_SETUPRABBITRUNTIME_H
#define IMEBRA_SETUPRABBITRUNTIME_H


const char *MQ_ADDRESS = "amqp://root:root@localhost/imebra";

const char *MQ_EXCHANG = "exchange.imebra";
const char *MQ_QUEUE = "queue.imebra";
const char *MQ_ROUTING_KEY = "DICOM";


const char *DEAD_EXCAHGE = "exchange.imebra.dead";
const char *DEAD_QUEUE = "queue.imebra.dead";
const char *DEAD_ROUTING_KEY = "DICOM.dead";


const char *SUCCESS(" Success");
const char *FAILED(" Failed:");
const char *CreateExchange("create Exchange :");
const char *CreateQueue("create Queue :");
const char *BindQueue("bind Queue :");
const char *ClearResource("clear Resource ");


// 1分钟
const size_t ONE_MINUTE = 60 * 1000;

const size_t ONE_HOUR = 60 * ONE_MINUTE;

const size_t ONE_DAY = 24 * ONE_HOUR;

// 5M
const int max_size = 1048576 * 5;
// 30
const int max_files = 300;

void setupRabbitRuntime() {

    std::wcout << L"setupRabbitRuntime  Begin ！" << std::endl;
    uv_loop_t *loop = static_cast<uv_loop_t *>(malloc(sizeof(uv_loop_t)));
    uv_loop_init(loop);


    DicomMessageHandler mqHandler(loop);
    AMQP::Address mqAddres(MQ_ADDRESS);
    AMQP::TcpConnection mqConn(&mqHandler, mqAddres);
    AMQP::TcpChannel deadChannel(&mqConn);


//    AMQP::Table arguments;
//    arguments["x-dead-letter-exchange"] = DEAD_EXCAHGE;
//    arguments["x-dead-letter-routing-key"] = DEAD_ROUTING_KEY;
//    //  消息过期时间为60 分钟
//    arguments["x-message-ttl"] =  1 * ONE_DAY;

    //////////////////////////////////////////////////
    //////创建DICOM 收图消息队列
    //////////////////////////////////////////////////////
    std::once_flag onceFlag;
    auto createDicomExchangeAndQueue = [&] {

        AMQP::TcpChannel mqPtr(&mqConn);
        AMQP::Table arguments;
        arguments["x-dead-letter-exchange"] = DEAD_EXCAHGE;
        arguments["x-dead-letter-routing-key"] = DEAD_ROUTING_KEY;
        //  消息过期时间为1天 分钟
        arguments["x-message-ttl"] = 1 * ONE_DAY;
        mqPtr.declareExchange(MQ_EXCHANG, AMQP::fanout, AMQP::durable)
                .onSuccess([&]() {
                    //by now the exchange is created
                    std::wcout << CreateExchange << MQ_EXCHANG << SUCCESS << std::endl;
                })
                .onError([&](const char *message) {
                    //something went wrong creating the exchange
                    std::wcout << CreateExchange << MQ_EXCHANG << FAILED << message << std::endl;
                });

        mqPtr.declareQueue(MQ_QUEUE, AMQP::durable, arguments)
                .onSuccess([&](const std::string &name, uint32_t messagecount, uint32_t consumercount) {
                    std::wcout << CreateQueue << name.c_str() << SUCCESS << std::endl;
                })
                .onError([&](const char *message) {
                    // none of the messages were published
                    // now we have to do it all over again
                    std::wcout << CreateQueue << MQ_QUEUE << FAILED << message << std::endl;
                });
        mqPtr.bindQueue(MQ_EXCHANG, MQ_QUEUE, MQ_ROUTING_KEY).onSuccess([&]() {
            std::wcout << BindQueue << MQ_QUEUE << " TO:" << MQ_EXCHANG << SUCCESS << std::endl;

        }).onError([&](const char *message) {
            std::wcout << BindQueue << MQ_QUEUE << " TO:" << MQ_EXCHANG << FAILED << message << std::endl;
        }).onFinalize([&]() {
            std::wcout << L"Dicom TcpChannel " << ClearResource << std::endl;
            std::wcout << L"Create Dicom Exchange And Queue End" << std::endl;

            std::call_once(onceFlag, uv_stop, loop);
        });

    };






    //-----------创建相关的消息队列
    ///-----------------创建死信消息队列
    AMQP::Table deadArguments;
    // 在声明队列的时候可以通过 x-message-ttl 属性来控制消息的TTL， 这个参数的单位是毫秒
    //  指定消息过期时间（毫秒），消息过期后自动删除死信消息，防止把磁盘用完！
    //  1小时过期：
    deadArguments["x-message-ttl"] = 7 * ONE_DAY;
    deadChannel.declareExchange(DEAD_EXCAHGE, AMQP::fanout, AMQP::durable)
            .onSuccess([&]() {
                //by now the exchange is created
                std::wcout << CreateExchange << DEAD_EXCAHGE << SUCCESS << std::endl;
            })
            .onError([&](const char *message) {
                //something went wrong creating the exchange
                std::wcout << CreateExchange << DEAD_EXCAHGE << FAILED << message << std::endl;
            });
    deadChannel.declareQueue(DEAD_QUEUE, AMQP::durable, deadArguments)
            .onSuccess([&](const std::string &name, uint32_t messagecount, uint32_t consumercount) {
                std::wcout << CreateQueue << name.c_str() << SUCCESS << std::endl;
            })
            .onError([&](const char *message) {
                // none of the messages were published
                // now we have to do it all over again
                std::wcout << CreateQueue << DEAD_QUEUE << FAILED << message << std::endl;
            });
    deadChannel.bindQueue(DEAD_EXCAHGE, DEAD_QUEUE, DEAD_ROUTING_KEY)
            .onSuccess([&]() {
                std::wcout << BindQueue << DEAD_QUEUE << " TO:" << DEAD_EXCAHGE << SUCCESS << std::endl;
            })
            .onError([&](const char *message) {
                // none of the messages were published
                // now we have to do it all over again
                std::wcout << BindQueue << DEAD_QUEUE << " TO:" << DEAD_EXCAHGE << FAILED << message << std::endl;
                std::call_once(onceFlag, uv_stop, loop);

            }).onFinalize([&]() {
                std::wcout << L"Dead TcpChannel " << ClearResource << std::endl;
                std::wcout << L"Create Dicom Exchange And Queue Begin" << std::endl;
                createDicomExchangeAndQueue();
            });
    // std::call_once(onceFlag, wating);
    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    free(loop);
    std::wcout << L"setupRabbitRuntime  End ！" << std::endl;

}

void onMessageCallback(std::set<DcmInfo> &dicomMessages) {
    uv_loop_t *cLoop = static_cast<uv_loop_t *>(malloc(sizeof(uv_loop_t)));
    uv_loop_init(cLoop);
    DicomMessageHandler jpHandler(cLoop);
    AMQP::Address jpAddr(MQ_ADDRESS);
    AMQP::TcpConnection jpConn(&jpHandler, jpAddr);
    AMQP::TcpChannel channel(&jpConn);
    channel.startTransaction();
    for (DcmInfo cmsg: dicomMessages) {
        std::shared_ptr<AMQP::Envelope> envelope = cmsg.createMessage();
        channel.publish(MQ_EXCHANG, MQ_ROUTING_KEY, *envelope.get());
    }
    channel.commitTransaction().onSuccess([&dicomMessages]() {
        spdlog::info("Commit   Rabbit Messages：{}  with {}", dicomMessages.size(), SUCCESS);
    }).onError([&dicomMessages, &channel](const char *msg) {
        spdlog::error("Commit   Rabbit Messages：{} with {}", dicomMessages.size(), msg);
        channel.rollbackTransaction();
        try {
            auto logger = spdlog::rotating_logger_mt("mq", "mq/message.txt", max_size, max_files);
            logger->set_pattern("%v");
            logger->set_level(spdlog::level::level_enum::info);
            for (DcmInfo cmsg: dicomMessages) {
                std::shared_ptr<AMQP::Envelope> envelope = cmsg.createMessage();
                logger->info("pid={},suid={},ssuid={},sopuid={}", cmsg.getPatientId(), cmsg.getStudyUid(),
                             cmsg.getSeriesUid(), cmsg.getSopInstUid());
            }
            logger->flush();
        } catch (const spdlog::spdlog_ex &) {
        }


    }).onFinalize([&channel, &dicomMessages, &cLoop]() {
        spdlog::warn("Commit   Rabbit Messages  Over ，Clear Resource ");
        dicomMessages.clear();
        channel.close();
        uv_stop(cLoop);
    });
    uv_run(cLoop, UV_RUN_DEFAULT);
    uv_loop_close(cLoop);
    free(cLoop);

}


#endif //IMEBRA_SETUPRABBITRUNTIME_H
