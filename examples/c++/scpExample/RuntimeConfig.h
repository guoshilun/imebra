//
// Created by dhz on 2021/11/1.
//

#ifndef IMEBRA_RUNTIMECONFIG_H
#define IMEBRA_RUNTIMECONFIG_H

#include <set>
#include <vector>
#include <amqpcpp.h>
#include <amqpcpp/linux_tcp.h>
#include "DcmInfo.h"
#include "scpConstant.h"
#include "imebra/imebra.h"
class RuntimeConfig {

public:
    RuntimeConfig(std::string dcmStore);

    virtual  ~RuntimeConfig();

    void setupRabbitDispatcher();

    void perfermFileStorek(std::set<DcmInfo> &messages, imebra::DataSet &payload, std::string &dcmStoreDir);

    void publishCStoreMessage(std::set<DcmInfo> &dicomMessages);




    RuntimeConfig &operator=(const RuntimeConfig &) = default;

    RuntimeConfig &Runtime(RuntimeConfig &&) = delete;


    RuntimeConfig(const RuntimeConfig &);

protected:
    static void createQueue(void *args);

    static void createExchage(void *args);

    static void bindQueue(void *args);

    static void bindExchage(void *args);

    static void fillTableArgument(std::vector<RabbitMqArgument> &args, AMQP::Table &table);

public:
     bool setupSpdlogRuntime();

protected:
    std::vector<DicomTagConverter> modalityConverter;
    std::vector<DicomTagConverter> examedBodyPartConverter;
    std::string messagePubExchange;
    std::string messagePubRoutingKey;
    std::map<std::string,std::string> mapModality;
    std::map<std::string,std::string> mapBodyPart;

    std::string dcmStoreDirectory;






};


#endif //IMEBRA_RUNTIMECONFIG_H
