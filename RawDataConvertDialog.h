#ifndef RAWDATACONVERTDIALOG_H
#define RAWDATACONVERTDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>

class RawDataConvertDialog : public QDialog
{
    Q_OBJECT
private:
    enum ConvertType
    {
        RemoveHight8bit,
        RemoveHight16bit,
        RemoveLow8bit,
        RemoveLow16bit,
    };

private:
    QLineEdit* m_filePathEdit;
    QPushButton* m_scanBtn;
    QRadioButton* m_removeHigh8bitRBtn;
    QRadioButton* m_removeHigh16bitRBtn;
    QRadioButton* m_removeLow8bitRBtn;
    QRadioButton* m_removeLow16bitRBtn;
    QPushButton* m_okBtn;

    ConvertType m_covertType;

    void constructUI();
    void connectSlots();

    QString getCovertFilePath(const QString& filePath);
    bool convertFile();
protected:
    void dragEnterEvent(QDragEnterEvent*event); //文件拖拽到窗体内，触发
    void dropEvent(QDropEvent *event); //文件拖拽到窗体内，释放，触发

public:
    RawDataConvertDialog(QWidget *parent = nullptr);
    ~RawDataConvertDialog();

public slots:
    void onScanBtn();
    void onRemoveHigh8bitRBtn(bool checked);
    void onRemoveHigh16bitRBtn(bool checked);
    void onRemoveLow8bitRBtn(bool checked);
    void onRemoveLow16bitRBtn(bool checked);
    void onOkBtn();
};

#endif // RAWDATACONVERTDIALOG_H
