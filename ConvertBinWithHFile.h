#ifndef CONVERTBINWITHHFILE_H
#define CONVERTBINWITHHFILE_H

#include <QDialog>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>

class ConvertBinWithHFile : public QDialog
{
    Q_OBJECT
private:
    enum ConvertType
    {
        ConvertBinToHFile,
        ConvertHFileToBin,
    };

private:
    QLineEdit* m_filePathEdit;
    QPushButton* m_scanBtn;
    QRadioButton* m_binToHFileRBtn;
    QRadioButton* m_hFileToBinRBtn;
    QPushButton* m_okBtn;

    ConvertType m_covertType;

    void constructUI();
    void connectSlots();

    QString getHFilePath(const QString& filePath);
    QString getBinFilePath(const QString& filePath);
    bool binToHFile();
    bool hFileToBin();

protected:
    void dragEnterEvent(QDragEnterEvent*event); //文件拖拽到窗体内，触发
    void dropEvent(QDropEvent *event); //文件拖拽到窗体内，释放，触发

public:
    ConvertBinWithHFile(QWidget *parent = nullptr);
    ~ConvertBinWithHFile();

public slots:
    void onScanBtn();
    void onConvertBinToHFileRBtn(bool checked);
    void onConvertHFileToBinRBtn(bool checked);
    void onOkBtn();
};

#endif // CONVERTBINWITHHFILE_H
