#include "../../includes/Server.hpp"
#include <unistd.h>
#include <cstring>

/**
 * @brief Create, bind, listen and set non-blocking a listening socket for given port
 */
int Server::init_server()
{
	srand(time(NULL));

	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0)
	{
		std::cerr << "socket fail" << std::endl;
		return 1;
	}

	int is_reuse = 1;
	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &is_reuse, sizeof(is_reuse)) < 0)
	{
		std::cerr << "setsockopt fail" << std::endl;
		close(listen_fd);
		return 1;
	}

	sockaddr_in serv_addr;
	std::memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(_port);

	if (bind(listen_fd, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		std::cerr << "bind fail" << std::endl;
		close(listen_fd);
		return 1;
	}

	if (listen(listen_fd, SOMAXCONN) < 0)
	{
		std::cerr << "listen fail" << std::endl;
		close(listen_fd);
		return 1;
	}

	if (set_nonblocking(listen_fd) < 0)
	{
		std::cerr << "failed to set non-blocking" << std::endl;
		close(listen_fd);
		return 1;
	}

	_listen_fd = listen_fd;
	return 0;
}
