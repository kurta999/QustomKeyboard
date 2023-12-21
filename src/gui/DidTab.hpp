
#ifndef DIDTAB_H
#define DIDTAB_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QStringListModel>
#include <QTextEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTableWidget>
#include <QRegularExpression>
#include <memory>
#include "../src/Sensors.hpp"

#include "../src/gui/CanTab/CanTab.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class DidEntry;
enum DidGridCol : int
{
    Did_ID,
    Did_Type,
    Did_Name,
    Did_Value,
    Did_Len,
    Did_MinVal,
    Did_MaxVal,
    Did_Timestamp,
    Did_Max
};

class DidGrid : public QWidget
{
public:
    DidGrid(QWidget* parent);

    void AddRow(std::unique_ptr<DidEntry>& entry);

    QTableWidget* table_widget = nullptr;

    std::map<uint16_t, DidEntry*> grid_to_entry;  /* Helper map for storing an additional ID to CanTxEntry */
    std::map<uint16_t, uint16_t> did_to_row;  /* Helper map for storing an additional ID to CanTxEntry */

    size_t cnt = 0;
};

class DidTab : public QWidget

{
    //Q_OBJECT
public:
    DidTab(QWidget* parent = nullptr);


    //void dragMoveEvent(QDragMoveEvent* evt) override;

    DidGrid* did_grid;
    QPushButton* m_Refresh = nullptr;
    QPushButton* m_RefreshSelected = nullptr;
    QPushButton* m_Abort = nullptr;
    QPushButton* m_ClearDids = nullptr;
    QPushButton* m_SaveCache = nullptr;

    void UpdateDidList();
    void WriteDid(uint16_t did, uint8_t* data_to_write, uint16_t size);
    void On100msTimer();

private:
    bool is_dids_initialized = false;
    std::string search_pattern;
};

#endif // STRUCTPARSERTAB_H
