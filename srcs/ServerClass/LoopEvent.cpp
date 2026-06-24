#include "../../includes/Server.hpp"
#include <unistd.h>
#include <cerrno>
#include <sstream>

extern int g_sig;

/**
 * @brief Run the main "infinite" poll() event loop on listening fd.
 */
void Server::run_event_loop()
{
	struct pollfd listen_pollfd;
	listen_pollfd.fd = _listen_fd;
	listen_pollfd.events = POLLIN;
	listen_pollfd.revents = 0;
	_fds.push_back(listen_pollfd);

	std::cout << "Server port: " << _port << std::endl;
	std::cout << "Server password: " << _pwd << std::endl;
	std::cout << "Server listening ..." << std::endl;

	while (!g_sig)
	{
		int ready = poll(&_fds[0], _fds.size(), -1);
		if (ready <= 0)
		{
			if (errno == EINTR)
				continue;
			std::cerr << "poll fail" << std::endl;
			break;
		}

		// incoming connections
		if (_fds[0].revents & POLLIN)
		{
			while (!g_sig)
			{
				sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				int client_fd = accept(_listen_fd, (sockaddr *)&client_addr, &client_len);
				if (client_fd < 0)
				{
					if (errno == EAGAIN || errno == EWOULDBLOCK)
						break;
					if (errno == EINTR)
						continue;
					std::cerr << "accept fail" << std::endl;
					break;
				}

				if (set_nonblocking(client_fd) < 0)
				{
					close(client_fd);
					continue;
				}
				std::string ip = inet_ntoa(client_addr.sin_addr);
				struct pollfd client_pollfd;
				client_pollfd.fd = client_fd;
				client_pollfd.events = POLLIN;
				client_pollfd.revents = 0;
				_fds.push_back(client_pollfd);

				// create placeholder client name so the client appears registered to hexChat
				std::ostringstream cn;
				cn << "client" << client_fd;
				std::string placeholderName = cn.str();
				_clients.push_back(Client(std::string(""), client_fd, ip));
				std::cout << "client connected fd " << client_fd << " with ip= " << ip << std::endl;
				display_status();
			}
		}
		// events on client sockets
		for (size_t i = 1; i < _fds.size(); ++i)
		{
			short revents = _fds[i].revents;
			if (revents == 0)
				continue;

			int clientFd = _fds[i].fd;
			if (revents & (POLLHUP | POLLERR | POLLNVAL))
			{
				std::cout << "client disconnected POLLHUP fd " << clientFd << std::endl;
				CommandQuit(clientFd, 1);
				break;
			}
			// if a client have send something we handle what we received
			if (revents & POLLIN)
			{
				bool connected = handleClientInput(clientFd);
				if (!connected)
				{
					std::cout << "client disconnected POLLIN fd " << clientFd << std::endl;
					CommandQuit(clientFd, 1);
					break;
				}
			}
		}
	}

	//close all fd when server shutdown
	for (size_t i = 0; i < _fds.size(); ++i)
		close(_fds[i].fd);

	for (size_t i = 0; i < _fds.size(); ++i)
	{
		std::cout << "connected at close : " << _fds[i].fd << std::endl;
	}
}
