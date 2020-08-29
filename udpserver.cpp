#include "udpserver.h"
#include <QtNetwork/qudpsocket.h>
#include <QTimer>
#include <QTime>
#include <QDataStream>
#include <QString>

UdpServer::UdpServer(QObject *parent) :
	QObject(parent)
{
	//del?
	m_receiver_socket = new QUdpSocket(this);
	connect(m_receiver_socket, SIGNAL(readyRead()), this, SLOT(readDatagram()));

	m_sender_socket = new QUdpSocket(this);
	connect(m_sender_socket, SIGNAL(readyRead()), this, SLOT(readDatagram()));

	m_map_sender_socket = new QUdpSocket(this);
	connect(m_map_sender_socket, SIGNAL(readyRead()), this, SLOT(readDatagram()));

	//del?
	m_backward_sender_socket = new QUdpSocket(this);
	connect(m_backward_sender_socket, SIGNAL(readyRead()), this, SLOT(backwardReadDatagram()));

	m_sound_sender_socket = new QUdpSocket(this);
	//connect(m_sound_sender_socket, SIGNAL(readyRead()), this, SLOT(readDatagram()));
	m_plots_socket = new QUdpSocket(this);
	

	QTimer* visTimer = new QTimer(this);
	visTimer->setObjectName("visTimer");
	visTimer->setInterval(10);
	connect(visTimer, &QTimer::timeout, this, &UdpServer::visTimerTimeout);

	QTimer* aerodromsTimer = new QTimer(this);
	aerodromsTimer->setObjectName("aerodromsTimer");
	aerodromsTimer->setInterval(1000);
	connect(aerodromsTimer, &QTimer::timeout, this, &UdpServer::aerodromsTimerTimeout);

	QTimer* meteoTimer = new QTimer(this);
	meteoTimer->setObjectName("meteoTimer");
	meteoTimer->setInterval(1000);
	connect(meteoTimer, &QTimer::timeout, this, &UdpServer::meteoTimerTimeout);

	  QTimer* backwardTimer = new QTimer(this);
	  backwardTimer->setObjectName("backwardTimer");
	  backwardTimer->setInterval(10);
	  connect(backwardTimer, &QTimer::timeout, this, &UdpServer::backwTimerTimeout);		

	QTimer* mapTimer = new QTimer(this);
	mapTimer->setObjectName("mapTimer");
	mapTimer->setInterval(10);
	connect(mapTimer, &QTimer::timeout, this, &UdpServer::mapTimerTimeout);

	m_timerList << visTimer << aerodromsTimer << meteoTimer << backwardTimer <<mapTimer;

	m_olegSoundPacker.Initialize(0.01); 
}

UdpServer::~UdpServer()
{
	delete m_receiver_socket;
	delete m_sender_socket;
	delete m_plots_socket;
}

void UdpServer::changeTimerInterval(const QString& timerObjName, int interval)
{
	QTimer* timer = this->findChild<QTimer*>(timerObjName);

	if (timer)
	{
		timer->setInterval(interval);
	}
	else {
		qDebug() << "Timer not found: " << timerObjName << "\n";
	}
}

void UdpServer::meteoTimerTimeout()
{
	if (m_enabledPackets["METEO_DATA"] == true)
	{
		QByteArray meteoData_ = QByteArray::fromRawData(reinterpret_cast<const char*>(m_meteoData), sizeof(_MeteoData));
		_MeteoData* meteo_ptr = reinterpret_cast<_MeteoData*>(meteoData_.data());

		meteo_ptr->model_simulation_time = m_time.elapsed();
		//sendUDPOnce(meteoData_);
	}
}

void UdpServer::visTimerTimeout()
{
	if (m_enabledPackets["VISUAL_DATA"] == true)
	{
		QByteArray visualData = QByteArray::fromRawData(reinterpret_cast<const char*>(m_visualData), sizeof(_MainVisualData));
		_MainVisualData* visual_ptr = reinterpret_cast<_MainVisualData*>(visualData.data());

		visual_ptr->model_simulation_time = m_time.elapsed();
		//sendUDPOnce(visualData);
	}
}


void UdpServer::backwTimerTimeout()
{
	if (m_enabledPackets["BACKWARD_DATA"] == true)
	{
		QByteArray d2m = QByteArray::fromRawData(reinterpret_cast<const char*>(m_backwardData), sizeof(_DataToModel));
		_DataToModel* backw_ptr = reinterpret_cast<_DataToModel*>(d2m.data());

		QByteArray visualData = QByteArray::fromRawData(reinterpret_cast<const char*>(m_visualData), sizeof(_MainVisualData));
		_MainVisualData* visual_ptr = reinterpret_cast<_MainVisualData*>(visualData.data());

		backw_ptr->p_coord.H = visual_ptr->p_coord.H;
		backw_ptr->p_coord.X = visual_ptr->p_coord.X;
		backw_ptr->p_coord.Z = visual_ptr->p_coord.Z;

		backw_ptr->simulation_time = visual_ptr->model_simulation_time;

		//sendBACKWARDUDPOnce(d2m);
	}
}


void UdpServer::stopSending()
{
	foreach(QTimer* timer, m_timerList)
	{
		timer->stop();
	}
	qInfo() << "UDP sending stopped.\n";
}

void UdpServer::sendOnce()
{
	//qDebug()<<"sending staff";
	if (m_enabledPackets["BACKWARD_DATA"] == true)
	{
		//  qDebug()<<"m_backwardPacket staff";
		_DataToModel* backw_ptr = reinterpret_cast<_DataToModel*>(m_backwardPacket.data());
		_MainVisualData* visual_ptr = reinterpret_cast<_MainVisualData*>(m_visualPacket.data());

		backw_ptr->p_coord.H = visual_ptr->p_coord.H;
		backw_ptr->p_coord.X = visual_ptr->p_coord.X;
		backw_ptr->p_coord.Z = visual_ptr->p_coord.Z;

		backw_ptr->simulation_time = visual_ptr->model_simulation_time;
		sendUDPOnce(m_backwardPacket);
	}

	
}


void UdpServer::setSendData_BACKWARD(const _DataToModel* data, bool check)
{
	//print meteo
	m_enabledPackets["BACKWARD_DATA"] = check;
	m_backwardPacket = QByteArray::fromRawData(reinterpret_cast<const char*>(data), sizeof(_DataToModel));
	m_backwardData = data;
}

void UdpServer::setDataFromReceived(const QByteArray &received)
{
	//  qDebug()<<"setting data from received function";
	QDataStream stream(received);

	unsigned char message_type = received.at(0);

	switch (message_type)
	{
	
	case NPR_PACKET_TYPE_BACK_DATA: //bckw
	{
		if (received.size() != sizeof(_DataToModel))
		{
			qWarning() << "paket with \"_DataToModel\" identifier has wrong size.\n";
			break;
		}
		m_backward_data = *(reinterpret_cast<const _DataToModel*>(received.data()));
		emit dataUpdated(&m_backward_data);
		break;
	}
	
}

void UdpServer::restartListening(quint16 _port)
{
	if(!setReceivingPort(_port)) {
		return;
	}
	// qDebug()<<"listen port is"<<receiving_port;

	if (m_receiver_socket->state() != QAbstractSocket::UnconnectedState)
	{
		//   m_receiver_socket->disconnectFromHost();
		m_receiver_socket->abort();
	}
	// qDebug() <<"try to bind";
	if (!m_receiver_socket->bind(QHostAddress::Any, receiving_port))
	{
		auto error = m_receiver_socket->errorString();
		qWarning() << "Could not create socket: " << QHostAddress(QHostAddress::Any)
			<< ":" << receiving_port << " " << error << "!\n";
		return;
	}

	qInfo() << "Listening on" << QHostAddress(QHostAddress::Any) << ":" << receiving_port << "\n";

}

void UdpServer::restartBACKWARDListening(quint16 _port)
{
	if (!setBackwardReceivingPort(_port)) {
		return;
	}
	//qDebug() << "listen port is" << backward_receive_port;

	if (m_backward_sender_socket->state() != QAbstractSocket::UnconnectedState)
	{
		//   m_receiver_socket->disconnectFromHost();
		m_backward_sender_socket->abort();
	}
	//qDebug() << "try to bind";
	if (!m_backward_sender_socket->bind(QHostAddress::Any, backward_receive_port))
	{
		auto error = m_backward_sender_socket->errorString();
		qWarning() << "Could not create socket: " << QHostAddress(QHostAddress::Any)
			<< ":" << backward_receive_port << " " << error << "!\n";
		return;
	}

	qInfo() << "Listening on" << QHostAddress(QHostAddress::Any) << ":" << backward_receive_port << "\n";

}

void UdpServer::startSending()
{
	foreach(QTimer* timer, m_timerList)
	{
		timer->start();
	}

	qInfo() << "UDP sending to " << address2send.toString() << " : "
		<< QString::number(sender_port) << " started.";
}

void UdpServer::sendUDPOnce(const QByteArray& packet)
{
	if (m_sender_socket->writeDatagram(packet, address2send, sender_port) == -1)
	{
		qWarning() << m_sender_socket->errorString();
	}

}

void UdpServer::sendBACKWARDUDPOnce(const QByteArray& packet)
{
	if (m_backward_sender_socket->writeDatagram(packet, backward_address2send, backward_sender_port) == -1)
	{
		qWarning() << m_backward_sender_socket->errorString();
	}

}
void UdpServer::readDatagram()
{
	QByteArray datagram;
	QHostAddress sender;
	quint16 senderPort;
	//  qDebug()<<"reading datagrams";
	  // read all availible datagrams
	if (m_receiver_socket->hasPendingDatagrams())
	{
		//  qDebug()<<"hasPendingDatagrams";

		while (m_receiver_socket->hasPendingDatagrams())
		{
			datagram.resize(m_receiver_socket->pendingDatagramSize());

			m_receiver_socket->readDatagram(datagram.data(), datagram.size(),
				&sender, &senderPort);

			if (datagram.size() == 0)
			{
				qWarning() << m_receiver_socket->errorString();
			}
			else
			{
				//отсылка дальше
				if (!m_send_from_this)
				{
				//	sendUDPOnce(datagram);
					  
					 //send map indication
					MAP_fill_route(&m_mapData, &m_vis_data, &m_airoports_lights_data);


					m_mapData.seconds = m_vis_data.model_simulation_time;
					//костыль!!! 	   fixme
				//	if (m_airoports_lights_data)
				//	m_mapData.updateRoute++;

					m_mapPacket = QByteArray::fromRawData(reinterpret_cast<const char*>(&m_mapData), sizeof(UDP_data_t));
					sendMAPUDPOnce(m_mapPacket);
					
					send_to_sound(m_vis_data, m_guiSoundSettings);
					send_to_plot(m_vis_data);
				}

				setDataFromReceived(datagram);
			}
		}
	}

}


void UdpServer::backwardReadDatagram()
{
	QByteArray datagram;
	QHostAddress sender;
	quint16 senderPort;

	if (m_backward_sender_socket->hasPendingDatagrams())
	{
		while (m_backward_sender_socket->hasPendingDatagrams())
		{
			datagram.resize(m_backward_sender_socket->pendingDatagramSize());

			m_backward_sender_socket->readDatagram(datagram.data(), datagram.size(),
				&sender, &senderPort);

			if (datagram.size() == 0)
			{
				qWarning() << m_backward_sender_socket->errorString();
			}
			else
			{
					setDataFromReceived(datagram);
					//sendBACKWARDUDPOnce(datagram);
			}
		}
	}

}


void UdpServer::send_to_plot(const _MainVisualData & visualData)
{

	if (visualData.SimulationReset) {
		send_reset_to_plot();

		// send channel names msg 
		auto namesMsg = m_plotPacker.packChannelNamesFromFile("plot_channel_names.json");
		if (namesMsg.size()) {
			if (
				m_plots_socket->writeDatagram(namesMsg.data(), namesMsg.size(), plot_address2send, plot_sender_port) == -1
				)
			{
				qWarning() << Q_FUNC_INFO << plot_address2send.toString() << ":" << plot_sender_port << " " << m_plots_socket->errorString();
			}
		}
	}

	// set data msg
	auto dataMsg = m_plotPacker.packVisualMessage(visualData);
	if (
		m_plots_socket->writeDatagram(dataMsg.data(), dataMsg.size(), plot_address2send, plot_sender_port) == -1
		)
	{
		qWarning() << Q_FUNC_INFO << plot_address2send.toString() << ":" << plot_sender_port << " " << m_plots_socket->errorString();
	}
}

void UdpServer::send_reset_to_plot()
{
	// send clear msg 
	auto clearMsg = m_plotPacker.packClearRequest();
	if (
		m_plots_socket->writeDatagram(clearMsg.data(), clearMsg.size(), plot_address2send, plot_sender_port) == -1
		)
	{
		qWarning() << Q_FUNC_INFO << plot_address2send.toString() << ":" << plot_sender_port << " " << m_plots_socket->errorString();
	}
}

void UdpServer::onNewGuiSoundSettings(const SOUND_FUNC_SETTINGS& newSettings)
{
	m_guiSoundSettings = newSettings;
}

void UdpServer::setSendToAddress(const QHostAddress& address, quint16 port)
{
	address2send = address;
	sender_port = port;
}