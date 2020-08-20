#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    startInit();
    myCom = NULL;

    ui->qter->setText(
            tr("<a href=\"http://www.qter.org\">www.qter.org</a>"));
// #ifdef Q_OS_LINUX
//    ui->portNameComboBox->addItem( "ttyUSB0");
//    ui->portNameComboBox->addItem( "ttyUSB1");
//    ui->portNameComboBox->addItem( "ttyUSB2");
//    ui->portNameComboBox->addItem( "ttyUSB3");
//    ui->portNameComboBox->addItem( "ttyS0");
//    ui->portNameComboBox->addItem( "ttyS1");
//    ui->portNameComboBox->addItem( "ttyS2");
//    ui->portNameComboBox->addItem( "ttyS3");
//    ui->portNameComboBox->addItem( "ttyS4");
//    ui->portNameComboBox->addItem( "ttyS5");
//    ui->portNameComboBox->addItem( "ttyS6");
//#elif defined (Q_OS_WIN)
    ui->portNameComboBox->addItem("COM0");
    ui->portNameComboBox->addItem("COM1");
    ui->portNameComboBox->addItem("COM2");
    ui->portNameComboBox->addItem("COM3");
    ui->portNameComboBox->addItem("COM4");
    ui->portNameComboBox->addItem("COM5");
    ui->portNameComboBox->addItem("COM6");
    ui->portNameComboBox->addItem("COM7");
    ui->portNameComboBox->addItem("COM8");
    ui->portNameComboBox->addItem("COM9");
    ui->portNameComboBox->addItem("COM10");
    ui->portNameComboBox->addItem("COM11");
    ui->portNameComboBox->addItem("COM12");
    ui->portNameComboBox->addItem("COM13");
    ui->portNameComboBox->addItem("COM14");
    ui->portNameComboBox->addItem("COM15");

//#endif

    ui->statusBar->showMessage(tr("welcome!"));
}

MainWindow::~MainWindow()
{
    if(myCom != NULL){
        if(myCom->isOpen()){
            myCom->close();
        }
        delete myCom;
    }
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::startInit(){
    setActionsEnabled(false);
    ui->delayspinBox->setEnabled(false);
    ui->sendmsgBtn->setEnabled(false);
    ui->sendMsgLineEdit->setEnabled(false);
    ui->obocheckBox->setEnabled(false);
    ui->actionAdd->setEnabled(true);

    obotimerdly = OBO_TIMER_INTERVAL;

    obotimer = new QTimer(this);
    connect(obotimer, SIGNAL(timeout()), this, SLOT(sendMsg()));

}

void MainWindow::setActionsEnabled(bool status)
{
    ui->actionSave->setEnabled(status);
    ui->actionClose->setEnabled(status);
    ui->actionLoadfile->setEnabled(status);
    ui->actionCleanPort->setEnabled(status);
    ui->actionWriteToFile->setEnabled(status);
}

void MainWindow::setComboBoxEnabled(bool status)
{
    ui->portNameComboBox->setEnabled(status);
    ui->baudRateComboBox->setEnabled(status);
    ui->dataBitsComboBox->setEnabled(status);
    ui->parityComboBox->setEnabled(status);
    ui->stopBitsComboBox->setEnabled(status);
}

//打开串口
void MainWindow::on_actionOpen_triggered()
{
    QString portName = ui->portNameComboBox->currentText();
//#ifdef Q_OS_LINUX
//    myCom = new QextSerialPort("/dev/" + portName);
//#elif defined (Q_OS_WIN)
    myCom = new QextSerialPort(portName);
//#endif
    connect(myCom, SIGNAL(readyRead()), this, SLOT(readMyCom()));

    myCom->setBaudRate((BaudRateType)ui->baudRateComboBox->currentText().toInt());

    myCom->setDataBits((DataBitsType)ui->dataBitsComboBox->currentText().toInt());

    switch(ui->parityComboBox->currentIndex()){
    case 0:
         myCom->setParity(PAR_NONE);
         break;
    case 1:
        myCom->setParity(PAR_ODD);
        break;
    case 2:
        myCom->setParity(PAR_EVEN);
        break;
    default:
        myCom->setParity(PAR_NONE);
        qDebug("set to default : PAR_NONE");
        break;
    }


    switch(ui->stopBitsComboBox->currentIndex()){
    case 0:
        myCom->setStopBits(STOP_1);
        break;
    case 1:
 #ifdef Q_OS_WIN
        myCom->setStopBits(STOP_1_5);
 #endif
        break;
    case 2:
        myCom->setStopBits(STOP_2);
        break;
    default:
        myCom->setStopBits(STOP_1);
        qDebug("set to default : STOP_1");
        break;
    }    

    myCom->setFlowControl(FLOW_OFF);

    myCom->setTimeout(TIME_OUT);

    if(myCom->open(QIODevice::ReadWrite)){
        QMessageBox::information(this, tr("open successfully"), tr("serial port successfully opened ") + portName, QMessageBox::Ok);

		ui->sendmsgBtn->setEnabled(true);
        setComboBoxEnabled(false);

        ui->sendMsgLineEdit->setEnabled(true);

        ui->actionOpen->setEnabled(false);
        ui->sendMsgLineEdit->setFocus();
        ui->obocheckBox->setEnabled(true);
        ui->actionAdd->setEnabled(false);

        setActionsEnabled(true);
    }else{
        QMessageBox::critical(this, tr("open failed"), tr("Can not open serial port ") + portName + tr("\nPort is occupied or invalid..."), QMessageBox::Ok);
        return;
    }

    ui->statusBar->showMessage(tr("Serial port opened successfully"));
}


void MainWindow::on_actionClose_triggered()
{

    myCom->close();
    delete myCom;
    myCom = NULL;

    ui->sendmsgBtn->setEnabled(false);

    setComboBoxEnabled(true);

    ui->actionOpen->setEnabled(true);
    ui->sendMsgLineEdit->setEnabled(false);
    ui->obocheckBox->setEnabled(false);
    ui->actionAdd->setEnabled(true);

    setActionsEnabled(false);

    ui->actionWriteToFile->setChecked(false);
    ui->statusBar->showMessage(tr("port is closed"));
}

void MainWindow::on_actionAbout_triggered()
{
    aboutdlg.show();

    int x =this->x() + (this->width() - aboutdlg.width()) / 2;
    int y =this->y() + (this->height() - aboutdlg.height()) / 2;
    aboutdlg.move(x, y);
    ui->statusBar->showMessage(tr("About Wincom"));
}

void MainWindow::readMyCom()
{
    QByteArray temp = myCom->readAll();
    QString buf;

    if(!temp.isEmpty()){
            ui->textBrowser->setTextColor(Qt::black);
            if(ui->ccradioButton->isChecked()){
                buf = temp;
            }else if(ui->chradioButton->isChecked()){
                QString str;
                for(int i = 0; i < temp.count(); i++){
                    QString s;
                    s.sprintf("0x%02x, ", (unsigned char)temp.at(i));
                    buf += s;
                }
            }

        if(!write2fileName.isEmpty()){
            QFile file(write2fileName);

            if(!file.open(QFile::WriteOnly | QIODevice::Text)){
                QMessageBox::warning(this, tr("Write file"), tr("Open %1 failed, unable to write file\n%2").arg(write2fileName).arg(file.errorString()), QMessageBox::Ok);
                return;
            }
            QTextStream out(&file);
            out<<buf;
            file.close();
        }

        ui->textBrowser->setText(ui->textBrowser->document()->toPlainText() + buf);
        QTextCursor cursor = ui->textBrowser->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->textBrowser->setTextCursor(cursor);

        ui->recvbyteslcdNumber->display(ui->recvbyteslcdNumber->value() + temp.size());
        ui->statusBar->showMessage(tr("successfully read %1 bytes").arg(temp.size()));
    }
}

void MainWindow::sendMsg()
{
    QByteArray buf;
    if(ui->sendAsHexcheckBox->isChecked()){
        QString str;
        bool ok;
        char data;
        QStringList list;
        str = ui->sendMsgLineEdit->text();
        list = str.split(" ");
        for(int i = 0; i < list.count(); i++){
            if(list.at(i) == " ")
                continue;
            if(list.at(i).isEmpty())
                continue;
            data = (char)list.at(i).toInt(&ok, 16);
            if(!ok){
                QMessageBox::information(this, tr("message"), tr("format error"), QMessageBox::Ok);
                if(obotimer != NULL)
                    obotimer->stop();
                ui->sendmsgBtn->setText(tr("Send"));
                ui->sendmsgBtn->setIcon(QIcon(":new/prefix1/src/send.png"));
                return;
            }
            buf.append(data);
        }
    }else{
//#if QT_VERSION < 0x050000
//        buf = ui->sendMsgLineEdit->text().toAscii();
//#else
        buf = ui->sendMsgLineEdit->text().toLocal8Bit();
//#endif
    }

    myCom->write(buf);
    ui->statusBar->showMessage(tr("Send successfully"));

    ui->textBrowser->setTextColor(Qt::lightGray);
}

void MainWindow::on_sendmsgBtn_clicked()
{

    if(ui->sendmsgBtn->text() == tr("pause")){
        obotimer->stop();
        ui->sendmsgBtn->setText(tr("Send"));
        ui->sendmsgBtn->setIcon(QIcon(":new/prefix1/src/send.png"));
        return;
    }

    if(ui->sendMsgLineEdit->text().isEmpty()){
        QMessageBox::information(this, tr("message"), tr("no data to send"), QMessageBox::Ok);
        return;
    }


    if(!ui->obocheckBox->isChecked()){
        ui->sendMsgLineEdit->setFocus();

        sendMsg();
    }else{
        obotimer->start(obotimerdly);
        ui->sendmsgBtn->setText(tr("pause"));
        ui->sendmsgBtn->setIcon(QIcon(":new/prefix1/src/pause.png"));
    }
}

void MainWindow::on_clearUpBtn_clicked()
{
    ui->textBrowser->clear();
    ui->statusBar->showMessage(tr("records cleared"));
}



void MainWindow::on_actionClearBytes_triggered()
{
    if(ui->recvbyteslcdNumber->value() == 0){
        QMessageBox::information(this, tr("message"), tr("already cleared"), QMessageBox::Ok);
    }else{
        ui->recvbyteslcdNumber->display(0);
        ui->statusBar->showMessage(tr("Cleared"));
    }
}

void MainWindow::on_obocheckBox_clicked()
{
    if(ui->obocheckBox->isChecked()){
        ui->delayspinBox->setEnabled(true);

        ui->statusBar->showMessage(tr("repeatly sending"));
    }else{
        ui->delayspinBox->setEnabled(false);

        ui->statusBar->showMessage(tr("stop repeatly sending"));
    }
}


void MainWindow::on_actionSave_triggered()
{

    if(ui->textBrowser->toPlainText().isEmpty()){
        QMessageBox::information(this, "message", tr("edit message first"), QMessageBox::Ok);
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this, tr("Save as"), tr("data.txt"));
    QFile file(filename);

    if(file.fileName().isEmpty()){
        return;
    }

    if(!file.open(QFile::WriteOnly | QIODevice::Text)){
        QMessageBox::warning(this, tr("Save file"), tr("Cannot create %1, unable to save\n%2").arg(filename).arg(file.errorString()), QMessageBox::Ok);
        return;
    }

    QTextStream out(&file);
    out<<ui->textBrowser->toPlainText();
    file.close();

    setWindowTitle("saved: " + QFileInfo(filename).canonicalFilePath());

}


void MainWindow::on_actionExit_triggered()
{
    this->close();
}


void MainWindow::on_actionAdd_triggered()
{
    bool ok = false;
    QString portname;

    portname = QInputDialog::getText(this, tr("add serial port"), tr("Port name"), QLineEdit::Normal, 0, &ok);
    if(ok && !portname.isEmpty()){
        ui->portNameComboBox->addItem(portname);
        ui->statusBar->showMessage(tr("success"));
    }
}


void MainWindow::on_delayspinBox_valueChanged(int )
{
    obotimerdly = ui->delayspinBox->value();
}


void MainWindow::on_actionLoadfile_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("read"), 0, tr("save (*.txt *.log)"));\
    QFile file(filename);

    if(file.fileName().isEmpty()){
        return;
    }

    if(!file.open(QFile::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(this, tr("failed"),
                             tr("Sorry, Wincom failed."),
                             QMessageBox::Ok);
        return;
    }

    if(file.size() > MAX_FILE_SIZE){
        QMessageBox::critical(this, tr("load error"), tr("file size = %1 Bytes > 10000 Bytes").arg(file.size()), QMessageBox::Ok);
        return;
    }

    if(file.size() > TIP_FILE_SIZE){
        if(QMessageBox::question(this, tr("message"), tr("too big file, open anyway?"),
                                 QMessageBox::Yes | QMessageBox::No) == QMessageBox::No){

            return;
        }
    }

    QByteArray line;
    while(!file.atEnd()){
        line = file.readLine();
        ui->sendMsgLineEdit->setText(ui->sendMsgLineEdit->text() + tr(line));

    }
    file.close();
    ui->statusBar->showMessage(tr("success"));
}


void MainWindow::on_actionCleanPort_triggered()
{
    myCom->flush();
}

void MainWindow::on_actionWriteToFile_triggered()
{
    if(ui->actionWriteToFile->isChecked()){
        QString filename = QFileDialog::getSaveFileName(this, tr("write"), 0, tr("Save as (*.*)"));
        if(filename.isEmpty()){
            ui->actionWriteToFile->setChecked(false);
        }else{
            write2fileName = filename;
            //ui->textBrowser->setEnabled(false);
            ui->actionWriteToFile->setToolTip(tr("stop"));
        }
    }else{
        write2fileName.clear();
        //ui->textBrowser->setEnabled(true);
        ui->actionWriteToFile->setToolTip(tr("write to file"));
    }

}
