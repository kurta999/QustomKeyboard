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
#include <QTextEdit>
#include <map>
#include <future>
#include "../src/CanEntryHandler.hpp"

class CanTxEntry;
class CanRxData;

#define MAX_BITEDITOR_FIELDS      32

class CanUdsRawDialog : public QDialog
{
public:
    explicit CanUdsRawDialog(QWidget* parent = nullptr);
    ~CanUdsRawDialog();

    int ShowDialog();
    bool IsApplyClicked() { return m_IsApplyClicked; }

    uint32_t GetSenderId();
    uint32_t GetReceiverId();
    uint32_t GetDelayBetweenFrames();
    uint32_t GetWaitingTimeForFrames();
    std::string GetSentData();


private:
    void clicked(QAbstractButton* button);

    void HandleFrameSending();

    QFormLayout* main_form = nullptr;
    QDialogButtonBox* button_box = nullptr;

    QLineEdit* m_SenderId = nullptr;
    QLineEdit* m_ReceiverId = nullptr;
    QLineEdit* m_DelayBetweenFrames = nullptr;
    QLineEdit* m_RecvDelayFrames = nullptr;
    QPushButton* m_ReadDid = nullptr;
    QPushButton* m_WriteDid = nullptr;
    QPushButton* m_EcuReset = nullptr;
    QPushButton* m_Clear = nullptr;
    QTextEdit* m_DataToSend = nullptr;
    QPushButton* m_SendBtn = nullptr;
    QTextEdit* m_DataRecv = nullptr;

    uint32_t m_LastUdsSenderId = 0;
    uint32_t m_LastUdsReceiverId = 0;
    uint32_t m_LastDelayBetweenFrames = 0;
    uint32_t m_LastRecvWaitingTime = 0;
    std::string m_LastUdsInput;
    std::future<bool> m_isotp_future;

    bool m_IsApplyClicked = false;

};
