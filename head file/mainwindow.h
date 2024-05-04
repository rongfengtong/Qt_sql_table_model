#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QDataWidgetMapper>
#include "qwcomboboxdelegate.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QSqlDatabase DB;
    QSqlTableModel *tab_model;
    QItemSelectionModel *sel_model;

    QWComboBoxDelegate delegate_sex;
    QWComboBoxDelegate delegate_depart;

    QDataWidgetMapper *data_mapper;

    void open_table();

private slots:
    void do_current_changed(const QModelIndex &current, const QModelIndex &previous);
    void do_current_row_changed(const QModelIndex &current, const QModelIndex &previous);


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actOpenDB_triggered();

    void on_actRecAppend_triggered();

    void on_actRecInsert_triggered();

    void on_actRecDelete_triggered();

    void on_actPhoto_triggered();

    void on_actPhotoClear_triggered();

    void on_actScan_triggered();

    void on_actSubmit_triggered();

    void on_actRevert_triggered();

    void on_comboFields_currentIndexChanged(int index);

    void on_radioBtnAscend_clicked();

    void on_radioBtnDescend_clicked();

    void on_radioBtnMan_clicked();

    void on_radioBtnWoman_clicked();

    void on_radioBtnBoth_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
