#include "DolbyEsParseDialog.h"
#include <QDebug>

DolbyEsParseDialog::DolbyEsParseDialog(QString filePath, QWidget *parent)
    :QDialog(parent), m_filePath(filePath)
{

    qDebug() << filePath;
}

DolbyEsParseDialog::~DolbyEsParseDialog()
{

}
