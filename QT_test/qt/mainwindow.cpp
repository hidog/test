#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QString>
#include <QImage>
#include <QPainter>

#include "qrencode.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    bool	check	=	true;
    check	&=	(bool)(		connect( ui->testButton, SIGNAL( clicked() ), this, SLOT( generate_qr_code()) )     );
    assert(check);
}





MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::generate_qr_code()
{
    QString str = ui->lineEdit->text();
    //QMessageBox::warning( this, QString(tr("test")), str );

    QRcode *qr = QRcode_encodeString( str.toStdString().c_str(), 1, QR_ECLEVEL_M, QR_MODE_8, 1 );

    if( qr != NULL )
    {
        QImage		img( 300, 300, QImage::Format_RGB888 );
        QPainter	painter(&img);

        QColor		foreground("black");
        QColor		background("white");

        painter.setBrush( background );
        painter.setPen( Qt::NoPen );
        painter.drawRect( 0, 0, img.width(), img.height() );
        painter.setBrush( foreground );

        const int		s	=	qr->width > 0 ? qr->width : 1;
        const double	w	=	img.width();
        const double	h	=	img.height();
        const double	aspect	=	w / h;
        const double	scale	=	( (aspect > 1.0) ? h : w) / s;

        // 
        for( int y = 0; y < s; y++ )
        {
            const int	yy	=	y * s;	// temp

            for( int x = 0; x < s; x++ )
            {
                const int	xx	=	yy + x;
                const unsigned char	b	=	qr->data[xx];
                if( b & 0x01 )
                {
                    const double	rx1	=	x * scale, 
                        ry1	=	y * scale;
                    QRectF	r( rx1, ry1, scale, scale );
                    painter.drawRects( &r, 1 );
                }
            }
        }

#ifdef _DEBUG
        img.save("QRcode.bmp");
#endif

        qr_img	=	img;
        //ui->qrView->setImage(qr_img);
        ui->qrLabel->setPixmap(QPixmap::fromImage(qr_img));

        QRcode_free(qr);
        qr	=	NULL;
    }
    else
    {
        assert(false);
    }

}
