#ifndef DOLBYDIALOG_H
#define DOLBYDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include "DolbyEsParseDialog.h"

class DolbyDialog : public QDialog
{
    Q_OBJECT
private:
    enum ActionType
    {
        AdjustEndianness,
        ParseEsInfo,
    };

private:
    QLineEdit* m_filePathEdit;
    QPushButton* m_scanBtn;
    QRadioButton* m_adjustEndiannessRBtn;
    QRadioButton* m_parseEsInfoRBtn;
    QPushButton* m_okBtn;

    ActionType m_actionType;

    void constructUI();
    void connectSlots();

    QString getAdjustFilePath(const QString& filePath);
    bool adjustEndianness();

protected:
    void dragEnterEvent(QDragEnterEvent*event); //文件拖拽到窗体内，触发
    void dropEvent(QDropEvent *event); //文件拖拽到窗体内，释放，触发

public:
    DolbyDialog(QWidget *parent = nullptr);
    ~DolbyDialog();

public slots:
    void onScanBtn();
    void onAdjustEndiannessRBtn(bool checked);
    void onParseEsInfoRBtn(bool checked);
    void onOkBtn();
};

#endif // DOLBYDIALOG_H
