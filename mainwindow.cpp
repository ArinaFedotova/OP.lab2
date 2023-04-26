#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){
    ui->setupUi(this);
    this->setWindowTitle("Csv files");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_choseFileName_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Choose file", "", "*.csv");
    key=0;
    ui->lbl_filename->setText(filename);
    ui->btn_Load_data->setEnabled(true);
    ui->btn_calc_metrics->setEnabled(false);
    ui->box_column->clear();
    ui->box_region->clear();
    ui->tb_widget->setColumnCount(0);
    ui->tb_widget->setRowCount(0);
    ui->tb_widget->clearContents();
    ui->lbl_min->setText("Min value: ");
    ui->lbl_max->setText("Max value: ");
    ui->lbl_median->setText("Median value: ");
    if (filename.isEmpty())
        QMessageBox::critical(this,"Error","You should chose file");
}

void MainWindow::on_btn_Load_data_clicked()
{
    QString filename=ui->lbl_filename->text();
    if (!filename.isEmpty()){
        FuncArgument fa = {.filename = QstringToCharArray(filename)};
        FuncReturningValue* frv = entryPoint(getData, &fa);
        if (frv->error == FILE_OPEN_ERROR)
            QMessageBox::information(this,"Error","There are problems with opening the file");
        else{
            showData(frv);
            FuncArgument fa2 = {
                .filename = fa.filename,
                .data = frv->data,
                .headers = frv->headers,
                .len = frv->len,
                .fields_num = frv->fields_num
            };
            key=1;
            entryPoint(cleanData, &fa2);
            free(frv);
        }
        ui->btn_Load_data->setEnabled(false);
        ui->btn_calc_metrics->setEnabled(true);
    }
}

void MainWindow::on_btn_calc_metrics_clicked()
{
    on_btn_Load_data_clicked();
    size_t index_of_column=(size_t)ui->box_column->currentIndex();
    FuncArgument fa = {
            .filename=QstringToCharArray(ui->lbl_filename->text()),
            .data = getDataFromTable(),
            .region=QstringToCharArray(ui->box_region->currentText()),
            .column=index_of_column+calculateColumns(index_of_column),
            .len = (size_t)ui->tb_widget->rowCount(),
            .fields_num = (size_t)ui->tb_widget->columnCount(),
            .region_number=(size_t)ui->box_region->currentIndex(),
            .region_index_at_header=(size_t)headers.indexOf("region")
    };
    FuncReturningValue* frv = entryPoint(calculateData, &fa);
    showData(frv);
    if (frv->error != CALCULATE_ERROR){
        ui->lbl_min->setText("Min value: " + QString::number(frv->solution_min));
        ui->lbl_max->setText("Max value: " + QString::number(frv->solution_max));
        ui->lbl_median->setText("Median value: " + QString::number(frv->solution_median));
        drawGraph(true);
        entryPoint(cleanData, &fa);
    }
    else{
        QMessageBox::critical(this,"Error","It's impossible to calculate metrics for this column");
        ui->lbl_min->setText("Min value: ");
        ui->lbl_max->setText("Max value: ");
        ui->lbl_median->setText("Median value: ");
    }
    free(frv);
}

void MainWindow::on_box_region_currentTextChanged()
{
    if (key==1)
    {
        ui->tb_widget->setColumnCount(0);
        ui->tb_widget->setRowCount(0);
        ui->tb_widget->clearContents();
        ui->lbl_min->setText("Min value: ");
        ui->lbl_max->setText("Max value: ");
        ui->lbl_median->setText("Median value: ");
    }
}


void MainWindow::on_box_column_currentTextChanged()
{
    if (key==1)
    {
        ui->lbl_min->setText("Min value: ");
        ui->lbl_max->setText("Max value: ");
        ui->lbl_median->setText("Median value: ");
    }
}

void MainWindow::drawGraph(bool notEmpty){
    QStringList years = getYears();
    QStringList nums = getNums();

    int pictHeight = 350; // задаем высоту картинки
    int pictWidth = 370; // и её ширину
    int leftX = years[0].toInt();
    int rightX = years[years.length() - 1].toInt(); // и начальные значения границ

    QPixmap graph(pictWidth, pictHeight); // создаем саму картинку
    QPainter paint; // и пэинтер
    paint.begin(&graph); // запускаем отрисовку
    paint.eraseRect(0,0,370,350); // очищаем рисунок
    paint.drawLine(0, pictHeight/2, pictWidth, pictHeight/2); // ox
    paint.drawLine(0, 0, 0, pictHeight); // oy

    double mX = pictWidth / (rightX - leftX);
    double mY = pictHeight / (ui->lbl_max->text().toDouble() - ui->lbl_min->text().toDouble());
    double h = 1/mX;

    paint.setPen(QPen(Qt::black,3));
    for(double i = 0; i <= pictWidth; i += mX) // рисуем черточки на координатой оси
        paint.drawPoint(i, pictHeight/2);
    for(double i = 0; i <= pictHeight; i += mY) // рисуем черточки на координатой оси
        paint.drawPoint(0, i);

    paint.setPen (QPen(QColor(0,100,50,255), 4));
    QList points = {QPoint(0, pictHeight / 2 - nums[0].toInt())};
    double xCur, yCur;
    double x = 0;
    for (int i = 0; i < nums.length(); i++){
        if (x >= years.length() + h/2)
            break;
        xCur = mX * x;
        yCur = pictHeight - mY*(nums[i].toDouble() - ui->lbl_min->text().toDouble());
        points.append(QPoint(xCur, yCur));
        x+=h;
    }
    paint.drawPoints(points);

    paint.end(); // заканчиваем рисование
    ui->graphic->setPixmap(graph); // и помещаем рисунок на форму

}
