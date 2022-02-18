#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QFileDialog>
#include <QDebug>


#include "mser/mser_process.h"
#include "draw_ellipse.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    set_connect();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::set_connect()
{
    connect( ui->loadImageButton, &QPushButton::clicked, this, &MainWindow::load_image_slot );
    connect( ui->pushButton, &QPushButton::clicked, this, &MainWindow::draw_ellipse_slot );
}



void MainWindow::draw_ellipse_slot()
{
    QString filename = QFileDialog::getOpenFileName( this, tr("Open Image"), ".", tr("Image Files (*.png *.jpg *.bmp)") );

    qDebug() << filename;

    QImage img(filename);
    draw_ellipse( img );
    
    label.setPixmap(QPixmap::fromImage(img));
    label.show();

}




void MainWindow::load_image_slot()
{
    QString filename = QFileDialog::getOpenFileName( this, tr("Open Image"), ".", tr("Image Files (*.png *.jpg *.bmp)") );

    qDebug() << filename;

    QImage img = mser_process( filename.toStdString().c_str() );
    
    label.setPixmap(QPixmap::fromImage(img));
    label.show();
}
