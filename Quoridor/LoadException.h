#include <iostream>
#include <exception>

/**
	classe servant à lancer des exceptions lors d'un chargement invalide
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class LoadException : public std::exception
{
private:
	std::string msg; // message d'erreur

public:
	/**
		Constructeur avec message d'erreur
		@param msg message d'erreur
	*/
	LoadException(const char *msg)
	{
		this->msg = msg;
	}

	/**
		Obtenir le message d'erreur associé à cette exception
		@return message d'erreur
	*/
	virtual const char *what() const throw()
	{
		return this->msg.c_str();
	}
};
