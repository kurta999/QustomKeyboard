#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QDialog>
#include <QRadioButton>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <map>

#include "BitEditorDialog.hpp"
#include "CanUdsRawDialog.hpp"
#include "CanLogTab.hpp"
#include "CanScriptTab.hpp"

#include "../src/CanEntryHandler.hpp"

class CanTxEntry;
class CanRxData;

enum CanSenderGridCol : int
{
    Sender_Id,
    Sender_DataSize,
    Sender_Data,
    Sender_Period,
    Sender_Count,
    Sender_LogLevel,
    Sender_FavouriteLevel,
    Sender_Comment,
    Sender_Max
};

class CanGridTx
{
public:
    CanGridTx(QTableWidget* table_widget);
    void AddRow(std::unique_ptr<CanTxEntry>& e);
    void UpdateTxCounter(uint32_t frame_id, size_t count);

    QTableWidget* table_widget = nullptr;
    std::map<uint16_t, CanTxEntry*> grid_to_entry;  /* Helper map for storing an additional ID to CanTxEntry */
};

class CanGridRx
{
public:
    CanGridRx(QTableWidget* table_widget);
    void AddRow(std::unique_ptr<CanRxData>& e);
    void UpdateRow(int num_row, uint32_t frame_id, std::unique_ptr<CanRxData>& e, std::string& comment);
    void ClearGrid();

    QTableWidget* table_widget = nullptr;
    std::map<uint16_t, CanRxData*> rx_grid_to_entry;  /* Helper map for storing an additional ID to CanRxData */
};


namespace Ui {
class CanTab;
}

class CanTab : public QWidget
{
    Q_OBJECT

public:
    explicit CanTab(QWidget *parent = nullptr);
    ~CanTab();

    void On10MsTimer();
    void RefreshTx();
    void UpdateGridForTxFrame(uint32_t frame_id, uint8_t* buffer);

    CanGridTx* can_grid_tx = nullptr;
    CanGridRx* can_grid_rx = nullptr;
    CanLogPanel* log_panel = nullptr;
    CanScriptTab* can_script_tab = nullptr;
private:


    std::string search_pattern_tx;
    std::string search_pattern_rx;

    std::string m_LastDataInput;

    Ui::CanTab *ui;
    void OnCustomContextMenuRequested(const QPoint& pos);
    void OnHeaderCustomContextMenuRequested(const QPoint& pos);

    BitEditorDialog* m_BitfieldEditor = nullptr;
    CanLogForFrameDialog* m_LogForFrame = nullptr;
    CanUdsRawDialog* m_UdsRawDialog = nullptr;
private slots:
    void OnCellValueChanged(int row, int col);
    
};


#endif // FORM_H
