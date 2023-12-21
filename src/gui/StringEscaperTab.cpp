#include "pch.hpp"

StringEscaperTab::StringEscaperTab(QTextEdit* input, QCheckBox* escape_percent, QCheckBox* add_slash_at_end, QPushButton* escape, QPushButton* base64_encode, QPushButton* base64_decode) :
    m_Input(input), m_EscapePercent(escape_percent), m_AddSlashAtEnd(add_slash_at_end), m_Escape(escape), m_Base64Encode(base64_encode), m_Base64Decode(base64_decode)
{
    m_Escape->setToolTip("Escape string to C format");
    connect(m_Escape, &QPushButton::clicked, [this]()
        {
            QString input = m_Input->toPlainText();
            std::string input_str = input.toStdString();

            StringEscaper escaper;
            escaper.EscapeString(input_str, m_EscapePercent->isChecked(), m_AddSlashAtEnd->isChecked());

            QClipboard* clipboard = QGuiApplication::clipboard();
            if(clipboard)
                clipboard->setText(QString::fromStdString(input_str));

            BalloonTip::showNotificaiton(QMessageBox::Information, "C string Escaper", "Strings escaped", 1000);
        });
}