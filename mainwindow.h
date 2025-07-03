#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QMap<QString, QStringList> m_dataMap;

private slots:
    void on_category_currentTextChanged(const QString &text);
    void on_addStage_clicked();
    void on_deleteItem_clicked();

};
#endif // MAINWINDOW_H
