#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QTabWidget>

#include "RawDataConvertDialog.h"

#define VERSION "V1.0.0"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QWidget* m_mainWidget;
    QTabWidget* m_tabWidget;

    RawDataConvertDialog* m_rawData_convert;

    void constructUI();
    void connectSlots();
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
