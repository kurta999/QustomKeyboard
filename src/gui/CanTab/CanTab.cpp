#include "pch.hpp"

#include "ui_CanTab.h"

extern ApplicationBase* g_app;

CanGridTx::CanGridTx(QTableWidget* table_widget) : table_widget(table_widget)
{
    table_widget->verticalHeader()->setVisible(false);
    table_widget->setSelectionBehavior(QAbstractItemView::SelectRows);

    QFont font;
    font.setWeight(QFont::Bold);
    table_widget->horizontalHeader()->setFont(font);
    table_widget->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color: #82817e }");
    table_widget->setContextMenuPolicy(Qt::CustomContextMenu);

    //table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_widget->horizontalHeader()->setStretchLastSection(true);
    table_widget->setColumnWidth(CanSenderGridCol::Sender_Id, 100);
    table_widget->setColumnWidth(CanSenderGridCol::Sender_DataSize, 50);
    table_widget->setColumnWidth(CanSenderGridCol::Sender_Data, 150);
    table_widget->setColumnWidth(CanSenderGridCol::Sender_Period, 70);
    table_widget->setColumnWidth(CanSenderGridCol::Sender_Count, 70);
    table_widget->setColumnWidth(CanSenderGridCol::Sender_LogLevel, 50);
    table_widget->setColumnWidth(CanSenderGridCol::Sender_FavouriteLevel, 50);

}

CanGridRx::CanGridRx(QTableWidget* table_widget) : table_widget(table_widget)
{
    table_widget->verticalHeader()->setVisible(false);
    table_widget->setSelectionBehavior(QAbstractItemView::SelectRows);

    QFont font;
    font.setWeight(QFont::Bold);
    table_widget->horizontalHeader()->setFont(font);
    table_widget->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color: #82817e }");
    table_widget->setContextMenuPolicy(Qt::CustomContextMenu);

    table_widget->horizontalHeader()->setStretchLastSection(true);
    table_widget->setColumnWidth(CanSenderGridCol::Sender_Id, 100);
    table_widget->setColumnWidth(CanSenderGridCol::Sender_DataSize, 50);
    table_widget->setColumnWidth(CanSenderGridCol::Sender_Data, 150);
    table_widget->setColumnWidth(CanSenderGridCol::Sender_Period, 70);
    table_widget->setColumnWidth(CanSenderGridCol::Sender_Count, 70);
    table_widget->setColumnWidth(CanSenderGridCol::Sender_LogLevel, 50);
    table_widget->setColumnWidth(CanSenderGridCol::Sender_FavouriteLevel, 50);
}

void CanGridTx::AddRow(std::unique_ptr<CanTxEntry>& e)
{
    table_widget->blockSignals(true);
    int num_row = table_widget->rowCount();
    table_widget->insertRow(num_row);

    table_widget->setItem(num_row, CanSenderGridCol::Sender_Id, new QTableWidgetItem(std::format("{:X}", e->id).c_str()));
    table_widget->setItem(num_row, CanSenderGridCol::Sender_DataSize, new QTableWidgetItem(std::format("{}", e->data.size()).c_str()));

    std::string hex;
    utils::ConvertHexBufferToString(e->data, hex);
    table_widget->setItem(num_row, CanSenderGridCol::Sender_Data, new QTableWidgetItem(hex.c_str()));
    table_widget->setItem(num_row, CanSenderGridCol::Sender_Period, new QTableWidgetItem(std::format("{}", e->period).c_str()));
    table_widget->setItem(num_row, CanSenderGridCol::Sender_Count, new QTableWidgetItem("0"));
    table_widget->setItem(num_row, CanSenderGridCol::Sender_LogLevel, new QTableWidgetItem(std::format("{}", e->log_level).c_str()));
    table_widget->setItem(num_row, CanSenderGridCol::Sender_FavouriteLevel, new QTableWidgetItem(std::format("{}", e->favourite_level).c_str()));
    table_widget->setItem(num_row, CanSenderGridCol::Sender_Comment, new QTableWidgetItem(e->comment.c_str()));

    table_widget->item(num_row, CanSenderGridCol::Sender_Count)->setFlags(table_widget->item(num_row, CanSenderGridCol::Sender_Count)->flags() & ~Qt::ItemIsEditable);

    if(e->m_color)
    {
        for(uint8_t i = 0; i != CanSenderGridCol::Sender_Max; i++)
            table_widget->item(num_row, i)->setForeground(QBrush(QColor::fromRgba(*e->m_color | 0xAA << 24)));
    }

    if(e->m_bg_color)  /* Set custom color if it's given */
    {
        for(uint8_t i = 0; i != CanSenderGridCol::Sender_Max; i++)
            table_widget->item(num_row, i)->setBackground(QBrush(QColor::fromRgba(*e->m_bg_color | 0xAA << 24)));
    }
    else  /* Otherway use two colors alternately for all of the lines */
    {
        for(uint8_t i = 0; i != CanSenderGridCol::Sender_Max; i++)
            table_widget->item(num_row, i)->setBackground(QBrush(QColor::fromRgba((num_row & 1) ? 0xAAE6E6E6 : 0xAAFFFFFF)));
    }

    if(e->m_is_bold)
    {
        QFont font;
        font.setWeight(QFont::Bold);
        for(uint8_t i = 0; i != CanSenderGridCol::Sender_Max; i++)
            table_widget->item(num_row, i)->setFont(font);
    }

    grid_to_entry[num_row] = e.get();
    table_widget->blockSignals(false);
}

void CanGridTx::UpdateTxCounter(uint32_t frame_id, size_t count)
{
    for(auto& i : grid_to_entry)
    {
        if(i.second->id == frame_id)
        {
            if(table_widget->item(i.first, CanSenderGridCol::Sender_Count))
                table_widget->item(i.first, CanSenderGridCol::Sender_Count)->setText(std::format("{}", count).c_str());
            else
                DBG("invalid column");
        }
    }
}

void CanGridRx::AddRow(std::unique_ptr<CanRxData>& e)
{
    std::unique_ptr<CanEntryHandler>& can_handler = g_app->can_entry;
    table_widget->blockSignals(true);
    int num_row = table_widget->rowCount();
    table_widget->insertRow(num_row);

    table_widget->setItem(num_row, CanSenderGridCol::Sender_Period, new QTableWidgetItem("0"));
    table_widget->setItem(num_row, CanSenderGridCol::Sender_Count, new QTableWidgetItem("0"));
    table_widget->setItem(num_row, CanSenderGridCol::Sender_LogLevel, new QTableWidgetItem(std::format("{}", e->log_level).c_str()));
    table_widget->setItem(num_row, CanSenderGridCol::Sender_FavouriteLevel, new QTableWidgetItem(std::format("{}", e->favourite_level).c_str()));
   
    if(table_widget->item(num_row, CanSenderGridCol::Sender_Id))
        table_widget->item(num_row, CanSenderGridCol::Sender_Id)->setFlags(table_widget->item(num_row, CanSenderGridCol::Sender_Id)->flags() & ~Qt::ItemIsEditable);
    if(table_widget->item(num_row, CanSenderGridCol::Sender_DataSize))
        table_widget->item(num_row, CanSenderGridCol::Sender_DataSize)->setFlags(table_widget->item(num_row, CanSenderGridCol::Sender_DataSize)->flags() & ~Qt::ItemIsEditable);
    if(table_widget->item(num_row, CanSenderGridCol::Sender_Data))
        table_widget->item(num_row, CanSenderGridCol::Sender_Data)->setFlags(table_widget->item(num_row, CanSenderGridCol::Sender_Data)->flags() & ~Qt::ItemIsEditable);
    if(table_widget->item(num_row, CanSenderGridCol::Sender_Period))
        table_widget->item(num_row, CanSenderGridCol::Sender_Period)->setFlags(table_widget->item(num_row, CanSenderGridCol::Sender_Period)->flags() & ~Qt::ItemIsEditable);
    if(table_widget->item(num_row, CanSenderGridCol::Sender_Count))
        table_widget->item(num_row, CanSenderGridCol::Sender_Count)->setFlags(table_widget->item(num_row, CanSenderGridCol::Sender_Count)->flags() & ~Qt::ItemIsEditable);
        
    for(uint8_t i = 0; i != CanSenderGridCol::Sender_Max; i++)
    {
        if(table_widget->item(num_row, i))
            table_widget->item(num_row, i)->setBackground(QBrush(QColor::fromRgba((num_row & 1) ? 0xAAE6E6E6 : 0xAAFFFFFF)));
    }

    rx_grid_to_entry[num_row] = e.get();
    table_widget->blockSignals(false);
}

void CanGridRx::UpdateRow(int num_row, uint32_t frame_id, std::unique_ptr<CanRxData>& e, std::string& comment)
{
    table_widget->blockSignals(true);
    table_widget->setItem(num_row, CanSenderGridCol::Sender_Id, new QTableWidgetItem(std::format("{:X}", frame_id).c_str()));
    table_widget->setItem(num_row, CanSenderGridCol::Sender_DataSize, new QTableWidgetItem(std::format("{}", e->data.size()).c_str()));

    std::string hex;
    utils::ConvertHexBufferToString(e->data, hex);
    table_widget->setItem(num_row, CanSenderGridCol::Sender_Data, new QTableWidgetItem(hex.c_str()));
    table_widget->setItem(num_row, CanSenderGridCol::Sender_Period                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      {
            QTableWidgetItem* tempItem = ui->m_RxTable->itemAt(pos);
            if(tempItem)
            {
                uint32_t frame_id = can_grid_rx->table_widget->item(tempItem->row(), CanSenderGridCol::Sender_Id)->text().toUInt(nullptr, 16);
                CanBitfieldInfo info = can_handler->GetMapForFrameId(frame_id, true);
                if(info.size() == 0)
                {
                    QMessageBox::warning(this, "No mapping for selected frame", "There are no mapping found for selected CAN Frame", QMessageBox::Ok, QMessageBox::Cancel);
                    return;
                }

                m_BitfieldEditor->ShowDialog(frame_id, true, info);
            }

        }
        else if(result == Log)
        {
            QTableWidgetItem* tempItem = ui->m_RxTable->itemAt(pos);
            if(tempItem)
            {
                uint32_t frame_id = can_grid_rx->table_widget->item(tempItem->row(), CanSenderGridCol::Sender_Id)->text().toUInt(nullptr, 16);

                std::vector<std::string> logs;
                can_handler->GenerateLogForFrame(frame_id, true, logs);

                if(logs.empty())
                {
                    QMessageBox::information(this, "Error", "In order to see the logs for frames, enable Recording in Log panel", QMessageBox::Ok, QMessageBox::Cancel);
                    return;
                }

                m_LogForFrame->ShowDialog(logs);
            }
        }
        else if(result == Remove)
        {
            auto rows = ui->m_RxTable->selectionModel()->selectedRows();
            for(auto& i : rows)
            {
                int row = i.row();

                uint32_t frame_id = 0;
                if(can_grid_rx->table_widget->item(row, CanSenderGridCol::Sender_Id))
                    frame_id = can_grid_rx->table_widget->item(row, CanSenderGridCol::Sender_Id)->text().toUInt(nullptr, 16);

                can_handler->m_rxData.erase(frame_id);
                can_grid_rx->table_widget->removeRow(row);
                can_grid_rx->ClearGrid();
            }
        }
    }
    else if(sender() == ui->m_TxTable)
    {
        QMenu* menu = new QMenu(this);
        QAction* EditBits = new QAction("Edit bits");
        QAction* Log = new QAction("Log");
        menu->addAction(EditBits);
        menu->addAction(Log);

        QAction* result = nullptr;
        result = menu->exec(ui->m_TxTable->viewport()->mapToGlobal(pos));
        if(result == EditBits)
        {
            QTableWidgetItem* tempItem = ui->m_TxTable->itemAt(pos);
            if(tempItem)
            {
                uint32_t frame_id = can_grid_tx->table_widget->item(tempItem->row(), CanSenderGridCol::Sender_Id)->text().toUInt(nullptr, 16);
                CanBitfieldInfo info = can_handler->GetMapForFrameId(frame_id, false);
                if(info.size() == 0)
                {
                    QMessageBox::warning(this, "No mapping for selected frame", "There are no mapping found for selected CAN Frame", QMessageBox::Ok, QMessageBox::Cancel);
                    return;
                }

                bool to_exit = false;
                while(!to_exit)
                {
                    int click_type = m_BitfieldEditor->ShowDialog(frame_id, false, info);

                    if(m_BitfieldEditor->GetClickType() == BitEditorDialog::ClickType::Apply || m_BitfieldEditor->GetClickType() == BitEditorDialog::ClickType::Ok)
                    {
                        int row = tempItem->row();
                        std::string hex;
                        utils::ConvertHexBufferToString(can_grid_tx->grid_to_entry[row]->data, hex);

                        ui->m_RxTable->item(row, CanSenderGridCol::Sender_Data)->setText(QString::fromStdString(hex));
                        ui->m_RxTable->item(row, CanSenderGridCol::Sender_DataSize)->setText(std::format("{}", can_grid_tx->grid_to_entry[row]->data.size()).c_str());
                    }

                    if(m_BitfieldEditor->GetClickType() != BitEditorDialog::ClickType::Apply)
                    {
                        to_exit = true;
                        break;
                    }
                }

            }
        }
        else if(result == Log)
        {
            QTableWidgetItem* tempItem = ui->m_TxTable->itemAt(pos);
            if(tempItem)
            {
                uint32_t frame_id = can_grid_tx->table_widget->item(tempItem->row(), CanSenderGridCol::Sender_Id)->text().toUInt(nullptr, 16);

                std::vector<std::string> logs;
                can_handler->GenerateLogForFrame(frame_id, false, logs);

                if(logs.empty())
                {
                    QMessageBox::information(this, "Error", "In order to see the logs for frames, enable Recording in Log panel", QMessageBox::Ok, QMessageBox::Cancel);
                    return;
                }

                m_LogForFrame->ShowDialog(logs);
            }

        }
    }

    qDebug() << "OnCustomContextMenuRequested: " << pos;
}

void CanTab::OnHeaderCustomContextMenuRequested(const QPoint& pos)
{
    std::unique_ptr<CanEntryHandler>& can_handler = g_app->can_entry;
    if(sender() == ui->m_RxTable->horizontalHeader() || sender() == ui->m_TxTable->horizontalHeader())
    {
        QTableWidget* table = sender() == ui->m_RxTable->horizontalHeader() ? ui->m_RxTable : ui->m_TxTable;

        QMenu* menu = new QMenu(this);
        QAction* EditLogLevel = new QAction("Edit log level");
        QAction* EditFavourites = new QAction("Edit favourites");
        menu->addAction(EditLogLevel);
        menu->addAction(EditFavourites);

        QAction* result = nullptr;
        result = menu->exec(table->viewport()->mapToGlobal(pos));
        if(result == EditLogLevel)
        {
            bool ok;
            QString text = QInputDialog::getText(this, tr("Default log level"), tr("Enter default log level for TX & RX list"),
                QLineEdit::Normal, QString::number(can_handler->GetRecordingLogLevel()), &ok);
            if(ok)
            {
                uint8_t loglevel = 0;
                try
                {
                    loglevel = std::stoi(text.toStdString());
                }
                catch(const std::exception& e)
                {
                    LOG(LogLevel::Warning, "stoi exception: {}", e.what());
                }
                can_handler->SetRecordingLogLevel(loglevel);
            }
        }
        else if(result == EditFavourites)
        {
            bool ok;
            QString text = QInputDialog::getText(this, tr("Default favourite level"), tr("Enter default favourite level for TX & RX list"),
                QLineEdit::Normal, QString::number(can_handler->GetRecordingLogLevel()), &ok);
            if(ok)
            {
                uint8_t loglevel = 0;
                try
                {
                    loglevel = std::stoi(text.toStdString());
                }
                catch(const std::exception& e)
                {
                    LOG(LogLevel::Warning, "stoi exception: {}", e.what());
                }
                can_handler->SetFavouriteLevel(loglevel);

                RefreshTx();
            }
        }
    }
}

void CanTab::RefreshTx()
{
    std::unique_ptr<CanEntryHandler>& can_handler = g_app->can_entry;
    std::scoped_lock lock{ can_handler->m };

    can_grid_tx->table_widget->setRowCount(0);

    uint8_t default_favourite_level = can_handler->GetFavouriteLevel();
    if(search_pattern_tx.empty())
    {
        for(auto& i : can_handler->entries)
        {
            if(default_favourite_level <= i->favourite_level)
                can_grid_tx->AddRow(i);
        }
    }
    else
    {
        for(auto& i : can_handler->entries)
        {
            if(default_favourite_level <= i->favourite_level)
            {
                if(boost::icontains(i->comment, search_pattern_tx))
                    can_grid_tx->AddRow(i);
            }
        }
    }

}

void CanTab::On10MsTimer()
{
    std::unique_ptr<CanEntryHandler>& can_handler = g_app->can_entry;
    if(search_pattern_rx.empty())
    {
        for(auto& entry : can_handler->m_rxData)
        {
            bool found_in_grid = false;
            for(auto& i : can_grid_rx->rx_grid_to_entry)
            {
                if(can_grid_rx->rx_grid_to_entry[i.first] == entry.second.get())
                {
                    std::string comment;
                    auto it = can_handler->rx_entry_comment.find(entry.first);
                    if(it != can_handler->rx_entry_comment.end())
                        comment = it->second;
                    can_grid_rx->UpdateRow(i.first, entry.first, entry.second, comment);

                    found_in_grid = true;
                    break;
                }
            }

            if(!found_in_grid)
            {
                can_grid_rx->AddRow(entry.second);
            }
        }
    }
    else
    {
        for(auto& entry : can_handler->m_rxData)
        {
            std::string comment;
            auto it = can_handler->rx_entry_comment.find(entry.first);
            if(it != can_handler->rx_entry_comment.end())
                comment = it->second;
            if(boost::icontains(comment, search_pattern_rx))
            {
                bool found_in_grid = false;
                for(auto& i : can_grid_rx->rx_grid_to_entry)
                {
                    if(can_grid_rx->rx_grid_to_entry[i.first] == entry.second.get())
                    {
                        can_grid_rx->UpdateRow(i.first, entry.first, entry.second, comment);
                        found_in_grid = true;
                        break;
                    }
                }

                if(!found_in_grid)
                {
                    can_grid_rx->AddRow(entry.second);
                }
            }
        }
    }

    log_panel->On10MsTimer();
    can_script_tab->On10MsTimer();
}

void CanTab::UpdateGridForTxFrame(uint32_t frame_id, uint8_t* buffer)
{
    std::string hex_str;
    utils::ConvertHexBufferToString((const char*)buffer, 8, hex_str);

    int rows = can_grid_tx->table_widget->rowCount();
    for(int i = 0; i != rows; i++)
    {
        if(can_grid_tx->grid_to_entry[i]->id == frame_id)
        {
            can_grid_tx->table_widget->blockSignals(true);
            ui->m_TxTable->item(i, CanSenderGridCol::Sender_Data)->setText(QString::fromStdString(hex_str));
            ui->m_TxTable->item(i, CanSenderGridCol::Sender_DataSize)->setText(std::format("{}", hex_str.length() / 2).c_str());
            can_grid_tx->table_widget->blockSignals(false);
            break;
        }
    }
}