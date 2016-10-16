// main.cpp

#include <iostream>
#include <Core/Interfaces/IToString.h>
#include <Core/Interfaces/IFromString.h>
#include <Core/Reflection/Any.h>

namespace sge
{
	void print(Any<IToString> value)
	{
		std::cout << value.call(&IToString::to_string) << std::endl;
	}

	void read(AnyMut<IFromString> value)
	{
		std::string str;
		std::cin >> str;
		value.call(&IFromString::from_string, str);
	}

	void do_stuff(AnyMut<IToString, IFromString> value)
	{
		std::cout << "Current value is: ";
		print(value);

		std::cout << "Enter a number: ";
		read(value);

		std::cout << "New value is: ";
		print(value);
	}
}

int main()
{
	int i = 59;
	sge::do_stuff(i);

	std::cin.get();
	std::cin.get();
}
