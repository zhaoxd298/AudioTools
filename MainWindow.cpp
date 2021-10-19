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
    m_tabWidget->addTab(m_rawDataConvert, "Raw Data Convert");

    m_convertBinWithHFile = new ConvertBinWithHFile;
    m_tabWidget->addTab(m_convertBinWithHFile, "Bin <-> x.h");

    m_dolbyDialog = new DolbyDialog;
    m_tabWidget->addTab(m_dolbyDialog, "Dolby");

    QVBoxLayout* mainWidgetVLayout = new QVBoxLayout(m_mainWidget);
    mainWidgetVLayout->addWidget(m_tabWidget);

    //setWindowIcon(QIcon(":/img/img/icon.png"));

    this->resize(450, 200);

    setCentralWidget(m_mainWidget);
}

void MainWindow::connectSlots()
{

}

MainWindow::~MainWindow()
{
}

