#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

int delay = 100*1000;
int height;

typedef struct {
	int* arr;
	int len;
} int_array;

void init_int_array(int_array* init, int size) {
	init->arr = calloc(size,sizeof(int));
	init->len = size;
}

void free_int_array(int_array* array) {
	free(array->arr);
}

void print_int_array(int_array array, int red, int green) {
	int lastColor = -1;
	for (int draw = height-1; draw >= 0; draw--) {
		for (int i = 0; i < array.len; i++) {
			if (draw*array.len < array.arr[i]*height) {
				if (i == red) {
					printf("\x1b[41m");
					lastColor = 2;
				} else if (i == green) {
					printf("\x1b[42m");
					lastColor = 2;
				} else if (lastColor != 1) {
					printf("\x1b[47m");
					lastColor = 1;
				}
			} else if (lastColor != 0) {
				printf("\x1b[49m");
				lastColor = 0;
			}
			printf(" ");
		}
		if (height == array.len) {
			printf("\x1b[m");
			if (height-draw-1 == red)
				printf("\x1b[31m");
			else if (height-draw-1 == green)
				printf("\x1b[32m");
			printf(" %d",array.arr[height - draw - 1]);
			lastColor = -1;
		}
		printf("\x1b[m\x1b[K\n");
	}
}

void printover_int_array(int_array array, int red, int green) {
	print_int_array(array,red, green);
	printf("\x1b[%dA",array.len);
	usleep(delay);
}

void swap_int_array(int_array array, int from, int to) {
	int temp = array.arr[from];
	array.arr[from] = array.arr[to];
	array.arr[to] = temp;

	printover_int_array(array,from,to);
}

bool lt_int_array(int_array array, int from, int to) {
	printover_int_array(array,to,from);
	return (array.arr[from] < array.arr[to]);
}

bool gt_int_array(int_array array, int from, int to) {
	printover_int_array(array,to,from);
	return (array.arr[from] > array.arr[to]);
}

void randomize_int_array(int_array array,int randomseed) {
	srand(randomseed);
	for (int i = 0; i < array.len; i++)
		array.arr[i] = rand() % array.len;
}

//O(n**2) sorts----------------------------------------------------------------
void insertion_sort(int_array array) {
	if (array.len < 2) return;
	for (int i = 0; i < array.len; i++)
		for (int j = i; j > 0 && lt_int_array(array, j, j-1); j--)
			swap_int_array(array,j-1,j);
}

void selection_sort(int_array array) {
	if (array.len < 2) return;
	for (int i = 0; i < array.len-1; i++) {
		int min = i;
		for (int j = i; j < array.len; j++) {
			if (lt_int_array(array,j,min)) //if *j < *min
				min = j;
		}
		swap_int_array(array,i,min);
	}
}

void bubble_sort(int_array array) {
	if (array.len < 2) return;
	for (int max = array.len; max > 1; max--) {
		for (int i = 0; i < max-1; i++)
			if (lt_int_array(array,i+1,i))
				swap_int_array(array,i+1,i);
	}
}

void cocktail_sort(int_array array) {
	if (array.len < 2) return;
	bool swap;
	int max = array.len-1,min = 0;
	do {
		swap = false;
		int i = min;
		for (; i < max; i++)
			if (lt_int_array(array,i+1,i)) {
				swap_int_array(array,i+1,i);
				swap = true;
			}
		if (!swap) break;
		max--;
		for (; i >= min; --i)
			if (gt_int_array(array,i,i+1)) {
				swap_int_array(array,i,i+1);
				swap = true;
			}
		i++,min++;
	} while (swap);
}

//Quicksort--------------------------------------------------------------------
int quick_sort_partition(int_array array, int min, int max) {
	swap_int_array(array,(min+max)/2,max); //put the middle element last as the pivot
	int pivot = max, lastmin = min;
	for (; pivot > min; pivot--) {
		swap_int_array(array,pivot-1,pivot);	//the pivot value (if this swap was in error) is pivot-1
		if (lt_int_array(array,pivot,pivot-1)) {
			for (;lt_int_array(array,lastmin,pivot-1); lastmin++);
			swap_int_array(array,lastmin,pivot);
			if (lastmin == pivot-1)
				return pivot;
		}
	}
	return pivot;
}

void quick_sort_backend(int_array array, int min, int max) {
	if (max <= min) return;
	int pivot = quick_sort_partition(array,min,max);
	quick_sort_backend(array,min,pivot-1);
	quick_sort_backend(array,pivot+1,max);
}

void quick_sort(int_array array) {
	quick_sort_backend(array,0,array.len-1);
}

//Heapsort---------------------------------------------------------------------
void heap_sift_up(int_array array, int siftIndex) {	//TODO de-recurse?
	int parentIndex = (siftIndex - 1) >> 1;
	if (parentIndex < 0) return;
	if (lt_int_array(array,parentIndex,siftIndex)) //if the array[parent] < array[sift]
		swap_int_array(array,parentIndex,siftIndex);
	heap_sift_up(array, parentIndex);	   //swap the parent and 
}

void heap_sift_down(int_array array, int siftIndex, int max) {
	int leftIndex = (siftIndex << 1) + 1;
	int rightIndex = leftIndex+1;
	if (leftIndex >= max) return;
	//make the rightIndex the "right index" (i.e which subtree to go down)
	if (rightIndex == max || lt_int_array(array,rightIndex,leftIndex))
		rightIndex = leftIndex;
	//if the top is more than the "bigger" subtree
	if (lt_int_array(array,siftIndex,rightIndex)) { 
		swap_int_array(array,siftIndex,rightIndex);
		heap_sift_down(array,rightIndex,max);
	}
}
void print_heap(int_array array, int max) {
	int printSize = 1;
	int start = 0;
	while (printSize <= max) {
		for (int i = start; (i < start+printSize) && (i < max); i++)
			printf("%d%c",array.arr[i],' ');
		printf("\n");
		start += printSize;
		printSize <<= 1;
	}
	(void)getchar();
}

void heap_sort(int_array array) {
	//build the heap
	for (int i = 1; i < array.len-1; i++)
		heap_sift_up(array,i);
	//swap the top of the heap with the 'back' of the array
	for (int max = array.len-1; max > 0; max--) {
		swap_int_array(array,0,max);
		heap_sift_down(array,0,max);
	}
}

int main(int argc, char** argv) {
	if (argc < 3 || argc > 5)
		return -1;
	height = atoi(argv[1]);
	if (argc > 3)
		delay = atoi(argv[3])*1000;
	int seed = time(NULL);
	if (argc > 4)
		seed = atoi(argv[4]);
	int_array tester;
	init_int_array(&tester,height);
	
	randomize_int_array(tester,seed);

	if (!strcmp(argv[2],"selection"))
		selection_sort(tester);
	else if (!strcmp(argv[2],"insertion"))
		insertion_sort(tester);
	else if (!strcmp(argv[2],"bubble"))
		bubble_sort(tester);
	else if (!strcmp(argv[2],"cocktail"))
		cocktail_sort(tester);
	else if (!strcmp(argv[2],"quick"))
		quick_sort(tester);
	else if (!strcmp(argv[2],"heap"))
		heap_sort(tester);

	print_int_array(tester,-1,-1);

	free_int_array(&tester);
}
