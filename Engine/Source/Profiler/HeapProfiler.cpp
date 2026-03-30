#include "HeapProfiler.h"

#include "Input/EnhancedInputManager.h"

#define DECLARE_HEAP_TRACKED(Type)                                  \
public:                                                             \
    static constexpr const char* GetHeapTypeName() { return #Type; }\
    static void* operator new(size_t Size)                          \
    {                                                               \
        return FHeapProfiler::Allocate(Size, #Type);                \
    }                                                               \
    static void operator delete(void* Ptr) noexcept                 \
    {                                                               \
        FHeapProfiler::Deallocate(Ptr);                             \
    }                                                               \
    static void* operator new[](size_t Size)                        \
    {                                                               \
        return FHeapProfiler::Allocate(Size, #Type "[]");           \
    }                                                               \
    static void operator delete[](void* Ptr) noexcept               \
    {                                                               \
        FHeapProfiler::Deallocate(Ptr);                             \
    }

void* FHeapProfiler::Allocate(size_t Size, const char* TypeName)
{
	void* Ptr = std::malloc(Size);
	if (!Ptr)
	{
		throw std::bad_alloc();
	}

	std::scoped_lock Lock(GetMutex());

	GetLiveAllocations()[Ptr] = { Size, TypeName };

	FTypeStats& Stats = GetTypeStats()[TypeName];
	Stats.AllocCount++;
	Stats.CurrentBytes += Size;
	Stats.CurrentLiveCount++;
	Stats.PeakBytes = std::max(Stats.PeakBytes, Stats.CurrentBytes);

	return Ptr;
}

void FHeapProfiler::Deallocate(void* Ptr)
{
	if (!Ptr)
	{
		return;
	}

	std::lock_guard Lock(GetMutex());

	auto& Live = GetLiveAllocations();
	auto It = Live.find(Ptr);
	if (It != Live.end())
	{
		const FAllocationInfo Info = It->second;

		auto& Stats = GetTypeStats()[Info.TypeName];
		Stats.FreeCount++;
		Stats.CurrentBytes -= Info.Size;
		Stats.CurrentLiveCount--;

		Live.erase(It);
	}

	std::free(Ptr);
}

void FHeapProfiler::DumpStats()
{
	std::lock_guard<std::mutex> Lock(GetMutex());

	std::puts("===== Heap Profile =====");
	for (const auto& Pair : GetTypeStats())
	{
		const FString& TypeName = Pair.first;
		const FTypeStats& Stats = Pair.second;

		std::printf(
			"%-32s alloc=%llu free=%llu live=%llu current=%llu bytes peak=%llu bytes\n",
			TypeName.c_str(),
			static_cast<unsigned long long>(Stats.AllocCount),
			static_cast<unsigned long long>(Stats.FreeCount),
			static_cast<unsigned long long>(Stats.CurrentLiveCount),
			static_cast<unsigned long long>(Stats.CurrentBytes),
			static_cast<unsigned long long>(Stats.PeakBytes)
		);
	}
}

std::mutex& FHeapProfiler::GetMutex()
{
	static std::mutex Mutex;
	return Mutex;
}

TMap<void*, FAllocationInfo>& FHeapProfiler::GetLiveAllocations()
{
	static TMap<void*, FAllocationInfo> Live;
	return Live;
}

TMap<FString, FTypeStats>& FHeapProfiler::GetTypeStats()
{
	static TMap<FString, FTypeStats> Stats;
	return Stats;
}
