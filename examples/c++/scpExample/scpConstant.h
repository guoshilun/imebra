//
// Created by dhz on 2021/10/26.
//
#include <yaml-cpp/yaml.h>
#include <yaml-cpp/node/parse.h>
#include <string>
#include <vector>
#include <list>
#include <iostream>

#ifndef IMEBRA_SCPCONSTANT_H
#define IMEBRA_SCPCONSTANT_H


class ScpConstant {
protected:
    ScpConstant();

public:
    ScpConstant(const ScpConstant &) = delete;

    ScpConstant(ScpConstant &&) = delete;

    ScpConstant operator=(const ScpConstant &) = delete;

public:
    static constexpr const char *MQ_ADDRESS = "amqp://root:root@rabbitmq_host/imebra";
    static constexpr const char *FAILED = " Failed:";
    static constexpr const char *CreateQueue = "create RabbitMqQueueInfo :";
    static constexpr const char *ClearResource = "clear Resource ";
    static constexpr const char *SCP_LOGGER_NAME = "scp-logger";


    /// 1分钟
    static const size_t ONE_MINUTE = 60 * 1000;

    static const size_t ONE_HOUR = 60 * ONE_MINUTE;

    static const size_t ONE_DAY = 24 * ONE_HOUR;


/// 5M
    static const int SPDLOG_MAX_SIZE_SINGLE_FILE = 1048576 * 5;

/// 20
    static const int SPDLOG_MAX_ROATING_FILES = 20;

/// 4
    static const int SPDLOG_MAX_THREADS = 4;
};


struct RabbitMqArgument {

    std::string key;
    std::string value;
    std::string type;
};

struct RabbitMqQueueInfo {

    std::string name;
    std::vector<RabbitMqArgument> tableArguments;
};

struct RabbitMqBindQueueInfo {
    std::string queuename;
    std::string exchange;
    std::string routingkey;
    std::vector<RabbitMqArgument> tableArguments;
};


struct RabbitMqExchangeInfo {

    std::string name;
    std::string type;
    std::vector<RabbitMqArgument> tableArguments;
};


struct RabbitMqBindExchangeInfo {

    std::string from;
    std::string to;
    std::string routingkey;
};


struct DicomTagConverter {

    std::string key;
    std::vector<std::string> values;
};

/**
    * FUNCTION: join
    * USE: Concatenates elements in the given range [first, last), using the
    *   specified separator between each item
    * @param first: The first position of the sequence
    * @param last: The last position of the sequence
    * @param separator: The character(s) to use as a separator. Separator is
    *   included in the returned string only if there is more than one element
    * @return: A string containing the elements delimited by the separator
    *   character, or empty if values has zero elements
    *   http://programmingnotes.org/
    */
template<typename InputIt>
std::string join(InputIt first, InputIt last, const std::string &separator = ",") {
    std::ostringstream result;
    if (first != last) {
        result << *first;
        while (++first != last) {
            result << separator << *first;
        }
    }
    return result.str();
}


namespace YAML {
    template<>
    struct convert<RabbitMqArgument> {
        static Node encode(const RabbitMqArgument &rhs) {
            Node node;
            node.push_back(rhs.key);
            node.push_back(rhs.value);
            node.push_back(rhs.type);
            return node;
        }

        static bool decode(const Node &node, RabbitMqArgument &rhs) {
            rhs.key = node["key"].as<std::string>();
            rhs.value = node["value"].as<std::string>();
            rhs.type = node["type"].as<std::string>();
            return true;
        }
    };

    template<>
    struct convert<RabbitMqQueueInfo> {
        static Node encode(const RabbitMqQueueInfo &rhs) {
            Node node;
            node.push_back(rhs.name);

            if (!rhs.tableArguments.empty()) {
                node.push_back(rhs.tableArguments);
            }

            return node;
        }

        static bool decode(const Node &node, RabbitMqQueueInfo &rhs) {
            rhs.name = node["name"].as<std::string>();
            if (node["arguments"]) {
                rhs.tableArguments = node["arguments"].as<std::vector<RabbitMqArgument>>();
            }
            return true;
        }
    };

    template<>
    struct convert<RabbitMqBindQueueInfo> {
        static Node encode(const RabbitMqBindQueueInfo &rhs) {
            Node node;
            node.push_back(rhs.queuename);
            node.push_back(rhs.exchange);
            node.push_back(rhs.routingkey);
            if (!rhs.tableArguments.empty()) {
                node.push_back(rhs.tableArguments);
            }

            return node;
        }

        static bool decode(const Node &node, RabbitMqBindQueueInfo &rhs) {
            rhs.queuename = node["queuename"].as<std::string>();
            rhs.exchange = node["exchange"].as<std::string>();
            rhs.routingkey = node["routingkey"].as<std::string>();
            if (node["arguments"]) {
                rhs.tableArguments = node["arguments"].as<std::vector<RabbitMqArgument>>();
            }
            return true;
        }
    };

    template<>
    struct convert<RabbitMqExchangeInfo> {
        static Node encode(const RabbitMqExchangeInfo &rhs) {
            Node node;
            node.push_back(rhs.name);
            node.push_back(rhs.type);
            if (!rhs.tableArguments.empty()) {
                node.push_back(rhs.tableArguments);
            }
            return node;
        }

        static bool decode(const Node &node, RabbitMqExchangeInfo &rhs) {
            rhs.name = node["name"].as<std::string>();
            rhs.type = node["type"].as<std::string>();
            if (node["arguments"]) {
                rhs.tableArguments = node["arguments"].as<std::vector<RabbitMqArgument>>();
            }
            return true;
        }
    };

    template<>
    struct convert<DicomTagConverter> {
        static Node encode(const DicomTagConverter &rhs) {
            Node node;
            node.push_back(rhs.key);
            node.push_back(rhs.values);

            return node;
        }

        static bool decode(const Node &node, DicomTagConverter &rhs) {
            rhs.key = node["key"].as<std::string>();
            rhs.values = node["values"].as<std::vector<std::string>>();
            return true;
        }
    };

    template<>
    struct convert<RabbitMqBindExchangeInfo> {
        static Node encode(const RabbitMqBindExchangeInfo &rhs) {
            Node node;
            node.push_back(rhs.from);
            node.push_back(rhs.to);
            node.push_back(rhs.routingkey);
            return node;
        }

        static bool decode(const Node &node, RabbitMqBindExchangeInfo &rhs) {
            rhs.from = node["from"].as<std::string>();
            rhs.to = node["to"].as<std::string>();
            rhs.routingkey = node["routingkey"].as<std::string>();
            return true;
        }
    };


}


#endif //IMEBRA_SCPCONSTANT_H
