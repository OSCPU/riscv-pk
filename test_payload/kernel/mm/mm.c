#include <os/mm.h>
#include <os/list.h>

ptr_t memCurr = FREEMEM;

static LIST_HEAD(freePageList);

static ptr_t realAllocPage(int numPage)
{
    // align PAGE_SIZE
    ptr_t ret = ROUND(memCurr, PAGE_SIZE);
    memCurr = ret + numPage * PAGE_SIZE;
    return ret;
}

ptr_t allocPage(int numPage)
{
    if (numPage == 1 && !list_empty(&freePageList)) {
        ptr_t ret = (ptr_t)freePageList.next;
        list_del(freePageList.next);
        return ret;
    }
    return realAllocPage(numPage);
}

void freePage(ptr_t baseAddr, int numPage)
{
    for (int i = 0; i < numPage; ++i) {
        list_add_tail((list_node_t*)(baseAddr + i * PAGE_SIZE),
                      &freePageList);
    }
}

void* kmalloc(size_t size)
{
    ptr_t ret = ROUND(memCurr, 4);
    memCurr = ret + size;
    return (void*)ret;
}
