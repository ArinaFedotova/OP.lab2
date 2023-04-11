#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define SIZE 1024
#define STR_LEN 512

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <QMainWindow>
#include <QStandardItemModel>
#include <QTableView>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QPainter>
#include <QDebug>
#include <QtGui>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void drawGraph(bool notEmpty = 0); // функция, отвечающая за отрисовку графика
    void recountPixels(); // в зависимости от точности (об этом позже) считаем количество пикселей на один условный сантиметр
    void getData(int col_ind); // получаем значения границ графика и точности

private slots:
   void on_btn_data_clicked();
   void on_btn_file_clicked();
   void on_btn_metrics_clicked();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *csvModel; //Объявим указатель на модель данных, которая будет содержать данные из CSV файла

    int row_count, col_count;

    QStringList ind_colums;
    QStringList headers;

    QStringList columns();
    QStringList regions();

    void calc_max(int col_ind);
    void calc_min(int col_ind);
    void calc_med();

    double *nums;
    int len_nums = 0;
    int *years;
    int len_years = 0;

    double leftX,rightX; // границы по х
    double leftY, rightY;; // границы по у
    int pictWidth,pictHeight; // ширина и высота картинки
    double step; // шаг (точность)
    double onePixelX,onePixelY; // количество пикселей на шаг
    double Ox,Oy; // координаты центра

};
#endif // MAINWINDOW_H
