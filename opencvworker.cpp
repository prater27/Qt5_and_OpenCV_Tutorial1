#include "opencvworker.h"
#include <opencv2/imgproc/imgproc.hpp>

OpenCVWorker::OpenCVWorker(QObject *parent) :
    QObject(parent),
    status(false),
    toggleStream(false),
    binaryTresholdEnable(false),
    binaryTreshold(127)
{
    cap = new cv::VideoCapture();
}

OpenCVWorker::~OpenCVWorker()
{
    if(cap->isOpened()) cap->release();
    delete cap;
}

void OpenCVWorker::receiveGrabFrame()
{
    if(!toggleStream) return;

    (*cap) >> _frameOriginal;
    if(_frameOriginal.empty()) return;

    process();

    QImage output((const unsigned char*) _frameProcessed.data, _frameProcessed.cols, _frameProcessed.rows, QImage::Format_Indexed8);

    emit sendFrame(output);
}

void OpenCVWorker::checkIfDeviceAlreadyOpened(const int device)
{
    if(cap->isOpened()) cap->release();
    cap->open(device);
}

void OpenCVWorker::process()
{
    cv::cvtColor(_frameOriginal, _frameProcessed, cv::COLOR_BGR2GRAY);

    if(binaryTresholdEnable){
        cv::threshold(_frameProcessed, _frameProcessed, binaryTreshold, 255, cv::THRESH_BINARY);
    }
}

void::OpenCVWorker::receiveSetup(const int device)
{
    checkIfDeviceAlreadyOpened(device);
    if(!cap->isOpened()){
        status=false;
    }

    status=true;
}

void OpenCVWorker::receiveToggleStream()
{
    toggleStream = !toggleStream;
}

void OpenCVWorker::receiveEnableBinaryTreshold()
{
    binaryTresholdEnable = !binaryTresholdEnable;
}

void OpenCVWorker::receiveBinaryTreshold(int threshold)
{
    binaryTreshold=threshold;
}
