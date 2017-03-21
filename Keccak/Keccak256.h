// The GPL version 3 License (GPLv3)
// 
// Copyright (c) 2017 vtdev.com
// This file is part of the CEX Cryptographic library.
// 
// This program is free software : you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.If not, see <http://www.gnu.org/licenses/>.
//
// 
// Principal Algorithms:
// An implementation of the SHA-3 digest based on Keccak, designed by Guido Bertoni, Joan Daemen, Micha�l Peeters, and Gilles Van Assche. 
// SHA3 <a href="http://keccak.noekeon.org/Keccak-submission-3.pdf">Keccak Submission</a>.
// 
// Implementation Details:
// An implementation of the SHA-3 digest with a 256 bit return size. 
// Written by John Underhill, September 19, 2014
// Contact: develop@vtdev.com

#ifndef _CEX_KECCAK256_H
#define _CEX_KECCAK256_H

#include "IDigest.h"
#include "KeccakParams.h"

NAMESPACE_DIGEST

/// <summary>
/// An implementation of the SHA-3 Keccak digest
/// </summary>
///
/// <example>
/// <description>Example using the Compute method:</description>
/// <code>
/// Keccak256 digest;
/// std:vector&lt;byte&gt; hash(digest.DigestSize(), 0);
/// // compute a hash
/// digest.Compute(Input, hash);
/// </code>
/// </example>
///
/// <remarks>
/// <description>Implementation Notes:</description>
/// <list type="bullet">
/// <item><description>Hash sizes are 28, 32, and 36 bytes (224, 256, and 288 bits).</description></item>
/// <item><description>Block sizes are 144, 128, and 136 bytes (1152, 1024, 1088 bits).</description></item>
/// <item><description>Use the <see cref="BlockSize"/> property to determine block sizes at runtime.</description></item>
/// <item><description>The <see cref="Compute(byte[])"/> method wraps the <see cref="Update(byte[], int, int)"/> and Finalize methods.</description>/></item>
/// <item><description>The <see cref="Finalize(byte[], int)"/> method resets the internal state.</description></item>
/// </list>
/// 
/// <list type="number">
/// <item><description>SHA3 <a href="http://keccak.noekeon.org/Keccak-submission-3.pdf">Keccak Submission</a>.</description></item>
/// <item><description>SHA3 <a href="http://csrc.nist.gov/groups/ST/hash/sha-3/documents/Keccak-slides-at-NIST.pdf">Keccak Slides</a>.</description></item>
/// <item><description>SHA3 <a href="http://nvlpubs.nist.gov/nistpubs/ir/2012/NIST.IR.7896.pdf">Third-Round Report</a> of the SHA-3 Cryptographic Hash Algorithm Competition.</description></item>
/// </list>
/// </remarks>
class Keccak256 : public IDigest
{
private:

	struct Keccak256State
	{
		const ulong MAX_ULL = 0xFFFFFFFFFFFFFFFF;
		std::vector<ulong> H;
		ulong T;

		Keccak256State()
			:
			H(25, 0),
			T(0)
		{
		}

		void Increase(size_t Length)
		{
			T += Length;
		}

		void Reset()
		{
			if (H.size() > 0)
				memset(&H[0], 0, H.size() * sizeof(ulong));

			H[1] = MAX_ULL;
			H[2] = MAX_ULL;
			H[8] = MAX_ULL;
			H[12] = MAX_ULL;
			H[17] = MAX_ULL;
			H[20] = MAX_ULL;
			T = 0;
		}
	};

	static const size_t BLOCK_SIZE = 136;
	static const size_t DIGEST_SIZE = 32;

	// size of reserved state buffer subtracted from parallel size calculations
	const size_t STATE_PRECACHED = 2048;
	static const size_t STATE_SIZE = 25;
	static const size_t DEF_PRLDEGREE = 8;

	KeccakParams m_treeParams;
	std::vector<Keccak256State> m_dgtState;
	bool m_isDestroyed;
	std::vector<byte> m_msgBuffer;
	size_t m_msgLength;
	ParallelOptions m_parallelProfile;

public:

	Keccak256(const Keccak256&) = delete;
	Keccak256& operator=(const Keccak256&) = delete;
	Keccak256& operator=(Keccak256&&) = delete;

	//~~~Properties~~~//

	/// <summary>
	/// Get: The Digests internal blocksize in bytes
	/// </summary>
	virtual size_t BlockSize() { return BLOCK_SIZE; }

	/// <summary>
	/// Get: Size of returned digest in bytes
	/// </summary>
	virtual size_t DigestSize() { return DIGEST_SIZE; }

	/// <summary>
	/// Get: The digests type name
	/// </summary>
	virtual Digests Enumeral() { return Digests::Keccak256; }

	/// <summary>
	/// Get: Processor parallelization availability.
	/// <para>Indicates whether parallel processing is available with this mode.
	/// If parallel capable, input data array passed to the transform must be ParallelBlockSize in bytes to trigger parallelization.</para>
	/// </summary>
	virtual const bool IsParallel() { return m_parallelProfile.IsParallel(); }

	/// <summary>
	/// Get: The digests class name
	/// </summary>
	virtual const std::string Name() { return "Keccak256"; }

	/// <summary>
	/// Get: Parallel block size; the byte-size of the input/output data arrays passed to a transform that trigger parallel processing.
	/// <para>This value can be changed through the ParallelProfile class.<para>
	/// </summary>
	const size_t ParallelBlockSize() { return m_parallelProfile.ParallelBlockSize(); }

	/// <summary>
	/// Get/Set: Contains parallel settings and SIMD capability flags in a ParallelOptions structure.
	/// <para>The maximum number of threads allocated when using multi-threaded processing can be set with the ParallelMaxDegree() property.
	/// The ParallelBlockSize() property is auto-calculated, but can be changed; the value must be evenly divisible by ParallelMinimumSize().
	/// Note: The ParallelMaxDegree property can not be changed through this interface, use the ParallelMaxDegree(size_t) function to change the thread count 
	/// and reinitialize the state, or initialize the digest using a SkeinParams with the FanOut property set to the desired number of threads.</para>
	/// </summary>
	ParallelOptions &ParallelProfile() { return m_parallelProfile; }

	//~~~Constructor~~~//

	/// <summary>
	/// Initialize the class with either the Parallel or Sequential hashing engine.
	/// <para>Initialize as parallel instantiates tree hashing, if false uses the standard SHA-3 256bit hashing instance.</para>
	/// </summary>
	/// 
	/// <param name="Parallel">Setting the Parallel flag to true, instantiates the multi-threaded SHA-3 variant.</param>
	explicit Keccak256(bool Parallel = false);

	/// <summary>
	/// Initialize the class with an KeccakParams structure.
	/// <para>The parameters structure allows for tuning of the internal configuration string,
	/// and changing the number of threads used by the parallel mechanism (FanOut).
	/// If the parallel degree is greater than 1, multi-threading hash engine is instantiated.
	/// The default thread count is 8, changing this value will produce a different output hash code.</para>
	/// </summary>
	/// 
	/// <param name="Params">The KeccakParams structure, containing the tree configuration settings.</param>
	///
	/// <exception cref="CryptoDigestException">Thrown if the KeccakParams structure contains invalid values</exception>
	explicit Keccak256(KeccakParams &Params);

	/// <summary>
	/// Finalize objects
	/// </summary>
	virtual ~Keccak256();

	//~~~Public Functions~~~//

	/// <summary>
	/// Get the Hash value
	/// </summary>
	/// 
	/// <param name="Input">Input data</param>
	/// <param name="Output">The hash output value array</param>
	virtual void Compute(const std::vector<byte> &Input, std::vector<byte> &Output);

	/// <summary>
	/// Release all resources associated with the object
	/// </summary>
	virtual void Destroy();

	/// <summary>
	/// Do final processing and get the hash value
	/// </summary>
	/// 
	/// <param name="Output">The Hash output value array</param>
	/// <param name="OutOffset">The starting offset within the Output array</param>
	/// 
	/// <returns>Size of Hash value</returns>
	///
	/// <exception cref="CryptoDigestException">Thrown if the output buffer is too short</exception>
	virtual size_t Finalize(std::vector<byte> &Output, const size_t OutOffset);

	/// <summary>
	/// Set the number of threads allocated when using multi-threaded tree hashing processing.
	/// <para>Thread count must be an even number, and not exceed the number of processor cores.
	/// Changing this value from the default (8 threads), will change the output hash value.</para>
	/// </summary>
	///
	/// <param name="Degree">The desired number of threads</param>
	///
	/// <exception cref="Exception::CryptoCipherModeException">Thrown if an invalid degree setting is used</exception>
	virtual void ParallelMaxDegree(size_t Degree);

	/// <summary>
	/// Reset the internal state
	/// </summary>
	virtual void Reset();

	/// <summary>
	/// Update the digest with a single byte
	/// </summary>
	///
	/// <param name="Input">Input byte</param>
	virtual void Update(byte Input);

	/// <summary>
	/// Update the buffer
	/// </summary>
	/// 
	/// <param name="Input">Input data</param>
	/// <param name="InOffset">The starting offset within the Input array</param>
	/// <param name="Length">Amount of data to process in bytes</param>
	///
	/// <exception cref="CryptoDigestException">Thrown if the input buffer is too short</exception>
	virtual void Update(const std::vector<byte> &Input, size_t InOffset, size_t Length);

private:

	void Compress(const std::vector<byte> &Input, size_t InOffset, Keccak256State &State);
	void HashFinal(std::vector<byte> &Input, size_t InOffset, size_t Length, Keccak256State &State);
	void ProcessLeaf(const std::vector<byte> &Input, size_t InOffset, Keccak256State &State, ulong Length);
};

NAMESPACE_DIGESTEND
#endif
