#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDebug>
#include "mmapGpio.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void setPinType(int pin, int type);
    void setPinState(int pin, int state);
    unsigned int readPin(unsigned int pin);
    void writePinHigh(unsigned int pinnum);
    void writePinLow(unsigned int pinnum);
    bool pinIsHigh(int pin);

public slots:
    void procTCR();
    void procT1();
private slots:
    void on_pushButtonEmular_toggled(bool checked);

    void on_spinBoxMsEmular_valueChanged(int arg1);

    void on_spinBoxMsCR_valueChanged(int arg1);

    void on_pushButtonEncender_clicked(bool checked);



private:
    Ui::MainWindow *ui;

    bool rpmVirtualEncendido=false;

    int vT1=0;
    int cantRevDetectadas=0;
    QTimer *tCuentaRev;
    QTimer *t1;

    bool pinHighVirtual=false;



#ifdef __arm__
    mmapGpio *rpiGpio;
#endif
};

#endif // MAINWINDOW_H
