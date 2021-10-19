#ifndef DOLBYESPARSEDIALOG_H
#define DOLBYESPARSEDIALOG_H

#include <QDialog>
#include <QString>
#include <QList>
#include <QTableWidget>
#include <QLabel>

#define DOLBY_HEADER_LEN 8

enum DolbyType_e
{
    AC3,
    EAC3,
};

struct DolbyHeaderInfo
{
    char syncword[8];       // 16 bit
    int crcl;               // 16 bit (ac3 only)
    int strmtyp;            // 2 bit
    int substreamid;        // 3 bit
    int frmsiz;             // 11 bit
    int fscod;              // 2 bit
    int frmsizecod;         // 6 bit (ac3 only)
    int numblkscod;         // 2 bit
    int acmod;              // 3 bit
    int cmixlev;            // 2 bit (ac3 only)
    int surmixlev;          // 2 bit (ac3 only)
    int lfeon;              // 1 bit
    int bsid;               // 5 bit
    int dialnorm;           // 5 bit
    int compre;             // 1 bit
    int compr;              // 8 bit
    int langcode;           // 1 bit (ac3 only)
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
    int timecod1e;          // 1 bit (ac3 only)
    int timecod2e;          // 1 bit (ac3 only)
    int addbsie;            // 1 bit
    int addbsil;            // 6 bit

    unsigned long offset;
    DolbyType_e dolbyType;
    int frmSize;
    int sampleRate;
    int blkPerFrm;

    DolbyHeaderInfo();
};

class DolbyEsParseDialog : public QDialog
{
    Q_OBJECT
private:
    unsigned long m_offset;
    QString m_filePath;
    QList<DolbyHeaderInfo> m_dolbyHeaderList;

    QTableWidget* m_tableWidget;
    QLabel* m_headerInfoLabel;

    void tableWidgetSetCellData(int row, int column, QString data);

    bool adjustEndianness(unsigned char* buf, int size);
    bool parseDolbyHeader(const QString& filePath);
    void setDolbyInfoToTableWidget();

    void displayDolbyHeaderInfo(const DolbyHeaderInfo& info);

    void constructUI();
    void connectSlots();
public:
    DolbyEsParseDialog(QString filePath = "", QWidget *parent = nullptr);
    ~DolbyEsParseDialog();

public slots:
    void onSelectionChanged();
};

#endif // DOLBYESPARSEDIALOG_H
