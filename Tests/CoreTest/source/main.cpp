// main.cpp

#include <iostream>
#include <Core/Math/Vec3.h>

void set_float(sge::AnyMut value)
{
	*static_cast<float*>(value.get_value()) = 5;
}

int main()
{
	sge::Vec3 vec{ 1, 2, 3 };

	vec.get_type().mutate_property("y", &vec, nullptr, set_float);

	std::cout << vec.y();
	std::cin.get();
}
