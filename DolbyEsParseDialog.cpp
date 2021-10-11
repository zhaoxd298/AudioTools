#include "DolbyEsParseDialog.h"
#include <QFile>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QStringList>
#include <QHeaderView>
#include <QDebug>

static const unsigned char eac3Blocks[4] = {
    1, 2, 3, 6
};

static const int ac3SampleRateTab[] = { 48000, 44100, 32000, 0 };

DolbyHeaderInfo::DolbyHeaderInfo()
{
    syncword = 0;
    strmtyp = 0;
    substreamid = 0;
    frmsiz = 0;
    fscod = 0;
    numblkscod = 0;
    acmod = 0;
    lfeon = 0;
    bsid = 0;
    dialnorm = 0;
    compre = 0;
    compr = 0;
    mixmdate = 0;
    ltrtcmixlev = 0;
    lorocmixlev = 0;
    ltrtsurmixlev = 0;
    lorosurmixlev = 0;
    lfemixlevcode = 0;
    pgmscle = 0;
    extpgmscle = 0;
    mixdef = 0;
    mixdata = 0;
    frmmixcfginfoe = 0;
    infomdate = 0;
    bsmod = 0;
    copyrightb = 0;
    origbs = 0;
    dsurexmod = 0;
    audprodie = 0;
    sourcefscod = 0;
    convsync = 0;
    addbsie = 0;
}

void DolbyEsParseDialog::tableWidgetSetCellData(int row, int column, QString data)
{
    QTableWidgetItem* it = m_tableWidget->item(row, column);
    if (NULL == it)
    {
        it = new QTableWidgetItem(data);
        it->setToolTip(data);
        //it->setData(Qt::DisplayRole, QVariant(data));
        m_tableWidget->setItem(row, column, it);
    }
    else
    {
        it->setText(data);
        it->setToolTip(data);
        //it->setData(Qt::DisplayRole, QVariant(data));
    }
}

bool DolbyEsParseDialog::adjustEndianness(unsigned char* buf, int size)
{
    if ((NULL == buf) || (size <= 0))
    {
        qDebug() << __func__ << "Invalid parameter!";
        return false;
    }

    for (int i=0; i<size-i; i+=2)
    {
        unsigned char tmp = buf[i];
        buf[i] = buf[i + 1];
        buf[i + 1] = tmp;
    }

    return true;
}

bool DolbyEsParseDialog::parseDolbyHeader(const QString& filePath)
{
    if (filePath.isEmpty())
    {
        qDebug() << "file path is empty!";
        return false;
    }

    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    unsigned char buf[8192];
    int leftSize = 0;

    while (1)
    {
        int i;
        unsigned char* ptr = NULL;
        DolbyHeaderInfo dolbyHeader;

        int size = file.read((char*)buf, sizeof(buf)-leftSize);
        leftSize += size;

        for (i=0; i<leftSize-1; i++)
        {
            if (((buf[i] == 0x0b) && (buf[i + 1] == 0x77)) ||
                ((buf[i] == 0x77) && (buf[i + 1] == 0x0b)))
            {
                dolbyHeader.syncword = buf[i];
                dolbyHeader.syncword |= (buf[i + 1] << 8);
                ptr = buf + i;
                break;
            }
        }
        leftSize -= i;

        if (leftSize < DOLBY_HEADER_LEN)
        {
            if (NULL != ptr)
            {
                memmove(buf, ptr, leftSize);
            }
            continue;
        }

        if ((0x77 == ptr[0]) && (0x0b == ptr[1]))
        {
            adjustEndianness(ptr, DOLBY_HEADER_LEN);
        }

        dolbyHeader.bsid = ptr[5] >> 3;

        if (dolbyHeader.bsid <= 10)     // AC3 [0, 8]
        {

        }
        else        // EAC3 [11, 16]
        {
            dolbyHeader.strmtyp = ptr[2] >> 6;

            dolbyHeader.substreamid = (ptr[2] >> 3) & 0x7;

            int high = ptr[2] & 0x7;
            int low = ptr[3] & 0xff;
            dolbyHeader.frmsiz = (high << 8) + low;
            dolbyHeader.frmSize = (dolbyHeader.frmsiz + 1) * 2;

            dolbyHeader.fscod = ptr[4] >> 6;
            if (0x3 == dolbyHeader.fscod)
            {
                int fscod2 = 0;
                fscod2 = (ptr[4] >> 4) & 3;
                dolbyHeader.numblkscod = 0x3;
                dolbyHeader.sampleRate = ac3SampleRateTab[fscod2];
            }
            else
            {
                dolbyHeader.numblkscod = (ptr[4] >> 4) & 3;
                dolbyHeader.sampleRate = ac3SampleRateTab[dolbyHeader.fscod];
            }
            dolbyHeader.blkPerFrm = eac3Blocks[dolbyHeader.numblkscod];

        }

        m_dolbyHeaderList.append(dolbyHeader);
        if (leftSize >= dolbyHeader.frmSize)
        {
            ptr += dolbyHeader.frmSize;
            leftSize -= dolbyHeader.frmSize;
            memmove(buf, ptr, leftSize);
        }
        else
        {
            leftSize = 0;
        }


        if (file.atEnd())
        {
            qDebug() << "read file over!";
            //QString okStr = QString("File convert success!");
            //QMessageBox::information(this, QString(tr("Info")), okStr, QString(tr("OK")));
            break;
        }
    }

    file.close();

    return true;
}

void DolbyEsParseDialog::setDolbyInfoToTableWidget()
{
    for (int i=0; i<m_dolbyHeaderList.size(); i++)
    {
        int row = m_tableWidget->rowCount();
        m_tableWidget->insertRow(row);

        tableWidgetSetCellData(row, 0, "");
        tableWidgetSetCellData(row, 1, QString::number(m_dolbyHeaderList[i].frmSize));
        tableWidgetSetCellData(row, 2, QString::number(m_dolbyHeaderList[i].sampleRate));
    }
}

void DolbyEsParseDialog::constructUI()
{
    m_tableWidget = new QTableWidget;
    m_tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //设置表头背景色
    m_tableWidget->setStyleSheet("selection-background-color:lightblue;"); //设置选中背景色
    m_tableWidget->verticalHeader()->setVisible(false);    // 隐藏纵坐标表头
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);    //整行选中的方式
    m_tableWidget->setColumnCount(3);
    QStringList headerList;
    headerList << "Offset" << "Frame Size" << "Sample Rate";
    m_tableWidget->setHorizontalHeaderLabels(headerList);

    m_headerInfoLabel = new QLabel;

    QHBoxLayout* mainHLayout = new QHBoxLayout(this);
    mainHLayout->addWidget(m_tableWidget);
    mainHLayout->addWidget(m_headerInfoLabel);
}

void DolbyEsParseDialog::connectSlots()
{
    connect(m_tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(onTableWidgetCellClick(int,int)));
    connect(m_tableWidget, SIGNAL(cellActivated(int,int)), this, SLOT(onTableWidgetCellClick(int,int)));
    connect(m_tableWidget, SIGNAL(cellEntered(int,int)), this, SLOT(onTableWidgetCellClick(int,int)));
}

DolbyEsParseDialog::DolbyEsParseDialog(QString filePath, QWidget *parent)
    :QDialog(parent), m_filePath(filePath)
{

    qDebug() << filePath;
    constructUI();
    connectSlots();
    parseDolbyHeader(m_filePath);
    setDolbyInfoToTableWidget();
}

void DolbyEsParseDialog::onTableWidgetCellClick(int row, int line)
{
    qDebug() << row << line;
}

DolbyEsParseDialog::~DolbyEsParseDialog()
{

}
