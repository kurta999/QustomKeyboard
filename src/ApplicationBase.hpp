#pragma once

#include "CanEntryHandler.hpp"
#include "CmdExecutor.hpp"
#include "DidHandler.hpp"

class ApplicationBase
{
public:
    ApplicationBase();
    ~ApplicationBase();

    void Init();

    std::unique_ptr<CmdExecutor> cmd_executor;
    std::unique_ptr<DidHandler> did_handler;
    std::unique_ptr<CanEntryHandler> can_entry;

private:
    XmlCanEntryLoader xml;
    XmlCanRxEntryLoader rx_xml;
    XmlCanMappingLoader mapping_xml;
    XmlDidLoader did_xml_loader;
    XmlDidCacheLoader did_xml_chace_loader;
};