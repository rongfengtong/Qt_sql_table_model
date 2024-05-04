#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>


//打开数据库表格
void MainWindow::open_table()
{
    //创建模型，打开数据库表格
    tab_model = new QSqlTableModel(this, DB);
    tab_model->setTable("employee");
    tab_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    tab_model->setSort(tab_model->fieldIndex("empNo"), Qt::AscendingOrder);

    if(!(tab_model->select())){
        QMessageBox::critical(this, "错误", "数据库表格打开失败: " +tab_model->lastError().text());
    }
    ui->statusbar->showMessage(QString("记录条数为: %1").arg(tab_model->rowCount()));

    //修改表头标题
    tab_model->setHeaderData(tab_model->fieldIndex("empNo"), Qt::Horizontal, "工号");
    tab_model->setHeaderData(tab_model->fieldIndex("Name"), Qt::Horizontal, "姓名");
    tab_model->setHeaderData(tab_model->fieldIndex("Gender"), Qt::Horizontal, "性别");
    tab_model->setHeaderData(tab_model->fieldIndex("Birthday"), Qt::Horizontal, "出生日期");
    tab_model->setHeaderData(tab_model->fieldIndex("Province"), Qt::Horizontal, "省份");
    tab_model->setHeaderData(tab_model->fieldIndex("Department"), Qt::Horizontal, "部门");
    tab_model->setHeaderData(tab_model->fieldIndex("Salary"), Qt::Horizontal, "工资");
    // tab_model->setHeaderData(tab_model->fieldIndex("Memo"), Qt::Horizontal, "备注");
    // tab_model->setHeaderData(tab_model->fieldIndex("Photo"), Qt::Horizontal, "照片");

    sel_model = new QItemSelectionModel(tab_model, this);

    ui->tableView->setModel(tab_model);
    ui->tableView->setSelectionModel(sel_model);
    ui->tableView->setColumnHidden(tab_model->fieldIndex("Memo"), true);
    ui->tableView->setColumnHidden(tab_model->fieldIndex("Photo"), true);


    //设置下拉代理
    QStringList str_list;
    str_list<<"男"<<"女";
    delegate_sex.setItems(str_list, false);
    ui->tableView->setItemDelegateForColumn(tab_model->fieldIndex("Gender"), &delegate_sex);

    str_list.clear();
    str_list << "技术部" <<"销售部"<<"市场部";
    delegate_depart.setItems(str_list, false);
    ui->tableView->setItemDelegateForColumn(tab_model->fieldIndex("Department"), &delegate_depart);

    //字段与widget映射
    data_mapper = new QDataWidgetMapper(this);
    data_mapper->setModel(tab_model);
    data_mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);    //数据的提交方式

    data_mapper->addMapping(ui->dbSpinEmpNo, tab_model->fieldIndex("embpNo"));
    data_mapper->addMapping(ui->dbEditName, tab_model->fieldIndex("Name"));
    data_mapper->addMapping(ui->dbComboSex, tab_model->fieldIndex("Gender"));
    data_mapper->addMapping(ui->dbEditBirth, tab_model->fieldIndex("Birthday"));
    data_mapper->addMapping(ui->dbComboProvince, tab_model->fieldIndex("Province"));
    data_mapper->addMapping(ui->dbComboDep, tab_model->fieldIndex("Department"));
    data_mapper->addMapping(ui->dbSpinSalary, tab_model->fieldIndex("Salary"));
    data_mapper->addMapping(ui->dbEditMemo, tab_model->fieldIndex("Memo"));
    data_mapper->addMapping(ui->dbSpinHeight, tab_model->fieldIndex("Height"));
    data_mapper->addMapping(ui->dbEditMobile, tab_model->fieldIndex("Mobile"));
    data_mapper->addMapping(ui->dbComboEdu, tab_model->fieldIndex("Education"));
    data_mapper->addMapping(ui->dbEditCity, tab_model->fieldIndex("City"));
    data_mapper->toFirst();     //初始化展示第一条记录

    //action状态 点击
    ui->actOpenDB->setEnabled(false);
    ui->actRecAppend->setEnabled(true);
    ui->actRecDelete->setEnabled(true);
    ui->actRecInsert->setEnabled(true);
    ui->actRevert->setEnabled(true);
    ui->actScan->setEnabled(true);
    ui->groupBoxFilter->setEnabled(true);
    ui->groupBoxSort->setEnabled(true);

    //更新 排序字段comboFields
    QSqlRecord empty_rec = tab_model->record();
    for(int i=0; i<empty_rec.count(); ++i){
        ui->comboFields->addItem(empty_rec.fieldName(i));
    }

    connect(sel_model, &QItemSelectionModel::currentChanged, this, &MainWindow::do_current_changed);
    connect(sel_model, &QItemSelectionModel::currentRowChanged, this, &MainWindow::do_current_row_changed);

}

void MainWindow::do_current_changed(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(current);Q_UNUSED(previous);
    ui->actSubmit->setEnabled(tab_model->isDirty());
    ui->actRevert->setEnabled(tab_model->isDirty());
}

void MainWindow::do_current_row_changed(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if(!current.isValid()){
        ui->dbLabPhoto->clear();
        return;
    }
    ui->actPhoto->setEnabled(true);
    ui->actPhotoClear->setEnabled(true);

    //修改映射
    data_mapper->setCurrentIndex(current.row());

    //显示照片
    QSqlRecord cur_rec = tab_model->record(current.row());
    if(cur_rec.isNull("Photo")){
        ui->dbLabPhoto->clear();
    }
    else{
        QPixmap pic;
        pic.loadFromData(cur_rec.value("Photo").toByteArray());
        ui->dbLabPhoto->setPixmap(pic.scaledToWidth(ui->dbLabPhoto->size().width()));
    }
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->splitter);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setAlternatingRowColors(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actOpenDB_triggered()
{
    QString aFile = QFileDialog::getOpenFileName(this,"选择文件","","SQLite数据库(*.db3)");
    if(aFile.isEmpty())
        return;

    DB = QSqlDatabase::addDatabase("QSQLITE");  //添加驱动
    DB.setDatabaseName(aFile);
    if(!DB.open())
        QMessageBox::warning(this, "错误", "数据库打开失败");
    else
        open_table();

}


void MainWindow::on_actRecAppend_triggered()
{
    QSqlRecord empty_rec = tab_model->record();
    empty_rec.setValue(tab_model->fieldIndex("Gender"), "男");
    tab_model->insertRecord(tab_model->rowCount(), empty_rec);

    sel_model->clearSelection();
    sel_model->setCurrentIndex(tab_model->index(tab_model->rowCount()-1, 1),
                               QItemSelectionModel::Select);

    ui->statusbar->showMessage(QString("记录条数为: %1").arg(tab_model->rowCount()));

}


void MainWindow::on_actRecInsert_triggered()
{
    QSqlRecord rec = tab_model->record();
    rec.setValue(tab_model->fieldIndex("Gender"), "女");
    QModelIndex cur_index = ui->tableView->currentIndex();
    tab_model->insertRecord(cur_index.row(), rec);

    sel_model->clearSelection();
    sel_model->setCurrentIndex(cur_index, QItemSelectionModel::Select);
    ui->statusbar->showMessage(QString("记录条数为: %1").arg( tab_model->rowCount()) );
}


void MainWindow::on_actRecDelete_triggered()    //删除行
{
    QModelIndex index = ui->tableView->currentIndex();
    tab_model->removeRow(index.row());

    ui->statusbar->showMessage(QString("记录条数为: %1").arg(tab_model->rowCount()));
}


void MainWindow::on_actPhoto_triggered()
{
    QString file_str = QFileDialog::getOpenFileName(this,"选择照片", "", "照片(*.jpg)");
    if(file_str.isEmpty())
        return;

    QFile *file = new QFile(file_str);
    if(file->open(QIODevice::ReadOnly)){
        QByteArray data = file->readAll();
        file->close();
        delete file;

        // QSqlRecord cur_rec = tab_model->record(sel_model->currentIndex().row());
        QSqlRecord cur_rec = tab_model->record(ui->tableView->currentIndex().row());
        cur_rec.setValue("Photo", data);

        //更新model当前行的数据
        tab_model->setRecord(ui->tableView->currentIndex().row(), cur_rec);

        //label中放入照片要用Pixmap格式
        QPixmap pix;
        pix.load(file_str);
        ui->dbLabPhoto->setPixmap(pix.scaledToWidth(ui->dbLabPhoto->width()));
    }
}


void MainWindow::on_actPhotoClear_triggered()
{
    QSqlRecord rec = tab_model->record(ui->tableView->currentIndex().row());
    rec.setNull("Photo");
    tab_model->setRecord(ui->tableView->currentIndex().row(), rec);
    ui->dbLabPhoto->clear();
}


void MainWindow::on_actScan_triggered()
{
    if(tab_model->rowCount() == 0) return;

    for(int i=0; i<tab_model->rowCount(); ++i){
        QSqlRecord rec = tab_model->record(i);
        rec.setValue("Salary", rec.value("Salary").toFloat() *2);

        tab_model->setRecord(i, rec);
    }

    if(tab_model->submitAll())
        QMessageBox::information(this, "消息", "涨工资操作完毕");
}


void MainWindow::on_actSubmit_triggered()
{
    bool res = tab_model->submitAll();
    if(!res){
        QMessageBox::warning(this, "警告", "保存失败\n"+tab_model->lastError().text());
    }
    else{
        ui->actSubmit->setEnabled(false);
        ui->actRevert->setEnabled(false);
    }
}


void MainWindow::on_actRevert_triggered()
{
    tab_model->revertAll();
    ui->actSubmit->setEnabled(false);
    ui->actRevert->setEnabled(false);
}


void MainWindow::on_comboFields_currentIndexChanged(int index)
{
    if(ui->radioBtnAscend->isChecked())
        tab_model->setSort(index, Qt::AscendingOrder);
    else if(ui->radioBtnDescend->isChecked())
        tab_model->setSort(index, Qt::DescendingOrder);

    tab_model->select();    //用的是setSort()，设置完要刷新写入,或者可以直接用sort()
}

void MainWindow::on_radioBtnAscend_clicked()
{
    tab_model->sort(ui->comboFields->currentIndex(), Qt::AscendingOrder);
}

void MainWindow::on_radioBtnDescend_clicked()
{
    tab_model->sort(ui->comboFields->currentIndex(), Qt::DescendingOrder);
}


void MainWindow::on_radioBtnMan_clicked()
{
    tab_model->setFilter("Gender='男'");
    ui->statusbar->showMessage(QString("记录条数为: %1").arg(tab_model->rowCount()));
}

void MainWindow::on_radioBtnWoman_clicked()
{
    tab_model->setFilter("Gender='女'");
    ui->statusbar->showMessage(QString("记录条数为: %1").arg(tab_model->rowCount()));
}

void MainWindow::on_radioBtnBoth_clicked()
{
    tab_model->setFilter("");
    ui->statusbar->showMessage(QString("记录条数为: %1").arg(tab_model->rowCount()));
}









