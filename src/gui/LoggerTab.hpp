
#ifndef LOGGERTAB_H
#define LOGGERTAB_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QStringListModel>
#include <QTextEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <memory>
#include "../src/Sensors.hpp"

#include "../src/gui/CanTab/CanTab.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class LoggerTab : public QWidget, public ILogHelper

{
    Q_OBJECT
public:
    LoggerTab(QWidget* parent = nullptr);

    void Update();

private:
    void ExecuteSearchInLogfile();
    template <typename T> bool IsFilterered(const T& file);

    virtual void ClearEntries() override;
    virtual void AppendLog(const std::string& file, const std::string& line, bool scroll_to_end = false) override;
    virtual void AppendLog(const std::wstring& file, const std::wstring& line, bool scroll_to_end = false) override;

    bool m_AutoScroll = true;
    std::vector<std::string> m_LogFilters;

    QComboBox* m_LogLevel = nullptr;
    QLineEdit* m_FilterInput = nullptr;
    QPushButton* m_FilterButton = nullptr;
    QPushButton* m_ToggleAutoscroll = nullptr;
    QListView* m_LogListView = nullptr;
    QPushButton* m_ClearButton = nullptr;

    QStringListModel* m_LogModel = nullptr;
};

#endif // STRUCTPARSERTAB_H
