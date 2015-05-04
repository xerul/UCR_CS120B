
/*	queue.c

	Implementation of a FIFO queue abstract data type.

	by: Steven Skiena
	begun: March 27, 2002
	
	Modified by Rex Lu
*/


#include "queue.h"
	
void initialize_queue(queue *q)
{
	q->first = 0;
	q->last = QUEUE_SIZE-1;
	q->count = 0;
}

void enqueue(queue *q, int x)
{
	if (q->count >= QUEUE_SIZE)
		return;
	else {
		q->last = ((q->last)+1) % QUEUE_SIZE;
		q->q[ q->last ] = x;
		q->count = q->count + 1;
	}
}

int dequeue(queue *q)
{
	int x;

	if (q->count <= 0) printf("Warning: empty queue dequeue.\n");
	else {
		x = q->q[ q->first ];
		q->first = (q->first+1) % QUEUESIZE;
		q->count = q->count - 1;
	}

	return(x);
}

int empty(queue *q)
{
	if (q->count <= 0) return (TRUE);
	else return (FALSE);
}

print_queue(queue *q)
{
	int i,j;

	i=q->first;
	
	while (i != q->last) {
		printf("%c ",q->q[i]);
		i = (i+1) % QUEUESIZE;
	}

	printf("%2d ",q->q[i]);
	printf("\n");
}

