#include "pch.hpp"

extern ApplicationBase* g_app;

DidGrid::DidGrid(QWidget* parent)
{
    table_widget = new QTableWidget(parent);
    table_widget->setColumnCount(DidGridCol::Did_Max);
    table_widget->setHorizontalHeaderLabels({ "DID", "Type", "Name", "Value", "Len", "Min", "Max", "Timestamp"});

    table_widget->verticalHeader()->setVisible(false);
    table_widget->setSelectionBehavior(QAbstractItemView::SelectRows);

    QFont font;
    font.setWeight(QFont::Bold);
    table_widget->horizontalHeader()->setFont(font);
    table_widget->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color: #82817e }");
    table_widget->setContextMenuPolicy(Qt::CustomContextMenu);

    //table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_widget->horizontalHeader()->setStretchLastSection(true);
    table_widget->setColumnWidth(DidGridCol::Did_ID, 50);
    table_widget->setColumnWidth(DidGridCol::Did_Type, 100);
    table_widget->setColumnWidth(DidGridCol::Did_Name, 200);
    table_widget->setColumnWidth(DidGridCol::Did_Value, 250);
    table_widget->setColumnWidth(DidGridCol::Did_Timestamp, 135);

}

void DidGrid::AddRow(std::unique_ptr<DidEntry>& entry)
{
    table_widget->blockSignals(true);
    int num_row = table_widget->rowCount();
    table_widget->insertRow(num_row);

    table_widget->setItem(num_row, DidGridCol::Did_ID, new QTableWidgetItem(std::format("{:X}", entry->id).c_str()));
    table_widget->setItem(num_row, DidGridCol::Did_Type, new QTableWidgetItem(std::format("{}", XmlDidLoader::GetStringFromType(entry->type)).c_str()));
    table_widget->setItem(num_row, DidGridCol::Did_Name, new QTableWidgetItem(QString::fromStdString(entry->name)));
    table_widget->setItem(num_row, DidGridCol::Did_Value, new QTableWidgetItem(QString::fromStdString(entry->value_str)));
    table_widget->setItem(num_row, DidGridCol::Did_Len, new QTableWidgetItem(std::format("{}", entry->len).c_str()));
    table_widget->setItem(num_row, DidGridCol::Did_MinVal, new QTableWidgetItem(std::format("{}", entry->min).c_str()));
    table_widget->setItem(num_row, DidGridCol::Did_MaxVal, new QTableWidgetItem(std::format("{}", entry->max).c_str()));
    table_widget->setItem(num_row, DidGridCol::Did_Timestamp, new QTableWidgetItem("N/A"));

    QString last_update_str;
    if(!entry->last_update.is_not_a_date_time())
    {
        last_update_str = QString::fromStdString(boost::posix_time::to_iso_extended_string(entry->last_update));
    }


    if(!last_update_str.isEmpty())
    {
        if(entry->nrc != 0)  /* TODO: create a function for this, because it's a duplicate */
        {
            switch(entry->nrc)
            {
                case 0x78:
                {
                    //m_grid->SetCellValue(wxGridCellCoords(cnt, DidGridCol::Did_Value), "Pending... NRC 78");
                    //m_grid->SetCellBackgroundColour(cnt, DidGridCol::Did_Value, *wxBLUE);
                    break;
                }
                default:
                {/*
                    m_grid->SetCellValue(wxGridCellCoords(cnt, DidGridCol::Did_Value), wxString::Format("NRC %X", entry->nrc));
                    m_grid->SetCellBackgroundColour(cnt, DidGridCol::Did_Value, *wxRED);

                    wxFont cell_font = m_grid->GetCellFont(cnt, Did_Name);
                    cell_font.SetWeight(wxFONTWEIGHT_NORMAL);
                    m_grid->SetCellFont(cnt, Did_Name, cell_font);*/
                    break;
                }
            }
        }
        else
        {/*
            m_grid->SetCellValue(wxGridCellCoords(cnt, DidGridCol::Did_Value), entry->value_str);
            wxFont cell_font = m_grid->GetCellFont(cnt, Did_Name);
            cell_font.SetWeight(wxFONTWEIGHT_BOLD);
            m_grid->SetCellFont(cnt, Did_Name, cell_font);
            */
            for(uint8_t i = 0; i != DidGridCol::Did_Max; i++)
                table_widget->item(num_row, i)->setBackground(QBrush(QColor::fromRgba((num_row & 1) ? 0xAAE6E6E6 : 0xAAFFFFFF)));

            QFont font;
            font.setBold(true);
            table_widget->item(num_row, DidGridCol::Did_Value)->setFont(font);
        }
    }

    table_widget->item(num_row, DidGridCol::Did_Timestamp)->setText(last_update_str);
    if(table_widget->item(num_row, DidGridCol::Did_Timestamp))
        table_widget->item(num_row, DidGridCol::Did_Timestamp)->setFlags(table_widget->item(num_row, DidGridCol::Did_Timestamp)->flags() & ~Qt::ItemIsEditable);

    grid_to_entry[cnt] = entry.get();
    did_to_row[entry->id] = num_row;
    table_widget->blockSignals(false);
}

DidTab::DidTab(QWidget* parent) : QWidget(parent)
{
    did_grid = new DidGrid(this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(did_grid->table_widget);


    QHBoxLayout* h_layout = new QHBoxLayout(this);

    m_Refresh = new QPushButton(this);
    m_Refresh->setText("Refresh");
    h_layout->addWidget(m_Refresh);

    m_RefreshSelected = new QPushButton(this);
    m_RefreshSelected->setText("Refresh selected");
    h_layout->addWidget(m_RefreshSelected);

    m_Abort = new QPushButton(this);
    m_Abort->setText("Abort");
    connect(m_Abort, &QPushButton::clicked, [this]()
        {
            std::unique_ptr<DidHandler>& did_handler = g_app->did_handler;
            did_handler->AbortDidUpdate();
        });

    h_layout->addWidget(m_Abort);

    m_ClearDids = new QPushButton(this);
    m_ClearDids->setText("Clear");
    connect(m_ClearDids, &QPushButton::clicked, [this]()
        {
            int num_rows = did_grid->table_widget->rowCount();
            for(int i = 0; i != num_rows; i++)
            {
                did_grid->table_widget->item(i, DidGridCol::Did_Value)->setText("");

                QFont font;
                font.setBold(false);
                did_grid->table_widget->item(i, DidGridCol::Did_Value)->setFont(font);

                for(uint8_t x = 0; x != DidGridCol::Did_Max; x++)
                    did_grid->table_widget->item(i, DidGridCol::Did_Value)->setBackground(QBrush(QColor::fromRgba((i & 1) ? 0xAAE6E6E6 : 0xAAFFFFFF)));
            }
        });

    h_layout->addWidget(m_ClearDids);

    QSpacerItem* spacer_1 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    h_layout->addSpacerItem(spacer_1);

    layout->addLayout(h_layout);

    QHBoxLayout* h_layout_2 = new QHBoxLayout(this);
    m_SaveCache = new QPushButton(this);
    m_SaveCache->setText("Save cache");
    connect(m_SaveCache, &QPushButton::clicked, [this]()
        {
            std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
            std::unique_ptr<DidHandler>& did_handler = g_app->did_handler;

            bool ret = did_handler->SaveChache();
            if(ret)
            {
                std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
                int64_t dif = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

                char work_dir[1024] = {};
#ifdef _WIN32
                GetCurrentDirectoryA(sizeof(work_dir) - 1, work_dir);
#endif
            }
        });

    h_layout_2->addWidget(m_SaveCache);

    QSpacerItem* spacer_2 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    h_layout_2->addSpacerItem(spacer_2);

    layout->addLayout(h_layout_2);

    setLayout(layout);
}

void DidTab::UpdateDidList()
{
    std::unique_ptr<DidHandler>& did_handler = g_app->did_handler;
    did_grid->table_widget->setRowCount(0);
    did_grid->cnt = 0;

    did_handler->m_UpdatedDids.clear();
    for(auto& i : did_handler->m_DidList)
    {
        bool add_row = false;
        if(search_pattern.empty())
            add_row = true;
        else
        {
            if(boost::icontains(i.second->name, search_pattern))
                add_row = true;
        }

        if(add_row)
        {
            did_grid->AddRow(i.second);
            did_handler->m_UpdatedDids.push_back(i.first);
        }
    }

}

void DidTab::WriteDid(uint16_t did, uint8_t* data_to_write, uint16_t size)
{
    std::unique_ptr<DidHandler>& did_handler = g_app->did_handler;
    did_handler->WriteDid(did, data_to_write, size);
    did_handler->NotifyDidUpdate();
}

void DidTab::On100msTimer()
{
    std::unique_ptr<DidHandler>& did_handler = g_app->did_handler;
    std::unique_ptr<CanEntryHandler>& can_handler = g_app->can_entry;

    if(!is_dids_initialized)
    {
        for(auto& i : did_handler->m_DidList)
        {
            did_grid->AddRow(i.second);
        }
        is_dids_initialized = true;
    }

    if(!did_handler->m_UpdatedDids.empty())
    {
        //std::unique_lock lock{ did_handler->m };
        for(auto& did : did_handler->m_UpdatedDids)
        {
            auto& did_it = did_handler->m_DidList[did];
            uint16_t did_row = did_grid->did_to_row[did];

            if(did_it->nrc != 0)
            {
                switch(did_it->nrc)
                {
                    case 0x78:
                    {
                        did_grid->table_widget->item(did_row, DidGridCol::Did_Value)->setText("Pending... NRC 78");
                        did_grid->table_widget->item(did_row, DidGridCol::Did_Value)->setBackground(QColorConstants::Blue);

                        QFont font;
                        font.setBold(false);
                        did_grid->table_widget->item(did_row, DidGridCol::Did_Value)->setFont(font);
                        break;
                    }
                    default:
                    {
                        did_grid->table_widget->item(did_row, DidGridCol::Did_Value)->setText(std::format("NRC: {:X}", did_it->nrc).c_str());
                        did_grid->table_widget->item(did_row, DidGridCol::Did_Value)->setBackground(QColorConstants::Red);

                        QFont font;
                        font.setBold(false);
                        did_grid->table_widget->item(did_row, DidGridCol::Did_Value)->setFont(font);
                        break;
                    }
                }
            }
            else
            {
                did_grid->table_widget->item(did_row, DidGridCol::Did_Value)->setText(QString::fromStdString(did_it->value_str));
                did_grid->table_widget->item(did_row, DidGridCol::Did_Value)->setBackground(QBrush(QColor::fromRgba((did_row & 1) ? 0xAAE6E6E6 : 0xAAFFFFFF)));

                QFont font;
                font.setBold(true);
                did_grid->table_widget->item(did_row, DidGridCol::Did_Value)->setFont(font);
            }

            if(!did_it->last_update.is_not_a_date_time())
            {
                std::string last_update_str = boost::posix_time::to_iso_extended_string(did_it->last_update);
                did_grid->table_widget->item(did_row, DidGridCol::Did_Timestamp)->setText(QString::fromStdString(last_update_str));
            }
        }
        did_handler->m_UpdatedDids.clear();
    }
}