#include "../src/Utility/Vector.h"



void vectorTest()
{
	Vector<uint32_t> test;

	test.emplace(4);
	test.emplace(4);
	test.emplace(4);

	assert(test.getSize() == 3);

	assert(test.at(0) == test.at(1));
	assert(test.at(1) == test.at(2));

	test.insert(3, 0);

	assert(test.at(0) == 3);
	assert(test.at(3) == 4);

	test.set(2, 3);

	assert(test.at(3) == 2);

	printf("ALL VECTOR TEST WERE SUCCESSFUL\n");

}
