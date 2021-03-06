#pragma once

#include "MIPS.h"
#include "MemoryFunction.h"
#ifdef AOT_BUILD_CACHE
#include "StdStream.h"
#include <mutex>
#endif

struct AOT_BLOCK_KEY
{
	uint32 crc;
	uint32 begin;
	uint32 end;

	bool operator<(const AOT_BLOCK_KEY& k2) const
	{
		const auto& k1 = (*this);
		if(k1.crc == k2.crc)
		{
			if(k1.begin == k2.begin)
			{
				return k1.end < k2.end;
			}
			else
			{
				return k1.begin < k2.begin;
			}
		}
		else
		{
			return k1.crc < k2.crc;
		}
	}
};
static_assert(sizeof(AOT_BLOCK_KEY) == 0x0C, "AOT_BLOCK_KEY must be 12 bytes long.");

namespace Jitter
{
	class CJitter;
};

extern "C"
{
	void EmptyBlockHandler(CMIPS*);
	void NextBlockTrampoline(CMIPS*);
}

class CBasicBlock
{
public:
	enum LINK_SLOT
	{
		LINK_SLOT_NEXT,
		LINK_SLOT_BRANCH,
		LINK_SLOT_MAX,
	};

	CBasicBlock(CMIPS&, uint32 = MIPS_INVALID_PC, uint32 = MIPS_INVALID_PC);
	virtual ~CBasicBlock() = default;
	void Execute();
	void Compile();
	virtual void CompileRange(CMipsJitter*);

	uint32 GetBeginAddress() const;
	uint32 GetEndAddress() const;
	bool IsCompiled() const;
	bool IsEmpty() const;

	uint32 GetRecycleCount() const;
	void SetRecycleCount(uint32);

	uint32 GetLinkTargetAddress(LINK_SLOT);
	void SetLinkTargetAddress(LINK_SLOT, uint32);
	void LinkBlock(LINK_SLOT, CBasicBlock*);
	void UnlinkBlock(LINK_SLOT);

#ifdef AOT_BUILD_CACHE
	static void SetAotBlockOutputStream(Framework::CStdStream*);
#endif

protected:
	uint32 m_begin;
	uint32 m_end;
	CMIPS& m_context;

	void CompileProlog(CMipsJitter*);
	void CompileEpilog(CMipsJitter*);

private:
	void HandleExternalFunctionReference(uintptr_t, uint32, Jitter::CCodeGen::SYMBOL_REF_TYPE);

#ifdef DEBUGGER_INCLUDED
	bool HasBreakpoint() const;
	static uint32 BreakpointFilter(CMIPS*);
	static void BreakpointHandler(CMIPS*);
#endif

#ifdef AOT_BUILD_CACHE
	static Framework::CStdStream* m_aotBlockOutputStream;
	static std::mutex m_aotBlockOutputStreamMutex;
#endif

#ifndef AOT_USE_CACHE
	CMemoryFunction m_function;
#else
	void (*m_function)(void*);
#endif
	uint32 m_recycleCount = 0;
	uint32 m_linkTargetAddress[LINK_SLOT_MAX];
	uint32 m_linkBlockTrampolineOffset[LINK_SLOT_MAX];
#ifdef _DEBUG
	CBasicBlock* m_linkBlock[LINK_SLOT_MAX];
#endif
};
