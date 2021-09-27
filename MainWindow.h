#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QTabWidget>

#include "RawDataConvertDialog.h"
#include "ConvertBinWithHFile.h"
#include "DolbyDialog.h"

#define VERSION "V1.0.1"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QWidget* m_mainWidget;
    QTabWidget* m_tabWidget;

    RawDataConvertDialog* m_rawDataConvert;
    ConvertBinWithHFile* m_convertBinWithHFile;
    DolbyDialog* m_dolbyDialog;

    void constructUI();
    void connectSlots();
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
