// GigaMOOG.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdafx.h>
#include <GigaMoogCore.h>
#include <test_reopen.h>
#include <test_reopen_latest.h>

void gm();
int test2();

int main()
{
	gm();
	return 0;
}

void gm()
{
	{
		GigaMoogCore core("COM5", 115200);
		core.disconnectPort();
		std::cout << "Hello World!\n";
		core.reconnectPort();
		std::cout << "Fuck World!\n";

	}
	std::cout << "Hello World!\n";
	std::cout << "Hello World!\n";
}

//int main()
//{
//	// on Unix POSIX based systems, turn off line buffering of input, so cin.get() returns after every keypress
//		// On other systems, you'll need to look for an equivalent
//#ifdef POSIX
//	termios stored_settings;
//	tcgetattr(0, &stored_settings);
//	termios new_settings = stored_settings;
//	new_settings.c_lflag &= (~ICANON);
//	new_settings.c_lflag &= (~ISIG); // don't automatically handle control-
//	C
//		tcsetattr(0, TCSANOW, &new_settings);
//#endif
//	try
//	{
//		boost::asio::io_service io_service;
//		// define an instance of the main class of this program
//		minicom_client c(io_service, 8, "COM5");
//		// run the IO service as a separate thread, so the main thread can block on standard input
//
//		c.open
//		boost::thread t(boost::bind
//		(&boost::asio::io_service::run, &io_service));
//
//		boost::asio::io_service io;
//		c.close(); // close the minicom client connection
//		t.join(); // wait for the IO service thread to close
//		std::cout << "-------------------" << std::endl;
//	}
//	catch (exception& e)
//	{
//		cerr << "Exception: " << e.what() << "\n";
//	}
//#ifdef POSIX // restore default buffering of standard input
//	tcsetattr(0, TCSANOW, &stored_settings);
//#endif
//	return 0;
//}


int test2()
{

	try
	{
		boost::asio::io_service io_service;
		// define an instance of the main class of this program
		minicom_client2 c(io_service, 115200);

		// run the IO service as a separate thread, so the main thread		can block on standard input


		while (true) // PROBLEM: reentering this loop fails - why?
		{

			std::string port = "COM5";
			c.open(port);

			
			boost::thread t(boost::bind
			(&boost::asio::io_service::run, &io_service));

			Sleep(100);

			c.close(); // close the minicom client connection
			t.join(); // wait for the IO service thread to close
			std::cout << "-------------------" << std::endl;

			Sleep(100);

			io_service.restart();

			Sleep(100);

		}
	}
	catch (exception& e)
	{
		cerr << "Exception: " << e.what() << "\n";
	}
	return 0;
}





// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
