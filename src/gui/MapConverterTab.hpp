
#ifndef MAPCONVERTERTAB_H
#define MAPCONVERTERTAB_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QStringListModel>
#include <QTextEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QPushButton>
#include <memory>
#include "../src/Sensors.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MapConverterTab : public QTextEdit

{
    //Q_OBJECT
public:
    MapConverterTab(QLineEdit* offset, QCheckBox* only_create_vehicle, QCheckBox* inline_veh_components, QCheckBox* convert_numberplate,
        QCheckBox* hide_when_alpha_is_set, QCheckBox* convert_item_name, QTextEdit* input, QTextEdit* output, QPushButton* convert, QPushButton* clear);


private:
    QLineEdit* m_Offset = nullptr;
    QCheckBox* m_OnlyCreateVehicle = nullptr;
    QCheckBox* m_InlineVehComponents = nullptr;
    QCheckBox* m_ConvertNumberplate = nullptr;
    QCheckBox* m_HideWhenAlphaIsSet = nullptr;
    QCheckBox* m_ConvertItemName = nullptr;
    QTextEdit* m_Input = nullptr;
    QTextEdit* m_Output = nullptr;
    QPushButton* m_Convert = nullptr;
    QPushButton* m_Clear = nullptr;
};

#endif // MAPCONVERTERTAB_H
