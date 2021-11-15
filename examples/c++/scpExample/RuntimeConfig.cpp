//
// Created by dhz on 2021/11/1.
//

#include "RuntimeConfig.h"
#include "DicomMessageHandler.h"
#include "scpConstant.h"
#include <utility>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async_logger.h>
#include <spdlog/async.h>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <zlib.h>

RuntimeConfig::RuntimeConfig() {

}

RuntimeConfig::~RuntimeConfig() {
    modalityConverter.empty();
    examedBodyPartConverter.empty();
}

RuntimeConfig::RuntimeConfig(const RuntimeConfig &other) {
    for (auto ax: other.modalityConverter) {
        modalityConverter.push_back(ax);
    }

    for (auto ab: other.examedBodyPartConverter) {
        examedBodyPartConverter.push_back(ab);
    }
}


void RuntimeConfig::fillTableArgument(std::vector<RabbitMqArgument> &args, AMQP::Table &table) {
    if (args.empty()) {
        return;
    }
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

void RuntimeConfig::createQueue(void *req) {
    uv_loop_t *loop = static_cast<uv_loop_t *>(malloc(sizeof(uv_loop_t)));
    uv_loop_init(loop);
    DicomMessageHandler mqHandler(loop);
    AMQP::Address mqAddres(ScpConstant::MQ_ADDRESS);
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

            });
    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    free(loop);
}

void RuntimeConfig::createExchage(void *req) {
    uv_loop_t *loop = static_cast<uv_loop_t *>(malloc(sizeof(uv_loop_t)));
    uv_loop_init(loop);

    DicomMessageHandler mqHandler(loop);
    AMQP::Address mqAddres(ScpConstant::MQ_ADDRESS);
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

            });
    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    free(loop);
}

void RuntimeConfig::bindQueue(void *req) {
    uv_loop_t *loop = static_cast<uv_loop_t *>(malloc(sizeof(uv_loop_t)));
    uv_loop_init(loop);

    DicomMessageHandler mqHandler(loop);
    AMQP::Address mqAddres(ScpConstant::MQ_ADDRESS);
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

            });

    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    free(loop);
}

void RuntimeConfig::bindExchage(void *req) {
    uv_loop_t *loop = static_cast<uv_loop_t *>(malloc(sizeof(uv_loop_t)));
    uv_loop_init(loop);

    DicomMessageHandler mqHandler(loop);
    AMQP::Address mqAddres(ScpConstant::MQ_ADDRESS);
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

            });
    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    free(loop);
}

void RuntimeConfig::perfermFileStorek(std::set<DcmInfo> &messages, imebra::DataSet &payload, std::string &dcmStoreDir) {
    DcmInfo dcmInfo(payload);

    std::string patientId = dcmInfo.getPatientId();
    std::string studyUid = dcmInfo.getStudyUid();

    std::string seriesUid = dcmInfo.getSeriesUid();
    std::string sopInstUid = dcmInfo.getSopInstUid();
    if (patientId.empty() || studyUid.empty() || seriesUid.empty() || sopInstUid.empty()) {
        spdlog::error(
                "patientId， studyUid， sereisUid or sopInstUid   not exists or one of those are empty:[{},{},{},{}]",
                patientId, studyUid, seriesUid, sopInstUid);
        return;

    }
    // std::string saveTo(dcmStoreDir + patientId + "/" + studyUid + "/" + seriesUid + "/");

    std::stringstream ss;
    ss << dcmStoreDir << patientId.c_str() << "/" << dcmInfo.getShortCrcCode()  << "/";
    std::string saveTo = ss.str();
    if (access(saveTo.c_str(), F_OK) != 0) {
        std::string cmdText("mkdir -p \"" + saveTo + "\"");
        int retur = system(cmdText.c_str());
        spdlog::debug("{}=mkdir:{}", retur, saveTo.substr(dcmStoreDir.size()));
    }

    if (access(saveTo.c_str(), F_OK) != 0) {
        spdlog::error("dir:{} denied access !", saveTo);
    } else {
        ss << sopInstUid <<".dcm";
        imebra::CodecFactory::save(payload, ss.str(), imebra::codecType_t::dicom);
        messages.insert(dcmInfo);
    }
    ss.clear();
}

bool RuntimeConfig::setupSpdlogRuntime() {

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
        spdlog::init_thread_pool(8192, ScpConstant::SPDLOG_MAX_THREADS);
        ///---------------------滚动日志


        ///--------------全部输出
        auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        stdout_sink->set_level(spdlog::level::debug);
        stdout_sink->set_pattern("[%^%l%$] %v");

        size_t maxSize(ScpConstant::SPDLOG_MAX_SIZE_SINGLE_FILE);
        size_t files(ScpConstant::SPDLOG_MAX_ROATING_FILES);

        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/info.txt",
                                                                                    maxSize,
                                                                                    files);
        rotating_sink->set_level(spdlog::level::info);
        rotating_sink->set_pattern("[%H:%M:%S] [%^-%L-%$] %v");


        std::string loggerName(ScpConstant::SCP_LOGGER_NAME);
        std::vector<spdlog::sink_ptr> sinks{stdout_sink, rotating_sink};
        auto defLogger = std::make_shared<spdlog::async_logger>(loggerName, sinks.begin(), sinks.end(),
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

void RuntimeConfig::setupRabbitDispatcher() {
    /// 查找符合要求的设备类型

    std::string utf8 = u8"从当前目录./config.yaml 读取配置文件";
    spdlog::debug("setupRabbitRuntime  Begin");
    spdlog::info(utf8.c_str());
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


    if (config["BindExchange"]) {
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


}


void RuntimeConfig::publishCStoreMessage(std::set<DcmInfo> &dicomMessages) {
    if (dicomMessages.empty()) {
        spdlog::warn("onMessageCallback with EmptyArguments");
        return;
    }

    spdlog::debug("onMessageCallback with EmptyArguments begin：{}", dicomMessages.size());
    uv_loop_t *cLoop = static_cast<uv_loop_t *>(malloc(sizeof(uv_loop_t)));
    if (cLoop == nullptr) {
        spdlog::error("onMessageCallback 申请内存失败");
        return;
    }
    std::once_flag onlyOne;
    try {


        uv_loop_init(cLoop);
        DicomMessageHandler jpHandler(cLoop);
        AMQP::Address jpAddr(ScpConstant::MQ_ADDRESS);
        AMQP::TcpConnection jpConn(&jpHandler, jpAddr);
        AMQP::TcpChannel channel(&jpConn);
        channel.startTransaction();
        for (DcmInfo cmsg: dicomMessages) {
            std::shared_ptr<AMQP::Envelope> envelope = cmsg.createMessage(mapModality, mapBodyPart);
            channel.publish(messagePubExchange, messagePubRoutingKey, *envelope.get());
        }
        channel.commitTransaction().onSuccess([&dicomMessages]() {
            spdlog::info("Commit Messages：{}  with  Success", dicomMessages.size());
        }).onError([&dicomMessages, &channel](const char *msg) {
            spdlog::error("Commit Messages：{} with  {}", dicomMessages.size(), msg);
            channel.rollbackTransaction();
        }).onFinalize([&channel, &dicomMessages, &cLoop]() {
            spdlog::debug("Commit Messages OverClear Resource");
            uv_stop(cLoop);
        });
        uv_run(cLoop, UV_RUN_DEFAULT);

        std::call_once(onlyOne, [&cLoop]() {
            if (cLoop) {
                spdlog::debug("onMessageCallback release cLoop");
                uv_loop_close(cLoop);
                free(cLoop);
                cLoop = nullptr;
            }
        });

        spdlog::debug("onMessageCallback with EmptyArguments End");
    }

    catch (const std::exception &exception) {

        spdlog::debug("onMessageCallback with EmptyArguments End：{}", exception.what());
    };

}
