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
#include <QTextEdit>

#include <map>

#include "../src/CanEntryHandler.hpp"

#include <ICanResultPanel.hpp>

class CanLogEntry;
class CanScriptTab : public QObject, public ICanResultPanel
{
public:
    CanScriptTab(QTextEdit* input, QTextEdit* output, QPushButton* run, QPushButton* run_selected, QPushButton* abort, QPushButton* clear,
        QPushButton* clear_output);

    void AddToLog(std::string str) override;
    void On10MsTimer();

private:
    QTableWidget* table_widget = nullptr;
    QLabel* m_LogStatsLabel = nullptr;

    QTextEdit* m_Input = nullptr;
    QTextEdit* m_Output = nullptr;
    QPushButton* m_Run = nullptr;
    QPushButton* m_RunSelected = nullptr;
    QPushButton* m_Abort = nullptr;
    QPushButton* m_Clear = nullptr;
    QPushButton* m_ClearOutput = nullptr;

    std::unique_ptr<CanScriptHandler> m_Script;

    std::mutex m_Mutex;
    std::vector<std::string> m_PendingLogMessages;
private:
};

