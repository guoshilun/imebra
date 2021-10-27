//
// Created by dhz on 2021/10/26.
//

#ifndef IMEBRA_SCPCONSTANT_H
#define IMEBRA_SCPCONSTANT_H


const char *MQ_ADDRESS = "amqp://root:root@localhost/imebra";

const char *MQ_EXCHANG = "exchange.imebra";
const char *MQ_QUEUE = "queue.imebra";
const char *MQ_ROUTING_KEY = "DICOM";


const char *DEAD_EXCAHGE = "exchange.imebra.dead";
const char *DEAD_QUEUE = "queue.imebra.dead";
const char *DEAD_ROUTING_KEY = "DICOM.dead";


const char *SUCCESS(" Success");
const char *FAILED(" Failed:");
const char *CreateQueue("create Queue :");
const char *ClearResource("clear Resource ");


const char * SCP_LOGGER_NAME("scp-logger");


/// 1分钟
const size_t ONE_MINUTE = 60 * 1000;

const size_t ONE_HOUR = 60 * ONE_MINUTE;

const size_t ONE_DAY = 24 * ONE_HOUR;

/// 5M
const int SPDLOG_MAX_SIZE_SINGLE_FILE = 1048576 * 5;

/// 30
const int SPDLOG_MAX_ROATING_FILES = 20;


#endif //IMEBRA_SCPCONSTANT_H
