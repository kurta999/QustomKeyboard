
#ifndef STRINGESCAPERTAB_H
#define STRINGESCAPERTAB_H

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
#include <memory>
#include "../src/Sensors.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class StringEscaperTab : public QObject

{
    //Q_OBJECT
public:
    StringEscaperTab(QTextEdit* input, QCheckBox* escape_percent, QCheckBox* add_slash_at_end, QPushButton* escape, QPushButton* base64_encode, QPushButton* base64_decode);

private:
    QTextEdit* m_Input = nullptr;
    QCheckBox* m_EscapePercent = nullptr;
    QCheckBox* m_AddSlashAtEnd = nullptr;
    QPushButton* m_Escape = nullptr;
    QPushButton* m_Base64Encode = nullptr;
    QPushButton* m_Base64Decode = nullptr;
};

#endif // STRINGESCAPERTAB_H
