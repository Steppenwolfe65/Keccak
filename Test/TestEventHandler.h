#ifndef _KECCAKTEST_TESTEVENTHANDLER_H
#define _KECCAKTEST_TESTEVENTHANDLER_H

#include "TestEvent.h"
#include "ConsoleUtils.h"

namespace TestKeccak
{
	class TestEventHandler : public TestEvent<TestEventHandler>
	{
	public:
		void operator()(const char* Data)
		{
			ConsoleUtils::WriteLine(std::string(Data));
		}
	};
}

#endif