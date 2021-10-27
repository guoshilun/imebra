#include <utility>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async_logger.h>
#include <spdlog/async.h>
#include "scpConstant.h"
//
// Created by dhz on 2021/10/25.
//

#ifndef IMEBRA_SETUPRABBITRUNTIME_H
#define IMEBRA_SETUPRABBITRUNTIME_H



bool  setupSpdlogRuntime(){
    bool createLog(false);
    try {
        //////////////////////////////////////////////////
        ///  spdlog 参考：http://www.mianshigee.com/project/spdlog
        ///  低于设置级别的日志将不会被输出。各level排序，数值越大级别越高：
        ///    trace =  0,
        ///    debug =1,
        ///    info = 2,
        ///    warn = 3,
        ///    err = 4,
        ///    critical = 5,
        ///    off = 6,
        ///    n_levels=7
        //////////////////////////////////////////////////
        spdlog::init_thread_pool(8192, 1);
        ///---------------------滚动日志


        ///--------------全部输出
        auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        stdout_sink->set_level(spdlog::level::debug);
        stdout_sink->set_pattern("[%^%l%$] %v");

        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/info.txt",
                                                                                    SPDLOG_MAX_SIZE_SINGLE_FILE,
                                                                                    SPDLOG_MAX_ROATING_FILES);
        rotating_sink->set_level(spdlog::level::info);
        rotating_sink->set_pattern("[%H:%M:%S] [%^-%L-%$] %v");


        std::vector<spdlog::sink_ptr> sinks{stdout_sink, rotating_sink};
        auto defLogger = std::make_shared<spdlog::async_logger>(SCP_LOGGER_NAME, sinks.begin(), sinks.end(),
                                                                spdlog::thread_pool(),
                                                                spdlog::async_overflow_policy::block);
        spdlog::register_logger(defLogger);


//            spdlog::logger logger("multi_sink", {stdout_sink, rotating_sink});

//           std::shared_ptr<spdlog::logger> defLogger = std::make_shared<spdlog::logger>(logger);
        spdlog::set_level(spdlog::level::trace); // Set global log level to debug
        spdlog::set_default_logger(defLogger);
        //---每隔60秒刷新一下日志
        spdlog::flush_every(std::chrono::seconds(60));
        spdlog::set_error_handler(
                [](const std::string &msg) { spdlog::get("console")->error("*** LOGGER ERROR ***: {}", msg); });


        createLog = true;
    }
    catch (const spdlog::spdlog_ex &ex) {
        std::wcout << L"Log init failed: " << ex.what() << std::endl;
    }
    return  createLog;
}



void setupRabbitRuntime() {

    spdlog::debug("setupRabbitRuntime  Begin");
    uv_loop_t *loop = static_cast<uv_loop_t *>(malloc(sizeof(uv_loop_t)));
    uv_loop_init(loop);


    DicomMessageHandler mqHandler(loop);
    AMQP::Address mqAddres(MQ_ADDRESS);
    AMQP::TcpConnection mqConn(&mqHandler, mqAddres);
    AMQP::TcpChannel deadChannel(&mqConn);


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
                    spdlog::info("create Exchange {} {}  and x-message-ttl={} ", DEAD_EXCAHGE, SUCCESS , 1 * ONE_DAY );
                })
                .onError([&](const char *message) {
                    spdlog::error("create Exchange {} {}:{}", MQ_EXCHANG, FAILED, message);
                });

        mqPtr.declareQueue(MQ_QUEUE, AMQP::durable, arguments)
                .onSuccess([&](const std::string &name, uint32_t messagecount, uint32_t consumercount) {
                    spdlog::info("create Queue {} {}", name, SUCCESS);
                })
                .onError([&](const char *message) {
                    spdlog::error("create Queue {} {}:{}", MQ_QUEUE, FAILED, message);
                });

        mqPtr.bindQueue(MQ_EXCHANG, MQ_QUEUE, MQ_ROUTING_KEY).onSuccess([&]() {

            spdlog::info("bindQueue  {} TO {} {}", MQ_QUEUE, MQ_EXCHANG, SUCCESS);

        }).onError([&](const char *message) {
            spdlog::error("bindQueue  {} TO {} {}:{}", MQ_QUEUE, MQ_EXCHANG, FAILED, message);
        }).onFinalize([&]() {
            spdlog::debug(" Create Dicom Exchange And Queue End");
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
                spdlog::info("create Exchange {} {}  and x-message-ttl={}", DEAD_EXCAHGE, SUCCESS , 7* ONE_DAY );
            })
            .onError([&](const char *message) {
                //something went wrong creating the exchange
                spdlog::error("create Exchange {} {}:{}", DEAD_EXCAHGE,FAILED, message);
            });
    deadChannel.declareQueue(DEAD_QUEUE, AMQP::durable, deadArguments)
            .onSuccess([&](const std::string &name, uint32_t messagecount, uint32_t consumercount) {
                spdlog::info("create Queue {} {}", DEAD_QUEUE, SUCCESS);
            })
            .onError([&](const char *message) {
                spdlog::error("create Queue {} {}:{}", DEAD_QUEUE,FAILED, message);
            });
    deadChannel.bindQueue(DEAD_EXCAHGE, DEAD_QUEUE, DEAD_ROUTING_KEY)
            .onSuccess([&]() {
                spdlog::info("bindQueue   {} {} {}", DEAD_QUEUE, DEAD_EXCAHGE,  SUCCESS);
            })
            .onError([&](const char *message) {
                // none of the messages were published
                // now we have to do it all over again
                spdlog::error("bindQueue   {} {} {}:{}", DEAD_QUEUE, DEAD_EXCAHGE,  FAILED,message);
                std::call_once(onceFlag, uv_stop, loop);
            }).onFinalize([&]() {
                spdlog::debug("Dead TcpChannel and Create Dicom Exchange And Queue Begin");
                createDicomExchangeAndQueue();
            });

    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    free(loop);
    spdlog::debug("setupRabbitRuntime  End ！");

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
        spdlog::info("Commit Messages：{}  with {}", dicomMessages.size(), SUCCESS);
    }).onError([&dicomMessages, &channel](const char *msg) {
        spdlog::error("Commit Messages：{} with {}", dicomMessages.size(), msg);
        channel.rollbackTransaction();
        try {
            auto logger = spdlog::rotating_logger_mt("mq", "mq/message.txt", SPDLOG_MAX_SIZE_SINGLE_FILE, SPDLOG_MAX_ROATING_FILES);
            logger->set_pattern("%v");
            logger->set_level(spdlog::level::level_enum::info);
            for (DcmInfo cmsg: dicomMessages) {
                logger->info("pid={},suid={},ssuid={},sopuid={},exam={},mod={},thick={}",
                             cmsg.getPatientId(), cmsg.getStudyUid(), cmsg.getSeriesUid(), cmsg.getSopInstUid(),
                             cmsg.getExamPart(), cmsg.getModality(), cmsg.getThickness());
            }
            logger->flush();
        } catch (const spdlog::spdlog_ex &) {
        }

    }).onFinalize([&channel, &dicomMessages, &cLoop]() {
        spdlog::debug("Commit Messages OverClear Resource");
        dicomMessages.clear();
        channel.close();
        uv_stop(cLoop);
    });
    uv_run(cLoop, UV_RUN_DEFAULT);
    uv_loop_close(cLoop);
    free(cLoop);
    spdlog::debug("onMessageCallback execute Over !");
}


#endif //IMEBRA_SETUPRABBITRUNTIME_H
