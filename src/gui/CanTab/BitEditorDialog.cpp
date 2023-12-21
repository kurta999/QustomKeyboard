#include "pch.hpp"

BitEditorDialog::BitEditorDialog(QWidget* parent) : QDialog(parent)
{
    main_form = new QFormLayout(this);
    main_form->setRowWrapPolicy(QFormLayout::WrapAllRows);

    QHBoxLayout* h_layout = new QHBoxLayout(this);

    m_IsDecimal = new QRadioButton(this);
    m_IsDecimal->setText("Is decimal?");
    h_layout->addWidget(m_IsDecimal);

    m_IsHex = new QRadioButton(this);
    m_IsHex->setText("Is hex?");
    h_layout->addWidget(m_IsHex);

    m_IsBinary = new QRadioButton(this);
    m_IsBinary->setText("Is binary?");
    h_layout->addWidget(m_IsBinary);

    main_form->addRow(h_layout);

    for(int i = 0; i != MAX_BITEDITOR_FIELDS; i++)
    {
        m_InputLabel[i] = new QLabel(this);
        m_Input[i] = new QLineEdit(this);
        main_form->addRow(m_InputLabel[i], m_Input[i]);
    }


    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    main_form->addWidget(buttonBox);

    bool conn = connect(buttonBox, &QDialogButtonBox::accepted, this, &BitEditorDialog::accept);
    Q_ASSERT(conn);
    conn = connect(buttonBox, &QDialogButtonBox::rejected, this, &BitEditorDialog::reject);
    Q_ASSERT(conn);
    conn = connect(buttonBox, &QDialogButtonBox::clicked, this, &BitEditorDialog::clicked);
    Q_ASSERT(conn);

    setLayout(main_form);
#if 0
    connect(color_btn, &QPushButton::clicked, [this]()
        {
            if(color_dlg->exec() == QDialog::Accepted)
            {
                m_color = color_dlg->currentColor();
            }
        });

    connect(bck_color_btn, &QPushButton::clicked, [this]()
        {
            if(bck_color_dlg->exec() == QDialog::Accepted)
            {
                m_background_color = bck_color_dlg->currentColor();
            }
        });
#endif

}

void BitEditorDialog::clicked(QAbstractButton* button)
{
    if(buttonBox->buttonRole(button) == QDialogButtonBox::ButtonRole::ApplyRole)
    {
        DBG("apply");
        m_ClickType = ClickType::Apply;
        close();
    }
}

int BitEditorDialog::ShowDialog(uint32_t frame_id, bool is_rx, CanBitfieldInfo& values)
{
    if(values.size() > MAX_BITEDITOR_FIELDS)
    {
        values.resize(MAX_BITEDITOR_FIELDS);
        LOG(LogLevel::Warning, "Too much bitfields used for can frame mapping. FrameID: {:X}, Used: {}, Maximum supported: {}", frame_id, values.size(), MAX_BITEDITOR_FIELDS);
    }

    m_Id = 0;
    m_DataFormat = 0;
    m_BitfieldInfo = values;

    for(const auto& [label, value, frame] : values)
    {
        m_InputLabel[m_Id]->setText(QString::fromStdString(label));

        //m_InputLabel[m_Id]->SetForegroundColour(RGB_TO_WXCOLOR(frame->m_color));  /* input for red: 0x00FF0000, excepted input for wxColor 0x0000FF */
        //m_InputLabel[m_Id]->SetBackgroundColour(RGB_TO_WXCOLOR(frame->m_bg_color));
#if 0
        wxFont font;
        font.SetWeight(frame->m_is_bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
        font.Scale(1.0f);  /* Scale has to be set to default first */
        m_InputLabel[m_Id]->SetFont(font);
        font.Scale(frame->m_scale);
        font.SetFaceName("Segoe UI");
        m_InputLabel[m_Id]->SetFont(font);
#endif

        m_InputLabel[m_Id]->setToolTip(QString::fromStdString(frame->m_Description));
        m_InputLabel[m_Id]->show();
        m_Input[m_Id]->setText(QString::fromStdString(value));
        m_Input[m_Id]->show();
        m_Id++;
    }

    for(int i = m_Id; i != MAX_BITEDITOR_FIELDS; i++)
    {
        m_InputLabel[i]->hide();
        m_InputLabel[i]->setToolTip("");
        m_Input[i]->hide();
    }
    main_form->update();

    setWindowTitle(std::format("Bit editor - {:X} ({})", frame_id, is_rx ? "RX" : "TX").c_str());
    bit_sel = BitSelection::Decimal;
    m_IsDecimal->setChecked(true);
    m_IsHex->setChecked(false);
    m_IsBinary->setChecked(false);
    m_FrameId = frame_id;


    m_ClickType = ClickType::None;
    int ret = exec();
    DBG("ShowModal ret: %d\n", ret);
    return ret;
}

std::vector<std::string> BitEditorDialog::GetOutput()
{
    std::vector<std::string> ret;
    for(int i = 0; i != m_Id; i++)
    {
        std::string input_ret = m_Input[i]->text().toStdString();
        switch(bit_sel)
        {
            case BitSelection::Hex:
            {
                try
                {
                    input_ret = std::to_string(std::stoll(input_ret, nullptr, 16));
                }
                catch(...)
                {
                    LOG(LogLevel::Error, "Exception with std::stoll, str: {}", input_ret);
                }
                break;
            }
            case BitSelection::Binary:
            {
                try
                {
                    input_ret = std::to_string(std::stoll(input_ret, nullptr, 2));
                }
                catch(...)
                {
                    LOG(LogLevel::Error, "Exception with std::to_string, str: {}", input_ret);
                }
                break;
            }
        }

        DBG("input_ret: %s\n", input_ret.c_str());
        ret.push_back(input_ret);
    }
    return ret;

}
