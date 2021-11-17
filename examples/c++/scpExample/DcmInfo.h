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

    ///拷贝构造函数
    DcmInfo(const DcmInfo &that);
    ////赋值符重载 -- 默认调用 拷贝构造函数
    DcmInfo &operator=(const DcmInfo &a) = default ;
    //移动构造函数
    DcmInfo(DcmInfo&&) noexcept =delete;



    /**
     *  Destructor
     */
    virtual ~DcmInfo() noexcept;



    std::string getPatientId() const ;

    std::string getStudyUid() const;

    std::string getSeriesUid() const;

    std::string getSopInstUid() const;




    int  getInstanceNumber() const {
        return  mInstanceNumber;
    }

    std::shared_ptr<AMQP::Envelope> createMessage(std::map<std::string,std::string>& modalityMap, std::map<std::string,std::string>& bodyPartMap);

    bool operator<(const DcmInfo &rhs) const;


protected:

    void computeCrc();


private:

    std::string mSopInstUid;
    std::string mPatientId;
    std::string mStudyUid;
    std::string mSeriesUid;
    std::string mThickness;
    std::string mModality;
    std::string mExamPart;
    ulong   _crcCode;
    int     mInstanceNumber;
private:
    const char* UNKOWN="UNKOWN";


};


#endif //IMEBRA_DCMINFO_H
