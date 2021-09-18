#include "MainWindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    constructUI();
    connectSlots();
}



void MainWindow::constructUI()
{
    setWindowTitle(tr("Audio Tools ") + VERSION);

    m_mainWidget = new QWidget;
    m_tabWidget = new QTabWidget;

    m_rawData_convert = new RawDataConvertDialog;
    QWidget* widget1 = new QWidget;
    QWidget* widget2 = new QWidget;

    m_tabWidget->addTab(m_rawData_convert, "Raw Data Convert");
    //m_tabWidget->insertTab(1, widget1, "Bin To h");
    m_tabWidget->addTab(widget1, "Bin To *.h");
    m_tabWidget->addTab(widget2, "*.h To Bin");

    QVBoxLayout* mainWidgetVLayout = new QVBoxLayout(m_mainWidget);
    mainWidgetVLayout->addWidget(m_tabWidget);

    this->resize(450, 200);

    setCentralWidget(m_mainWidget);
}

void MainWindow::connectSlots()
{

}

MainWindow::~MainWindow()
{
}

