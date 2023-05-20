#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "buisenesslogic.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_btn_choseFileName_clicked();

    void on_btn_Load_data_clicked();

    void on_btn_calc_metrics_clicked();

    void on_box_region_currentTextChanged();

    void on_box_column_currentTextChanged();

private:
    Ui::MainWindow *ui;
    bool key;
    void showData(FuncReturningValue* frv);
    void clean2DArray(char **arr, size_t size);
    void clean3DArray(char ***arr, size_t sizeX, size_t sizeY);
    void draw(FuncReturningValue*frv);
    char*** getDataFromTable();
    size_t calculateColumns(size_t);
    QStringList getColumns();
    QStringList getRegions();
    char*QstringToCharArray(QString qstr);
    QStringList ConvertRowToQTFormat(char **row, size_t size);
    QStringList getYears();
    void clearTable();
    void setLblText();
};

#endif // MAINWINDOW_H
