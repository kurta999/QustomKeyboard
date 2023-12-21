#ifndef CMDEXECUTORAB_H
#define CMDEXECUTORAB_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QStringListModel>
#include <QTextEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QPushButton>
#include <QMouseEvent>
#include <QGridLayout>
#include <QLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QColorDialog>
#include <memory>
#include "../src/Sensors.hpp"

#include "ICmdHelper.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QCustomButton : public QPushButton
{
    Q_OBJECT

public:
    explicit QCustomButton(QWidget* parent = 0);

private slots:
    void mousePressEvent(QMouseEvent* e);

signals:
    void leftClicked();
    void rightClicked();
    void middleClicked();

public slots:

};

class CmdEditorDialog : public QDialog
{
    //Q_OBJECT
public:
    explicit CmdEditorDialog(QWidget* parent = nullptr);
    int ShowDialog(const std::string& name, const std::string& cmdline, uint32_t color, uint32_t background_color, bool bold, uint16_t font_size_px);

    std::string GetName() { return name_line->text().toStdString(); }
    std::string GetCmd() { return cmd_line->text().toStdString(); }
    uint32_t GetColor() { return m_color.rgba(); }
    uint32_t GetBackgroundColor() { return m_background_color.rgba(); }
    bool IsBold() { return is_bold->isChecked(); }
    uint32_t GetFontSize() { return font_size->value(); }

private:
    QLineEdit* name_line = nullptr;
    QLineEdit* cmd_line = nullptr;
    QPushButton* color_btn = nullptr;
    QPushButton* bck_color_btn = nullptr;
    QCheckBox* is_bold = nullptr;
    QSpinBox* font_size = nullptr;
    QColorDialog* color_dlg = nullptr;
    QColorDialog* bck_color_dlg = nullptr;

    QColor m_color;
    QColor m_background_color;
};


class CmdExecutorPanelPage;
class CmdExecutorPanelBase : public QWidget, public ICmdHelper

{
    //Q_OBJECT
public:
    CmdExecutorPanelBase();

    void OnPreReload(uint8_t page) override;
    void OnPreReloadColumns(uint8_t pages, uint8_t cols) override;
    void OnCommandLoaded(uint8_t page, uint8_t col, CommandTypes cmd) override;
    void OnPostReload(uint8_t page, uint8_t cols, CommandPageNames& names) override;

    void ReloadCommands();

    std::vector<CmdExecutorPanelPage*> m_Pages;

    QTabWidget* m_tabs;
private:
};

class CmdExecutorPanelPage : public QWidget
{
public:
    CmdExecutorPanelPage(QObject* parent, uint8_t id, uint8_t cols);

    void OnPreload(uint8_t cols);
    void OnPostReloadUpdate();
    void OnCommandLoaded(uint8_t col, CommandTypes cmd);

    uint8_t m_Id = 0;

private:

    void ToggleAllButtonClickability(bool toggle);
    void Execute(Command* c);

    //void OnSize(wxSizeEvent& evt);
    //void OnPanelRightClick(wxMouseEvent& event);
    void OnClick();
    void OnRightClick();
    void OnMiddleClick();

    void AddCommandElement(uint8_t col, Command* c);
    void AddSeparatorElement(uint8_t col, Separator s);

    void UpdateCommandButon(Command* c, QPushButton* btn, bool force_font_reset = false);
    void DeleteCommandButton(Command* c, QPushButton* btn);

    //CmdExecutorEditDialog* edit_dlg = nullptr;
    QGridLayout* m_BaseGrid = nullptr;
    std::vector<QVBoxLayout*> m_VertialBoxes;
    std::multimap<uint8_t, std::variant<QCustomButton*, QLine*>> m_ButtonMap;
    //wxDECLARE_EVENT_TABLE();
};
#endif // CMDEXECUTORAB_H
