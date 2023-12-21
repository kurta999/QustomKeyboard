#include "pch.hpp"

StructParserTab::StructParserTab(QTextEdit* input, QTextEdit* output, QPushButton* generate, QCheckBox* is_modbus, QSpinBox* padding, QComboBox* ptr_size) :
    m_StructInput(input), m_StructOutput(output), m_Generate(generate), m_IsModbus(is_modbus), m_Padding(padding), m_PtrSize(ptr_size)
{
    m_PtrSize->addItems({ "1", "2", "4", "8" });

    in_highlighter = new Highlighter(m_StructInput->document());
    out_highlighter = new Highlighter(m_StructOutput->document());

    m_StructInput->acceptDrops();

    m_Generate->setToolTip("Generate structur offsets for structure members");
    connect(m_Generate, &QPushButton::clicked, [this]()
        {
            static const uint8_t pointer_sizes[4] = { 1, 2, 4, 8 };
            size_t pointer_size = pointer_sizes[m_PtrSize->currentIndex()];
            int struct_padding = m_Padding->value();

            QString input = m_StructInput->toPlainText();
            std::string input_str = input.toStdString();

            std::string output;
            try
            {
                StructParser::Get()->ParseStructure(input_str, output, struct_padding, pointer_size);
            }
            catch(std::exception& e)
            {
                LOG(LogLevel::Error, "Exception: {}", e.what());
            }
            m_StructOutput->setPlainText(QString::fromStdString(output));
        });

    /*
    connect(m_StructInput, &QTextEdit::dragMoveEvent, [this]()
        {
            qDebug() << "test";
        });
        */
}

void StructParserTab::dragMoveEvent(QDragMoveEvent* evt)
{
    qDebug() << "test";
}

Highlighter::Highlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
        QStringLiteral("\\bchar\\b"), QStringLiteral("\\bclass\\b"), QStringLiteral("\\bconst\\b"),
        QStringLiteral("\\bdouble\\b"), QStringLiteral("\\benum\\b"), QStringLiteral("\\bexplicit\\b"),
        QStringLiteral("\\bfriend\\b"), QStringLiteral("\\binline\\b"), QStringLiteral("\\bint\\b"),
        QStringLiteral("\\blong\\b"), QStringLiteral("\\bnamespace\\b"), QStringLiteral("\\boperator\\b"),
        QStringLiteral("\\bprivate\\b"), QStringLiteral("\\bprotected\\b"), QStringLiteral("\\bpublic\\b"),
        QStringLiteral("\\bshort\\b"), QStringLiteral("\\bsignals\\b"), QStringLiteral("\\bsigned\\b"),
        QStringLiteral("\\bslots\\b"), QStringLiteral("\\bstatic\\b"), QStringLiteral("\\bstruct\\b"),
        QStringLiteral("\\btemplate\\b"), QStringLiteral("\\btypedef\\b"), QStringLiteral("\\btypename\\b"),
        QStringLiteral("\\bunion\\b"), QStringLiteral("\\bunsigned\\b"), QStringLiteral("\\bvirtual\\b"),
        QStringLiteral("\\bvoid\\b"), QStringLiteral("\\bvolatile\\b"), QStringLiteral("\\bbool\\b"),
        QStringLiteral("\\bfloat\\b")
    };
    for(const QString& pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
        //! [0] //! [1]
    }
    //! [1]

    //! [2]
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression(QStringLiteral("\\bQ[A-Za-z]+\\b"));
    rule.format = classFormat;
    highlightingRules.append(rule);
    //! [2]

    //! [3]
    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);
    //! [3]

    //! [4]
    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);
    //! [4]

    //! [5]
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format = functionFormat;
    highlightingRules.append(rule);
    //! [5]

    //! [6]
    commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}
//! [6]

//! [7]
void Highlighter::highlightBlock(const QString& text)
{
    for(const HighlightingRule& rule : std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while(matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    //! [7] //! [8]
    setCurrentBlockState(0);
    //! [8]

    //! [9]
    int startIndex = 0;
    if(previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    //! [9] //! [10]
    while(startIndex >= 0) {
        //! [10] //! [11]
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if(endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else {
            commentLength = endIndex - startIndex
                + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}