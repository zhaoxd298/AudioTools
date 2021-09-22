#include "ConvertBinWithHFile.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDebug>

ConvertBinWithHFile::ConvertBinWithHFile(QWidget *parent)
    :QDialog(parent)
{
    constructUI();
    connectSlots();

    if (m_binToHFileRBtn->isChecked())
    {
        m_covertType = ConvertBinToHFile;
    }

    if (m_hFileToBinRBtn->isChecked())
    {
        m_covertType = ConvertHFileToBin;
    }

    this->setAcceptDrops(true); //必须设置，不然无法接收拖放事件

}

void ConvertBinWithHFile::constructUI()
{
    m_filePathEdit = new QLineEdit;
    m_scanBtn = new QPushButton("Scan");
    QHBoxLayout* filePathHLayout = new QHBoxLayout;
    filePathHLayout->addWidget(m_filePathEdit);
    filePathHLayout->addWidget(m_scanBtn);

    m_binToHFileRBtn = new QRadioButton("Bin -> x.h");
    m_binToHFileRBtn->setChecked(true);
    m_hFileToBinRBtn = new QRadioButton("x.h -> Bin");
    QGridLayout* radioGridLayout = new QGridLayout;
    radioGridLayout->addWidget(m_binToHFileRBtn, 0, 0);
    radioGridLayout->addWidget(m_hFileToBinRBtn, 0, 1);

    m_okBtn = new QPushButton("Convert");
    QHBoxLayout* okHLayout = new QHBoxLayout;
    okHLayout->addStretch();
    okHLayout->addWidget(m_okBtn);

    QVBoxLayout* mainVLayout = new QVBoxLayout(this);
    mainVLayout->addLayout(filePathHLayout);
    mainVLayout->addLayout(radioGridLayout);
    mainVLayout->addLayout(okHLayout);
}
void ConvertBinWithHFile::connectSlots()
{
    connect(m_scanBtn, SIGNAL(clicked(bool)), this, SLOT(onScanBtn()));
    connect(m_binToHFileRBtn, SIGNAL(toggled(bool)), this, SLOT(onConvertBinToHFileRBtn(bool)));
    connect(m_hFileToBinRBtn, SIGNAL(toggled(bool)), this, SLOT(onConvertHFileToBinRBtn(bool)));
    connect(m_okBtn, SIGNAL(clicked(bool)), this, SLOT(onOkBtn()));
}

QString ConvertBinWithHFile::getHFilePath(const QString& filePath)
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
    strncat(dstStr, "_cvt.h", sizeof(dstStr)-strlen(dstStr)-1);

    str = QString(dstStr);

    return str;
}

QString ConvertBinWithHFile::getBinFilePath(const QString& filePath)
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
    strncat(dstStr, "_cvt.bin", sizeof(dstStr)-strlen(dstStr)-1);

    str = QString(dstStr);

    return str;
}

#define SIZE_ONE_LINE 16
bool ConvertBinWithHFile::binToHFile()
{
    QString inputFilePath = m_filePathEdit->text();
    if (inputFilePath.isEmpty())
    {
        return false;
    }

    QFile inputFile(inputFilePath);
    inputFile.open(QIODevice::ReadOnly);

    QString outputFilePath = getHFilePath(inputFilePath);
    //qDebug() << outputFilePath;

    QFile outputFile(outputFilePath);
    outputFile.open(QIODevice::WriteOnly);  //QIODevice::Text
    //QTextStream out(&outputFile);
    //out.setIntegerBase(16);
    //out.setNumberFlags(out.numberFlags() | QTextStream::ShowBase);
    //out.setFieldWidth(2);

    unsigned char buf[SIZE_ONE_LINE];
    char tmpBuf[1024];
    int n, i;

    const char* arrayName =  "static const char array[] = {\n";
    outputFile.write(arrayName, strlen(arrayName));

    while (1)
    {
        memset(tmpBuf, 0, sizeof(tmpBuf));
        n = inputFile.read((char*)buf, sizeof(buf));
        for (i=0; i<n; i++)
        {
            if (0 == i)
            {
                sprintf(tmpBuf+strlen(tmpBuf), "\t");
            }

            sprintf(tmpBuf+strlen(tmpBuf), "0x%02x,", buf[i]);

            if (i < SIZE_ONE_LINE-1)
            {
                sprintf(tmpBuf+strlen(tmpBuf), " ");
            }
        }

        if (strlen(tmpBuf))
        {
            //out << tmpBuf << endl;
            outputFile.write(tmpBuf, strlen(tmpBuf));
            outputFile.write("\n", 1);
        }

        if (inputFile.atEnd())
        {
            outputFile.write("};", 2);
            QString okStr = QString("Convert bin to h file success!");
            QMessageBox::information(this, QString(tr("Info")), okStr, QString(tr("OK")));
            break;
        }
    }

    inputFile.close();
    outputFile.close();

    return true;
}

bool ConvertBinWithHFile::hFileToBin()
{
    bool ret = false;

    QString inputFilePath = m_filePathEdit->text();
    if (inputFilePath.isEmpty())
    {
        return false;
    }

    QFile inputFile(inputFilePath);
    inputFile.open(QIODevice::ReadOnly);

    QString outputFilePath = getBinFilePath(inputFilePath);
    //qDebug() << outputFilePath;
    QFile outputFile(outputFilePath);
    outputFile.open(QIODevice::WriteOnly);

    char rdBuf[1025];
    char wtBuf[1024];
    int rdSize, wt;
    QString tmpStr;
    //char* ptr = NULL;

    while (1)
    {
        wt = 0;
        memset(rdBuf, 0, sizeof(rdBuf));
        rdSize = inputFile.readLine(rdBuf, sizeof(rdBuf)-1);
        if (rdSize < 0)
        {
            qDebug() << __func__ << "():" << __LINE__ << "read err!";
            ret = false;
            break;
        }

        /*ptr = rdBuf;
        char* p = strstr(rdBuf, "{");
        if (p)
        {
            p++;
            ptr = p;
        }

        p = strstr(rdBuf, "}");
        if (p)
        {
           p[0] = 0;
        }
        tmpStr = QString(ptr);
        */

        tmpStr = QString(rdBuf);
        tmpStr = tmpStr.simplified();
        QStringList strList = tmpStr.split(",");
        for (int i=0; i<strList.size(); i++)
        {
            if (strList[i].isEmpty())
            {
                continue;
            }

            wtBuf[wt] = strList[i].toInt(&ret, 16);
            if (true == ret)
            {
                wt++;
            }
            else
            {
                qDebug() << strList[i] << tmpStr;
            }
        }
        outputFile.write(wtBuf, wt);

        if (inputFile.atEnd())
        {
            qDebug() << "read file over!";
            ret = true;
            QString okStr = QString("Convert h to bin file success!");
            QMessageBox::information(this, QString(tr("Info")), okStr, QString(tr("OK")));
            break;
        }
    }

    inputFile.close();
    outputFile.close();

    return ret;
}

void ConvertBinWithHFile::dragEnterEvent(QDragEnterEvent*event)
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

void ConvertBinWithHFile::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if(mimeData->hasUrls())
    {
        QList<QUrl> urls = mimeData->urls();
        QString fileName = urls.at(0).toLocalFile();

        m_filePathEdit->setText(fileName);
     }
}

void ConvertBinWithHFile::onScanBtn()
{
    qDebug() << __func__;
    QFileDialog dialog;
    QString filePath = dialog.getOpenFileName();

    m_filePathEdit->setText(filePath);
}

void ConvertBinWithHFile::onConvertBinToHFileRBtn(bool checked)
{
    if (checked)
    {
        m_covertType = ConvertBinToHFile;
    }
    qDebug() << __func__ << checked << m_covertType;
}

void ConvertBinWithHFile::onConvertHFileToBinRBtn(bool checked)
{
    if (checked)
    {
        m_covertType = ConvertHFileToBin;
    }
    qDebug() << __func__ << checked << m_covertType;
}

void ConvertBinWithHFile::onOkBtn()
{
    QString filePath = m_filePathEdit->text();
    if (filePath.isEmpty())
    {
        QString errStr = QString(tr("请选择需要转换的文件，可以将需要转换的文件拖进来！"));
        QMessageBox::critical(this, QString(tr("错误")), errStr, QString(tr("确定")));
        return;
    }

    if (ConvertBinToHFile == m_covertType)
    {
        binToHFile();
    }
    else if (ConvertHFileToBin == m_covertType)
    {
        hFileToBin();
    }
    else
    {
        QString errStr = QString(tr("Unknow convert type:%1")).arg(m_covertType);
        QMessageBox::critical(this, QString(tr("Err")), errStr, QString(tr("OK")));

    }
    qDebug() << __func__;
}

ConvertBinWithHFile::~ConvertBinWithHFile()
{

}
