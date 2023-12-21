#include "pch.hpp"

LoggerTab::LoggerTab(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* h_layout = new QHBoxLayout(this);
    
    m_LogLevel = new QComboBox(this);
    m_LogLevel->addItems({ "All", "Normal", "Notification", "Warning", "Error", "Critical"});
    h_layout->addWidget(m_LogLevel);

    m_FilterInput = new QLineEdit(this);
    h_layout->addWidget(m_FilterInput);

    m_FilterButton = new QPushButton(this);
    m_FilterButton->setText("Filter");
    m_FilterButton->setToolTip("Filter log messages based on input filter");
    h_layout->addWidget(m_FilterButton);

    m_ToggleAutoscroll = new QPushButton(this);
    m_ToggleAutoscroll->setText("Toggle Auto-Scroll");
    m_ToggleAutoscroll->setToolTip("Toggle auto-scroll");
    h_layout->addWidget(m_ToggleAutoscroll);

    QSpacerItem* spacer_1 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    h_layout->addSpacerItem(spacer_1);

    layout->addLayout(h_layout);

    m_LogListView = new QListView(this);
    layout->addWidget(m_LogListView);

    QHBoxLayout* h_layout_2 = new QHBoxLayout(this);
    m_ClearButton = new QPushButton(this);
    m_ClearButton->setText("Clear");
    m_ClearButton->setToolTip("Clear log messages");
    h_layout_2->addWidget(m_ClearButton);

    QSpacerItem* spacer_2 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    h_layout_2->addSpacerItem(spacer_2);
    layout->addLayout(h_layout_2);

    connect(m_FilterButton, &QPushButton::clicked, [this]()
        {
            ExecuteSearchInLogfile();
        });

    connect(m_ToggleAutoscroll, &QPushButton::clicked, [this]()
        {
            m_AutoScroll ^= 1;
            if(m_AutoScroll)
                m_ToggleAutoscroll->setStyleSheet("QPushButton{ color: black; }");
            else
                m_ToggleAutoscroll->setStyleSheet("QPushButton{ color: red; }");
        });

    connect(m_ClearButton, &QPushButton::clicked, [this]()
        {
            ClearEntries();
        });

    m_LogModel = new QStringListModel(this);
    m_LogListView->setModel(m_LogModel);
    m_LogListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_LogListView->setSelectionMode(QAbstractItemView::ExtendedSelection);

}

void LoggerTab::Update()
{
    std::unique_lock lock(Logger::Get()->m_mutex);
    for(auto& i : Logger::Get()->preinit_entries)
    {
        if(!i.message.empty())
        {
            if(!IsFilterered(i.file))
            {
                // Get the position
                int row = m_LogModel->rowCount();

                m_LogModel->insertRows(row, 1);
                m_LogModel->setData(m_LogModel->index(row), QString::fromStdString(i.message));

                if(m_AutoScroll)
                    m_LogListView->scrollToBottom();
            }
        }

    }
    Logger::Get()->preinit_entries.clear();
}

void LoggerTab::ExecuteSearchInLogfile()
{
    std::string filter = m_FilterInput->text().toStdString();
    std::string log_level = m_LogLevel->currentText().toStdString();
    if(log_level == "All")
        log_level.clear();

    bool ret = Logger::Get()->SearchInLogFile(filter, log_level);
    if(ret)
        m_LogListView->scrollToBottom();
}


template<typename T> bool LoggerTab::IsFilterered(const T& file)
{
    for(auto& i : m_LogFilters)
    {
        if(i.empty()) continue;
        if(std::search(file.begin(), file.end(), i.begin(), i.end()) != file.end())
        {
            return true;
        }
    }
    return false;
}

void LoggerTab::ClearEntries()
{
    m_LogModel->removeRows(0, m_LogModel->rowCount());
}

void LoggerTab::AppendLog(const std::string& file, const std::string& line, bool scroll_to_end)
{
    if(!IsFilterered(file))
    {
        // Get the position
        int row = m_LogModel->rowCount();

        m_LogModel->insertRows(row, 1);
        m_LogModel->setData(m_LogModel->index(row), QString::fromStdString(line));

        if(m_AutoScroll)
            m_LogListView->scrollToBottom();
    }
}

void LoggerTab::AppendLog(const std::wstring& file, const std::wstring& line, bool scroll_to_end)
{
    if(!IsFilterered(file))
    {
        /*m_Log->Append(wxString(line));
        if(scroll_to_end && m_AutoScroll)
            m_Log->ScrollLines(m_Log->GetCount());*/
    }
}
