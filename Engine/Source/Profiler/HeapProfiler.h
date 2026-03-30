#pragma once
#include <mutex>

#include "EngineAPI.h"
#include "Types/Map.h"
#include "Types/String.h"

struct FAllocationInfo
{
	size_t Size = 0;
	const char* TypeName = "Unknown";
};

struct FTypeStats
{
	uint64_t AllocCount = 0;
	uint64_t FreeCount = 0;
	uint64_t CurrentBytes = 0;
	uint64_t PeakBytes = 0;
	uint64_t CurrentLiveCount = 0;
};

class ENGINE_API FHeapProfiler
{
public:
	static void* Allocate(size_t Size, const char* TypeName);
	static void	 Deallocate(void* Ptr);
	static void	 DumpStats();

private:
	static std::mutex& GetMutex();
	static TMap<void*, FAllocationInfo>& GetLiveAllocations();
	static TMap<FString, FTypeStats>& GetTypeStats();
};