#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    vT1=0;
    tCuentaRev = new QTimer();
    setGeometry(x(), 20, width(), height());
    t1 = new QTimer();

    timerInyeccion = new QTimer();
    connect(timerInyeccion,SIGNAL(timeout()),this,SLOT(detenerInyeccion()));
    timerRevPin21 = new QTimer();
    connect(timerRevPin21,SIGNAL(timeout()),this,SLOT(revisarPin()));
    timerRevPin21->start(1);

    //Definir Rango de SpinBox
    ui->spinBoxMsEmular->setRange(1,500);
    ui->spinBoxMsEmular->setValue(100);
    ui->spinBoxMsEmular->setSingleStep(10);//Cada cuanto sube

    ui->spinBoxMsCR->setRange(1,500);
    ui->spinBoxMsCR->setValue(100);
    ui->spinBoxMsCR->setSingleStep(1);//Cada cuanto sube



#ifdef __arm__
    rpiGpio = new mmapGpio();
#endif
    setPinType(21,1);
    setPinType(20,0);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::revisarPin(){
    //qDebug()<<"Revisando...";
    if(pinIsHigh(20)==1){
        qDebug()<<"20-->1";
        ui->widget->setStyleSheet("#widget{background-color:green}");
    }else{
        qDebug()<<"20-->0";
        ui->widget->setStyleSheet("#widget{background-color:red}");
    }
    if(readPin(21)==1){
        //qDebug()<<"21-->1";
        //ui->widget->setStyleSheet("#widget{background-color:green}");
        //writePinLow(20);
    }else{
        //qDebug()<<"21-->0";
        //ui->widget->setStyleSheet("#widget{background-color:red}");
        writePinHigh(20);
        //timerInyeccion = new QTimer();
        //connect(timerInyeccion,SIGNAL(timeout()),this,SLOT(detenerInyeccion()));
        vT1++;
        timerInyeccion->stop();
        timerInyeccion->start(ui->spinBoxTiempoInyeccion->value());

    }
}
void MainWindow::detenerInyeccion(){
    setPinState(20,0);
    writePinLow(20);
}
void MainWindow::setPinType(int pin, int type)
{
#ifdef __arm__
    if(type!=0&&type!=1){
        return;
    }
    if(type==0){
        rpiGpio->setPinDir(pin,mmapGpio::OUTPUT);
    }else{
        rpiGpio->setPinDir(pin,mmapGpio::INPUT);
    }
#endif
}

void MainWindow::setPinState(int pin, int state)
{
#ifdef __arm__
    if(state!=0&&state!=1){
        return;
    }
    if(state==0){
        rpiGpio->writePinLow(pin);
    }else{
        rpiGpio->writePinHigh(pin);
    }
#endif
}

unsigned int MainWindow::readPin(unsigned int pin)
{
#ifdef __arm__
    return rpiGpio->readPin(pin);
#else
    return 0;
#endif
}

void MainWindow::writePinHigh(unsigned int pinnum)
{
#ifdef __arm__
    rpiGpio->writePinHigh(pinnum);
#endif
}

void MainWindow::writePinLow(unsigned int pinnum)
{
#ifdef __arm__
    rpiGpio->writePinLow(pinnum);
#endif
}
bool MainWindow::pinIsHigh(int pin){
#ifdef __arm__
    unsigned int pinVal;
    pinVal = rpiGpio->readPin(pin);
    if(pinVal == mmapGpio::HIGH){
        return false;
    }
    return true;
#else
    return false;
#endif
}

void MainWindow::procTCR()
{
    //cantRevDetectadas++;
    if(vT1==0){
        return;
    }
    ui->labelCantRev->setText(QString::number(vT1));
    int msm=60000;
    float rps=(float)1000 / ui->spinBoxMsCR->value() * vT1; //rps rev por segmento del segundo.
    float rpm2=(float)rps * 60 / 2;
    if(QString::number(rpm2)!="inf"){
        ui->labelRPM->setText(QString::number(rpm2));
    }
    vT1=0;
}

void MainWindow::procT1()
{

    qDebug()<<"T1"<<vT1;
#ifndef __arm__
    if(pinHighVirtual){
        qDebug()<<">1";
        vT1++;
        pinHighVirtual=false;
    }else{
        qDebug()<<">0";
        pinHighVirtual=true;
    }
#else

    if(readPin(21)==1){
        qDebug()<<"-->1";
        vT1++;
    }else{
        qDebug()<<"-->0";
    }
#endif

}

void MainWindow::on_pushButtonEmular_toggled(bool checked)
{
    QByteArray s;
    if(checked){
        s.append("#pushButtonEmular{background-color:red; }");
        rpmVirtualEncendido=true;
        t1 = new QTimer();
        connect(t1, SIGNAL(timeout()), this, SLOT(procT1()));
        t1->start(ui->spinBoxMsEmular->value());
    }else{
        s.append("#pushButtonEmular{background-color:gray; }");
        t1->stop();
        vT1=0;
    }
    ui->pushButtonEmular->setStyleSheet(s);
}

void MainWindow::on_spinBoxMsEmular_valueChanged(int arg1)
{
    if(rpmVirtualEncendido){
        t1->stop();
        //t1 = new QTimer();
        //connect(t1, SIGNAL(timeout()), this, SLOT(procT1()));
        t1->start(ui->spinBoxMsEmular->value());
    }
}

void MainWindow::on_spinBoxMsCR_valueChanged(int arg1)
{
    tCuentaRev->stop();
    //tCuentaRev = new QTimer();
    //connect(tCuentaRev, SIGNAL(timeout()), this, SLOT(procTCR()));
    tCuentaRev->start(ui->spinBoxMsCR->value());
}

void MainWindow::on_pushButtonEncender_clicked(bool checked)
{
    QByteArray s;
    if(checked){
        s.append("#pushButtonEncender{background-color:red; }");
        ui->pushButtonEncender->setText("Encendido");
        tCuentaRev = new QTimer();
        connect(tCuentaRev, SIGNAL(timeout()), this, SLOT(procTCR()));
        tCuentaRev->start(100);
    }else{
        s.append("#pushButtonEncender{background-color:gray; }");
        ui->pushButtonEncender->setText("Encender");
        tCuentaRev->stop();
    }
    ui->pushButtonEncender->setStyleSheet(s);
}




