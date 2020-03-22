#ifndef QCVWIDGET_H
#define QCVWIDGET_H

#include <QWidget>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class QCvWidget; }
QT_END_NAMESPACE

class QCvWidget : public QWidget
{
    Q_OBJECT

private:
    Ui::QCvWidget *ui;
    QThread *thread;

    void setup();

public:
    explicit QCvWidget(QWidget *parent = 0);
    ~QCvWidget();

signals:
    void sendSetup(int device);
    void sendToggleStream();

private slots:
    void receiveFrame(QImage frame);
    void receiveToggleStream();
    void keypoints_BinaryThreshold_ToggleBinaryThreshold();
    void keypoints_BinaryThreshold_ToggleKeypoints();
};
#endif // QCVWIDGET_H
