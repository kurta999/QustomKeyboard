#include "pch.hpp"

extern ApplicationBase* g_app;

CanScriptTab::CanScriptTab(QTextEdit* input, QTextEdit* output, QPushButton* run, QPushButton* run_selected, QPushButton* abort, QPushButton* clear,
    QPushButton* clear_output) : 
    m_Input(input), m_Output(output), m_Run(run), m_RunSelected(run_selected), m_Abort(abort), m_Clear(clear), 
        m_ClearOutput(clear_output)
{
    m_Script = std::make_unique<CanScriptHandler>(*this);

    connect(m_Run, &QPushButton::clicked, [this]()
        {
            if(m_Script->IsScriptRunning())
            {
                //QMessageBox::warning(this, "Warning", "Given CAN Fame ID already added to the list!", QMessageBox::Ok, QMessageBox::Cancel);
                //QMessageBox::warning(this, "A script is already running", "Click on abort to abort it, before running another one", QMessageBox::Ok, QMessageBox::Cancel);
                return;
            }
           
            std::string input = m_Input->toPlainText().toStdString();
            try
            {
                boost::algorithm::replace_all(input, "\r", "");  /* Thanks Windows */
                m_Script->RunScript(input);
            }
            catch(std::exception& e)
            {
                LOG(LogLevel::Error, "Exception: {}", e.what());
               // wxMessageDialog(this, std::format("Invalid input!\n{}", e.what()), "Error", wxOK).ShowModal();
            }

            if(!input.empty())
            {
#ifdef _WIN32
                boost::algorithm::replace_all(input, "\n", "\r\n");  /* LF isn't enough for TextCtrl for some reason... */
#endif
                m_Output->clear();
                //m_RunButton->SetForegroundColour(*wxBLACK);
            }
            else
            {
                LOG(LogLevel::Verbose, "Empty input");
            }
            //path.Clear();

        });

    connect(m_Abort, &QPushButton::clicked, [this]()
        {
            m_Script->AbortRunningScript();
        });

    connect(m_Clear, &QPushButton::clicked, [this]()
        {
            m_Input->clear();
            m_Output->clear();
        });

    connect(m_ClearOutput, &QPushButton::clicked, [this]()
        {
            m_Output->clear();
        });

}

void CanScriptTab::AddToLog(std::string str)
{
    std::unique_lock lock(m_Mutex);
    m_PendingLogMessages.push_back(str);
}

void CanScriptTab::On10MsTimer()
{
    if(m_PendingLogMessages.empty())
        return;

    std::unique_lock lock(m_Mutex);
    for(auto& i : m_PendingLogMessages)
    {
        m_Output->append(QString::fromStdString(i));
    }
    m_PendingLogMessages.clear();
}