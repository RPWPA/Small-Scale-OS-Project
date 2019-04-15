#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>
#define heap_frames 40959
#define heap_page_tables 39
//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)

int lastAloocatedAdd = 0;
int taken[heap_frames];

uint32 allocatedSize[heap_frames];

uint32 useFul= 0;

uint32 Nextfit(int a)
{

	int found = 0;
	int counter = 0;
	int add = lastAloocatedAdd;
	uint32 startvirAdd = (add*PAGE_SIZE) + KERNEL_HEAP_START;
	uint32 returnStart = 0;
	do {
		//Checking if the frame is empty
		if(taken[add] == 0)
		{
			counter++;
			if(returnStart == 0){
				returnStart = (add*PAGE_SIZE) + KERNEL_HEAP_START;;
			}
			if(counter == a)
			{
				found = 1;
				break;
			}
			add++;
		}
		else
		{
			add++;
			counter = 0;
			returnStart = 0;
		}
		if((add*PAGE_SIZE+KERNEL_HEAP_START) == KERNEL_HEAP_MAX)
		{
			add  = 0;
			counter = 0;
			returnStart = 0;
		}
	}
	while(!found && (add*PAGE_SIZE+KERNEL_HEAP_START) != startvirAdd);
	if(found == 1)
	{
		lastAloocatedAdd = add;
		return returnStart;
	}
	else {
		return 0;
	}
}
void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT 2018 - MS1 - [1] Kernel Heap] kmalloc()
	// Write your code here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");

	//NOTE: Allocation is based on NEXT FIT strategy
	//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
	//refer to the project presentation and documentation for details


	//TODO: [PROJECT 2018 - BONUS1] Implement the BEST FIT strategy for Kernel allocation
	// Beside the NEXT FIT
	// use "isKHeapPlacementStrategyBESTFIT() ..." functions to check the current strategy

	//change this "return" according to your answer
	//return 0;

	uint32 neededSize = ROUNDUP(size,PAGE_SIZE);
		int a = neededSize/PAGE_SIZE;

		uint32 ret = Nextfit(a);
		uint32 assign = ret;
		if (ret == 0)
		{
			return NULL;
		}
		else
		{
			for(int i = 0; i <a; i++)
			{
				struct Frame_Info* newFrame = NULL;
				int ret = allocate_frame(&newFrame);
				if (ret == E_NO_MEM)
				{
						return NULL;
				}
				else
				{
					int ret = map_frame(ptr_page_directory,newFrame,(void*)assign, PERM_WRITEABLE|PERM_PRESENT);
					if (ret == E_NO_MEM)
					{
							free_frame(newFrame);
							return NULL;
					}
					taken[(assign-KERNEL_HEAP_START)/PAGE_SIZE] = 1;
					assign+=PAGE_SIZE;
				}
			}
		}
		useFul = assign;
		 allocatedSize[(ret-KERNEL_HEAP_START)/PAGE_SIZE] = a;
		 return (void*)ret;

}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT 2018 - MS1 - [1] Kernel Heap] kfree()
	// Write your code here, remove the panic and write your code

	//panic("kfree() is not implemented yet...!!");
	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details

	uint32 a = (uint32) virtual_address;
	int size = allocatedSize[(a - KERNEL_HEAP_START)/PAGE_SIZE];
	allocatedSize[(a - KERNEL_HEAP_START)/PAGE_SIZE] = 0;
	int c = 0;

	for (int i = 0; i < size; i ++)
	{
		unmap_frame(ptr_page_directory,(void*)a);
		taken[(a-KERNEL_HEAP_START)/PAGE_SIZE] = 0;
		a= (a + PAGE_SIZE);
	}
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT 2018 - MS1 - [1] Kernel Heap] kheap_virtual_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");

	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer


	struct Frame_Info * PhysFrame = to_frame_info(physical_address);

	unsigned int va;
	for(va= KERNEL_HEAP_START;va<useFul;va+=PAGE_SIZE)
	{
		uint32* ptr_PT =NULL;

		struct Frame_Info * VirtFrame = get_frame_info(ptr_page_directory,(void*)va,&ptr_PT);

		if(VirtFrame == PhysFrame)
		{
			return va;
		}
	}


	return 0;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT 2018 - MS1 - [1] Kernel Heap] kheap_physical_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");
	//return 0;

	uint32 offset=(virtual_address& (0x00000FFF));
	uint32* ptr_PT =NULL;

	get_page_table(ptr_page_directory,(void*)virtual_address,&ptr_PT);
	if(ptr_PT !=NULL)
	{
		uint32 x = ptr_PT[PTX(virtual_address)];

		uint32 frame_no = (x>>12);

		uint32 physical_address =(frame_no*PAGE_SIZE)+offset;
		return (unsigned int)physical_address;
	}
	return 0;

}


//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().
void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2018 - BONUS2] Kernel Heap Realloc
	// Write your code here, remove the panic and write your code


	panic("krealloc() is not implemented yet...!!");
	return NULL;

}
