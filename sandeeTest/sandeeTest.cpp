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
			array.appendAbsent(1);
			array.appendAbsent(1);
			Assert::AreEqual(static_cast<size_t>(1), array.size());
		}
	};
}
