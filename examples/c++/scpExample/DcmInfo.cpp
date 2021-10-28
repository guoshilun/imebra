//
// Created by dhz on 2021/10/22.
//

#include "DcmInfo.h"
#include <spdlog/spdlog.h>

DcmInfo::DcmInfo(const imebra::DataSet &payload) {

    mPatientId = payload.getString(
            imebra::TagId(imebra::tagId_t::PatientID_0010_0020), 0, "");

    mStudyUid = payload.getString(
            imebra::TagId(imebra::tagId_t::StudyInstanceUID_0020_000D), 0, "");

    mSeriesUid = payload.getString(
            imebra::TagId(imebra::tagId_t::SeriesInstanceUID_0020_000E), 0, "");

    mSopInstUid = payload.getString(
            imebra::TagId(imebra::tagId_t::SOPInstanceUID_0008_0018), 0, "");

    mModality = payload.getString(
            imebra::TagId(imebra::tagId_t::Modality_0008_0060), 0, "");

    mThickness = payload.getString(
            imebra::TagId(imebra::tagId_t::SliceThickness_0018_0050), 0, "");

    mExamPart = payload.getString(
            imebra::TagId(imebra::tagId_t::BodyPartExamined_0018_0015), 0, "");

}

DcmInfo::~DcmInfo() noexcept {

}

std::string DcmInfo::getSopInstUid() {
    return mSopInstUid;
}

std::string DcmInfo::getPatientId() {
    return mPatientId;
}

std::string DcmInfo::getExamPart() {
    return mExamPart;
}

std::string DcmInfo::getModality() {
    return mModality;
}

std::string DcmInfo::getThickness() {
    return mThickness;
}

std::string DcmInfo::getSeriesUid() {
    return mSeriesUid;
}

std::string DcmInfo::getStudyUid() {
    return mStudyUid;
}


DcmInfo::DcmInfo(const DcmInfo &that) :
        mSopInstUid(that.mSopInstUid),
        mPatientId(that.mPatientId),
        mStudyUid(that.mStudyUid),
        mSeriesUid(that.mSeriesUid),
        mThickness(that.mThickness),
        mModality(that.mModality),
        mExamPart(that.mExamPart) {
}

bool DcmInfo::operator<(const DcmInfo &rhs) const {
    return this->mSopInstUid < rhs.mSopInstUid;
}

std::shared_ptr<AMQP::Envelope> DcmInfo::createMessage(std::map<std::string, std::string> &mapModality,
                                                       std::map<std::string, std::string> &mapBodyPart) {
    std::shared_ptr<AMQP::Envelope> ptr = std::make_shared<AMQP::Envelope>(mSopInstUid.data(), mSopInstUid.size());

    ptr.get()->setDeliveryMode(2);
    ptr.get()->setContentEncoding("utf-8");
    ptr.get()->setContentType("text/plain");
    AMQP::Table messageHeaders;
    messageHeaders["xModality"] = mModality;
    messageHeaders["xBodyPartExamined"] = mExamPart;
    // std::string tx = std::toupper(mModality, std::locale("zh_CN.utf8"));
    {
        std::string mgx(mModality);
        transform(mgx.begin(), mgx.end(), mgx.begin(), ::toupper);
        if (0 != mapModality.count(mgx)) {
            messageHeaders["Modality"] = mapModality[mgx];
        } else {
            messageHeaders["Modality"] = "UNKOWN";
            spdlog::warn("Modality:{} NOT MAPPED", mgx);
        }
    }
    {

        std::string bp(mExamPart);
        transform(bp.begin(), bp.end(), bp.begin(), ::toupper);
        if (0 != mapBodyPart.count(bp)) {
            messageHeaders["BodyPartExamined"] = mapBodyPart[bp];
        } else {
            messageHeaders["BodyPartExamined"] = "UNKOWN";
            spdlog::warn("BodyPartExamed:{} NOT MAPPED", bp);
        }
    }


    messageHeaders["Thickness"] = mThickness;

    ptr.get()->setHeaders(messageHeaders);

    return ptr;
}

//DcmInfo &DcmInfo::operator=(const DcmInfo &a) {
//    this->mSopInstUid = (a.mSopInstUid);
//    this->mExamPart = a.mExamPart;
//    this->mModality = a.mModality;
//    this->mThickness = a.mThickness;
//    this->mSeriesUid = a.mSeriesUid;
//    this->mStudyUid = a.mStudyUid;
//    this->mPatientId = a.mPatientId;
//    return *this;
//}
//

