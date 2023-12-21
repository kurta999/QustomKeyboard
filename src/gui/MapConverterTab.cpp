#include "pch.hpp"

MapConverterTab::MapConverterTab(QLineEdit* offset, QCheckBox* only_create_vehicle, QCheckBox* inline_veh_components, QCheckBox* convert_numberplate,
    QCheckBox* hide_when_alpha_is_set, QCheckBox* convert_item_name, QTextEdit* input, QTextEdit* output, QPushButton* convert, QPushButton* clear) :
    m_Offset(offset), m_OnlyCreateVehicle(only_create_vehicle), m_InlineVehComponents(inline_veh_components), m_ConvertNumberplate(convert_numberplate), 
    m_HideWhenAlphaIsSet(hide_when_alpha_is_set), m_ConvertItemName(convert_item_name), m_Input(input), m_Output(output), m_Convert(convert), m_Clear(clear)
{
    m_Convert->setToolTip("Convert MTA Map format to SA:MP");
    connect(m_Convert, &QPushButton::clicked, [this]()
        {
            QString input = m_Input->toPlainText();
            std::string input_str = input.toStdString();
            Vec3D offset;
            MapConverterFlags flags = MapConverterFlags::NONE;

            sscanf(m_Offset->text().toStdString().c_str(), "%f%*c%f%*c%f%*c", &offset.x, &offset.y, &offset.z);

            if(m_OnlyCreateVehicle->isChecked())
                flags |= MapConverterFlags::ONLY_CREATE_VEHICLE;
            if(m_InlineVehComponents->isChecked())
                flags |= MapConverterFlags::VEHICLE_COMPONENTS_INLINE;
            if(m_ConvertNumberplate->isChecked())
                flags |= MapConverterFlags::SAVE_NUMBER_PLATE;
            if(m_HideWhenAlphaIsSet->isChecked())
                flags |= MapConverterFlags::HIDE_WHEN_ALPHA_NOT_255;
            if(m_ConvertItemName->isChecked())
                flags |= MapConverterFlags::CONVERT_ELEMENT_NAME;

            std::string output;
            try
            {
                output = MapConverter::Get()->MtaToSamp(input_str, flags, offset);
                BalloonTip::showNotificaiton(QMessageBox::Information, "Map Converter", "Maps converter", 1000);
            }
            catch(std::exception& e)
            {
                LOG(LogLevel::Error, "Exception: {}", e.what());
            }

            if(!output.empty())
            {
                m_Output->setPlainText(QString::fromStdString(output));
            }
        });

    /*
    connect(m_StructInput, &QTextEdit::dragMoveEvent, [this]()
        {
            qDebug() << "test";
        });
        */
}