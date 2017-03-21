#ifndef _BLAKE2TEST_DIGESTSPEEDTEST_H
#define _BLAKE2TEST_DIGESTSPEEDTEST_H

#include "ITest.h"
#include "../Keccak/DigestFromName.h"
#include "../Keccak/Digests.h"

namespace TestKeccak
{
	using CEX::Enumeration::Digests;

	/// <summary>
	/// Keccak Digest Speed Tests
	/// </summary>
	class DigestSpeedTest : public ITest
	{
	private:
		const std::string DESCRIPTION = "Digest Speed Tests.";
		const std::string FAILURE = "FAILURE! ";
		const std::string MESSAGE = "COMPLETE! Speed tests have executed succesfully.";
		static constexpr uint64_t KB1 = 1024;
		static constexpr uint64_t MB1 = KB1 * 1000;
		static constexpr uint64_t MB10 = MB1 * 10;
		static constexpr uint64_t MB100 = MB1 * 100;
		static constexpr uint64_t GB1 = MB1 * 1000;
		static constexpr uint64_t DATA_SIZE = MB100;
		static constexpr uint64_t DEFITER = 10;

		TestEventHandler m_progressEvent;

	public:
		/// <summary>
		/// Get: The test description
		/// </summary>
		virtual const std::string Description() { return DESCRIPTION; }

		/// <summary>
		/// Progress return event callback
		/// </summary>
		virtual TestEventHandler &Progress() { return m_progressEvent; }

		/// <summary>
		/// Test Keccak for performance
		/// </summary>
		DigestSpeedTest()
		{
		}

		/// <summary>
		/// Start the tests
		/// </summary>
		virtual std::string Run()
		{
			try
			{
				OnProgress("***The sequential Keccak 256 digest***");
				DigestBlockLoop(Digests::Keccak256, MB100, 10, false);
				OnProgress("***The parallel Keccak 256 digest***");
				DigestBlockLoop(Digests::Keccak256, MB100, 10, true);
				OnProgress("***The sequential Keccak 512 digest***");
				DigestBlockLoop(Digests::Keccak512, MB100, 10, false);
				OnProgress("***The parallel Keccak 512 digest***");
				DigestBlockLoop(Digests::Keccak512, MB100, 10, true);
				return MESSAGE;
			}
			catch (std::string &ex)
			{
				return FAILURE + " : " + ex;
			}
			catch (...)
			{
				return FAILURE + " : Internal Error";
			}
		}

	private:

		void DigestBlockLoop(Digests DigestType, size_t SampleSize, size_t Loops = DEFITER, bool Parallel = false);
		uint64_t GetBytesPerSecond(uint64_t DurationTicks, uint64_t DataSize);
		void OnProgress(char* Data);
	};
}

#endif