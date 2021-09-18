#include "RawDataConvertDialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDebug>

RawDataConvertDialog::RawDataConvertDialog(QWidget *parent)
    :QDialog(parent)
{
    constructUI();
    connectSlots();

    this->setAcceptDrops(true); //必须设置，不然无法接收拖放事件
}

void RawDataConvertDialog::constructUI()
{
    m_filePathEdit = new QLineEdit;
    m_scanBtn = new QPushButton("Scan");
    QHBoxLayout* filePathHLayout = new QHBoxLayout;
    filePathHLayout->addWidget(m_filePathEdit);
    filePathHLayout->addWidget(m_scanBtn);

    m_removeHigh8bitRBtn = new QRadioButton("Remove High 8bit");
    m_removeHigh16bitRBtn = new QRadioButton("Remove High 16bit");
    m_removeHigh16bitRBtn->setChecked(true);
    m_removeLow8bitRBtn = new QRadioButton("Remove Low 8bit");
    m_removeLow16bitRBtn = new QRadioButton("Remove Low 16bit");
    QGridLayout* radioGridLayout = new QGridLayout;
    radioGridLayout->addWidget(m_removeHigh8bitRBtn, 0, 0);
    radioGridLayout->addWidget(m_removeHigh16bitRBtn, 0, 1);
    radioGridLayout->addWidget(m_removeLow8bitRBtn, 1, 0);
    radioGridLayout->addWidget(m_removeLow16bitRBtn, 1, 1);

    m_okBtn = new QPushButton("OK");
    QHBoxLayout* okHLayout = new QHBoxLayout;
    okHLayout->addStretch();
    okHLayout->addWidget(m_okBtn);

    QVBoxLayout* mainVLayout = new QVBoxLayout(this);
    mainVLayout->addLayout(filePathHLayout);
    mainVLayout->addLayout(radioGridLayout);
    mainVLayout->addLayout(okHLayout);
}

void RawDataConvertDialog::connectSlots()
{
    connect(m_scanBtn, SIGNAL(clicked(bool)), this, SLOT(onScanBtn()));
    connect(m_removeHigh8bitRBtn, SIGNAL(toggled(bool)), this, SLOT(onRemoveHigh8bitRBtn(bool)));
    connect(m_removeHigh16bitRBtn, SIGNAL(toggled(bool)), this, SLOT(onRemoveHigh16bitRBtn(bool)));
    connect(m_removeLow8bitRBtn, SIGNAL(toggled(bool)), this, SLOT(onRemoveLow8bitRBtn(bool)));
    connect(m_removeLow16bitRBtn, SIGNAL(toggled(bool)), this, SLOT(onRemoveLow16bitRBtn(bool)));
    connect(m_okBtn, SIGNAL(clicked(bool)), this, SLOT(onOkBtn()));
}

QString RawDataConvertDialog::getCovertFilePath(const QString& filePath)
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

bool RawDataConvertDialog::convertFile()
{
    QString filePath = m_filePathEdit->text();
    if (filePath.isEmpty())
    {
        return false;
    }

    QString outputFilePath = getCovertFilePath(filePath);
    //qDebug() << outputFilePath;

    QFile outputFile(outputFilePath);
    outputFile.open(QIODevice::WriteOnly);

    QFile inputFile(filePath);
    inputFile.open(QIODevice::ReadOnly);

    while (1)
    {

    }

    return true;
}

void RawDataConvertDialog::onScanBtn()
{
    qDebug() << __func__;
    QFileDialog dialog;
    QString filePath = dialog.getOpenFileName();

    m_filePathEdit->setText(filePath);
}
void RawDataConvertDialog::onRemoveHigh8bitRBtn(bool checked)
{
    if (checked)
    {
        m_covertType = RemoveHight8bit;
    }
    qDebug() << __func__ << checked << m_covertType;
}

void RawDataConvertDialog::onRemoveHigh16bitRBtn(bool checked)
{
    if (checked)
    {
        m_covertType = RemoveHight16bit;
    }
    qDebug() << __func__ << checked << m_covertType;
}

void RawDataConvertDialog::onRemoveLow8bitRBtn(bool checked)
{
    if (checked)
    {
        m_covertType = RemoveLow8bit;
    }

    qDebug() << __func__ << checked << m_covertType;
}

void RawDataConvertDialog::onRemoveLow16bitRBtn(bool checked)
{
    if (checked)
    {
        m_covertType = RemoveLow16bit;
    }

    qDebug() << __func__ << checked << m_covertType;
}

void RawDataConvertDialog::onOkBtn()
{
    qDebug() << __func__;
    QString filePath = m_filePathEdit->text();
    if (filePath.isEmpty())
    {
        QString errStr = QString(tr("请选择需要转换的文件，可以将需要转换的文件拖进来！"));
        QMessageBox::critical(this, QString(tr("错误")), errStr, QString(tr("确定")));
        return;
    }

    convertFile();
}

void RawDataConvertDialog::dragEnterEvent(QDragEnterEvent*event)
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

void RawDataConvertDialog::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if(mimeData->hasUrls())
    {
        QList<QUrl> urls = mimeData->urls();
        QString fileName = urls.at(0).toLocalFile();

        m_filePathEdit->setText(fileName);
     }
}

RawDataConvertDialog::~RawDataConvertDialog()
{

}
