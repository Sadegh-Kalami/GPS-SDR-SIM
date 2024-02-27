#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void setprogressbar(int value);
signals:
    void updateTextSignal(const QString& text);

public slots:
    void updateTextSlot(const QString& text);

private slots:
    void on_BtnSelect_clicked();

    void on_BtnOperation_clicked();

    void on_BtnOperation_Tab3_clicked();

    void on_BtnSelect_Nasa_Tab3_clicked();

    void on_BtnSelect_Nmea_clicked();

    void performOperationTab2();

    void performOperationTab3();

    void resetApplication(int index);

    void on_BtnOperation_Tab4_clicked();

    void on_BtnSelect_4_clicked();

private:
    Ui::MainWindow *ui;
    QString fileName;
    QString fileNameNmea;
    QString fileNameNasaT3;
    QString fileNameT4;
    // tab2//
    QByteArray ba;
    QByteArray b;
    QByteArray ion;
    QByteArray ll;
    QString lon_lat;
    // Tab3 //
    QByteArray baT3;
    QByteArray baNmea;
    QByteArray bT3;
    QByteArray ionT3;
    // Tab4//

    



    int currentOperationTab;
    QString convertToPosixPath(QString windowsPath);
    void readtime();
    void readata();
    void setprog();
    void runmain();
    QTimer* timer;
};
#endif // MAINWINDOW_H
