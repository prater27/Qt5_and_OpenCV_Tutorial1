#ifndef OPENCVWORKER_H
#define OPENCVWORKER_H

#include <QObject>
#include <QImage>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class OpenCVWorker : public QObject
{
    Q_OBJECT

private:
    cv::Mat _frameOriginal;
    cv::Mat _frameProcessed;
    cv::VideoCapture *cap;

   bool status;
   bool toggleStream;

   bool binaryTresholdEnable;
   int binaryTreshold;

   bool keypointsEnable;
   int minHessian;

   void checkIfDeviceAlreadyOpened(const int device);
   void processBinaryThreshold();
   void processKeypoints();
   void greyScaleProcess();

public:
    explicit OpenCVWorker(QObject *parent = nullptr);
    ~OpenCVWorker();

signals:
    void sendFrame(QImage frameProcessed);

public slots:
    void receiveGrabFrame();
    void receiveSetup(const int device);
    void receiveToggleStream();

    void receiveEnableBinaryTreshold();
    void receiveBinaryTreshold(int treshold);

    void receiveEnableKeypoints();
    void receiveMinHessian(int hessian);
};

#endif // OPENCVWORKER_H
