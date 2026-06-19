/**
 * @include <iostream>
 * @include <signal.h>
 */
#include "../includes/Server.hpp"
#include <iostream>
#include <signal.h>

volatile int g_sig = 0;

/**
 * @brief handle signal
 */
void handler(sig_atomic_t signal)
{
	g_sig = signal;
	return;
}

int main (int ac, char **av)
{
	if (ac != 3) {std::cerr << "Error\n -> expected ./irc <port> <password>" << std::endl; return (1);}
	
	signal(SIGINT, handler);

	//parsing todo
	Server serv;
	if (!serv.parse_data(av)) {return 1;};
	if (serv.init_server() == 1) {return 1;};
	serv.run_event_loop();
	//test client joining channel
	/*serv.joinChannel(4, "#general");
	serv.joinChannel(5, "#general");*/
	return (g_sig);
}