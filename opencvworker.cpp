#include "opencvworker.h"
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <opencv2/core/core.hpp>
#ifdef HAVE_OPENCV_XFEATURES2D
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

#include <QDebug>

using namespace cv::xfeatures2d;
using namespace cv;

OpenCVWorker::OpenCVWorker(QObject *parent) :
    QObject(parent),
    status(false),
    toggleStream(false),
    binaryTresholdEnable(false),
    binaryTreshold(127),
    keypointsEnable(false),
    minHessian(50),
    contoursDetectionEnable(false),
    contoursThresh(100)
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

    if(keypointsEnable) processKeypoints();
    else if(binaryTresholdEnable) processBinaryThreshold();
    else if(contoursDetectionEnable) processContoursDetection();
    else greyScaleProcess();

    QImage output((const unsigned char*) _frameProcessed.data, _frameProcessed.cols, _frameProcessed.rows, QImage::Format_Indexed8);

    emit sendFrame(output);
}

void OpenCVWorker::checkIfDeviceAlreadyOpened(const int device)
{
    if(cap->isOpened()) cap->release();
    cap->open(device);
}

void OpenCVWorker::processBinaryThreshold()
{
    cv::cvtColor(_frameOriginal, _frameProcessed, cv::COLOR_BGR2GRAY);

    cv::threshold(_frameProcessed, _frameProcessed, binaryTreshold, 255, cv::THRESH_BINARY);
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
    keypointsEnable = false;
    contoursDetectionEnable = false;
}

void OpenCVWorker::receiveBinaryTreshold(int threshold)
{
    binaryTreshold=threshold;
}

void OpenCVWorker::receiveEnableKeypoints()
{
    keypointsEnable = !keypointsEnable;
    binaryTresholdEnable = false;
    contoursDetectionEnable = false;
}

void OpenCVWorker::receiveMinHessian(int hessian)
{
    minHessian=hessian;
}

void OpenCVWorker::receiveEnableContoursDetection()
{
    contoursDetectionEnable = !contoursDetectionEnable;
    keypointsEnable = false;
    binaryTresholdEnable = false;
}

void OpenCVWorker::receiveContoursDetectorThreshold(int contoursThreshold)
{
    contoursThresh = contoursThreshold;
}

void OpenCVWorker::processKeypoints()
{
    cv::cvtColor(_frameOriginal, _frameProcessed, cv::COLOR_BGR2GRAY);

    //-- Step 1: Detect the keypoints using SURF Detector
    cv::Ptr<SURF> detector = SURF::create( minHessian );
    std::vector<cv::KeyPoint> keypoints;
    detector->detect( _frameProcessed, keypoints );
    //-- Draw keypoints
    Mat img_keypoints;
    drawKeypoints( _frameProcessed, keypoints, img_keypoints );

    _frameProcessed=img_keypoints;
}

void OpenCVWorker::greyScaleProcess()
{
    cv::cvtColor(_frameOriginal, _frameProcessed, cv::COLOR_BGR2GRAY);
}

void OpenCVWorker::processContoursDetection()
{
    cv::cvtColor(_frameOriginal, _frameProcessed, cv::COLOR_BGR2GRAY);
    Mat canny_output;
    cv::Canny( _frameProcessed, canny_output, contoursThresh, contoursThresh*2 );
    std::vector<std::vector<Point> > contours;
    std::vector<Vec4i> hierarchy;
    findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );

    RNG rng(12345);

    for( size_t i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
        drawContours( drawing, contours, (int)i, color, 2, LINE_8, hierarchy, 0 );
    }

    _frameProcessed=drawing;
}

#endif
