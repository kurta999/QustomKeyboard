#pragma once

#include <boost/asio.hpp>

#ifdef _WIN32
#include <urlmon.h>
#endif


#ifndef _WIN32
#include <fmt/format.h>
namespace std
{
	template <typename... T>
	std::string format(fmt::format_string<T...> fmt_, T&&... args)
	{
		return fmt::format(fmt_, std::forward<T>(args)...);
	}

	/*
	std::string vformat(std::string_view format_arg, fmt::format_args args)
	{
		//return fmt::vformat(format_arg, args);
	}
	*/
}
#endif

#include "gui/CanTab/BitEditorDialog.hpp"
#include "gui/CanTab/CanLogTab.hpp"
#include "gui/CanTab/CanScriptTab.hpp"
#include "gui/CanTab/CanTab.hpp"
#include "gui/CanTab/BitEditorDialog.hpp"
#include "gui/CanTab/CanUdsRawDialog.hpp"
#include "gui/BalloonTip.hpp"
#include "gui/CmdExecutorTab.hpp"
#include "gui/DidTab.hpp"
#include "gui/LoggerTab.hpp"
#include "gui/mainwindow.hpp"
#include "gui/MapConverterTab.hpp"
#include "gui/StringEscaperTab.hpp"
#include "gui/StructParserTab.hpp"

#include "ApplicationBase.hpp"
#include "Logger.hpp"
#include "CustomMacro.hpp"
//#include "GuiEditor.hpp"
#include "StructParser.hpp"
#include "DatabaseImpl.hpp"
#include "DatabaseLogic.hpp"
#include "Settings.hpp"
#include "Server.hpp"
#include "Sensors.hpp"
#include "DirectoryBackup.hpp"
#include "PrintScreenSaver.hpp"
#include "PathSeparator.hpp"
#include "Session.hpp"
#include "SerialForwarder.hpp"
#include "SymlinkCreator.hpp"
#include "MacroRecorder.hpp"
#include "AntiLock.hpp"
#include "IdlePowerSaver.hpp"
#include "TerminalHotkey.hpp"
#include "SerialPort.hpp"
#include "CanSerialPort.hpp"
#include "CanDeviceStm32.hpp"
#include "CanDeviceLawicel.hpp"
//#include "ModbusMasterSerialPort.hpp"
#include "MapConverter.hpp"
#include "CryptoPrice.hpp"
#include "CanEntryHandler.hpp"
#include "DidHandler.hpp"
#include "CanScriptHandler.hpp"
#include "CorsairHid.hpp"
#include "StringToCEscaper.hpp"
#include "TcpMessageExecutor.hpp"
#include "CmdExecutor.hpp"
#include "ExceptionHandler.hpp"
#include "Utils.hpp"

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QDialog>
#include <QRadioButton>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QTabWidget>
#include <QLayout>
#include <QMessageBox>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QStringListModel>
#include <QHeaderView>
#include <QClipboard>
#include <QMenu>
#include <QInputDialog>
#include <QGuiApplication>

#include <boost/algorithm/string.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string_regex.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/crc.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/endian.hpp>
#include <boost/circular_buffer.hpp>

#include <assert.h>

#ifdef _WIN32
#include <shellapi.h>
#include <shlobj.h>
#include <exdisp.h>
#include <shlwapi.h>
#include <powrprof.h>
#include "Wtsapi32.h"
#endif

#include <any>
#include <iostream>
#include <fstream>
#include <array>
#include <variant>
#include <string>
#include <bitset>
#include <memory>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <future> 
#include <tuple>
#include <set>
#include <thread>
#include <codecvt>
#include <iterator>
#include <queue>
#include <deque>
#include <cstdint>
#include <stack>
#include <charconv>

#ifdef _WIN32
#include <enumser/enumser.h>
#endif
#include <lodepng/lodepng.h>

#include "utils/AsyncSerial.hpp"

extern "C"
{
	#include "sha256/sha256.h"
	#include <bitfield/bitfield.h>
	#include <isotp/isotp.h>
}

#ifdef _WIN32
#include <Windows.h>
#endif

using namespace std::chrono_literals;