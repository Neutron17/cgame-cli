#ifndef _NTR_ALL_ARR_H_
#include "arr.h"
#include <stdio.h>
#include <stdarg.h>

typedef struct {
	void **arr;
} Arr;

unsigned used;
unsigned max;
const extern bool isDebug;

Arr arr;
void *alloc(size_t size) {
	void *x = malloc(size);
	if(x != NULL) {
		allocArr_add(x);
	}
	return x;
}
void *acalloc(size_t __nmemb, size_t __size) {
	void *x = calloc(__nmemb, __size);
	if(x != NULL) {
		allocArr_add(x);
	}
	return x;
}

#include "config.h"
#include "exitCodes.h"
#include "error.h"

__attribute__((used)) void __allocArr_ctor() {
	if(isDebug)
		printf("Ctor\n");
	max = ARR_SIZE;
	arr.arr = (void **)malloc(ARR_SIZE);
	//printf("%p\n", arr->arr);
	if(arr.arr == NULL) {
		ERR("Couldn't allocate\n");
		exit(EX_ALLOC_ERR);
	}
	used = 0;
}
extern bool beforeAlloc;
__attribute__((used)) void __allocArr_dtor() {
	if(isDebug)
		printf("dtor\n");
	if(beforeAlloc)
		return;
	for(int i = 0; i < used; i++) {
		if(isDebug)
			printf("%p\n", arr.arr[i]);
		if(isDebug) {
			printf("Freeing %d. element\n",
				   i+1);
		}
		free(arr.arr[i]);
		arr.arr[i] = NULL;
	}
	free(arr.arr);
	arr.arr = NULL;
	for(int i = 0; i<MOV_SZ;i++) {
		printf("%d\n", i);
		free(movs[i].names);
		free(movs[i].chnames);
	}
	if(isDebug)
		printf("Successfuly freed all\n");
}
enum ArrErrors allocArr_add(void *n) {
	if(used == max)
		return MAX_ITEMS_REACHED;
	//	printf("%d\n", *((int*)n));
	//	printf("%p\n", arr.arr);

	arr.arr[used] = n;
	if(isDebug)
		printf("Added %d. element %p\n", used + 1, n);
	used++;
	return ARR_SUCCESS;
}
// TODO doesn't work
enum ArrErrors allocArr_adds(unsigned sz, void *n, ...) {
	va_list l;
	va_start(l, n);
	char last = ARR_SUCCESS;
	for(int i = 0; i < sz; i++) {
		if((last = allocArr_add(va_arg(l, void *))) != ARR_SUCCESS)
		return last;
	}
	va_end(l);
	return ARR_SUCCESS;
}
enum ArrErrors __allocArr_pop2(const int n) {
	int last = ARR_SUCCESS, count = 0;
	while((last = __allocArr_pop1()) == ARR_SUCCESS && count < n)
		count++;
	return last;
}
enum ArrErrors __allocArr_pop1() {
	free(arr.arr[used - 1]);
	arr.arr[used - 1] = NULL;
	used--;
	return ARR_SUCCESS;
}
bool allocArr_in(const void *n) {
	for(int i = 0; i < used; i++) {
		if(arr.arr[i] == n)
			return true;
	}
	return false;
}
// TODO: implement with vararg
bool allocArr_ins(const void *n, ...) {
	for(int i = 0; i < used; i++) {
		if(arr.arr[i] == n)
			return true;
	}
	return false;
}
#endif
