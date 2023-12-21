#pragma once

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QDialog>
#include <QRadioButton>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QListView>
#include <QStringListModel>
#include <map>
#include "../src/CanEntryHandler.hpp"

enum CanLogGridCol : int
{
    Log_Time,
    Log_Direction,
    Log_Id,
    Log_DataSize,
    Log_Data,
    Log_Comment,
    Log_Max
};

#define MAX_BITEDITOR_FIELDS      32

class CanLogForFrameDialog : public QDialog
{
public:
    explicit CanLogForFrameDialog(QWidget* parent = nullptr);

    int ShowDialog(std::vector<std::string>& values);


private:
    void clicked(QAbstractButton* button);

    int m_Id = 0;
    uint8_t m_DataFormat = 0;
    QListView* m_Log = nullptr;
    QStringListModel* m_LogModel = nullptr;

    QFormLayout* main_form = nullptr;
    QDialogButtonBox* buttonBox;
};


class CanLogEntry;
class CanLogPanel : public QObject
{
public:
    CanLogPanel(QTableWidget* table, QLabel* log_stats_label, QPushButton* record, QPushButton* pause, QPushButton* stop, QPushButton* clear,
        QPushButton* autoscroll, QSpinBox* loglevel, QPushButton* save_log);

    void On10MsTimer();
    void InsertRow(std::chrono::steady_clock::time_point& t1, uint8_t direction, uint32_t id, std::vector<uint8_t>& data, std::string& comment);

    QTableWidget* table_widget = nullptr;
    QLabel* m_LogStatsLabel = nullptr;

    QPushButton* m_Record;
    QPushButton* m_Pause;
    QPushButton* m_Stop;
    QPushButton* m_Clear;
    QPushButton* m_AutoScroll;
    QSpinBox* m_LogLevel;
    QPushButton* m_SaveLog;

private:
    void ClearRecordingsFromGrid();

    size_t cnt = 0;
    std::string search_pattern;
    bool m_IsAutoScroll = true;

    bool is_something_inserted = false;
    std::size_t inserted_until = 0;
};

