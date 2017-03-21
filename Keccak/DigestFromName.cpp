#include "DigestFromName.h"
#include "Keccak256.h"
#include "Keccak512.h"

NAMESPACE_HELPER

IDigest* DigestFromName::GetInstance(Digests DigestType, bool Parallel)
{
	try
	{
		switch (DigestType)
		{
		case Digests::Keccak256:
			return new Digest::Keccak256(Parallel);
		case Digests::Keccak512:
			return new Digest::Keccak512(Parallel);
		default:
			throw;
		}
	}
	catch (const std::exception)
	{
		throw;
	}
}

size_t DigestFromName::GetBlockSize(Digests DigestType)
{
	try
	{
		switch (DigestType)
		{
		case Digests::Keccak256:
			return 136;
		case Digests::Keccak512:
			return 72;
		case Digests::None:
			return 0;
		default:
			throw;
		}
	}
	catch (const std::exception)
	{
		throw;
	}
}

size_t DigestFromName::GetDigestSize(Digests DigestType)
{
	try
	{
		switch (DigestType)
		{
		case Digests::Keccak256:
			return 32;
		case Digests::Keccak512:
			return 64;
		case Digests::None:
			return 0;
		default:
			throw;
		}
	}
	catch (const std::exception)
	{
		throw;
	}
}

size_t DigestFromName::GetPaddingSize(Digests DigestType)
{
	try
	{
		switch (DigestType)
		{
		case Digests::Keccak256:
		case Digests::Keccak512:
			return 0;
		default:
			throw;
		}
	}
	catch (const std::exception)
	{
		throw;
	}
}

NAMESPACE_HELPEREND