#include "HeapManagerProxy.h"
#include "HeapManager.h"

namespace HeapManagerProxy {
	HeapManager* CreateHeapManager(void* i_pMemory, size_t i_sizeMemory, unsigned int i_numDescriptors) {
		// return new HeapManager(i_pMemory, i_sizeMemory, i_numDescriptors);
		return HeapManager::GetInstance(i_pMemory, i_sizeMemory, i_numDescriptors);
	}

	void Destroy(HeapManager* i_pManager) {
		delete i_pManager;
	}

	void* alloc(HeapManager* i_pManager, size_t i_size) {
		return i_pManager->alloc(i_size);
	}

	void* alloc(HeapManager* i_pManager, size_t i_size, unsigned int i_alignment) {
		return i_pManager->alloc(i_size,i_alignment);
	}

	bool free(HeapManager* i_pManager, void* i_ptr) {
		return i_pManager->freeMem(i_ptr);
	}

	void Collect(HeapManager* i_pManager) {
		i_pManager->collectMem();
	}

	bool Contains(const HeapManager* i_pManager, void* i_ptr) {
		return i_pManager->contain(i_ptr);
	}

	bool IsAllocated(const HeapManager* i_pManager, void* i_ptr) {
		return i_pManager->contain(i_ptr);
	}

	size_t GetLargestFreeBlock(const HeapManager* i_pManager) {
		return i_pManager->getLargestFreeBlock();
	}

	size_t GetTotalFreeMemory(const HeapManager* i_pManager) {
		return i_pManager->getTotalFreeMemory();
	}

	void ShowFreeBlocks(const HeapManager* i_pManager) {
		i_pManager->showFreeBlocks();
	}

	void ShowOutstandingAllocations(const HeapManager* i_pManager) {
		i_pManager->showOutstandingAllocations();
	}
}