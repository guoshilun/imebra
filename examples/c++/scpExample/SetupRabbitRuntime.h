#include <utility>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async_logger.h>
#include <spdlog/async.h>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include "scpConstant.h"
// Created by dhz on 2021/10/25.
//

#ifndef IMEBRA_SETUPRABBITRUNTIME_H
#define IMEBRA_SETUPRABBITRUNTIME_H

static std::vector<DicomTagConverter> modalityConverter;
static std::vector<DicomTagConverter> examedBodyPartConverter;



bool setupSpdlogRuntime() {
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
        spdlog::init_thread_pool(8192, SPDLOG_MAX_THREADS);
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
    return createLog;
}

void fillTableArgument(std::vector<RabbitMqArgument> &args, AMQP::Table &table) {


    for (auto a: args) {
        if (a.type == "num") {
            table[a.key] = std::stoi(a.value);
        } else if (a.type == "bool") {
            table[a.key] = a.value == "true";
        } else {
            table[a.key] = a.value;
        }
    }
}


void createQueue(void *req) {
    uv_loop_t *loop = static_cast<uv_loop_t *>(malloc(sizeof(uv_loop_t)));
    uv_loop_init(loop);

    DicomMessageHandler mqHandler(loop);
    AMQP::Address mqAddres(MQ_ADDRESS);
    AMQP::TcpConnection conn(&mqHandler, mqAddres);
    AMQP::TcpChannel scpChannel(&conn);

    RabbitMqQueueInfo *cqueue = (RabbitMqQueueInfo *) req;

    AMQP::Table qargs;
    fillTableArgument(cqueue->tableArguments, qargs);
    scpChannel.declareQueue(cqueue->name, AMQP::durable, qargs)
            .onSuccess([&](const std::string &name, uint32_t messagecount, uint32_t consumercount) {
                spdlog::info("create Queue: {} success", name);
            }).onError([&](const char *message) {
                spdlog::error("create Queue: {} Failed {}", cqueue->name, message);
            }).onFinalize([&] {
                spdlog::debug("create Queue: {} Over And Clear loop Resourse", cqueue->name);
                uv_stop(loop);
                uv_loop_close(loop);
                free(loop);
            });
    uv_run(loop, UV_RUN_DEFAULT);
}


void createExchage(void *req) {
    uv_loop_t *loop = static_cast<uv_loop_t *>(malloc(sizeof(uv_loop_t)));
    uv_loop_init(loop);

    DicomMessageHandler mqHandler(loop);
    AMQP::Address mqAddres(MQ_ADDRESS);
    AMQP::TcpConnection conn(&mqHandler, mqAddres);
    AMQP::TcpChannel scpChannel(&conn);

    RabbitMqExchangeInfo *exchangeInfo = (RabbitMqExchangeInfo *) req;

    AMQP::ExchangeType chgType = AMQP::ExchangeType::fanout;
    if (exchangeInfo->type == "direct") {
        chgType = AMQP::ExchangeType::direct;
    } else if (exchangeInfo->type == "topic") {
        chgType = AMQP::ExchangeType::topic;
    } else if (exchangeInfo->type == "headers") {
        chgType = AMQP::ExchangeType::headers;
    }

    AMQP::Table chgTable;
    fillTableArgument(exchangeInfo->tableArguments, chgTable);
    scpChannel.declareExchange(exchangeInfo->name, chgType, AMQP::durable, chgTable)
            .onSuccess([&]() {
                spdlog::info("create Exchange:{} with Type:{} Success", exchangeInfo->name, exchangeInfo->type);

            }).onError([&](const char *message) {
                spdlog::error("create Exchange:{} with Type:{} Failed With {}", exchangeInfo->name, exchangeInfo->type,
                              message);
            }).onFinalize([&] {
                spdlog::debug("create Exchange:{} Over And clear loop Resourse:", exchangeInfo->name);

                uv_stop(loop);
                uv_loop_close(loop);
                free(loop);
            });
    uv_run(loop, UV_RUN_DEFAULT);
}


void bindQueue(void *req) {
    uv_loop_t *loop = static_cast<uv_loop_t *>(malloc(sizeof(uv_loop_t)));
    uv_loop_init(loop);

    DicomMessageHandler mqHandler(loop);
    AMQP::Address mqAddres(MQ_ADDRESS);
    AMQP::TcpConnection conn(&mqHandler, mqAddres);
    AMQP::TcpChannel scpChannel(&conn);

    RabbitMqBindQueueInfo *bindingInfo = (RabbitMqBindQueueInfo *) req;

    AMQP::Table bindingTable;

    fillTableArgument(bindingInfo->tableArguments, bindingTable);


    scpChannel.bindQueue(bindingInfo->exchange, bindingInfo->queuename, bindingInfo->routingkey, bindingTable)
            .onSuccess([&]() {
                spdlog::info("bindQueue:{} to {} with routingKey {}  Success ", bindingInfo->queuename,
                             bindingInfo->exchange, bindingInfo->routingkey);
            })
            .onError([&](const char *msg) {
                spdlog::error("bindQueue:{} to {}  :{} ", bindingInfo->queuename, bindingInfo->exchange, msg);
            }).onFinalize([&]() {
                spdlog::debug("bindQueue:{} to {}  Over And clear loop Resourse:", bindingInfo->queuename,
                              bindingInfo->exchange);

                uv_stop(loop);
                uv_loop_close(loop);
                free(loop);
            });

    uv_run(loop, UV_RUN_DEFAULT);
}


void bindExchage(void *req) {
    uv_loop_t *loop = static_cast<uv_loop_t *>(malloc(sizeof(uv_loop_t)));
    uv_loop_init(loop);

    DicomMessageHandler mqHandler(loop);
    AMQP::Address mqAddres(MQ_ADDRESS);
    AMQP::TcpConnection conn(&mqHandler, mqAddres);
    AMQP::TcpChannel scpChannel(&conn);

    RabbitMqBindExchangeInfo *exchangeMap = (RabbitMqBindExchangeInfo *) req;


    scpChannel.bindExchange(exchangeMap->from, exchangeMap->to, exchangeMap->routingkey)
            .onSuccess([&]() {
                spdlog::info("bindExchange :{}->{} with Key:{} Success", exchangeMap->from, exchangeMap->to,
                             exchangeMap->routingkey);

            }).onError([&](const char *message) {
                spdlog::error("bindExchange :{}->{} with Key:{} Success Failed With {}", exchangeMap->from,
                              exchangeMap->to, exchangeMap->routingkey,
                              message);
            }).onFinalize([&] {
                spdlog::debug("bindExchange :{}->{} with Key:{} Over And ClearResources :", exchangeMap->from,
                              exchangeMap->to, exchangeMap->routingkey);

                uv_stop(loop);
                uv_loop_close(loop);
                free(loop);
            });
    uv_run(loop, UV_RUN_DEFAULT);
}


std::string findMatchedModality(std::string cmod) {
    bool find = false;
    std::string result;
    transform(cmod.begin(), cmod.end(), cmod.begin(), ::toupper);
    for (auto modality: modalityConverter) {
        std::vector<std::string>::iterator iter = std::find(modality.values.begin(), modality.values.end(), cmod);
        if (iter != modality.values.end()) {
            result = modality.key;
            find = true;
            break;
        }
    }
    return find ? result : cmod;
}


void setupRabbitDispatcher() {

    /// 查找符合要求的设备类型

    std::string utf8 = u8"从当前目录./config.yaml 读取配置文件";
    spdlog::debug("setupRabbitRuntime  Begin");
    spdlog::info( utf8.c_str());
    YAML::Node config = YAML::LoadFile("./config.yaml");
    messagePubExchange = config["MessagePub"]["exchange"].as<std::string>();
    messagePubRoutingKey = config["MessagePub"]["routingKey"].as<std::string>();

    if (config["ModalityConverter"]) {
        modalityConverter = config["ModalityConverter"].as<std::vector<DicomTagConverter>>();
        for (auto ct: modalityConverter) {
            std::string result = join(ct.values.begin(), ct.values.end(), ",");
            spdlog::info("ModalityConverter: {}=>{}", result, ct.key);

            for (const auto &modality: ct.values) {

                std::string mgx(modality.c_str());
                transform(mgx.begin(), mgx.end(), mgx.begin(), ::toupper);
                mapModality[mgx] = ct.key;
            }
        }


    }
    if (config["BodyPartConverter"]) {
        examedBodyPartConverter = config["BodyPartConverter"].as<std::vector<DicomTagConverter>>();
        for (auto bodypart: examedBodyPartConverter) {
            std::string result = join(bodypart.values.begin(), bodypart.values.end(), ",");
            spdlog::info("BodyPartConverter: {}=>{}", result, bodypart.key);

            for (const auto &body: bodypart.values) {

                std::string mgx(body.c_str());
                transform(mgx.begin(), mgx.end(), mgx.begin(), ::toupper);

                // std::string tx = std::toupper(body, std::locale("zh_CN.utf8"));
                mapBodyPart[mgx] = bodypart.key;
            }

        }
    }

    //BodyPartConverter

    spdlog::info("MessagePublish Exchange={},RoutingKey={}", messagePubExchange, messagePubRoutingKey);

    {


        std::vector<RabbitMqQueueInfo> vi = config["Queues"].as<std::vector<RabbitMqQueueInfo>>();
        std::list<uv_thread_t> threads;
        size_t vsize = vi.size();
        for (size_t index = 0; index < vsize; index++) {
            uv_thread_t ct;
            uv_thread_create(&ct, createQueue, &vi[index]);
            threads.push_back(ct);
        }
        for (auto t: threads) {
            uv_thread_join(&t);
        }
    }

    {
        std::vector<RabbitMqExchangeInfo> exchanges = config["Exchanges"].as<std::vector<RabbitMqExchangeInfo>>();
        std::list<uv_thread_t> exchangeThreads;
        size_t chgSize = exchanges.size();
        for (size_t index = 0; index < chgSize; index++) {
            uv_thread_t ct;
            uv_thread_create(&ct, createExchage, &exchanges[index]);
            exchangeThreads.push_back(ct);
        }
        for (auto t: exchangeThreads) {
            uv_thread_join(&t);
        }
    }


    {
        std::vector<RabbitMqBindQueueInfo> bindings = config["BindQueue"].as<std::vector<RabbitMqBindQueueInfo>>();
        std::list<uv_thread_t> bindingsThreads;
        size_t chgSize = bindings.size();
        for (size_t index = 0; index < chgSize; index++) {
            uv_thread_t ct;
            uv_thread_create(&ct, bindQueue, &bindings[index]);
            bindingsThreads.push_back(ct);
        }
        for (auto t: bindingsThreads) {
            uv_thread_join(&t);
        }
    }


    if(config["BindExchange"])
    {
        std::vector<RabbitMqBindExchangeInfo> bindExchange = config["BindExchange"].as<std::vector<RabbitMqBindExchangeInfo>>();
        std::list<uv_thread_t> bindExchangeThreads;
        size_t chgSize = bindExchange.size();
        for (size_t index = 0; index < chgSize; index++) {
            uv_thread_t ct;
            uv_thread_create(&ct, bindExchage, &bindExchange[index]);
            bindExchangeThreads.push_back(ct);
        }
        for (auto t: bindExchangeThreads) {
            uv_thread_join(&t);
        }
    }
    spdlog::flush_on( spdlog::level::info);

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
        std::shared_ptr<AMQP::Envelope> envelope = cmsg.createMessage(mapModality, mapBodyPart);
        channel.publish(messagePubExchange, messagePubRoutingKey, *envelope.get());
    }
    channel.commitTransaction().onSuccess([&dicomMessages]() {
        spdlog::info("Commit Messages：{}  with {}", dicomMessages.size(), SUCCESS);
    }).onError([&dicomMessages, &channel](const char *msg) {
        spdlog::error("Commit Messages：{} with {}", dicomMessages.size(), msg);
        channel.rollbackTransaction();
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
