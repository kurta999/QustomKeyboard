#include "pch.hpp"

extern ApplicationBase* g_app;

CanUdsRawDialog::CanUdsRawDialog(QWidget* parent) : QDialog(parent)
{
    main_form = new QFormLayout(this);
    main_form->setRowWrapPolicy(QFormLayout::WrapAllRows);

    QGroupBox* send_box = new QGroupBox();
    send_box->setTitle("Send");
    QVBoxLayout* send_box_sizer = new QVBoxLayout(this);

    QHBoxLayout* h_layout_1 = new QHBoxLayout(this);
    QLabel* label_1 = new QLabel(this);
    label_1->setText("Sender FrameID:");
    h_layout_1->addWidget(label_1);
    m_SenderId = new QLineEdit(this);
    h_layout_1->addWidget(m_SenderId);


    QLabel* label_2 = new QLabel(this);
    label_2->setText("Receiver FrameID:");
    h_layout_1->addWidget(label_2);
    m_ReceiverId = new QLineEdit(this);
    h_layout_1->addWidget(m_ReceiverId);

    QLabel* label_3 = new QLabel(this);
    label_3->setText("Frame delay:");
    h_layout_1->addWidget(label_3);
    m_DelayBetweenFrames = new QLineEdit(this);
    h_layout_1->addWidget(m_DelayBetweenFrames);

    QLabel* label_4 = new QLabel(this);
    label_4->setText("Recv delay:");
    h_layout_1->addWidget(label_4);
    m_RecvDelayFrames = new QLineEdit(this);
    h_layout_1->addWidget(m_RecvDelayFrames);

    send_box_sizer->addLayout(h_layout_1);

    QHBoxLayout* h_layout_2 = new QHBoxLayout(this);
    m_ReadDid = new QPushButton(this);
    m_ReadDid->setText("Read DID");
    connect(m_ReadDid, &QPushButton::clicked, [this]()
        {
            m_DataToSend->setText("10 03\r\n22 4000");
        });
    h_layout_2->addWidget(m_ReadDid);

    m_WriteDid = new QPushButton(this);
    m_WriteDid->setText("Write DID");
    connect(m_WriteDid, &QPushButton::clicked, [this]()
        {
            m_DataToSend->setText("10 03\r\n2E 4000 0C\r\nDELAY 10\r\n22 4000");
        });
    h_layout_2->addWidget(m_WriteDid);

    m_EcuReset = new QPushButton(this);
    m_EcuReset->setText("ECU Reset");
    connect(m_EcuReset, &QPushButton::clicked, [this]()
        {
            m_DataToSend->setText("10 02");
        });
    h_layout_2->addWidget(m_EcuReset);

    m_Clear = new QPushButton(this);
    m_Clear->setText("Clear");
    connect(m_Clear, &QPushButton::clicked, [this]()
        {
            m_DataToSend->setText("");
            m_DataRecv->setText("");
        });
    h_layout_2->addWidget(m_Clear);

    QSpacerItem* spacer_1 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    h_layout_2->addSpacerItem(spacer_1);
    send_box_sizer->addLayout(h_layout_2);

    QLabel* data_to_send_text = new QLabel(this);
    data_to_send_text->setText("UDS Data to send:");
    data_to_send_text->setToolTip("10 - Diagnostic Session Control\n11 - ECU Reset\n3E - Tester present\n22 - Read Data By Identifier\n23 - Read Memory By Address\n\
24 - Read Scaling Data By Identifier\n2A - Read Data By Identifier Periodic\n2E - Write Data By Identifier\n3D - Write Memory By Address\n14 - Delete all DTC\n\
19 - Read DTC Information");
    send_box_sizer->addWidget(data_to_send_text);


    QHBoxLayout* h_layout_send_btn = new QHBoxLayout(this);
    m_SendBtn = new QPushButton(this);
    m_SendBtn->setText("Send");
    connect(m_SendBtn, &QPushButton::clicked, [this]()
        {
            HandleFrameSending();
        });
    h_layout_send_btn->addWidget(m_SendBtn);
    QSpacerItem* spacer_2 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    h_layout_send_btn->addSpacerItem(spacer_2);
    

    m_DataToSend = new QTextEdit();
    send_box_sizer->addWidget(m_DataToSend);
    send_box_sizer->addLayout(h_layout_send_btn);

    /* Set groupbox layout and add it to base sizer */
    send_box->setLayout(send_box_sizer);
    main_form->addRow(send_box);

    /* Receive */
    QGroupBox* recv_box = new QGroupBox(this);
    recv_box->setTitle("Receive");
    QVBoxLayout* recv_layout = new QVBoxLayout(this);

    QLabel* data_to_recv_text = new QLabel(this);
    data_to_recv_text->setText("UDS Response:");
    data_to_recv_text->setToolTip("11 - serviceNotSupported\n12 - subFunctionNotSupported\n14 - responseTooLong\n21 - busyRepeatReques\n22 - conditionsNotCorrect\n\
31 - requestOutOfRange\n33 - securityAccessDenied\n78 - ResponsePending\n7E - subFunctionNotSupportedInActiveSession\n7F -serviceNotSupportedInActiveSession");
    recv_layout->addWidget(data_to_recv_text);

    m_DataRecv = new QTextEdit();
    recv_layout->addWidget(m_DataRecv);
    recv_box->setLayout(recv_layout);

    main_form->addRow(recv_box);

    button_box = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal, this);
    main_form->addWidget(button_box);

    bool conn = connect(button_box, &QDialogButtonBox::rejected, this, &CanUdsRawDialog::reject);
    Q_ASSERT(conn);
    setLayout(main_form);
}

CanUdsRawDialog::~CanUdsRawDialog()
{
    if(m_isotp_future.valid())
        if(m_isotp_future.wait_for(std::chrono::nanoseconds(1)) != std::future_status::ready)
            m_isotp_future.get();
}

int CanUdsRawDialog::ShowDialog()
{
    m_IsApplyClicked = false;

    std::unique_ptr<CanEntryHandler>& can_handler = g_app->can_entry;
    if(m_LastUdsSenderId == 0)
    {
        m_LastUdsSenderId = can_handler->GetDefaultEcuId();
        m_LastUdsReceiverId = can_handler->GetIsoTpResponseFrameId();
        m_LastDelayBetweenFrames = 100;
        m_LastRecvWaitingTime = 350;
        m_LastUdsInput.clear();
    }

    m_IsApplyClicked = false;
    m_SenderId->setText(std::format("{:X}", m_LastUdsSenderId).c_str());
    m_ReceiverId->setText(std::format("{:X}", m_LastUdsReceiverId).c_str());
    m_DelayBetweenFrames->setText(std::format("{}", m_LastDelayBetweenFrames).c_str());
    m_RecvDelayFrames->setText(std::format("{}", m_LastRecvWaitingTime).c_str());
    m_DataRecv->setText("");

    int ret = exec();
    DBG("ShowModal ret: %d\n", ret);
    return ret;
}

uint32_t CanUdsRawDialog::GetSenderId()
{
    uint32_t ret = 0;
    try
    {
        ret = std::stoi(m_SenderId->text().toStdString(), 0, 16);
    }
    catch(const std::exception& e)
    {
        LOG(LogLevel::Warning, "stoi exception: {}", e.what());
    }
    return ret;
}

uint32_t CanUdsRawDialog::GetReceiverId()
{
    uint32_t ret = 0;
    try
    {
        ret = std::stoi(m_ReceiverId->text().toStdString(), 0, 16);
    }
    catch(const std::exception& e)
    {
        LOG(LogLevel::Warning, "stoi exception: {}", e.what());
    }
    return ret;
}

uint32_t CanUdsRawDialog::GetDelayBetweenFrames()
{
    uint32_t ret = 0;
    try
    {
        ret = std::stoi(m_DelayBetweenFrames->text().toStdString());
    }
    catch(const std::exception& e)
    {
        LOG(LogLevel::Warning, "stoi exception: {}", e.what());
    }
    return ret;
}

uint32_t CanUdsRawDialog::GetWaitingTimeForFrames()
{
    uint32_t ret = 0;
    try
    {
        ret = std::stoi(m_RecvDelayFrames->text().toStdString());
    }
    catch(const std::exception& e)
    {
        LOG(LogLevel::Warning, "stoi exception: {}", e.what());
    }
    return ret;
}

std::string CanUdsRawDialog::GetSentData()
{
    return m_DataToSend->toPlainText().toStdString();
}

bool SendIsoTpFrameGlobal(uint32_t sender_id, char* arr_to_send, uint16_t len)
{
    std::unique_ptr<CanEntryHandler>& can_handler = g_app->can_entry;
    can_handler->SendIsoTpFrame(sender_id, (uint8_t*)arr_to_send, len);
    return true;
}

void CanUdsRawDialog::HandleFrameSending()
{
    std::unique_ptr<CanEntryHandler>& can_handler = g_app->can_entry;

    m_LastUdsSenderId = GetSenderId();
    m_LastUdsReceiverId = GetReceiverId();
    m_LastUdsInput = GetSentData();
    m_LastDelayBetweenFrames = GetDelayBetweenFrames();
    m_LastRecvWaitingTime = GetWaitingTimeForFrames();

    auto& uds_responses = can_handler->GetUdsRawBuffer();
    uds_responses.clear();  /* Clear every older request */

    uint32_t old_recv_frame_id = can_handler->GetIsoTpResponseFrameId();
    std::vector<std::string> lines;
    boost::split(lines, m_LastUdsInput, [](char input) { return input == '\n' || input == ';'; }, boost::algorithm::token_compress_on);
    for(auto& hex_str : lines)
    {
        int delay = 0;
        int delay_check = sscanf(hex_str.c_str(), "DELAY%*c%d%*c[^\n]", &delay);
        if(delay_check == 1)
        {
            {
                LOG(LogLevel::Notification, "Sending ISO-TP Frame, Delay: {}ms", delay);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            continue;
        }

        char byte_array[MAX_ISOTP_FRAME_LEN];
        boost::algorithm::erase_all(hex_str, " ");
        boost::algorithm::erase_all(hex_str, ".");
        utils::ConvertHexStringToBuffer(hex_str, std::span{ byte_array });

        uint16_t len = (hex_str.length() / 2);
        if(len == 0)
        {
            LOG(LogLevel::Warning, "Skipping IsoTP frame, input length is zero");
            continue;
        }

        m_isotp_future = std::async(&SendIsoTpFrameGlobal, m_LastUdsSenderId, byte_array, len);
        LOG(LogLevel::Notification, "Sending ISO-TP Frame, FrameID: {:X}, ResponseFrameID: {:X}, Len: {}", m_LastUdsSenderId, can_handler->GetIsoTpResponseFrameId(), len);

        std::this_thread::sleep_for(std::chrono::milliseconds(m_LastDelayBetweenFrames));
    }
    LOG(LogLevel::Warning, "Sending complete 0");
    std::this_thread::sleep_for(std::chrono::milliseconds(m_LastRecvWaitingTime));  /* TODO: use semaphore or something else */
    LOG(LogLevel::Warning, "Sending complete 1");

    while(can_handler->GetElapsedTimeSinceLastUdsFrame() < m_LastRecvWaitingTime)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    LOG(LogLevel::Warning, "Sending complete 2");

    std::string response;
    for(auto& i : uds_responses)
    {
        std::string tmp;
        utils::ConvertHexBufferToString(i.c_str(), i.length(), tmp);
        response += tmp + "\r\n";
    }
    uds_responses.clear();
    m_DataRecv->setText(QString::fromStdString(response));

    if(old_recv_frame_id != m_LastUdsReceiverId)
        can_handler->SetIsoTpResponseFrame(old_recv_frame_id);
}