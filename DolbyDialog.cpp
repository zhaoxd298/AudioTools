#include "DolbyDialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDebug>


DolbyDialog::DolbyDialog(QWidget *parent)
    :QDialog(parent)
{
    constructUI();
    connectSlots();

    if (m_adjustEndiannessRBtn->isChecked())
    {
        m_actionType = AdjustEndianness;
    }

    if (m_parseEsInfoRBtn->isChecked())
    {
        m_actionType = ParseEsInfo;
    }

    this->setAcceptDrops(true);

}

void DolbyDialog::constructUI()
{
    m_filePathEdit = new QLineEdit;
    m_scanBtn = new QPushButton("Scan");
    QHBoxLayout* filePathHLayout = new QHBoxLayout;
    filePathHLayout->addWidget(m_filePathEdit);
    filePathHLayout->addWidget(m_scanBtn);

    m_adjustEndiannessRBtn = new QRadioButton("Adjust Endianness");
    m_adjustEndiannessRBtn->setChecked(true);
    m_parseEsInfoRBtn = new QRadioButton("Parse Es Info");
    QGridLayout* radioGridLayout = new QGridLayout;
    radioGridLayout->addWidget(m_adjustEndiannessRBtn, 0, 0);
    radioGridLayout->addWidget(m_parseEsInfoRBtn, 0, 1);

    m_okBtn = new QPushButton("OK");
    QHBoxLayout* okHLayout = new QHBoxLayout;
    okHLayout->addStretch();
    okHLayout->addWidget(m_okBtn);

    QVBoxLayout* mainVLayout = new QVBoxLayout(this);
    mainVLayout->addLayout(filePathHLayout);
    mainVLayout->addLayout(radioGridLayout);
    mainVLayout->addLayout(okHLayout);
}

void DolbyDialog::connectSlots()
{
    connect(m_scanBtn, SIGNAL(clicked(bool)), this, SLOT(onScanBtn()));
    connect(m_adjustEndiannessRBtn, SIGNAL(toggled(bool)), this, SLOT(onAdjustEndiannessRBtn(bool)));
    connect(m_parseEsInfoRBtn, SIGNAL(toggled(bool)), this, SLOT(onParseEsInfoRBtn(bool)));
    connect(m_okBtn, SIGNAL(clicked(bool)), this, SLOT(onOkBtn()));
}

QString DolbyDialog::getAdjustFilePath(const QString& filePath)
{
    QString str;

    if (filePath.isEmpty())
    {
        return str;
    }

    char* chStr;
    QByteArray ba = filePath.toLatin1(); // must
    chStr = ba.data();

    int n = strlen(chStr);
    int pointIdx = n;
    for (int i=n-1; i>=0; i--)
    {
        if ('.' == chStr[i])
        {
            pointIdx = i;
            break;
        }
    }

    char dstStr[1024] = {0};
    memcpy(dstStr, chStr, pointIdx);
    strncat(dstStr, "_cvt", sizeof(dstStr)-strlen(dstStr)-1);
    if (pointIdx != n)
    {
        strncat(dstStr, chStr+pointIdx, sizeof(dstStr)-strlen(dstStr)-1);
    }

    str = QString(dstStr);

    return str;
}

bool DolbyDialog::adjustEndianness()
{
    QString inputFilePath = m_filePathEdit->text();
    if (inputFilePath.isEmpty())
    {
        return false;
    }

    QFile inputFile(inputFilePath);
    inputFile.open(QIODevice::ReadOnly);

    QString outputFilePath = getAdjustFilePath(inputFilePath);
    qDebug() << outputFilePath;

    QFile outputFile(outputFilePath);
    outputFile.open(QIODevice::WriteOnly);

    char buf[1024];
    while (1)
    {
        int size = inputFile.read(buf, sizeof(buf));

        for (int i=0; i<size-1; i+=2)
        {
            char tmp = buf[i];
            buf[i] = buf[i+1];
            buf[i+1] = tmp;
        }

        outputFile.write(buf, size);

        if (inputFile.atEnd())
        {
            qDebug() << "read file over!";
            QString okStr = QString("File convert success!");
            QMessageBox::information(this, QString(tr("Info")), okStr, QString(tr("OK")));
            break;
        }
    }

    inputFile.close();
    outputFile.close();

    return true;
}

void DolbyDialog::onScanBtn()
{
    qDebug() << __func__;
    QFileDialog dialog;
    QString filePath = dialog.getOpenFileName();

    m_filePathEdit->setText(filePath);
}

void DolbyDialog::onAdjustEndiannessRBtn(bool checked)
{
    if (checked)
    {
        m_actionType = AdjustEndianness;
    }
    qDebug() << __func__ << checked << m_actionType;
}

void DolbyDialog::onParseEsInfoRBtn(bool checked)
{
    if (checked)
    {
        m_actionType = ParseEsInfo;
    }
    qDebug() << __func__ << checked << m_actionType;
}

void DolbyDialog::onOkBtn()
{
    QString filePath = m_filePathEdit->text();
    if (filePath.isEmpty())
    {
        QString errStr = QString(tr("请选择需要转换的文件，可以将需要转换的文件拖进来！"));
        QMessageBox::critical(this, QString(tr("错误")), errStr, QString(tr("确定")));
        return;
    }

    qDebug() << __func__;
    if (AdjustEndianness == m_actionType)
    {
        adjustEndianness();
    }
    else if (ParseEsInfo == m_actionType)
    {
        DolbyEsParseDialog dialog(filePath);
        dialog.exec();
    }
    else
    {
        QString errStr = QString(tr("Unknow action type:%1")).arg(m_actionType);
        QMessageBox::critical(this, QString(tr("Err")), errStr, QString(tr("OK")));
    }
}

void DolbyDialog::dragEnterEvent(QDragEnterEvent*event)
{
    if(event->mimeData()->hasUrls())
    {
        event->acceptProposedAction(); //事件数据中存在路径，方向事件
    }
    else
    {
        event->ignore();
    }
}

void DolbyDialog::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if(mimeData->hasUrls())
    {
        QList<QUrl> urls = mimeData->urls();
        QString fileName = urls.at(0).toLocalFile();

        m_filePathEdit->setText(fileName);
     }
}

DolbyDialog::~DolbyDialog()
{

}
