#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Создаём модель данных для отображения таблицы из CSV файла
    csvModel = new QStandardItemModel(this);
    ui->tbl->setModel(csvModel); // Устанавливаем модель в таблицу
    ui->tbl->setHorizontalScrollBar(ui->horizontalScrollBar);
    ui->tbl->setVerticalScrollBar(ui->verticalScrollBar);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete csvModel;
}


void MainWindow::on_btn_data_clicked()
{
     QFile file(ui->lbl_fileName->text());
     if ( !file.open(QFile::ReadOnly | QFile::Text) ) {
        QMessageBox::warning(this, "Error", "File not exists");
     } else {
        // Создаём поток для извлечения данных из файла
        QTextStream in(&file);
        // Считываем данные до конца файла
        while (!in.atEnd())
        {
            QString line = in.readLine();
            // Добавляем в модель по строке с элементами
            QList<QStandardItem *> standardItemsList;
            // учитываем, что строка разделяется запятой на колонки
            for (QString item : line.split(",")) {
                standardItemsList.append(new QStandardItem(item));
             }
             csvModel->insertRow(csvModel->rowCount(), standardItemsList);
         }
         file.close();
     }
     row_count = csvModel->rowCount();
     col_count = csvModel->columnCount();

     /*char *file_name=(char *)malloc(STR_LEN);
     QString temp = ui->lbl_fileName->text();
     QByteArray ba=temp.toLatin1();
     strcpy(file_name,ba.data());*/

     ui->comb_column->addItems(columns());
     ui->comb_region->addItems(regions());

     csvModel->removeRow(0);
     csvModel->setHorizontalHeaderLabels(headers);
}

QStringList MainWindow::regions(){
    QStringList regions = {};
    for (size_t i = 0; i < (size_t)row_count; i++)
    {
        QStandardItem *item = csvModel->item(i,1);
        regions.append(item->text());
    }
    regions.removeDuplicates();
    return regions;
}


QStringList MainWindow::columns(){
    for(size_t i = 0; i < (size_t)col_count; i++){
        QStandardItem *item = csvModel->item(0, i);
        headers.append(item->text());
        ind_colums.append(QString::number(i));
    }
    return headers;
}


void MainWindow::on_btn_file_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(0, "Open", "", "*.csv");
    ui->lbl_fileName->setText(fileName);
}


void MainWindow::on_btn_metrics_clicked(){
    QString selected_region = ui->comb_region->currentText();
    int column_num = ui->comb_column->currentIndex();
    csvModel->clear();
    csvModel->setHorizontalHeaderLabels(headers);

    QFile file(ui->lbl_fileName->text());
    if ( !file.open(QFile::ReadOnly | QFile::Text) ) {
       QMessageBox::warning(this, "Error", "File not exists");
    } else {
       // Создаём поток для извлечения данных из файла
       QTextStream in(&file);
       // Считываем данные до конца файла
       while (!in.atEnd())
       {
           QString line = in.readLine();
           if (line.contains(selected_region)){
               // Добавляем в модель по строке с элементами
               QList<QStandardItem *> standardItemsList;
               // учитываем, что строка разделяется запятой на колонки
               for (QString item : line.split(",")){
                   standardItemsList.append(new QStandardItem(item));
                }
                csvModel->insertRow(csvModel->rowCount(), standardItemsList);
           }
        }
        row_count = csvModel->rowCount();
        file.close();
     }

    getData(column_num);
    drawGraph();
    calc_max(column_num);
    calc_min(column_num);
    calc_med();
}

// метод вычисляет середину экрана и пересчитывает количество пикселей на шаг
void MainWindow::recountPixels()
{
    onePixelX = 480 / (rightX-leftX);
    onePixelY = 230 / (upY-downY);
    Ox = fabs(leftX);
    Oy = upY;
}

void MainWindow::getData(int col_ind){
    nums = (double*)calloc(row_count, sizeof(double));
    if (nums != NULL){
        for (int i = 0; i < row_count; i++){
            auto res = csvModel->item(i, col_ind)->text();
            bool isNumber;
            res.toDouble(&isNumber);
            if (isNumber){
                double ress = res.toDouble();
                *(nums + len_nums) = ress;
                len_nums ++;
            }
        }
    }else
        QMessageBox::critical(this, "Error", "Error with memory allocation!");

    years = (int*)calloc(row_count, sizeof(int));
    if (nums != NULL){
        for (int i = 0; i < row_count; i++){
            auto res = csvModel->item(i, 0)->text();
            bool isNumber;
            res.toInt(&isNumber);
            if (isNumber){
                double ress = res.toInt();
                *(nums + len_nums) = ress;
                len_years ++;
            }
        }
    }else
        QMessageBox::critical(this, "Error", "Error with memory allocation!");
}

void MainWindow::drawGraph(bool notEmpty){
    pictHeight = 230; // задаем высоту картинки
    pictWidth = 480; // и её ширину
    step = 5; // задаем начальный шаг
    leftX = 0;
    rightX = 430; // и начальные значения границ
    leftY = 0;
    rightY = 200;


    QPixmap graph(pictWidth, pictHeight); // создаем саму картинку
    QPainter paint; // и пэинтер
    paint.begin(&graph); // запускаем отрисовку
    paint.eraseRect(0,0,400,210); // очищаем рисунок
    paint.drawLine(0, pictHeight/2, pictWidth, pictHeight/2); // ox
    paint.drawLine(0, 0, 0, pictHeight); // oy

    paint.setPen(QPen(Qt::black,3));
    for(double i = 0;i <= 400;i+=10) // рисуем черточки на координатой оси
        paint.drawPoint(i + 10, pictHeight/2);
    for(double i = downY;i<=upY;i+=10)
        paint.drawPoint(Ox*onePixelX,(i+Oy)*onePixelY);

    double mX, mY;
    mX = 10;
    paint.setPen (QPen(QColor(0,100,50,255), 4));
    QList points = {QPoint(0, pictHeight / 2 - (*(nums + 0)))};
    for (int i = 1; i < len_years; i++)
        points.append(QPoint(mX * i, pictHeight / 2 - *(nums + i)));
    paint.drawPoints(points);

    paint.end(); // заканчиваем рисование
    ui->graphic->setPixmap(graph); // и помещаем рисунок на форму

}

void MainWindow::calc_max(int col_ind){
    double max_value = csvModel->item(0, col_ind)->text().toDouble();
    for (int i = 1; i < row_count; i++){
        double elem = csvModel->item(i, col_ind)->text().toDouble();
        if (csvModel->item(i, col_ind)->text() != "" && elem > max_value)
            max_value = elem;
    }
    ui->lbl_max->setText(QString::number(max_value));
}

void MainWindow::calc_min(int col_ind){
    double min_value = csvModel->item(0, col_ind)->text().toDouble();
    int i = 1;
    while (i < row_count && min_value == 0){
        min_value = csvModel->item(i, col_ind)->text().toDouble();
        i++;
    }
    for (int j = i; j < row_count; j++){
        double elem = csvModel->item(j, col_ind)->text().toDouble();
        if (csvModel->item(j, col_ind)->text() != "" && elem < min_value)
            min_value = elem;
    }
    ui->lbl_min->setText(QString::number(min_value));
}

void MainWindow::calc_med(){
    nums = (double*)realloc(nums, len_nums * sizeof(double));
    if (nums != NULL){
        for (int i = 0; i < len_nums - 1; i++)
            for (int j = i+1; j < len_nums; j++){
                double temp_i = *(nums + i);
                *(nums + i) = *(nums + j);
                *(nums + j) = temp_i;
            }
        double med;
        if (len_nums % 2)
            med = *(nums + len_nums/2);
        else
            med = (*(nums + len_nums/2 - 1) + *(nums + len_nums/2))/2;

        ui->lbl_med->setText(QString::number(med));
        free(nums);
    }else
        QMessageBox::critical(this, "Error", "Error with memory allocation!");
}





//void MainWindow::paint_event(QPaintEvent *event){
//    QPixmap *pix = new QPixmap(500, 500);
//    QPainter paint(pix);
//    //paint.fillRect(0, 0, 500, 500, QBrush(QColor(Qt::GlobalColor::white)));
//    //paint.setPen(*(new QColor(255, 34, 255, 255)));
//    //paint.drawRect(15, 15, 100, 100);
//    //paint.setPen(QColor(Qt::GlobalColor::red));
//    //paint.drawLine(0, 180, 100, 0);
//    //paint.drawPoint(450, 450);
//    //paint.drawText(250, 200, "Red text");
//    ui->graphic->setPixmap(*pix);

//    double years[28];
//    for (int i = 1990; i < 2018; i++)
//        *(years + i - 1990) = i;

//    double values[28] = {1, 2, 3, 7, 10, 11, 23, 0, 42, 34, 22, 51, 1, 2, 3, 14, 8, 3, 44, 20, 21, 50, 34, 2, 33, 48};
//    const double stepX = 10;
//    const double stepY = 30;
//    const double startX = 50;
//    const double startY = 30 * stepY + 100;

//    paint.setPen(QColor(255, 34, 255));
//    paint.drawLine(startX, startY, startX, startY - (30 * stepY + 50));
//}
