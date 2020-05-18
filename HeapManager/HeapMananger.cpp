#include "HeapManager.h"
#include <Windows.h>
//debug
#include <assert.h>
#include <iostream>

HeapManager* HeapManager::my_manager = nullptr;

struct Descriptor
{
	void* MemoryBlockAdd;
	size_t BlockSize;

	Descriptor* previous;
	Descriptor* next;
};

HeapManager::HeapManager()
{
	size_t sizeHeap = 1024 * 1024;
#ifdef USE_HEAP_ALLOC
	void* pHeapMemory = HeapAlloc(GetProcessHeap(), 0, sizeHeap);
#else
	// Get SYSTEM_INFO, which includes the memory page size
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	// round our size to a multiple of memory page size
	assert(SysInfo.dwPageSize > 0);
	size_t sizeHeapInPageMultiples = static_cast<size_t>(SysInfo.dwPageSize) * ((1024 * 1024 + SysInfo.dwPageSize) / SysInfo.dwPageSize);
	void* pHeapMemory = VirtualAlloc(NULL, sizeHeapInPageMultiples, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#endif
	start = pHeapMemory;
	size = sizeHeap;

	//initialize descriptor
	//FreeMemoryList = static_cast<Descriptor*>(pHeapMemory);
	FreeMemoryList = new (start)Descriptor();
	FreeMemoryList->MemoryBlockAdd = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pHeapMemory) + sizeof(Descriptor));
	FreeMemoryList->BlockSize = sizeHeap - sizeof(Descriptor);
	FreeMemoryList->previous = nullptr;
	FreeMemoryList->next = nullptr;

	UsedMemoryList = nullptr;
}

HeapManager::HeapManager(void* i_pMemory, size_t i_sizeMemory, unsigned int i_numDescriptors) {
	start = i_pMemory;
	size = i_sizeMemory;

	//FreeMemoryList = static_cast<Descriptor*>(i_pMemory);
	FreeMemoryList = new (i_pMemory)Descriptor();
	FreeMemoryList->MemoryBlockAdd = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(i_pMemory) + sizeof(Descriptor));
	FreeMemoryList->BlockSize = i_sizeMemory - sizeof(Descriptor);
	FreeMemoryList->previous = nullptr;
	FreeMemoryList->next = nullptr;

	UsedMemoryList = nullptr;
}

HeapManager::~HeapManager()
{
	if (start)
	{
#ifdef USE_HEAP_ALLOC
		HeapFree(GetProcessHeap(), 0, pHeapMemory);
#else
		VirtualFree(start, 0, MEM_RELEASE);
#endif
	}
}

HeapManager* HeapManager::GetInstance() {
	if (!my_manager) {
		my_manager = new HeapManager();
	}
	return my_manager;
}

void HeapManager::FreeInstance() {
	delete my_manager;
	my_manager = nullptr;
}


void* HeapManager::alloc(size_t i_size) {
	Descriptor* result = nullptr;
	Descriptor* curDescriptor = FreeMemoryList;
	while (curDescriptor) {
		if (curDescriptor->BlockSize >= i_size + padding + sizeof(Descriptor)) {
			result = curDescriptor;
			break;
		}
		curDescriptor = curDescriptor->next;
	}
	if (result) {
		if (result->BlockSize - i_size - padding - sizeof(Descriptor) > sizeof(Descriptor) + padding) {
			//separate
			uintptr_t userStart = reinterpret_cast<uintptr_t>(result->MemoryBlockAdd) + result->BlockSize - i_size - padding / 2;
			//Descriptor* newDes = reinterpret_cast<Descriptor*>(userStart - padding / 2 - sizeof(Descriptor));
			Descriptor* newDes = new (reinterpret_cast<Descriptor*>(userStart - padding / 2 - sizeof(Descriptor)))Descriptor();
			newDes->MemoryBlockAdd = reinterpret_cast<void*>(userStart - padding / 2);
			newDes->BlockSize = i_size + padding;
			newDes->next = nullptr;
			newDes->previous = nullptr;

			result->BlockSize = result->BlockSize - newDes->BlockSize - sizeof(Descriptor);

			if (UsedMemoryList) {
				UsedMemoryList->previous = newDes;
				newDes->previous = nullptr;
				newDes->next = UsedMemoryList;
			}
			UsedMemoryList = newDes;
			result = newDes;
		}
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(result->MemoryBlockAdd) + padding / 2);
	}
	return nullptr;
}

void* HeapManager::alloc(size_t i_size, unsigned int i_alignment) {
	size_t newSize = i_alignment - i_size % i_alignment + i_size;
	unsigned int mask = i_alignment - 1;

	Descriptor* result = nullptr;
	unsigned int adjustment = 0;
	Descriptor* curDescriptor = FreeMemoryList;
	while (curDescriptor) {
		//calculate adjustment
		adjustment = (reinterpret_cast<uintptr_t>(curDescriptor->MemoryBlockAdd) + curDescriptor->BlockSize - padding / 2 - newSize) & mask;
		if (curDescriptor->BlockSize > newSize + padding + adjustment + sizeof(Descriptor)) {
			result = curDescriptor;
			break;
		}
		curDescriptor = curDescriptor->next;
	}
	if (result) {
		if (result->BlockSize - newSize - adjustment - sizeof(Descriptor) - padding > sizeof(Descriptor) + padding) {
			//separate
			uintptr_t userStart = reinterpret_cast<uintptr_t>(result->MemoryBlockAdd) + result->BlockSize - padding / 2 - newSize - adjustment;
			//Descriptor* newDes = reinterpret_cast<Descriptor*>(userStart - padding / 2 - sizeof(Descriptor));
			Descriptor* newDes = new (reinterpret_cast<Descriptor*>(userStart - padding / 2 - sizeof(Descriptor)))Descriptor();
			newDes->MemoryBlockAdd = reinterpret_cast<void*>(userStart - padding / 2);
			newDes->BlockSize = newSize + padding + adjustment;
			newDes->next = nullptr;
			newDes->previous = nullptr;

			result->BlockSize = result->BlockSize - newDes->BlockSize - sizeof(Descriptor);

			if (UsedMemoryList) {
				UsedMemoryList->previous = newDes;
				newDes->previous = nullptr;
				newDes->next = UsedMemoryList;

			}
			UsedMemoryList = newDes;
			result = newDes;
			return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(result->MemoryBlockAdd) + padding / 2);
		}
		else {
			//return the whole block or not
			if (((reinterpret_cast<uintptr_t>(result->MemoryBlockAdd) + padding / 2) & mask) == 0){
				Descriptor* previous = result->previous;
				Descriptor* next = result->next;
				if (previous) {
					previous->next = next;
				}
				else {
					FreeMemoryList = next;
				}
				if (next) {
					next->previous = previous;
				}
				if (UsedMemoryList) {
					UsedMemoryList->previous = result;
					result->previous = nullptr;
					result->next = UsedMemoryList;
				}
				UsedMemoryList = result;
				return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(result->MemoryBlockAdd) + padding / 2);
			}
			return nullptr;
		}
		
	}
	return nullptr;
}

bool HeapManager::freeMem(void* i_ptr) {
	//TODO pointer check
	Descriptor* cur_des = reinterpret_cast<Descriptor*>(reinterpret_cast<uintptr_t>(i_ptr) - padding / 2 - sizeof(Descriptor));
	Descriptor* previous = cur_des->previous;
	Descriptor* next = cur_des->next;
	if (previous) {
		previous->next = next;
	}
	else {
		UsedMemoryList = next;
	}
	if (next) {
		next->previous = previous;
	}

	
	Descriptor* curDescriptor = FreeMemoryList;
	Descriptor* preDescriptor = curDescriptor->previous;
	while (curDescriptor) {
		if (curDescriptor < cur_des) {
			preDescriptor = curDescriptor;
			curDescriptor = curDescriptor->next;
		}
		else {
			break;
		}
	}
	cur_des->next = curDescriptor;
	cur_des->previous = preDescriptor;
	if (preDescriptor) {
		preDescriptor->next = cur_des;
	}
	else {
		FreeMemoryList = cur_des;
	}

	if (curDescriptor) {
		curDescriptor->previous = cur_des;
	}

	return true;
}

void HeapManager::collectMem() {
	Descriptor* curDescriptor = FreeMemoryList;
	Descriptor* nextDescriptor = curDescriptor->next;
	while (curDescriptor&&nextDescriptor) {
		if (reinterpret_cast<Descriptor*>(reinterpret_cast<uintptr_t>(curDescriptor->MemoryBlockAdd) + curDescriptor->BlockSize) == nextDescriptor) {
			curDescriptor->BlockSize += nextDescriptor->BlockSize + sizeof(Descriptor);
			curDescriptor->next = nextDescriptor->next;
			if (nextDescriptor->next) {
				nextDescriptor->next->previous = curDescriptor;
			}
			nextDescriptor = nextDescriptor->next;
		}
		else {
			curDescriptor = nextDescriptor;
			nextDescriptor = curDescriptor->next;
		}
	}
}

bool HeapManager::isAllocated(void* ptr) const {
	Descriptor* curDescriptor = UsedMemoryList;
	while (curDescriptor) {
		if (reinterpret_cast<uintptr_t>(curDescriptor->MemoryBlockAdd) + padding / 2 == reinterpret_cast<uintptr_t>(ptr)) {
			return true;
		}
		curDescriptor = curDescriptor->next;
	}
	return false;
}

size_t HeapManager::getLargestFreeBlock() const {
	Descriptor* curDescriptor = FreeMemoryList;
	size_t max = 0;
	while (curDescriptor) {
		if (curDescriptor->BlockSize>max) {
			max = curDescriptor->BlockSize;
		}
		curDescriptor = curDescriptor->next;
	}
	return max;
}

size_t HeapManager::getTotalFreeMemory()  const {
	Descriptor* curDescriptor = FreeMemoryList;
	size_t total = 0;
	while (curDescriptor) { 
		total += curDescriptor->BlockSize;
		curDescriptor = curDescriptor->next;
	}
	return total;
}

void HeapManager::showFreeBlocks()  const {
	Descriptor* curDescriptor = FreeMemoryList;
	std::cout << "Free blocks:"<<std::endl;
	while (curDescriptor) {
		std::cout << curDescriptor << ":" << curDescriptor->BlockSize <<","<< curDescriptor->MemoryBlockAdd<< std::endl;
		curDescriptor = curDescriptor->next;
	}
	std::cout << std::endl;
}

void HeapManager::showOutstandingAllocations()  const {
	Descriptor* curDescriptor = UsedMemoryList;
	std::cout << "Outstanding blocks:" << std::endl;
	while (curDescriptor) {
		std::cout << curDescriptor << ":" << curDescriptor->BlockSize << "," << curDescriptor->MemoryBlockAdd<< std::endl;
		curDescriptor = curDescriptor->next;
	}
	std::cout << std::endl;
}