                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                nce to type a text\n";
    out << "# KEY_SEQ[CTRL+C] = Press all given keys after each other and release it when each was pressed - ideal for key shortcats\n";
    out << "# DELAY[time in ms] = Waits for given milliseconds\n";
    out << "# DELAY[min ms - max ms] = Waits randomly between min ms and max ms\n";
    out << "# MOUSE_MOVE[x,y] = Move mouse to given coordinates\n";
    out << "# MOUSE_INTERPOLATE[x,y] = Move mouse with interpolation to given coordinates\n";
    out << "# MOUSE_PRESS[key] = Press given mouse key\n";
    out << "# MOUSE_RELEASE[key] = Release given mouse key\n";
    out << "# MOUSE_CLICK[key] = Click (press and release) with mouse\n";
    out << "# BASH[key] = Execute specified command(s) with command line and keeps terminal shown\n";
    out << "# CMD[key] = Execute specified command(s) with command line without terminal\n";
    out << "\n";
    out << "[Macro_Config]\n";
    out << "# Use per-application macros. AppName is searched in active window title, so window name must contain AppName\n";
    out << "UsePerApplicationMacros = " << CustomMacro::Get()->use_per_app_macro << "\n";
    out << "\n";
    out << "# If enabled, you can bind multiple key combinations with special keys like RSHIFT + 1, but can't bind SHIFT, CTRL and other special keys alone\n";
    out << "UseAdvancedKeyBinding = " << CustomMacro::Get()->advanced_key_binding << "\n";
    out << "\n";
    out << "# If set to valid key, pressing this key will bring this application to foreground or minimize it to the tray\n";
    out << "BringToForegroundKey = " << CustomMacro::Get()->bring_to_foreground_key << "\n";
    out << "\n";

    if(!write_default_macros)  /* True if settings.ini file doesn't exists - write a few macro lines here as example */
    {
        int cnt = 0;
        std::string key;
        auto& m = CustomMacro::Get()->GetMacros();
        for(auto& i : m)
        {
            if(!cnt)
                out << "[Keys_Global]\n";
            else
            {
                out << std::format("\n[Keys_Macro{}]\n", cnt);
                out << std::format("AppName = {}\n", i->app_name);
            }
            cnt++;
            for(auto& x : i->key_vec)
            {
                key = std::format("{} = BIND_NAME[{}]", x.first, i->bind_name[x.first]);

                for(auto& k : x.second)
                {
                    IKey* p = k.get();
                    key += p->GenerateText(true);
                }
                out << key << '\n';
                key.clear();
            }
        }
    }
    else
    {
        out << "[Keys_Global]\n";
        out << "NUM_0 = BIND_NAME[global macro 1] KEY_SEQ[A+B+C]\n";
        out << "NUM_1 = BIND_NAME[global macro 2] KEY_TYPE[global macro 1]\n";
        out << "\n";
        out << "[Keys_Macro1]\n";
        out << "AppName = Notepad\n";
        out << "NUM_1 = BIND_NAME[close notepad++] KEY_TYPE[test string from CustomKeyboard.exe] DELAY[100] KEY_TYPE[Closing window...] DELAY[100-3000] KEY_SEQ[LALT+F4] DELAY[100] KEY_SEQ[RIGHT] KEY_SEQ[ENTER]\n";
    }

    out << "\n";
    out << "[TCP_Backend]\n";
    out << "Enable = " << Server::Get()->is_enabled << "\n";
    out << "TCP_Port = " << Server::Get()->tcp_port << " # TCP Port for receiving measurements from sensors\n";
    out << "\n";
    out << "[COM_Backend]\n";
    out << "Enable = " << SerialPort::Get()->IsEnabled() << "\n";
    out << "COM = " << SerialPort::Get()->GetComPort() << " # Com port for UART where the data is received from STM32\n";
    out << "ForwardViaTcp = " << SerialPort::Get()->IsForwardToTcp() << " # Is data have to be forwarded to remote TCP server\n";
    out << "RemoteTcpIp = " << SerialPort::Get()->GetRemoteTcpIp() << "\n";
    out << "RemoteTcpPort = " << SerialPort::Get()->GetRemoteTcpPort() << "\n";
    out << "\n";
    out << "[COM_TcpBackend]\n";
    out << "Enable = " << SerialForwarder::Get()->is_enabled << " # Listening port from second instance where the TCP Forwarder forwards data received from COM port\n";
    out << "ListeningIp = " << SerialForwarder::Get()->bind_ip << "\n";
    out << "ListeningPort = " << SerialForwarder::Get()->tcp_port << "\n";
    out << "\n";
    out << "[CANSender]\n";
    out << "Enable = " << CanSerialPort::Get()->IsEnabled() << "\n";
    out << "COM = " << CanSerialPort::Get()->GetComPort() << " # Com port for CAN UART where data is received/sent from/to STM32\n";
    out << "DeviceType = " << static_cast<int>(CanSerialPort::Get()->GetDeviceType()) << " # 0 = STM32, 1 = LAWICEL\n";
    out << "AutoSend = " << can_handler->IsAutoSend() << "\n";
    out << "AutoRecord = " << can_handler->IsAutoRecord() << "\n";
    out << "DefaultRecordingLogLevel = " << static_cast<int>(can_handler->GetRecordingLogLevel()) << "\n";
    out << "DefaultFavouriteLevel = " << static_cast<int>(can_handler->GetFavouriteLevel()) << "\n";
    out << "DefaultEcuId = " << std::format("{:X}", can_handler->GetDefaultEcuId()) << "\n";
    out << "DefaultTxList = " << can_handler->default_tx_list.generic_string() << "\n";
    out << "DefaultRxList = " << can_handler->default_rx_list.generic_string() << "\n";
    out << "DefaultMapping = " << can_handler->default_mapping.generic_string() << "\n";
    out << "\n";
    out << "[ModbusMaster]\n";
    //out << "Enable = " << ModbusMasterSerialPort::Get()->IsEnabled() << "\n";
   // out << "COM = " << ModbusMasterSerialPort::Get()->GetComPort() << " # Com port for Modbus Master UART where data is received/sent from/to Modbus\n";
    out << "\n";
    out << "[App]\n";
    out << "MinimizeOnExit = " << minimize_on_exit << "\n";
    out << "MinimizeOnStartup = " << minimize_on_startup<< "\n";
    out << "DefaultPage = " << static_cast<uint16_t>(default_page) << "\n";
    out << "UsedPages = " << ParseUsedPagesToString(used_pages) << "\n";
    out << "RememberWindowSize = " << remember_window_size << "\n";
    if(remember_window_size)  /* get frame size when click on Save - not on exit, this is not a bug */
    {
       /* MyFrame* frame = ((MyFrame*)(wxGetApp().GetTopWindow()));
        window_size = frame->GetSize();*/
    }
    //out << "LastWindowSize = " << std::format("{}, {}", window_size.x, window_size.y) << "\n";
    out << "AlwaysOnNumLock = " << always_on_numlock << "\n";
    out << "SharedDriveLetter = " << shared_drive_letter << "\n";
    out << "CryptoPriceUpdate = " << crypto_price_update << "\n";
    out << "CorsairDebouncingInterval = " << CorsairHid::Get()->GetDebouncingInterval() << "\n";
    out << "\n";
    out << "[Screenshot]\n";
    out << "ScreenshotKey = " << PrintScreenSaver::Get()->screenshot_key << "\n";
    out << "ScreenshotDateFormat = " << PrintScreenSaver::Get()->timestamp_format << "\n";
    out << "ScreenshotPath = " << PrintScreenSaver::Get()->screenshot_path.generic_string() << "\n";
    out << "\n";
    out << "[PathSeparator]\n";
    out << "ReplacePathSeparatorKey = " << PathSeparator::Get()->replace_key << "\n";
    out << "\n";
    out << "[SymlinkCreator]\n";
    out << "Enable = " << SymlinkCreator::Get()->is_enabled << "\n";
    out << "MarkKey = " << SymlinkCreator::Get()->mark_key << "\n";
    out << "PlaceSymlinkKey = " << SymlinkCreator::Get()->place_symlink_key << "\n";
    out << "PlaceHardlinkKey = " << SymlinkCreator::Get()->place_hardlink_key << "\n";
    out << "\n";
    out << "[AntiLock]\n";
    out << "Enable = " << AntiLock::Get()->is_enabled << "\n";
    out << "Timeout = " << AntiLock::Get()->timeout << " # Seconds\n";
    out << "StartScreenSaver = " << AntiLock::Get()->is_screensaver << "\n";
    out << "Exclusions = " << AntiLock::Get()->SaveExclusions() << '\n';
    out << "\n";
    out << "[TerminalHotkey]\n";
    out << "Enable = " << TerminalHotkey::Get()->is_enabled << "\n";
    out << "Key = " << TerminalHotkey::Get()->GetKey() << "\n";
    out << "Type = " << static_cast<uint32_t>(TerminalHotkey::Get()->type) << " # 0 = WINDOWS_TERMINAL, 1 = cmd.exe, 2 = POWER_SHELL, 3 = BASH_TERMINAL" << "\n";
    out << "\n";
    out << "[IdlePowerSaver]\n";
    out << "Enable = " << IdlePowerSaver::Get()->is_enabled << "\n";
    out << "Timeout = " << IdlePowerSaver::Get()->timeout << "\n";
    out << "ReducedPowerPercent = " << static_cast<int>(IdlePowerSaver::Get()->reduced_power_percent) << "\n";
    out << "MinLoadThreshold = " << static_cast<int>(IdlePowerSaver::Get()->min_load_threshold) << "\n";
    out << "MaxLoadThreshold = " << static_cast<int>(IdlePowerSaver::Get()->max_load_threshold) << "\n";
    out << "\n";
    out << "[BackupSettings]\n";
    out << "BackupFileFormat = " << DirectoryBackup::Get()->backup_time_format << "\n";
    if(!write_default_macros)
    {
#if 0
        int cnt = 1;
        std::string key;
        for(auto& i : DirectoryBackup::Get()->backups)
        {
            out << std::format("\n[Backup_{}]\n", cnt++);
            out << "From = " << i->from.generic_string() << '\n';
            key.clear();
            for(auto& x : i->to)
            {
                key += x.generic_string() + '|';
            }
            if(key.back() == '|')
                key.pop_back();
            out << "To = " << key << '\n';
            key.clear();
            for(auto& x : i->ignore_list)
            {
                key += x + '|';
            }
            if(key.back() == '|')
                key.pop_back();

            std::string ignore_list;
            //utils::WStringToMBString(key, ignore_list);
            out << "Ignore = " << key << '\n';
            out << "MaxBackups = " << i->max_backups << '\n';
            out << "CalculateHash = " << i->calculate_hash << '\n';
            out << "BufferSize = " << i->hash_buf_size << " # Buffer size for file operations - determines how much data is read once, Unit: Megabytes" << '\n';
        }
#endif
    }
    else
    {
        out << "\n[Backup_1]\n";
        out << "From = C:\\Users\\Ati\\Desktop\\folder_from_backup\n";
        out << "To = C:\\Users\\Ati\\Desktop\\folder_where_to_backup|F:\\Backup\\folder_where_to_backup\n";
        out << "Ignore = git/COMMIT_EDITMSG|.git|.vs|Debug|Release|Screenshots|x64|Graphs/Line Chart|Graphs/Temperature.html|Graphs/Humidity.html|Graphs/CO2.html|Graphs/Lux.html|Graphs/VOC.html|Graphs/CCT.html|Graphs/PM10.html|Graphs/PM25.html\n";
        out << "MaxBackups = 5\n";
        out << "CalculateHash = 1\n";
        out << "BufferSize = 2\n";
    }
    out << "\n";
    out << "[Graph]\n";
    out << "Graph1HoursBack = " << DatabaseLogic::Get()->GetGraphHours(0) << " # One day\n";
    out << "Graph2HoursBack = " << DatabaseLogic::Get()->GetGraphHours(1) << " # One week\n";
}

void Settings::Init()
{
    LoadFile();
}

UsedPages Settings::ParseUsedPagesFromString(const std::string& in)
{
    UsedPages pages;
    pages.pages = 0;
    if(boost::icontains(in, "Main"))
        pages.main = 1;
    if(boost::icontains(in, "Config"))
        pages.config = 1;
    if(boost::icontains(in, "wxEditor"))
        pages.wxeditor = 1;
    if(boost::icontains(in, "Map"))
        pages.map_converter = 1;
    if(boost::icontains(in, "StringEscaper"))
        pages.escaper = 1;
    if(boost::icontains(in, "Debug"))
        pages.debug = 1;
    if(boost::icontains(in, "StructParser"))
        pages.struct_parser = 1;
    if(boost::icontains(in, "FileBrowser"))
        pages.file_browser = 1;
    if(boost::icontains(in, "CmdExecutor"))
        pages.cmd_executor = 1;
    if(boost::icontains(in, "CanSender"))
        pages.can = 1;
    if(boost::icontains(in, "Did"))
        pages.did = 1;
    if(boost::icontains(in, "ModbusMaster"))
        pages.modbus_master = 1;
    if(boost::icontains(in, "Log"))
        pages.log = 1;
    return pages;
}

std::string Settings::ParseUsedPagesToString(UsedPages& in)
{
    std::string pages;
    if(in.main)
        pages += "Main, ";
    if(in.config)
        pages += "Config, ";
    if(in.wxeditor)
        pages += "wxEditor, ";
    if(in.map_converter)
        pages += "Map, ";
    if(in.escaper)
        pages += "StringEscaper, ";
    if(in.debug)
        pages += "Debug, ";
    if(in.struct_parser)
        pages += "StructParser, ";
    if(in.file_browser)
        pages += "FileBrowser, ";
    if(in.cmd_executor)
        pages += "CmdExecutor, ";
    if(in.can)
        pages += "CanSender, ";
    if(in.did)
        pages += "Did, ";
    if(in.modbus_master)
        pages += "ModbusMaster, ";
    if(in.log)
        pages += "Log, ";
    if(pages.size() > 2)
        pages.pop_back(), pages.pop_back();
    return pages;
}