#ifndef DOLBYESPARSEDIALOG_H
#define DOLBYESPARSEDIALOG_H

#include <QDialog>
#include <QString>

class DolbyEsParseDialog : public QDialog
{
    Q_OBJECT
private:
    QString m_filePath;

public:
    DolbyEsParseDialog(QString filePath = "", QWidget *parent = nullptr);
    ~DolbyEsParseDialog();
};

#endif // DOLBYESPARSEDIALOG_H
