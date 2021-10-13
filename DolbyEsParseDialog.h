#ifndef DOLBYESPARSEDIALOG_H
#define DOLBYESPARSEDIALOG_H

#include <QDialog>
#include <QString>
#include <QList>
#include <QTableWidget>
#include <QLabel>

#define DOLBY_HEADER_LEN 8

struct DolbyHeaderInfo
{
    int syncword;           // 16 bit
    int strmtyp;            // 2 bit
    int substreamid;        // 3 bit
    int frmsiz;             // 11 bit
    int fscod;              // 2 bit
    int numblkscod;         // 2 bit
    int acmod;              // 3 bit
    int lfeon;              // 1 bit
    int bsid;               // 16 bit
    int dialnorm;           // 5 bit
    int compre;             // 1 bit
    int compr;              // 8 bit
    int mixmdate;           // 1 bit
    int ltrtcmixlev;        // 3 bit
    int lorocmixlev;        // 3 bit
    int ltrtsurmixlev;      // 3 bit
    int lorosurmixlev;      // 3 bit
    int lfemixlevcode;      // 1 bit
    int pgmscle;            // 1 bit
    int extpgmscle;         // 1 bit
    int mixdef;             // 2 bit
    int mixdata;            // 12 bit
    int frmmixcfginfoe;     // 1 bit
    int infomdate;          // 1 bit
    int bsmod;              // 3 bit
    int copyrightb;         // 1 bit
    int origbs;             // 1 bit
    int dsurexmod;          // 2 bit
    int audprodie;          // 1 bit
    int sourcefscod;        // 1 bit
    int convsync;           // 1 bit
    int addbsie;            // 1 bit

    int frmSize;
    int sampleRate;
    int blkPerFrm;
    DolbyHeaderInfo();
};

class DolbyEsParseDialog : public QDialog
{
    Q_OBJECT
private:
    QString m_filePath;
    QList<DolbyHeaderInfo> m_dolbyHeaderList;

    QTableWidget* m_tableWidget;
    QLabel* m_headerInfoLabel;

    void tableWidgetSetCellData(int row, int column, QString data);

    bool adjustEndianness(unsigned char* buf, int size);
    bool parseDolbyHeader(const QString& filePath);
    void setDolbyInfoToTableWidget();

    void constructUI();
    void connectSlots();
public:
    DolbyEsParseDialog(QString filePath = "", QWidget *parent = nullptr);
    ~DolbyEsParseDialog();

public slots:
    void onSelectionChanged();
};

#endif // DOLBYESPARSEDIALOG_H
