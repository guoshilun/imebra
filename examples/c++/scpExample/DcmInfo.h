//
// Created by dhz on 2021/10/22.
//

#ifndef IMEBRA_DCMINFO_H
#define IMEBRA_DCMINFO_H

#include "imebra/imebra.h"
#include <amqpcpp.h>
#include <amqpcpp/linux_tcp.h>
#include <amqpcpp/libuv.h>

class DcmInfo {
public:
    DcmInfo(const imebra::DataSet &payload);

    //-------禁止COPY
    DcmInfo(const DcmInfo &that);

    DcmInfo &operator=(const DcmInfo &that) = delete;

    /**
     *  Destructor
     */
    virtual ~DcmInfo() noexcept;

    std::string getSopInstUid();

    std::string getPatientId();

    std::string getStudyUid();

    std::string getSeriesUid();

    std::string getThickness();

    std::string getModality();

    std::string getExamPart();

    std::shared_ptr<AMQP::Envelope> createMessage();

    bool operator<(const DcmInfo& rhs) const ;

private:

    std::string mSopInstUid;
    std::string mPatientId;
    std::string mStudyUid;
    std::string mSeriesUid;
    std::string mThickness;
    std::string mModality;
    std::string mExamPart;



};


#endif //IMEBRA_DCMINFO_H
