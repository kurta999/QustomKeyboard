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
#include <map>
#include "../src/CanEntryHandler.hpp"

class CanTxEntry;
class CanRxData;

#define MAX_BITEDITOR_FIELDS      32

class BitEditorDialog : public QDialog
{
public:
    explicit BitEditorDialog(QWidget* parent = nullptr);

    int ShowDialog(uint32_t frame_id, bool is_rx, CanBitfieldInfo& values);
    std::vector<std::string> GetOutput();

    enum class BitSelection
    {
        Decimal,
        Hex,
        Binary,
    };

    enum class ClickType
    {
        None,
        Ok,
        Close,
        Apply,
    };

    ClickType GetClickType() { return m_ClickType; }

    uint32_t GetFrameId() { return m_FrameId; }

private:
    void clicked(QAbstractButton* button);

    int m_Id = 0;
    uint8_t m_DataFormat = 0;
    QRadioButton* m_IsDecimal = {};
    QRadioButton* m_IsHex = {};
    QRadioButton* m_IsBinary = {};
    QLabel* m_InputLabel[MAX_BITEDITOR_FIELDS] = {};
    QLineEdit* m_Input[MAX_BITEDITOR_FIELDS] = {};
    QFormLayout* main_form = nullptr;
    QDialogButtonBox* buttonBox;

    ClickType m_ClickType = ClickType::None;

    CanBitfieldInfo m_BitfieldInfo;
    BitSelection bit_sel = BitSelection::Decimal;
    uint32_t m_FrameId = {};
};
