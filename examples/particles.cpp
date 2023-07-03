#include "particles.hpp"

int main() //
{
	foo::particles p;
	foo::particles p2; //{ p };

	p.reserve(10u);

	p = std::move(p2);
	p.swap(p2);
	p2 = p;

	p.pop_back();

	return 0;
}
