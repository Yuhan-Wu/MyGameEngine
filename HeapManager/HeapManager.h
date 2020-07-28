#pragma once
#include <stdint.h>
struct Descriptor;

class HeapManager
{
public:
	static HeapManager* GetInstance();
	static HeapManager* GetInstance(void* p_Memory, size_t p_SizeMemory, unsigned int p_NumDescriptors);
	static void FreeInstance();

	void* alloc(size_t p_Size);
	void* alloc(size_t p_Size, unsigned int p_Alignment);

	bool freeMem(void* p_Ptr);
	void collectMem();

	bool contain(void* p_Ptr) const;
	bool isAllocated(void* p_Ptr) const;

	size_t getLargestFreeBlock() const;
	size_t getTotalFreeMemory() const;

	void showFreeBlocks() const;
	void showOutstandingAllocations() const;
	
	~HeapManager();

private:
	Descriptor* m_FreeMemoryList;
	Descriptor* m_UsedMemoryList;
	
	void* m_Start;
	size_t m_Size;

	const static unsigned int PADDING = 8;

	static HeapManager* MyManager;

	HeapManager();
	HeapManager(void* p_Memory, size_t p_SizeMemory, unsigned int p_NumDescriptors);
};

inline bool HeapManager::contain(void* p_Ptr) const {
	return p_Ptr >= m_Start && reinterpret_cast<uintptr_t>(p_Ptr) < reinterpret_cast<uintptr_t>(m_Start) + m_Size;
}