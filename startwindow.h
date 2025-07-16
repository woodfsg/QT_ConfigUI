#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include "mainwindow.h"

namespace Ui {
class StartWindow;
}

class StartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = nullptr);
    ~StartWindow();

private:
    Ui::StartWindow *ui;
    MainWindow *m_mainWindow;
    QStringList m_categories; // 存储类别列表

    void init_programTable();
    void loadCategories(); // 从配置文件加载类别
    void saveCategories(); // 保存类别到配置文件
    void updateComboBox(); // 更新下拉框显示

private slots:
    void on_editProgramPushButton_clicked();
    void on_addTypePushButton_clicked();
    void on_deleteCurrentTypePushButton_clicked();
    void handleMainWindowClosed();
};

#endif // STARTWINDOW_H
