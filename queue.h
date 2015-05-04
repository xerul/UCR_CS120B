
/*	queue.h

	Header file for queue implementation

	by: Steven Skiena
	
	Modified by Rex Lu
*/

#define QUEUE_SIZE       1000

typedef struct {
	unsigned int q[QUEUE_SIZE+1];		        /* body of queue */
	unsigned int first;                      /* position of first element */
	unsigned int last;                       /* position of last element */
	unsigned int count;                      /* number of queue elements */
} queue;

