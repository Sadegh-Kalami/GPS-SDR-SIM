#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QFuture>
#include <QtConcurrent>
#include <QThread>

extern "C" {
#include "gpssim.h"
}
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::readata);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::resetApplication);
    connect(this, &MainWindow::updateTextSignal, this, &MainWindow::updateTextSlot);

    ui->progressBar->setMaximum(300);
    ui->progressBar_Tab3->setMaximum(300);

    timer->setInterval(1000);
}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_BtnSelect_clicked()
{
//    QString filenName
    fileName = QFileDialog::getOpenFileName(this,
                                            tr("Open file"), "/", tr("file (*.*)"));
    
    QThread::msleep(500);
    ui->TxtPath->setText(fileName);


}
// ////////////////////////////////////Reset///////////////////////////////////////////
void MainWindow::resetApplication(int index) {
    // Reset logic here
    // Example: Clear text, reset progress bars, stop timers, etc.
    ui->TEXTRes->clear();
    ui->TEXTRes_Tab3->clear();
    ui->progressBar->setValue(0);
    ui->progressBar_Tab3->setValue(0);
    timer->stop();
    fileName.clear();
    fileNameNasaT3.clear();
    fileNameNmea.clear();
    currentOperationTab = 0; // Reset or set to a default value
    timehandel = 0;
}

// ////////////////////////////////////Button Opertaion Tab2/3////////////////////////////////////
void MainWindow::on_BtnOperation_clicked()
{

    ba = fileName.toLocal8Bit();
    b = ui->TxtB->text().toLocal8Bit();
    lon_lat= QString(ui->TxLatitude->text()+","+ui->TxtLongitude->text()+","+ui->TxtAltitude->text());
    ll =lon_lat.toLocal8Bit();
    ion = ui->Txtion->text().toLocal8Bit();
    QThread::msleep(500);
    performOperationTab2();
}
// tab3//
void MainWindow::on_BtnOperation_Tab3_clicked()
{

    baT3 = fileNameNasaT3.toLocal8Bit();
    baNmea = fileNameNmea.toLocal8Bit();
    bT3 = ui->TxtB_Tab3->text().toLocal8Bit();
    ionT3 = ui->Txtion_Tab3->text().toLocal8Bit();
    QThread::msleep(2000);
    performOperationTab3();


}

// ////////////////////////////////////Tab3 Dynamic////////////////////////////////////
void MainWindow::on_BtnSelect_Nasa_Tab3_clicked()
{
    fileNameNasaT3 = QFileDialog::getOpenFileName(this,
                                            tr("Open file"), "/", tr("file (*.*)"));
    QThread::msleep(500);
    ui->TxtNasa_Tab3->setText(fileNameNasaT3);


}


void MainWindow::on_BtnSelect_Nmea_clicked()
{
    fileNameNmea = QFileDialog::getOpenFileName(this,
                                            tr("Open file"), "/", tr("file (*.txt)"));
    QThread::msleep(500);
    ui->TxtNmea->setText(fileNameNmea);
}


// ////////////////////////////////connect signals//////////////////////////////////
void MainWindow::updateTextSlot(const QString& text)
{
    ui->TEXTRes_Tab4->append(text);
}


void MainWindow::on_BtnSelect_4_clicked()
{
    fileNameT4 = QFileDialog::getOpenFileName(this,
                                            tr("Open file"), "/", tr("file (*.bin)"));
    QThread::msleep(500);
    ui->TxtFile_4->setText(fileNameT4);
}


// ///////////////////////////////////////////////////////////////////////////////
QString MainWindow::convertToPosixPath(QString windowsPath)
{
    windowsPath.replace("\\", "/"); // Replace backslashes with forward slashes
    if(windowsPath.contains(":")) {
        windowsPath.replace(":", ""); // Remove the colon
        windowsPath.prepend("/cygdrive/"); // Prepend with /cygdrive/
    }
    return windowsPath;
}


// //////////////////////////////////Tab4-Transfer///////////////////////////////////
void MainWindow::on_BtnOperation_Tab4_clicked()
{
//    fileNameT4 = ui->TxtFile_4->text();
    QString posixPath = convertToPosixPath(fileNameT4);
    QString frequency = ui->TxtFreq_4->text();
    QString sampling = ui->TxtFreqSampl_4->text();
    QString gain = ui->TxtGain_4->text();
    QString rxtx = ui->TxtRXTX_4->text();

    QStringList arguments;
    arguments << "-t" << posixPath
              << "-f" << frequency
              << "-s" << sampling
              << "-a" << rxtx
              << "-x" << gain;

    QtConcurrent::run([this, arguments]() {
        QProcess process;
        process.start("transfer.exe", arguments);
        process.waitForFinished(-1);

        QString output = process.readAllStandardOutput();
        QString error = process.readAllStandardError();
        QString combinedOutput = output + "\n" + error;
        emit updateTextSignal(combinedOutput);

    });
}






// ///////////////////////////Operration Bar//////////////////////////////////////


void MainWindow::performOperationTab2() {
    currentOperationTab = 1; // Set current operation tab to 2
    timehandel = 0;
    ui->TEXTRes->clear();
    timer->start();
    ui->progressBar->setValue(0);
    QtConcurrent::run(this, &MainWindow::runmain);
}

void MainWindow::performOperationTab3() {
    currentOperationTab = 2; // Set current operation tab to 2
    timehandel = 0;
    ui->TEXTRes_Tab3->clear();
    timer->start();
    ui->progressBar_Tab3->setValue(0);
    QtConcurrent::run(this, &MainWindow::runmain);
}


// ///////////////////////////////////////////////////////////////////////////////

void MainWindow::readtime()
{

    if (currentOperationTab==1 && timehandel == 300)
    {
        timer->stop();
        QFile file("readfile.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);
        ui->TEXTRes->append(in.readAll());
    }
    else if (currentOperationTab==2 && timehandel>=299)
    {
        timer->stop();
        QFile file("readfile.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);
        ui->TEXTRes_Tab3->append(in.readAll());
    }
}

void MainWindow::readata()
{
    emit setprogressbar(timehandel);

    if (currentOperationTab == 1) {
        // Update progress bar for Tab 2
        QMetaObject::invokeMethod(ui->progressBar, "setValue", Q_ARG(int, timehandel));
    } else if (currentOperationTab == 2 && timehandel != 300) {
        // Update progress bar for Tab 3
        QMetaObject::invokeMethod(ui->progressBar_Tab3, "setValue", Q_ARG(int, timehandel));
    }
    else if (currentOperationTab == 2 && timehandel == 300) {
        // Delay execution
        QTimer::singleShot(3000, this, [this]() {
            QMetaObject::invokeMethod(ui->progressBar_Tab3, "setValue", Q_ARG(int, 300));
        });
    }

    QtConcurrent::run(this, &MainWindow::readtime);
}

void MainWindow::setprog()
{

}



// ///////////////////////////////////////////////////////Main///////////////////////////////////////////////////////
void MainWindow::runmain()
{
    if (currentOperationTab == 1)
    {

        char *arguments[9] ={}; //{"GPS.exe","-e",fileName, "-b", "8", "-l", "36.27,-6.035,100","-i","0"};

        char *arg[4]={};
        arguments[0]="GPS.exe";
        arguments[1]="-e";
        // QByteArray ba = fileName.toLocal8Bit();
        char *c_str2 = ba.data();
        // QByteArray b = ui->TxtB->text().toLocal8Bit();
        char *c_strb = b.data();

        arguments[2]=c_str2;
        arguments[3]="-b";
        arguments[4]=c_strb;
        arguments[5]="-l";
        // QString lon_lat= QString(ui->TxLatitude->text()+","+ui->TxtLongitude->text()+","+ui->TxtAltitude->text());

        // QByteArray ll =lon_lat.toLocal8Bit();
        char *c_str_lon_lat = ll.data();
        arguments[6]=c_str_lon_lat;//"[36.27,-6.035,100]";
        // Enable/Disable ionospheric delay
        // QByteArray ion = ui->Txtion->text().toLocal8Bit();
        char *c_strion = ion.data();
        arguments[7]="-i";
        arguments[8]=c_strion;

        Mmain(0,arg,arguments);
    }

    else if (currentOperationTab == 2)
    {
        char *argsT3[9] ={}; //{"GPS.exe","-e",fileName, "-b", "8", "-g",fileNameNmea,"-i", 0/1 };
        char *argT3[4]={};
        argsT3[0]="GPS.exe";
        argsT3[1]="-e";
        // QByteArray baT3 = fileNameNasaT3.toLocal8Bit();
        char *c_str1 = baT3.data();
        argsT3[2]=c_str1;

        argsT3[3]="-g";
        // QByteArray baNmea = fileNameNmea.toLocal8Bit();
        char *c_str2 = baNmea.data();
        argsT3[4]=c_str2;

        // QByteArray bT3 = ui->TxtB_Tab3->text().toLocal8Bit();
        char *c_strb = bT3.data();
        argsT3[5] ="-b" ;
        argsT3[6] =c_strb;
        // Enable/Disable ionospheric delay
        // QByteArray ionT3 = ui->Txtion_Tab3->text().toLocal8Bit();
        char *c_strionT3 = ionT3.data();
        argsT3[7]="-i";
        argsT3[8]=c_strionT3;
        Mmain(0,argT3,argsT3);

    }
}





