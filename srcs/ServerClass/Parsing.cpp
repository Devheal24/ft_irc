#include "../../includes/Server.hpp"

/**
 * @brief get User input and handle/parse them
 */
bool Server::parse_data(char **av) {
	//port parsing
	char *end = NULL;
	this->SetPort(std::strtol(av[1], &end, 10));
	if (GetPort() == 0 || end == av[1] || *end != '\0' || GetPort() < 6665 || GetPort() > 6669)
	{
		std::cerr << "Error\n -> port parsing : " << av[1] << std::endl;
		return false;  
	}

	//pwd parsing
	this->SetPwd(static_cast<std::string>(av[2]));
	if (!_pwd.empty() && (_pwd.length() < PWD_MINL || _pwd.length() > PWD_MAXL))
	{
		std::cout << "invalid length of pass (64> || <8)" << _pwd << std::endl;
		return false;
	}
	for (size_t i = 0; i < _pwd.length(); i++)
	{
		if (!isprint(_pwd[i]))
		{
			std::cout << "invalid character in passname" << _pwd << std::endl;
			return false;
		}
	}
	return true;
};
