#include "backward.h"
#include <QTime>
backwardW::backwardW(QWidget *parent) :
    QDialog(),
    ui(new Ui::backwardW),
    backw_data(new _DataToModel)
{
    backw_data->packet_id = NPR_PACKET_TYPE_BACK_DATA;
    setWindowModality(Qt::ApplicationModal);
    ui->setupUi(this);
}

backwardW::~backwardW()
{
    delete ui;
    delete backw_data;
}

void backwardW::setCurrentTime(QTime * _time)
{
	backw_data->simulation_time = double(_time->msec());
}

void backwardW::updateBackwardPacket(_MainVisualData _data)
{
	backw_data->packet_id = NPR_PACKET_TYPE_BACK_DATA;
	backw_data->p_coord.H = _data.p_coord.H;
	backw_data->p_coord.X = _data.p_coord.X;
	backw_data->p_coord.Z = _data.p_coord.Z;
	backw_data->simulation_time = _data.model_simulation_time;
	writeDataToFields(backw_data);
	emit sendData(backw_data);
}

void backwardW::on_simulation_timeLE_editingFinished()
{
    backw_data->simulation_time = ui->h_le_2->text().toDouble();
}

void backwardW::on_lat_le_editingFinished()
{
    backw_data->p_coord.X = ui->lat_le->text().toDouble();
}

void backwardW::on_lon_le_editingFinished()
{
    backw_data->p_coord.Z = ui->lon_le->text().toDouble();
}

void backwardW::on_h_le_editingFinished()
{
    backw_data->p_coord.H = ui->h_le->text().toFloat();
}

void backwardW::on_buttonBox_accepted()
{
    emit sendData(this->backw_data);
    this->close();
}

void backwardW::writeDataToFields(_DataToModel *_data)
{
    backw_data->packet_id = NPR_PACKET_TYPE_BACK_DATA;
    backw_data->p_coord.H = _data->p_coord.H ;
    backw_data->p_coord.X  = _data->p_coord.X ;
    backw_data->p_coord.Z  = _data->p_coord.Z;
    backw_data->simulation_time = _data->simulation_time;

	ui->h_le->setText(QString::number(backw_data->p_coord.H));
	ui->lat_le->setText(QString::number(backw_data->p_coord.X));
	ui->lon_le->setText(QString::number(backw_data->p_coord.Z));
	ui->h_le_2->setText(QString::number(backw_data->simulation_time));
}

void backwardW::readDefault(_DataToModel *_data)
{
	_data->packet_id = NPR_PACKET_TYPE_BACK_DATA;
	_data->p_coord.H = ui->h_le->text().toFloat();
	_data->p_coord.X = ui->lat_le->text().toDouble();
	_data->p_coord.Z = ui->lon_le->text().toDouble();
	_data->simulation_time = ui->h_le_2->text().toInt();
}



