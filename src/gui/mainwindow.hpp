
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QStringListModel>
#include <QGroupBox>
#include <memory>
#include "../src/Sensors.hpp"

#include "../src/gui/CanTab/CanTab.hpp"
#include "StructParserTab.hpp"
#include "MapConverterTab.hpp"
#include "StringEscaperTab.hpp"
#include "CmdExecutorTab.hpp"
#include "LoggerTab.hpp"
#include "DidTab.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    std::atomic<bool> is_meas_pending = {};
    Measurement m_Meas;

    void StoreNewMeasurements(std::unique_ptr<Measurement>& m);
    void dragEnterEvent(QDragEnterEvent* event);

    QSystemTrayIcon* m_tray_icon = nullptr;

public:
    Ui::MainWindow *ui = nullptr;
    CanTab* can_tab = nullptr;
    DidTab* did_tab = nullptr;
    StructParserTab* struct_parser_tab = nullptr;
    MapConverterTab* map_converter_tab = nullptr;
    StringEscaperTab* string_escaper_tab = nullptr;
    CmdExecutorPanelBase* cmd_exeturo_tab = nullptr;
    LoggerTab* logger_tab = nullptr;
    QTimer* timer;

    void UpdateSensors();
    void UpdateCoins();
    void UpdateLogs();
    void UpdateKeybindings();

public slots:
    void Timer_100ms();
    void on_exit();
    void on_do_something();

private:
    QString FormatCurrentWeekNumber();

    std::map<std::string, QGroupBox*> key_map;
    uint32_t num_recv_meas = 0;
};

#endif // MAINWINDOW_H
