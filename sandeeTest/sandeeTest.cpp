#include "CppUnitTest.h"
#include "../array.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace sandeeTest
{
	TEST_CLASS(arrayTest)
	{
	public:
		
		TEST_METHOD(appendAbsent)
		{
			Array<int> array;
			array.appendAbsent(42);
			array.appendAbsent(42);
			Assert::AreEqual(1ULL, array.size());

			array[0] = 7;
			Assert::AreEqual(1ULL, array.size());
		}
	};
}
