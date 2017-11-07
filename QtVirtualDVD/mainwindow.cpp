#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QFileDialog>
#include <QString>
#include <QStringList>
#include <QProcess>
//#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createTray();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    this->hide();
}

void MainWindow::mountImage()
{
    //Get username
    QString name = qgetenv("USER");
    if (name.isEmpty())
        name = qgetenv("USERNAME");

    //select image
    QFileDialog dialog(this,
                       "Mount Image File",
                       "/home/"+name,
                       "CD/DVD Image Files (*.iso *.bin *.nrg *.mdf *.img)"
                        );

    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    //find fuseiso
    QString which = "/usr/bin/which";
    QStringList arguments_w;

    arguments_w << "fuseiso";

    QProcess *myProcessw = new QProcess(this);
    myProcessw->start(which, arguments_w);

    //get output from QProcess
    myProcessw->waitForFinished(); // sets current thread to sleep and waits for pingProcess end
    QString output(myProcessw->readAllStandardOutput());

    output = output.left(output.length()-1);

    //mount image
    QString program = output;
    QStringList arguments;
    arguments << "-p" << fileNames[0] << "/home/"+name+"/Desktop/QtVirtualDVD";

    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);

    //avoid quit app
    this->show();
    this->hide();

}
void MainWindow::umountImage()
{
    //get username
    QString name = qgetenv("USER");
    if (name.isEmpty())
        name = qgetenv("USERNAME");

    //find fusermount
    QString which = "/usr/bin/which";
    QStringList arguments_w;

    arguments_w << "fusermount";

    QProcess *myProcessw = new QProcess(this);
    myProcessw->start(which, arguments_w);

    //get output
    myProcessw->waitForFinished(); // sets current thread to sleep and waits for pingProcess end
    QString output(myProcessw->readAllStandardOutput());

    output = output.left(output.length()-1);

    //umount image
    QString program = output;
    QStringList arguments;
    arguments << "-u" << "/home/"+name+"/Desktop/QtVirtualDVD";

    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
}

void MainWindow::createTray(){

    //create tray
    tray= new QSystemTrayIcon(this);

    //setup tray icon
    QPixmap icon("://drive-optical-300px.png");
    trayIcon= QIcon(icon);
    tray->setIcon(trayIcon);
    tray->setToolTip("QtVirtualDVD");

    //setup quit
    closeAction = new QAction(QIcon("://exit.png"), "Quit", this);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
    //setup mount
    mountAction = new QAction(QIcon("://mono-dvd-mount-300px.png"), "Mount Image", this);
    connect(mountAction, SIGNAL(triggered()), this, SLOT(mountImage()));
    //setup umount
    umountAction = new QAction(QIcon("://mono-dvd-umount-300px.png"), "Unmount Image", this);
    connect(umountAction, SIGNAL(triggered()), this, SLOT(umountImage()));

    //create system tray menu
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(mountAction);
    trayIconMenu->addAction(umountAction);
    trayIconMenu->addAction(closeAction);
    tray->setContextMenu(trayIconMenu);

    tray->setVisible(true);
}
