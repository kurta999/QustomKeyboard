                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     ret = false;
    }
    return ret;
}

bool XmlDidCacheLoader::Save(const std::filesystem::path& path, DidMap& m)
{
    bool ret = true;
    boost::property_tree::ptree pt;
    auto& root_node = pt.add_child("DidCacheXml", boost::property_tree::ptree{});
    for(auto& i : m)
    {
        auto& frame_node = root_node.add_child("DidCache", boost::property_tree::ptree{});
        frame_node.add("DID", std::format("{:X}", i.second->id));
        frame_node.add("Value", i.second->value_str);
        frame_node.add("NRC", std::format("{:X}", i.second->nrc));

        std::string last_update_str;
        if(!i.second->last_update.is_not_a_date_time())
            last_update_str = boost::posix_time::to_iso_extended_string(i.second->last_update);
        frame_node.add("Timestamp", last_update_str);
    }

    try
    {
        boost::property_tree::write_xml(path.generic_string(), pt, std::locale(),
            boost::property_tree::xml_writer_make_settings<boost::property_tree::ptree::key_type>('\t', 1));
    }
    catch(const std::exception& e)
    {
        LOG(LogLevel::Error, "Exception thrown: {}", e.what());
        ret = false;
    }
    return ret;
}

DidEntryType XmlDidLoader::GetTypeFromString(const std::string_view& input)
{
    auto ret = std::find_if(m_DidEntryTypeMap.cbegin(), m_DidEntryTypeMap.cend(), [&input](const auto& item) { return item.second == input; });
    if(ret == m_DidEntryTypeMap.cend())
        return DidEntryType::DET_INVALID;
    return ret->first;
}

const std::string_view XmlDidLoader::GetStringFromType(DidEntryType type)
{
    auto it = m_DidEntryTypeMap.find(type);
    if(it != m_DidEntryTypeMap.end())
        return it->second;
    return m_DidEntryTypeMap[DidEntryType::DET_INVALID];
}

bool DidHandler::SendUdsFrameAndWaitForResponse(std::vector<uint8_t> frame)
{
    if(!m_can_handler)
        return false;

    m_IsoTpBufLen = 0;
    m_can_handler->SendIsoTpFrame(m_can_handler->GetDefaultEcuId(), frame.data(), frame.size());
    LOG(LogLevel::Verbose, "DidHandler::SendUdsFrameAndWaitForResponse");

    bool ret = WaitForResponse();
    return ret;
}

bool DidHandler::WaitForResponse()
{
    std::unique_lock lk(m_CanMessageMutex);
    auto now = std::chrono::system_clock::now();
    bool ret = m_CanMessageCv.wait_until(lk, now + UDS_TIMEOUT_FOR_RESPONSE, [this]() {return m_IsoTpBufLen != 0; });
    if(ret)
    {
        if(m_IsoTpBufLen == 0xFFFF)
            return false;

        std::string hex;
        utils::ConvertHexBufferToString((const char*)m_IsoTpBuffer, m_IsoTpBufLen.load(), hex);
        LOG(LogLevel::Verbose, "DidHandler::Finished waiting. Response len: {} | \"{}\"", m_IsoTpBufLen.load(), hex);
    }
    else
    {
        LOG(LogLevel::Verbose, "DidHandler::Timeout");
    }
    return ret;
}

void DidHandler::ProcessReadDidResponse(std::unique_ptr<DidEntry>& entry)
{
    std::unique_lock lock(m);
    entry->nrc = 0x0;
    entry->last_update = boost::posix_time::second_clock::local_time();

    switch(entry->type)
    {
        case DET_UI8:
        {
            uint8_t val = m_IsoTpBuffer[3];
            entry->value_str = std::format("{:X}", val);
            break;
        }
        case DET_UI16:
        {
            uint16_t val = m_IsoTpBuffer[3] || m_IsoTpBuffer[4] << 8;
            entry->value_str = std::format("{:X}", val);
            break;
        }
        case DET_UI32:
        {
            uint32_t val = m_IsoTpBuffer[3] || m_IsoTpBuffer[4] << 8 || m_IsoTpBuffer[5] << 16 || m_IsoTpBuffer[6] << 24;
            entry->value_str = std::format("{:X}", val);
            break;
        }
        case DET_UI64:
        {
            /*
            uint64_t val = m_IsoTpBuffer[3] || m_IsoTpBuffer[4] << 8 || m_IsoTpBuffer[5] << 16 || m_IsoTpBuffer[6] << 24 || m_IsoTpBuffer[7] << 32 || m_IsoTpBuffer[8] << 40 || m_IsoTpBuffer[9] << 48 || m_IsoTpBuffer[10] << 56;
            entry->value_str = std::format("{:X}", val);
            */
            break;
        }
        case DET_STRING:
        {
            std::string hex;
            std::string response = std::string(m_IsoTpBuffer + 3, m_IsoTpBuffer + (m_IsoTpBufLen - 3));

            entry->value_str = response;
            break;
        }
        case DET_BYTEARRAY:
        {
            std::string hex;
            std::string response = std::string(m_IsoTpBuffer + 3, m_IsoTpBuffer + (m_IsoTpBufLen - 3));
            utils::ConvertHexBufferToString(response.data(), response.length(), hex);

            entry->value_str = hex;
            break;
        }
        default:
        {
            break;
        }
    }

    m_PendingDidReads.pop_front();
    m_UpdatedDids.push_back(entry->id);

    LOG(LogLevel::Verbose, "Received response for DID: {:X} | \"{}\"", entry->id, entry->value_str);
}

void DidHandler::ProcessRejectedNrc(std::unique_ptr<DidEntry>& entry)
{
    LOG(LogLevel::Warning, "Rejected");
    std::unique_lock lock(m);
    entry->nrc = 0x10;
    entry->last_update = boost::posix_time::second_clock::local_time();
    m_PendingDidReads.pop_front();
}

void DidHandler::HandleDidReading(std::stop_token& token)
{
    uint8_t extended_session_retry_count = 0;
    while(m_PendingDidReads.size() > 0 && !token.stop_requested() && !m_IsAborted)
    {
        {
            m.lock();
            uint16_t curr_did = m_PendingDidReads.front();
            m.unlock();

            auto& did_it = m_DidList[curr_did];

            //m_PendingDids.pop_front();

            bool is_ok = SendUdsFrameAndWaitForResponse({ 0x10, 0x03 });
            if(is_ok)
            {
                extended_session_retry_count = 0;
                m_IsoTpBufLen = 0;
                uint8_t data[3] = { 0x22 };
                data[1] = did_it->id >> 8 & 0xFF;
                data[2] = did_it->id & 0xFF;
                is_ok = SendUdsFrameAndWaitForResponse(std::vector<uint8_t>(data, data + 3));
                if(is_ok)
                {
                    {
                        std::unique_lock lock{ m };
                        m_CanMessageCv.wait_for(lock, token, 50ms, []() { return 0 == 1; });
                    }

                    if(m_IsoTpBuffer[0] == 0x62)
                    {
                        ProcessReadDidResponse(did_it);
                    }
                    else if(m_IsoTpBuffer[0] == 0x7F)
                    {
                        LOG(LogLevel::Warning, "7F received for DID ({:X} {:X}): {:X}", m_IsoTpBuffer[1], m_IsoTpBuffer[2], did_it->id);
                        if(m_IsoTpBuffer[1] == 0x22 && m_IsoTpBuffer[2] == 0x78)  /* Response pending */
                        {
                            while(!token.stop_requested() && !m_IsAborted)
                            {
                                m_IsoTpBufLen = 0;
                                bool is_recv_ok = WaitForResponse();

                                if(!is_recv_ok)
                                {
                                    LOG(LogLevel::Warning, "Pending response timeout");
                                    break;
                                }

                                if(m_IsoTpBuffer[0] == 0x62)
                                {
                                    ProcessReadDidResponse(did_it);
                                    break;
                                }
                                else if(m_IsoTpBuffer[1] == 0x22 && m_IsoTpBuffer[2] == 0x78)
                                {
                                    LOG(LogLevel::Warning, "Pending response NRC 78");
                                    did_it->nrc = 0x78;
                                }
                                else if(m_IsoTpBuffer[2] == 0x10)
                                {
                                    ProcessRejectedNrc(did_it);
                                    break;
                                }
                                else if(m_IsoTpBuffer[2] == 0x22 && (m_IsoTpBuffer[2] >= 0x11 && m_IsoTpBuffer[2] <= 0x93))
                                {
                                    LOG(LogLevel::Warning, "Pending response general NRC: {}", m_IsoTpBuffer[2]);
                                    did_it->nrc = m_IsoTpBuffer[2];
                                    did_it->last_update = boost::posix_time::second_clock::local_time();
                                    break;
                                }
                                else
                                {
                                    LOG(LogLevel::Warning, "Pending response else case: {}", m_IsoTpBuffer[1]);
                                }
                                m_UpdatedDids.push_back(curr_did);
                            }

                            LOG(LogLevel::Warning, "Pending response base");
                        }
                        else
                        {
                            if(m_IsoTpBuffer[2] == 0x10)  /* Rejected */
                            {
                                ProcessRejectedNrc(did_it);
                            }
                            else if(m_IsoTpBuffer[2] == 0x22 && (m_IsoTpBuffer[2] >= 0x11 && m_IsoTpBuffer[2] <= 0x93))
                            {
                                LOG(LogLevel::Warning, "Pending response general NRC: {}", m_IsoTpBuffer[2]);
                                did_it->nrc = m_IsoTpBuffer[2];
                                did_it->last_update = boost::posix_time::second_clock::local_time();
                                break;
                            }
                            else
                            {
                                LOG(LogLevel::Warning, "Unimplemented NRC received, rejecting this DID");
                                ProcessRejectedNrc(did_it);
                                did_it->nrc = m_IsoTpBuffer[2];
                            }
                        }
                        m_UpdatedDids.push_back(curr_did);
                    }
                    else if(m_IsoTpBuffer[0] == 0x50)
                    {
                        LOG(LogLevel::Verbose, "Ignoring extended session response");
                    }
                }
            }
            else
            {
                if(++extended_session_retry_count > MAX_EXTENDED_SESSION_RETRIES)
                {
                    LOG(LogLevel::Error, "No response for extended session after {} retries, abort questioning.", MAX_EXTENDED_SESSION_RETRIES);
                    extended_session_retry_count = 0;
                    m_PendingDidReads.clear();
                    break;
                }
            }
        }

        {
            std::unique_lock lock{ m };
            m_CanMessageCv.wait_for(lock, token, MAIN_THREAD_SLEEP, []() { return 0 == 1; });
        }
    }
}

void DidHandler::HandleDidWriting(std::stop_token& token)
{
    if(m_PendingDidWrites.size() > 0 && !token.stop_requested() && !m_IsAborted)
    {
        for(auto& [did, raw_value] : m_PendingDidWrites)
        {
            bool is_ok = SendUdsFrameAndWaitForResponse({ 0x10, 0x03 });
            if(is_ok)
            {
                m_IsoTpBufLen = 0;
                std::vector<uint8_t> data_to_write = {0x2E};
                data_to_write.push_back(did >> 8 & 0xFF);
                data_to_write.push_back(did & 0xFF);
                std::copy(raw_value.begin(), raw_value.end(), std::back_inserter(data_to_write));

                is_ok = SendUdsFrameAndWaitForResponse(data_to_write);
                if(is_ok)
                {
                    if(m_IsoTpBuffer[0] == 0x6E)
                    {
                        LOG(LogLevel::Warning, "Did write OK");
                    }
                    else if(m_IsoTpBuffer[0] == 0x7F)
                    {
                        LOG(LogLevel::Warning, "7F received for write DID ({:X} {:X}): {:X}", m_IsoTpBuffer[1], m_IsoTpBuffer[2], did);
                        if(m_IsoTpBuffer[1] == 0x2E && m_IsoTpBuffer[2] == 0x78)  /* Response pending */
                        {
                            while(!token.stop_requested() && !m_IsAborted)
                            {
                                m_IsoTpBufLen = 0;
                                bool is_recv_ok = WaitForResponse();

                                if(!is_recv_ok)
                                {
                                    LOG(LogLevel::Warning, "Pending response timeout");
                                    break;
                                }

                                if(m_IsoTpBuffer[0] == 0x6E)
                                {
                                    LOG(LogLevel::Warning, "Did write OK");
                                    break;
                                }
                                else if(m_IsoTpBuffer[1] == 0x2E && m_IsoTpBuffer[2] == 0x78)
                                {
                                    LOG(LogLevel::Warning, "Pending response NRC 78");
                                }
                                else if(m_IsoTpBuffer[2] == 0x10)
                                {
                                    LOG(LogLevel::Warning, "Did write rejected");
                                    break;
                                }
                                else
                                {
                                    LOG(LogLevel::Warning, "Pending write response else case: {}", m_IsoTpBuffer[1]);
                                }
                            }

                            LOG(LogLevel::Warning, "Pending response base");
                        }
                        else
                        {
                            if(m_IsoTpBuffer[2] == 0x10)  /* Rejected */
                            {
                                LOG(LogLevel::Warning, "Did write rejected");
                            }
                        }
                    }
                    else if(m_IsoTpBuffer[0] == 0x50)
                    {
                        LOG(LogLevel::Verbose, "Ignoring extended session response");
                    }
                }
            }

            {
                std::unique_lock lock{ m };
                m_CanMessageCv.wait_for(lock, token, MAIN_THREAD_SLEEP, []() { return 0 == 1; });
            }
        }

        {
            std::unique_lock lock{ m };
            m_PendingDidWrites.clear();
        }
    }
    m_IsAborted = false;
}

void DidHandler::WorkerThread(std::stop_token token)
{
    while(!token.stop_requested())
    {
        m_Semaphore.acquire();
        HandleDidReading(token);
        HandleDidWriting(token);
    }
}