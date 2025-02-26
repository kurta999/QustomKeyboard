nId":"","viewId":"2f9b905d-30ea-f2d2-ad0d-c60d598bd687"}                                                                                                                                                                                                                                                                                                                                                                                                                                                                        std::string>& values)
{
    m_Log->reset();
    for(const auto i : values)
    {
        // Get the position
        int row = m_LogModel->rowCount();

        m_LogModel->insertRows(row, 1);
        m_LogModel->setData(m_LogModel->index(row), QString::fromStdString(i));
    }
    int ret = exec();
    DBG("ret: %d", ret);
    return ret;
}


CanLogPanel::CanLogPanel(QTableWidget* table, QLabel* log_stats_label, QPushButton* record, QPushButton* pause, QPushButton* stop, QPushButton* clear,
    QPushButton* autoscroll, QSpinBox* loglevel, QPushButton* save_log) :
    table_widget(table), m_LogStatsLabel(log_stats_label), m_Record(record), m_Pause(pause), m_Stop(stop), m_Clear(clear), m_AutoScroll(autoscroll),
    m_LogLevel(loglevel), m_SaveLog(save_log)
{
    m_LogStatsLabel->setStyleSheet("font-weight:700; color:#062dd9;");

    //table_widget->verticalHeader()->setVisible(false);
    table_widget->setSelectionBehavior(QAbstractItemView::SelectRows);

    QFont font;
    font.setWeight(QFont::Bold);
    table_widget->horizontalHeader()->setFont(font);
    table_widget->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color: #82817e }");
    table_widget->setContextMenuPolicy(Qt::CustomContextMenu);

    table_widget->horizontalHeader()->setStretchLastSection(true);
    table_widget->setColumnWidth(CanLogGridCol::Log_Time, 70);
    table_widget->setColumnWidth(CanLogGridCol::Log_Direction, 70);
    table_widget->setColumnWidth(CanLogGridCol::Log_Id, 70);
    table_widget->setColumnWidth(CanLogGridCol::Log_DataSize, 25);
    table_widget->setColumnWidth(CanLogGridCol::Log_Data, 150);
    table_widget->setColumnWidth(CanLogGridCol::Log_Comment, 100);

    m_Record->setToolTip("Start recording for received & sent CAN frames");
    connect(m_Record, &QPushButton::clicked, [this]()
        {
            std::unique_ptr<CanEntryHandler>& can_handler = g_app->can_entry;
            can_handler->ToggleRecording(true, false);
        });

    m_Pause->setToolTip("Suspend recording for received & sent CAN frames");
    connect(m_Pause, &QPushButton::clicked, [this]()
        {
            std::unique_ptr<CanEntryHandler>& can_handler = g_app->can_entry;
            can_handler->ToggleRecording(false, true);
        });

    m_Stop->setToolTip("Suspend recording for received & sent CAN frames, clear everything");
    connect(m_Stop, &QPushButton::clicked, [this]()
        {
            ClearRecordingsFromGrid();

            std::unique_ptr<CanEntryHandler>& can_handler = g_app->can_entry;
            can_handler->ClearRecording();
        });

    m_AutoScroll->setToolTip("Toggle auto-scroll");
    connect(m_AutoScroll, &QPushButton::clicked, [this]()
        {
            std::unique_ptr<CanEntryHandler>& can_handler = g_app->can_entry;
            can_handler->ToggleRecording(false, true);

            m_IsAutoScroll ^= 1;
        });

    m_SaveLog->setToolTip("Save recording to file");
    connect(m_AutoScroll, &QPushButton::clicked, [this]()
        {
#ifdef _WIN32
            const auto now = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());

            if(std::filesystem::exists("Can"))
                std::filesystem::create_directory("Can");
            std::string log_format = std::format("Can/CanLog_{:%Y.%m.%d_%H_%M_%OS}.csv", now);
            std::filesystem::path p(log_format);

            std::unique_ptr<CanEntryHandler>& can_handler = g_app->can_entry;
            can_handler->SaveRecordingToFile(p);
#endif

        });

}

void CanLogPanel::On10MsTimer()
{
    std::unique_ptr<CanEntryHandler>& can_handler = g_app->can_entry;
    static std::string last_search_pattern;
    static uint64_t last_tx_cnt = 0, last_rx_cnt = 0;

    if(search_pattern.empty())
    {
        if(last_tx_cnt != can_handler->GetTxFrameCount() || last_rx_cnt != can_handler->GetRxFrameCount())
        {
            m_LogStatsLabel->setText(std::format("Log :: TX: {}, RX: {}, Total: {}", can_handler->GetTxFrameCount(), can_handler->GetRxFrameCount(),
                can_handler->GetTxFrameCount() + can_handler->GetRxFrameCount()).c_str());
        }
    }
    else
    {
        if(last_tx_cnt != can_handler->GetTxFrameCount() || last_rx_cnt != can_handler->GetRxFrameCount() || last_search_pattern != search_pattern)
        {
            m_LogStatsLabel->setText(std::format("Log :: Filter: {}, TX: {}, RX: {}, Total: {}", search_pattern,
                can_handler->GetTxFrameCount(), can_handler->GetRxFrameCount(), can_handler->GetTxFrameCount() + can_handler->GetRxFrameCount()).c_str());
        }
    }

    last_tx_cnt = can_handler->GetTxFrameCount();
    last_rx_cnt = can_handler->GetRxFrameCount();
    last_search_pattern = search_pattern;

    if(!can_handler->m_LogEntries.empty())
    {
        if(!is_something_inserted)
            inserted_until = 0;

        std::vector<std::unique_ptr<CanLogEntry>>::const_iterator it = can_handler->m_LogEntries.end();
        if(inserted_until == 0)
            it = can_handler->m_LogEntries.begin();
        else if(inserted_until < can_handler->m_LogEntries.size())
        {
            it = can_handler->m_LogEntries.begin();
            std::advance(it, inserted_until);
        }

        if(it == can_handler->m_LogEntries.end())
        {
            //DBG("shit happend");
            return;
        }

        if(it != can_handler->m_LogEntries.end())
        {
            std::scoped_lock lock{ can_handler->m };
            for(; it != can_handler->m_LogEntries.end(); ++it)
            {
                std::string comment;
                if((*it)->direction == 1)
                {
                    auto comment_it = can_handler->rx_entry_comment.find((*it)->frame_id);
                    if(comment_it != can_handler->rx_entry_comment.end())
                    {
                        comment = comment_it->second;
                    }
                }
                else
                {
                    for(auto& i : can_handler->entries)
                    {
                        if(i->id == (*it)->frame_id)
                        {
                            comment = i->comment;
                            break;
                        }
                    }
                }

                bool insert_row = false;
                if(search_pattern.empty())
                {
                    insert_row = true;
                }
                else
                {
                    if(boost::icontains(comment, search_pattern))
                        insert_row = true;
                }

                if(insert_row)
                    InsertRow((*it)->last_execution, (*it)->direction, (*it)->frame_id, (*it)->data, comment);
                inserted_until++;
            }
            //inserted_until = std::distance(can_handler->m_LogEntries.begin(), it);
            is_something_inserted = true;
        }
    }
}

void CanLogPanel::InsertRow(std::chrono::steady_clock::time_point& t1, uint8_t direction, uint32_t id, std::vector<uint8_t>& data, std::string& comment)
{
    std::unique_ptr<CanEntryHandler>& can_handler = g_app->can_entry;

    table_widget->blockSignals(true);
    int num_row = table_widget->rowCount();
    table_widget->insertRow(num_row);

    uint64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - can_handler->GetStartTime()).count();
    table_widget->setItem(num_row, CanLogGridCol::Log_Time, new QTableWidgetItem(std::format("{:.3f}", static_cast<double>(elapsed) / 1000.0).c_str()));

    std::string hex;
    utils::ConvertHexBufferToString(data, hex);
    table_widget->setItem(num_row, CanLogGridCol::Log_Data, new QTableWidgetItem(hex.c_str()));
    table_widget->setItem(num_row, CanLogGridCol::Log_Direction, new QTableWidgetItem(direction == CAN_LOG_DIR_TX ? "TX" : "RX"));
    table_widget->setItem(num_row, CanLogGridCol::Log_Id, new QTableWidgetItem(std::format("{:X}", id).c_str()));
    table_widget->setItem(num_row, CanLogGridCol::Log_DataSize, new QTableWidgetItem(std::format("{}", data.size()).c_str()));
    table_widget->setItem(num_row, CanLogGridCol::Log_Comment, new QTableWidgetItem(comment.c_str()));

    for(uint8_t i = 0; i != CanLogGridCol::Log_Max; i++)
    {
        if(table_widget->item(num_row, i))
        {
            table_widget->item(num_row, i)->setFlags(table_widget->item(num_row, i)->flags() & ~Qt::ItemIsEditable);
            table_widget->item(num_row, i)->setBackground(QBrush(QColor::fromRgba((direction == CAN_LOG_DIR_RX) ? 0xAAE6E6E6 : 0xAAFFFFFF)));
        }
    }

    if(m_IsAutoScroll)
        table_widget->scrollToBottom();

    table_widget->blockSignals(false);
}

void CanLogPanel::ClearRecordingsFromGrid()
{
    table_widget->blockSignals(true);
    table_widget->clearContents();
    table_widget->setRowCount(0);
    table_widget->blockSignals(false);

    cnt = 0;
    is_something_inserted = false;
    inserted_until = 0;
}