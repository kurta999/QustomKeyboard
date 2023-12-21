
#ifndef STRUCTPARSERTAB_H
#define STRUCTPARSERTAB_H

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

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QList<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

class StructParserTab : public QTextEdit

{
    //Q_OBJECT
public:
    StructParserTab(QTextEdit* input, QTextEdit* output, QPushButton* generate, QCheckBox* is_modbus, QSpinBox* padding, QComboBox* ptr_size);


    void dragMoveEvent(QDragMoveEvent* evt) override;

private:
    QTextEdit* m_StructInput = nullptr;
    QTextEdit* m_StructOutput = nullptr;
    QPushButton* m_Generate = nullptr;
    QCheckBox* m_IsModbus = nullptr;
    QSpinBox* m_Padding = nullptr;
    QComboBox* m_PtrSize = nullptr;
    Highlighter* in_highlighter;
    Highlighter* out_highlighter;
};

#endif // STRUCTPARSERTAB_H
