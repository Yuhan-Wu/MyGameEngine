#pragma once
#include <stdint.h>
struct Descriptor;

class HeapManager
{
public:
	HeapManager(void* i_pMemory, size_t i_sizeMemory, unsigned int i_numDescriptors);

	static HeapManager* GetInstance();
	static void FreeInstance();

	void* alloc(size_t i_size);
	void* alloc(size_t i_size, unsigned int i_alignment);

	bool freeMem(void* i_ptr);
	void collectMem();

	bool contain(void* ptr) const;
	bool isAllocated(void* ptr) const;

	size_t getLargestFreeBlock() const;
	size_t getTotalFreeMemory() const;

	void showFreeBlocks() const;
	void showOutstandingAllocations() const;
	
	~HeapManager();

private:
	Descriptor* FreeMemoryList;
	Descriptor* UsedMemoryList;
	
	const static unsigned int padding = 8;
	
	void* start;
	size_t size;

	static HeapManager* my_manager;

	HeapManager();
};

inline bool HeapManager::contain(void* ptr) const {
	return ptr >= start && reinterpret_cast<uintptr_t>(ptr) < reinterpret_cast<uintptr_t>(start) + size;
}