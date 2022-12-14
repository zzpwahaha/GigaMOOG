#pragma once
#include <winsock2.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <iostream>

class BoostAsyncSerial
{
public:
	//this is just a simple constructor with default options
	BoostAsyncSerial(std::string portID, int baudrate);
	//constructor with all options available
	BoostAsyncSerial(
		std::string portID,
		int baudrate,
		int character_size,
		boost::asio::serial_port_base::stop_bits::type stop_bits,
		boost::asio::serial_port_base::parity::type parity,
		boost::asio::serial_port_base::flow_control::type flow_control
	);
	~BoostAsyncSerial();

	void setReadCallback(const boost::function<void(int)> &read_callback);

	void write(std::vector<unsigned char>);
	void write(std::vector<int>);

	void disconnect();
	void reconnect();
private:
	const std::string portID;
	const int baudrate;
	boost::asio::io_service io_service_;
	//std::unique_ptr<boost::asio::io_service> io_service_;
	std::unique_ptr<boost::asio::serial_port> port_;
	
	void read();
	void run();
	
	std::array<unsigned char, 1024> readbuffer;

	void readhandler(
		const boost::system::error_code& error,
		std::size_t bytes_transferred
	);

	boost::thread io_thread;
	boost::mutex mutex_;

	boost::function<void(uint8_t)> read_callback_;

	//std::unique_ptr<boost::asio::io_service::work> work;
	typedef boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard;
	std::unique_ptr<work_guard> work;
};

