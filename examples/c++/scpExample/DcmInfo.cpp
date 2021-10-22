//
// Created by dhz on 2021/10/22.
//

#include "DcmInfo.h"


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
    return  this->mSopInstUid < rhs.mSopInstUid;
}

std::shared_ptr<AMQP::Envelope> DcmInfo::createMessage() {
    std::shared_ptr<AMQP::Envelope>  ptr=std::make_shared<AMQP::Envelope>(mSopInstUid.data(), mSopInstUid.size());

    ptr.get()->setDeliveryMode(2);
    ptr.get()->setContentEncoding("utf-8");
    ptr.get()->setContentType("text/plain");
    AMQP::Table messageHeaders;
    //消息头
    messageHeaders["Modality"] = mModality;
    messageHeaders["Thickness"] = mThickness;
    messageHeaders["BodyPartExamined"] = mExamPart;
    ptr.get()->setHeaders(messageHeaders);

    return  ptr;
}

