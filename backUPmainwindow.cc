#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QFuture>
#include <QtConcurrent>

extern "C" {
#include "gpssim.h"
}
//MainWindow::MainWindow(QWidget *parent)
//    : QMainWindow(parent)
//    , ui(new Ui::MainWindow)
//{
//    ui->setupUi(this);
//    int currentTabIndex = ui->tabWidget->currentIndex();
//    if (currentTabIndex == 1)
//    {
//        ui->progressBar->setMaximum(300);
//        timer = new QTimer(this);
//        connect(timer, &QTimer::timeout, this, &MainWindow::readata);
//        connect(this,SIGNAL(setprogressbar(int)),ui->progressBar,SLOT(setValue(int)));
//        timer->setInterval(1000);
//    }
//    else if (currentTabIndex == 2)
//    {
//        ui->progressBar_Tab3->setMaximum(300);
//        timer = new QTimer(this);
//        connect(timer, &QTimer::timeout, this, &MainWindow::readata);
//        connect(this,SIGNAL(setprogressbar(int)),ui->progressBar_Tab3,SLOT(setValue(int)));
//        timer->setInterval(1000);
//    }
//}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Initialize timer and connect it to readata slot for both operations
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::readata);

    // Setup progress bars maximum value
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
    int currentTabIndex = ui->tabWidget->currentIndex();
    if (currentTabIndex == 1)
    {
        ui->TxtPath->setText(fileName);

    }

}

void MainWindow::on_BtnOperation_clicked()
{


    ui->TEXTRes->clear();
    timer->start();
    ui->progressBar->setValue(0);
    QtConcurrent::run(this, &MainWindow::runmain);


}
//    int currentTabIndex = ui->tabWidget->currentIndex();
//    if (currentTabIndex==1)
//    {
//        ui->TEXTRes->clear();
//        timer->start();
//        ui->progressBar->setValue(0);
//        QtConcurrent::run(this, &MainWindow::runmain);
//    }
//    else if (currentTabIndex==2) //this will never happens TODO: make clean code
//    {
//        ui->TEXTRes_Tab3->clear();
//        timer->start();
//        ui->progressBar_Tab3->setValue(0);
//        QtConcurrent::run(this, &MainWindow::runmain);
//    }


//Tab3 Dynamic
void MainWindow::on_BtnSelect_Nasa_Tab3_clicked()
{
    int currentTabIndex = ui->tabWidget->currentIndex();
    fileNameNasaT3 = QFileDialog::getOpenFileName(this,
                                            tr("Open file"), "/", tr("file (*.*)"));

    if (currentTabIndex == 2)
    {
        ui->TxtNasa_Tab3->setText(fileNameNasaT3);
    }

}


void MainWindow::on_BtnSelect_Nmea_clicked()
{
    int currentTabIndex = ui->tabWidget->currentIndex();
    fileNameNmea = QFileDialog::getOpenFileName(this,
                                            tr("Open file"), "/", tr("file (*.txt)"));

    if (currentTabIndex == 2)
    {
        ui->TxtNmea->setText(fileNameNmea);
    }
}


void MainWindow::on_BtnOperation_Tab3_clicked()
{

    ui->TEXTRes_Tab3->clear();
    timer->start();
    ui->progressBar_Tab3->setValue(0);
    QtConcurrent::run(this, &MainWindow::runmain);
}
//    int currentTabIndex = ui->tabWidget->currentIndex();
//    if (currentTabIndex==2) //this will never happens TODO: make clean code
//    {
//        ui->TEXTRes_Tab3->clear();
//        timer->start();
//        ui->progressBar_Tab3->setValue(0);
//        QtConcurrent::run(this, &MainWindow::runmain);
//    }





// ///////////////////////////Operration Bar//////////////////////////////////////





// ///////////////////////////////////////////////////////////////////////////////

void MainWindow::readtime()
{

    if (timehandel == 300){
            timer->stop();
        QFile file("readfile.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        QTextStream in(&file);

//        ui->TEXTRes->append(in.readAll());
        int currentTabIndex = ui->tabWidget->currentIndex();
        if (currentTabIndex==1)
        {
            ui->TEXTRes->append(in.readAll());
        }
        else if (currentTabIndex==2)
        {
            ui->TEXTRes_Tab3->append(in.readAll());
        }

    }
}

void MainWindow::readata()
{
    emit setprogressbar(timehandel);

    QtConcurrent::run(this, &MainWindow::readtime);
}

void MainWindow::setprog()
{

}




void MainWindow::runmain()
{
    int currentTabIndex = ui->tabWidget->currentIndex();
    if (currentTabIndex == 1)
    {

        char *arguments[9] ={}; //{"GPS.exe","-e",fileName, "-b", "8", "-l", "[36.27,-6.035,100]","-i","0"};

        char *arg[4]={};
        arguments[0]="GPS.exe";
        arguments[1]="-e";
        QByteArray ba = fileName.toLocal8Bit();
        char *c_str2 = ba.data();
        QByteArray b = ui->TxtB->text().toLocal8Bit();
        char *c_strb = b.data();

        arguments[2]=c_str2;
        arguments[3]="-b";
        arguments[4]=c_strb;
        arguments[5]="-l";
        QString lon_lat= QString("['" + ui->TxLatitude->text()+"','"+ui->TxtLongitude->text()+"','"+ui->TxtAltitude->text()+"']");

        QByteArray ll =lon_lat.toLocal8Bit();
        char *c_str_lon_lat = ll.data();
        arguments[6]=c_str_lon_lat;//"[36.27,-6.035,100]";
        // Enable/Disable ionospheric delay
        QByteArray ion = ui->Txtion->text().toLocal8Bit();
        char *c_strion = ion.data();
        arguments[7]="-i";
        arguments[8]=c_strion;

        Mmain(0,arg,arguments);
    }

    else if (currentTabIndex == 2)
    {
        char *argsT3[9] ={}; //{"GPS.exe","-e",fileName, "-b", "8", "-g",fileNameNmea,"-i", 0/1 };
        char *argT3[4]={};
        argsT3[0]="GPS.exe";
        argsT3[1]="-e";
        QByteArray ba = fileNameNasaT3.toLocal8Bit();
        char *c_str1 = ba.data();
        argsT3[2]=c_str1;

        argsT3[3]="-g";
        QByteArray baNmea = fileNameNmea.toLocal8Bit();
        char *c_str2 = baNmea.data();
        argsT3[4]=c_str2;

        QByteArray b = ui->TxtB_Tab3->text().toLocal8Bit();
        char *c_strb = b.data();
        argsT3[5] ="-b" ;
        argsT3[6] =c_strb;
        // Enable/Disable ionospheric delay
        QByteArray ionT3 = ui->Txtion_Tab3->text().toLocal8Bit();
        char *c_strionT3 = ionT3.data();
        argsT3[7]="-i";
        argsT3[8]=c_strionT3;
        Mmain(0,argT3,argsT3);

    }
}


