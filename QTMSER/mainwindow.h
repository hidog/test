#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void set_connect();    

public slots:

    void load_image_slot();
    void draw_ellipse_slot();

private:
    Ui::MainWindow *ui;
    
    QLabel label;
};
#endif // MAINWINDOW_H
