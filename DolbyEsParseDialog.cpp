#include "DolbyEsParseDialog.h"
#include <QFile>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QStringList>
#include <QHeaderView>
#include <QFont>
#include <QDebug>

static const unsigned char eac3Blocks[4] =
{
    1, 2, 3, 6
};

static const unsigned short ac3Frmsizecod[3][38] =
{
    {
        64, 64, 80, 80, 96, 96, 112, 112, 128, 128, 160, 160, 192, 192, 224, 224, 256, 256, 320, 320,
        384, 384, 448, 448, 512, 512, 640, 640, 768, 768, 896, 896, 1024, 1024, 1152, 1152,1280, 1280
    },
    {
        69, 70, 87, 88, 104, 105, 121, 122, 139, 140, 174, 175, 208, 209, 243, 244, 278, 279, 348, 349,
        417, 418, 487, 488, 557, 558, 696, 697, 835, 836, 975, 976, 1114, 1115, 1253, 1254, 1393, 1394
    },
    {
        96, 96, 120, 120, 144, 144, 168, 168, 192, 192, 240, 240, 288, 288, 336, 336, 384, 384, 480, 480,
        576, 576, 672, 672, 768, 768, 960, 960, 1152, 1152, 1344, 1344, 1536, 1536, 1728, 1728, 1920, 1920
    }
};

static const int ac3SampleRateTab[] = {48000, 44100, 32000, 0};

DolbyHeaderInfo::DolbyHeaderInfo()
{
    memset(syncword, 0, sizeof(syncword));
    crcl = 0;
    strmtyp = 0;
    substreamid = 0;
    frmsiz = 0;
    fscod = 0;
    frmsizecod = 0;
    numblkscod = 0;
    acmod = 0;
    cmixlev = 0;
    surmixlev = 0;
    lfeon = 0;
    bsid = 0;
    dialnorm = 0;
    compre = 0;
    compr = 0;
    langcode = 0;
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
    timecod1e = 0;
    timecod2e = 0;
    addbsie = 0;
    addbsil = 0;

    offset = 0;
    dolbyType = AC3;
    frmSize = 0;
    sampleRate = 0;
    blkPerFrm = 0;
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

    int i;
    for (i=0; i<size-1; i+=2)
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
    m_dolbyHeaderList.clear();

    while (1)
    {
        int i;
        unsigned char* ptr = NULL;
        DolbyHeaderInfo dolbyHeader;

        if (file.atEnd())
        {
            qDebug() << "read file over!";
            //QString okStr = QString("File convert success!");
            //QMessageBox::information(this, QString(tr("Info")), okStr, QString(tr("OK")));
            break;
        }

        int size = file.read((char*)buf, sizeof(buf)-leftSize);
        leftSize += size;

        for (i=0; i<leftSize-1; i++)
        {
            if (((buf[i] == 0x0b) && (buf[i + 1] == 0x77)) ||
                ((buf[i] == 0x77) && (buf[i + 1] == 0x0b)))
            {
                snprintf(dolbyHeader.syncword, sizeof(dolbyHeader.syncword), "0x%02x%02x", (int)buf[i], (int)buf[i+1]);
                ptr = buf + i;
                break;
            }
        }
        leftSize -= i;
        m_offset += i;

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

        dolbyHeader.acmod = (ptr[4] >> 1) & 0x7;
        dolbyHeader.lfeon = ptr[4] & 0x1;

        dolbyHeader.bsid = ptr[5] >> 3;
        //qDebug() << "bsid:" << dolbyHeader.bsid;
        //qDebug("%02x %02x %02x %02x %02x %02x %02x %02x", ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6], ptr[7]);

        if ((0 <= dolbyHeader.bsid) && (dolbyHeader.bsid <= 8))     // AC3 [0, 8]
        {
            dolbyHeader.dolbyType = AC3;

            dolbyHeader.crcl = (ptr[2] << 8) | ptr[3];

            dolbyHeader.fscod = ptr[4] >> 6;
            dolbyHeader.sampleRate = ac3SampleRateTab[dolbyHeader.fscod];

            dolbyHeader.frmsizecod = ptr[4] & 0x3f;
            if ((dolbyHeader.fscod < 3) && (dolbyHeader.frmsizecod < 38))
            {
                dolbyHeader.frmsiz = ac3Frmsizecod[dolbyHeader.fscod][dolbyHeader.frmsizecod];
                dolbyHeader.frmSize = dolbyHeader.frmsiz * 2;
            }
            else
            {
                qDebug() << "Invalid fscode or frmsizecod" << dolbyHeader.fscod << dolbyHeader.frmsizecod;
                goto next_frame;
            }

            dolbyHeader.bsmod = ptr[5] & 0x07;
        }
        else if ((11 <= dolbyHeader.bsid) && (dolbyHeader.bsid <= 16))       // EAC3 [11, 16]
        {
            dolbyHeader.dolbyType = EAC3;

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
                if (fscod2 < 3)
                {
                    dolbyHeader.sampleRate = ac3SampleRateTab[fscod2];
                }
                else
                {
                    qDebug() << "Invalid fscod2:" << fscod2;
                    goto next_frame;
                }
            }
            else
            {
                dolbyHeader.numblkscod = (ptr[4] >> 4) & 3;
                if (dolbyHeader.fscod < 3)
                {
                    dolbyHeader.sampleRate = ac3SampleRateTab[dolbyHeader.fscod];
                }
                else
                {
                    qDebug() << "Invalid fscod:" << dolbyHeader.fscod;
                    goto next_frame;
                }
            }
            dolbyHeader.blkPerFrm = eac3Blocks[dolbyHeader.numblkscod];

        }

        if (dolbyHeader.frmSize > 0)
        {
            dolbyHeader.offset = m_offset;
            m_dolbyHeaderList.append(dolbyHeader);
        }

next_frame:
        int skipSize = dolbyHeader.frmSize;
        if (0 == skipSize)
        {
            skipSize = DOLBY_HEADER_LEN;
        }

        if (leftSize >= skipSize)
        {
            m_offset += skipSize;
            ptr += skipSize;
            leftSize -= skipSize;
            memmove(buf, ptr, leftSize);
        }
        else
        {
            m_offset += leftSize;
            leftSize = 0;
        }

        //qDebug() << "left:" << leftSize;
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

        tableWidgetSetCellData(row, 0, "0x" + QString::number(m_dolbyHeaderList[i].offset, 16));
        tableWidgetSetCellData(row, 1, QString::number(m_dolbyHeaderList[i].frmSize));
        tableWidgetSetCellData(row, 2, QString::number(m_dolbyHeaderList[i].sampleRate));
    }
}

void DolbyEsParseDialog::displayDolbyHeaderInfo(const DolbyHeaderInfo& info)
{
    QString str = "";

    if (AC3 == info.dolbyType)
    {
        str += "ac3_syncframe()\n";

        str += " +syncinfo()\n";
        str += QString("  |- syncword\t\t: %1 (16 bits)\n").arg(info.syncword);
        str += QString("  |- crcl\t\t: 0x%1 (16 bits)\n").arg(QString::number(info.crcl, 16));
        str += QString("  |- fscod\t\t: %1 [%2 Hz] (2 bits)\n").arg(info.fscod).arg(info.sampleRate);
        str += QString("  |- frmsizecod\t\t: %1 [%2 words] (6 bits)\n").arg(info.frmsizecod).arg(info.frmsiz);

        str += "\n +bsi()\n";
        str += QString("  |- bsid\t\t: %1 (5 bits)\n").arg(info.bsid);
        str += QString("  |- bsmod\t\t: %1 (3 bits)\n").arg(info.bsmod);
        str += QString("  |- acmod\t\t: %1 (3 bits)\n").arg(info.acmod);
        str += QString("  |- cmixlev\t\t: %1 [?????] (2 bits)\n").arg(info.cmixlev);
        str += QString("  |- surmixlev\t\t: %1 [?????] (2 bits)\n").arg(info.surmixlev);
        str += QString("  |- lfeon\t\t: %1 (1 bits)\n").arg(info.lfeon);
        str += QString("  |- dialnorm\t\t: %1 (5 bits)\n").arg(info.dialnorm);
        str += QString("  |- compre\t\t: %1 (1 bits)\n").arg(info.compre);
        str += QString("  |- compr\t\t: %1 (8 bits)\n").arg(info.compr);
        str += QString("  |- langcode\t\t: %1 (1 bits)\n").arg(info.langcode);
        str += QString("  |- audprodie\t\t: %1 (1 bits)\n").arg(info.audprodie);
        str += QString("  |- copyrightb\t\t: %1 (1 bits)\n").arg(info.copyrightb);
        str += QString("  |- origbs\t\t: %1 (1 bits)\n").arg(info.origbs);
        str += QString("  |- timecod1e\t\t: %1 (1 bits)\n").arg(info.timecod1e);
        str += QString("  |- timecod2e\t\t: %1 (1 bits)\n").arg(info.timecod2e);
        str += QString("  |- addbsie\t\t: %1 (1 bits)\n").arg(info.addbsie);

    }
    else if (EAC3 == info.dolbyType)
    {
        str += "eac3_syncframe()\n";

        str += " +syncinfo()\n";
        str += QString("  |- syncword\t\t: %1 (16 bits)\n").arg(info.syncword);

        str += "\n +bsi()\n";
        str += QString("  |- strmtyp\t\t: %1 [Type %2] (2 bits)\n").arg(info.strmtyp).arg(0);  // type need reset
        str += QString("  |- substreamid\t: %1 (3 bits)\n").arg(info.substreamid);
        str += QString("  |- frmsiz\t\t: %1 [%2 bytes] (11 bits)\n").arg(info.frmsiz).arg(info.frmSize);
        str += QString("  |- fscod\t\t: %1 [%2 Hz] (2 bits)\n").arg(info.fscod).arg(info.sampleRate);
        str += QString("  |- numblkscod\t\t: %1 [%2 block] (2 bits)\n").arg(info.numblkscod).arg(info.blkPerFrm);
        str += QString("  |- acmod\t\t: %1 (3 bits)\n").arg(info.acmod);
        str += QString("  |- lfeon\t\t: %1 (1 bits)\n").arg(info.lfeon);
        str += QString("  |- bsid\t\t: %1 (5 bits)\n").arg(info.bsid);
        str += QString("  |- dialnorm\t\t: %1 (5 bits)\n").arg(info.dialnorm);
        str += QString("  |- compre\t\t: %1 (1 bits)\n").arg(info.compre);
        str += QString("  |- compr\t\t: %1 (8 bits)\n").arg(info.compr);
        str += QString("  |- mixmdate\t\t: %1 (1 bits)\n").arg(info.mixmdate);
        str += QString("  |- ltrtcmixlev\t: %1 (3 bits)\n").arg(info.ltrtcmixlev);
        str += QString("  |- lorocmixlev\t: %1 (3 bits)\n").arg(info.lorocmixlev);
        str += QString("  |- ltrtsurmixlev\t: %1 (3 bits)\n").arg(info.ltrtsurmixlev);
        str += QString("  |- lorosurmixlev\t: %1 (3 bits)\n").arg(info.lorosurmixlev);
        str += QString("  |- lfemixlevcode\t: %1 (1 bits)\n").arg(info.lfemixlevcode);
        str += QString("  |- pgmscle\t\t: %1 (1 bits)\n").arg(info.pgmscle);
        str += QString("  |- extpgmscle\t\t: %1 (1 bits)\n").arg(info.extpgmscle);
        str += QString("  |- mixdef\t\t: %1 (2 bits)\n").arg(info.mixdef);
        str += QString("  |- mixdata\t\t: %1 (12 bits)\n").arg(info.mixdata);
        str += QString("  |- frmmixcfginfoe\t: %1 (1 bits)\n").arg(info.frmmixcfginfoe);
        str += QString("  |- infomdate\t\t: %1 (1 bits)\n").arg(info.infomdate);
        str += QString("  |- convsync\t\t: %1 (1 bits)\n").arg(info.convsync);
        str += QString("  |- addbsie\t\t: %1 (1 bits)\n").arg(info.addbsie);
        str += QString("  |- addbsil\t\t: %1 (1 bits)\n").arg(info.addbsil);

        //str += "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
    }


    m_headerInfoLabel->setText(str);
}

void DolbyEsParseDialog::constructUI()
{
    m_tableWidget = new QTableWidget;
    m_tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //设置表头背景色
    m_tableWidget->setStyleSheet("selection-background-color:lightblue;"); //设置选中背景色
    //m_tableWidget->verticalHeader()->setVisible(false);    // 隐藏纵坐标表头
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);    //整行选中的方式
    m_tableWidget->setColumnCount(3);
    QStringList headerList;
    headerList << "Offset" << "Frame Size" << "Sample Rate";
    m_tableWidget->setHorizontalHeaderLabels(headerList);
    //m_tableWidget->setMouseTracking(true);    // 设置鼠标追踪为真

    m_headerInfoLabel = new QLabel;
    m_headerInfoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_headerInfoLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_headerInfoLabel->setFont(QFont("Consolas", 11));
    //m_headerInfoLabel->setFont(QFont("Times", 14));

    QHBoxLayout* mainHLayout = new QHBoxLayout(this);
    mainHLayout->addWidget(m_tableWidget);
    mainHLayout->addWidget(m_headerInfoLabel);

    setWindowIcon(QIcon(":/img/img/dolby.png"));
    setWindowTitle("Dolby Info Parse (" + m_filePath + ")");
    setMinimumSize(880, 660);
}

void DolbyEsParseDialog::connectSlots()
{
    connect(m_tableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));
}

DolbyEsParseDialog::DolbyEsParseDialog(QString filePath, QWidget *parent)
    :QDialog(parent), m_filePath(filePath)
{

    qDebug() << filePath;
    m_offset = 0;

    constructUI();
    connectSlots();
    parseDolbyHeader(m_filePath);
    setDolbyInfoToTableWidget();

    if (m_dolbyHeaderList.size() > 0)
    {
        displayDolbyHeaderInfo(m_dolbyHeaderList[0]);
    }
}

void DolbyEsParseDialog::onSelectionChanged()
{
    QList<QTableWidgetSelectionRange> rangeList = m_tableWidget->selectedRanges();
    //qDebug() << __func__ << rangeList[0].topRow() << rangeList[0].bottomRow() << rangeList[0].leftColumn() << rangeList[0].rightColumn();
    //qDebug() << __func__ << rangeList[0].topRow();

    int index = rangeList[0].topRow();
    if (index < m_dolbyHeaderList.size())
    {
        displayDolbyHeaderInfo(m_dolbyHeaderList[index]);
    }
}

DolbyEsParseDialog::~DolbyEsParseDialog()
{

}
