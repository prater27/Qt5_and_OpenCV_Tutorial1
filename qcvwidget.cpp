#include "qcvwidget.h"
#include "ui_qcvwidget.h"
#include "opencvworker.h"
#include <QTimer>
#include <QDebug>

QCvWidget::QCvWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QCvWidget)
{
    ui->setupUi(this);
    ui->labelView->setScaledContents(true);
    setup();
}

QCvWidget::~QCvWidget()
{
    thread->quit();
    while(!thread->isFinished());

    delete thread;
    delete ui;
}

void QCvWidget::setup()
{
    thread = new QThread();
    OpenCVWorker *worker = new OpenCVWorker();
    QTimer *workerTrigger = new QTimer();
    workerTrigger->setInterval(1);

    connect(workerTrigger, SIGNAL(timeout()), worker, SLOT(receiveGrabFrame()));
    connect(this, SIGNAL(sendSetup(int)), worker, SLOT(receiveSetup(int)));
    connect(this, SIGNAL(sendToggleStream()), worker, SLOT(receiveToggleStream()));
    connect(ui->pushButtonPlay, SIGNAL(clicked(bool)), this, SLOT(receiveToggleStream()));

    connect(ui->spinBoxBinaryThreshold, SIGNAL(valueChanged(int)), worker, SLOT(receiveBinaryTreshold(int)));
    connect(ui->horizontalSliderMinHessian, SIGNAL(sliderMoved(int)), worker, SLOT(receiveMinHessian(int)));
    connect(ui->horizontalSliderContoursThreshold, SIGNAL(valueChanged(int)), worker, SLOT(receiveContoursDetectorThreshold(int)));

    connect(ui->checkBoxEnabledBinaryThreshold, SIGNAL(clicked(bool)), worker, SLOT(receiveEnableBinaryTreshold()));
    connect(ui->checkBoxEnabledBinaryThreshold, SIGNAL(clicked(bool)), this, SLOT(onlyOneCheckBox_activeBinaryThreshold()));

    connect(ui->checkBoxEnabledKeypoints, SIGNAL(clicked(bool)), worker, SLOT(receiveEnableKeypoints()));
    connect(ui->checkBoxEnabledKeypoints, SIGNAL(clicked(bool)), this, SLOT(onlyOneCheckBox_activeKeypoints()));

    connect(ui->checkBoxEnabledContoursDetector, SIGNAL(clicked(bool)), worker, SLOT(receiveEnableContoursDetection()));
    connect(ui->checkBoxEnabledContoursDetector, SIGNAL(clicked(bool)), this, SLOT(onlyOneCheckBox_activeContoursDetector()));

    connect(worker, SIGNAL(sendFrame(QImage)), this, SLOT(receiveFrame(QImage)));

    workerTrigger->start();
    worker->moveToThread(thread);
    workerTrigger->moveToThread(thread);

    thread->start();

    emit sendSetup(0);
}

void QCvWidget::receiveFrame(QImage frame)
{
    ui->labelView->setPixmap(QPixmap::fromImage(frame));
}

void QCvWidget::receiveToggleStream()
{
    if(!ui->pushButtonPlay->text().compare(">")) ui->pushButtonPlay->setText("||");
    else ui->pushButtonPlay->setText(">");

    emit sendToggleStream();
}

void QCvWidget::onlyOneCheckBox_activeKeypoints()
{
        ui->checkBoxEnabledBinaryThreshold->setChecked(false);
        ui->checkBoxEnabledContoursDetector->setChecked(false);
}

void QCvWidget::onlyOneCheckBox_activeBinaryThreshold()
{
    ui->checkBoxEnabledKeypoints->setChecked(false);
    ui->checkBoxEnabledContoursDetector->setChecked(false);
}

void QCvWidget::onlyOneCheckBox_activeContoursDetector()
{
    ui->checkBoxEnabledBinaryThreshold->setChecked(false);
    ui->checkBoxEnabledKeypoints->setChecked(false);
}
