#include "stdafx.h"
#include "BoostAsyncSerial.h"

BoostAsyncSerial::BoostAsyncSerial(std::string portID, int baudrate)
	: BoostAsyncSerial(portID, baudrate, 8,
		boost::asio::serial_port_base::stop_bits::one,
		boost::asio::serial_port_base::parity::none,
		boost::asio::serial_port_base::flow_control::none)
{ }

BoostAsyncSerial::BoostAsyncSerial(
	std::string portID,
	int baudrate,
	int character_size,
	boost::asio::serial_port_base::stop_bits::type stop_bits,
	boost::asio::serial_port_base::parity::type parity,
	boost::asio::serial_port_base::flow_control::type flow_control) 
	: portID(portID)
	, baudrate(baudrate)
	, continue_read(true)
{
	port_ = std::make_unique<boost::asio::serial_port>(io_service_);

	port_->open(portID);
	port_->set_option(boost::asio::serial_port_base::baud_rate(baudrate));
	port_->set_option(boost::asio::serial_port_base::character_size(character_size));
	port_->set_option(boost::asio::serial_port_base::stop_bits(stop_bits));
	port_->set_option(boost::asio::serial_port_base::parity(parity));
	port_->set_option(boost::asio::serial_port_base::flow_control(flow_control));

	io_thread = std::make_unique<boost::thread>(boost::bind(&BoostAsyncSerial::run, this));
	//io_thread = boost::thread([]() {
	//	std::cout << "caonima";
	//	});
	Sleep(1000);
	read();
}

BoostAsyncSerial::~BoostAsyncSerial()
{
	io_service_.stop();

	if (port_) {
		port_->cancel();
		port_->close();
	}
	
	io_thread->join();
}

void BoostAsyncSerial::setReadCallback(const boost::function<void(int)>& read_callback)
{
	read_callback_ = read_callback;
}

void BoostAsyncSerial::readhandler(const boost::system::error_code & error, std::size_t bytes_transferred)
{
	boost::mutex::scoped_lock look(mutex_);
	std::cout << "BoostAsyncSerial::readhandler" << std::endl;
	if (error) {
		if (!continue_read) {
			// probably due to the port closing, so just return
			std::string s = error.message();
			std::cout << "continue_read false: " << s << std::endl;
		}
		else {
			std::string s = error.message();
			throw("Error reading from serial: " + s);
		}
	}
	if (continue_read) {
		port_->async_read_some(boost::asio::buffer(readbuffer), boost::bind(&BoostAsyncSerial::readhandler, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		));
	}

	int c;
	for (int idx = 0; idx < bytes_transferred; idx++) {
		c = static_cast<int>(readbuffer[idx]) & 0xFF;
		if (!read_callback_.empty()) {
			read_callback_(c);
		}
	}
		
}

void BoostAsyncSerial::write(std::vector<unsigned char> data)
{
	if (!port_->is_open()) {
		throw("Serial port has not been opened");
	}
	boost::asio::write(*port_, boost::asio::buffer(data));
}

void BoostAsyncSerial::write(std::vector<int> data)
{
	std::vector<unsigned char> converted(data.size());
	for (int idx = 0; idx < data.size(); idx++) {
		if(data[idx] < 0 || data[idx] >255){
			throw("Byte value needs to be in range 0-255");
		}
		converted[idx] = data[idx];
	}

	std::cout << "Serial is writing: ";
	for (auto& byte : data)
		std::cout << (int)byte << " ";
	std::cout << "\n";

	boost::asio::write(*port_, boost::asio::buffer(converted));
}

void BoostAsyncSerial::disconnect()
{
	bool GIGAMOOG_SAFEMODE = false;
	if (GIGAMOOG_SAFEMODE) {
		return;
	}
	if (!port_) {
		throw("port_ is already closed. Can not disconnect again");
	}
	try {
		Sleep(1000);
		//io_service_.stop();
		continue_read = false;
		Sleep(100);
		if (port_) {
			//io_service_.post([this]() {
			//	//io_service_.stop();
			//	port_->cancel();
			//	port_->close();
			//	//work->reset(); 
			//	});
			//Sleep(100);
			
			boost::system::error_code ec;
			port_->cancel(ec);
			auto s1 = ec.message();
			port_->close(ec);
			auto s2 = ec.message();
			//io_service_.restart();

		}
		if (port_->is_open()) {
			throw("After port->close(), the port is still open??");
		}
	}
	catch (boost::system::system_error& ex) {
		throw(ex.what());
	}
	//io_service_.post([this]() {
	//	io_service_.stop();
	//	port_->close();
	//	work->reset(); });
	//port_.reset();
	//work.reset();
	//io_service_.restart();
	//io_thread.detach();
	
	work->reset();
	work.reset();
	//io_thread.interrupt();
	io_thread->join();
	Sleep(100);
	//io_service_.stop();
	//io_service_.restart();
	port_.reset();
	//io_thread.reset();
	Sleep(100);
}

void BoostAsyncSerial::reconnect()
{
	bool GIGAMOOG_SAFEMODE = false;
	if (GIGAMOOG_SAFEMODE) {
		return;
	}
	if (port_ && port_->is_open()/*io_service_.stopped()*/) {
		throw("port_ is already open. Can not connect again");
	}
	io_service_.restart();

	port_ = std::make_unique<boost::asio::serial_port>(io_service_);
	try {
		port_->open(portID);
	}
	catch (boost::system::system_error& ex) {
		std::string s = ex.what();
		throw(ex.what());
	}
	port_->set_option(boost::asio::serial_port_base::baud_rate(baudrate));
	port_->set_option(boost::asio::serial_port_base::character_size(8));
	port_->set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
	port_->set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
	port_->set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

	io_thread.reset(new boost::thread(boost::bind(&BoostAsyncSerial::run, this)));
	read();
}

void BoostAsyncSerial::read()
{
	if (!port_->is_open()) {
		throw("Serial port has not been opened");
	}
	port_->async_read_some(boost::asio::buffer(readbuffer), boost::bind(&BoostAsyncSerial::readhandler,
		this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred
	));
}

void BoostAsyncSerial::run()
{
	//work = std::make_unique<boost::asio::io_service::work>(io_service_);
	//work = boost::asio::make_work_guard(io_service_); // assigment operator is private, but not for copy. 
	work = std::make_unique<work_guard>(boost::asio::make_work_guard(io_service_));
	std::cout << "start run" << std::endl;
	io_service_.run();
	std::cout << "finally returned from run" << std::endl;
}