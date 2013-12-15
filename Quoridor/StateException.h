#include <iostream>
#include <exception>

/**
	classe servant à lancer des exceptions lorsque la classe est dans un état invalide
	et que l'on tente d'utiliser une de ces fonctions
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class StateException : public std::exception
{
private:
	std::string msg; // message d'erreur

public:
	/**
		Constructeur avec message d'erreur
		@param msg message d'erreur
	*/
	StateException(const char *msg)
	{
		this->msg = msg;
	}

	/**
		méthode de récupération du message d'erreur associé à cette exception
		@return message d'erreur
	*/
	virtual const char *what() const throw()
	{
		return this->msg.c_str();
	}
};
