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

    m_rawDataConvert = new RawDataConvertDialog;
    m_convertBinWithHFile = new ConvertBinWithHFile;

    m_tabWidget->addTab(m_rawDataConvert, "Raw Data Convert");
    m_tabWidget->addTab(m_convertBinWithHFile, "Bin <-> x.h");

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

