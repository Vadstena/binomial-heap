#include <stdio.h>
#include <stdlib.h>

typedef struct node* node;

struct node{
	int v;
	int rank;
	node child; /* highest ranked child*/
	node brother; /* In heap list points to larger rank. Inside a tree points to smaller rank. */
	node father;
};

node A;
int n;

int ptr2loc(node v, node A){
	int r;
	r = -1;
	if (NULL != v)
		r = ((size_t) v - (size_t) A) / sizeof(struct node);
	return (int) r;
}
void showNode(node v){
	if (NULL == v)
		printf("NULL\n");
	else{
		printf("node: %d ", ptr2loc(v, A));
		printf("v: %d ", v->v);
		printf("rank: %d ", v->rank);
		printf("child: %d ", ptr2loc(v->child, A));
		printf("brother: %d ", ptr2loc(v->brother, A));
		printf("father: %d ", ptr2loc(v->father, A));
		printf("\n");
	}
}
void showList(node n){
	showNode(n);
	if (n->brother == NULL)
		return;
	return showList(n->brother);
}
int argMin(node root){
	node next;
	int minRoot;
	int minV;
	if (root == NULL) {
		printf("\n - ERROR argMin -\n");
		return -1;
	}
	minV = root->v;
	minRoot = ptr2loc(root, A);
	next = root->brother;
	while (next != NULL){
		if (next->v < minV){
			minV = next->v;
			minRoot = ptr2loc(next, A);
		}
		next = next->brother;
	}
	return minRoot;
}
int min(node root){
	int minRoot;
	if (root == NULL) {
		printf("\n - ERROR min (1) -\n");
		return -1;
	}
	minRoot = argMin(root);
	if (minRoot == -1)
		printf("\n - ERROR min (2) -\n");
	return A[minRoot].v;
}
void link(node child, node father){
	child->father = father;
	child->brother = father->child;
	father->child = child;
	father->rank++;
	printf("link %d as child of %d\n", ptr2loc(child, A), ptr2loc(father, A));
}
node merge(node heap1, node heap2){
	node head, tail, aux;
	if (heap1 == NULL)
		return heap2;
	if (heap2 == NULL)
		return heap1;
	if (heap1->rank <= heap2->rank){
		head = heap1;
		tail = heap1;
		heap1 = heap1->brother;
	}
	else{
		head = heap2;
		tail = heap2;
		heap2 = heap2->brother;
	}
	tail->brother = NULL;
	while (!(heap2 == NULL && heap1 == NULL)){
		if (heap2 == NULL || (heap1 != NULL && heap1->rank <= heap2->rank)){
			aux = heap1->brother;
			tail->brother = heap1;
			tail = tail->brother;
			tail->brother = NULL;
			heap1 = aux;
		}
		else if (heap1 == NULL || (heap2 != NULL && heap2->rank < heap1->rank)){
			aux = heap2->brother;
			tail->brother = heap2;
			tail = tail->brother;
			tail->brother = NULL;
			heap2 = aux;
		}
	}
	return head;
}
int unite(node heap1, node heap2){
	node head, prev, x, next;
	if (heap1 == NULL && heap2 == NULL)
		return -1;
	head = merge(heap1, heap2);
	prev = NULL;
	x = head;
	next = x->brother;
	while (next != NULL){
		if ((x->rank != next->rank) || (next->brother != NULL && next->brother->rank == x->rank)){
			prev = x;
			x = next;
		}
		else{
			if (x->v <= next->v){
				x->brother = next->brother;
				link(next, x);
			}
			else{
				if (prev == NULL)
					head = next;
				else{
					prev->brother = next;
				}
				link(x, next);
				x = next;
			}
		}
		next = x->brother;
	}
	return ptr2loc(head, A);
}
int isSingleRoot(node root){ /*to be used on heap's head*/
	if (root->brother == NULL)
		return 1;
	return 0;
}
node reverseChildren(node highestRankChild){
	node head = highestRankChild, prev = NULL, next;
	while (head != NULL){
		head->father = NULL;
		next = head->brother;
		head->brother = prev;
		prev = head;
		head = next;
	}
	return prev;
}
void resetNode(node n){
	n->v = 0;
	n->rank = 0;
	n->child = NULL;
	n->brother = NULL;
	n->father = NULL;
}
node removeNode(node heap, int id){
	node next = heap;
	if (next == NULL){
		printf("\n - ERROR removeNode: heap empty -\n");
		return NULL;
	}
	if (ptr2loc(next, A) == id)
		return next->brother;
	while (ptr2loc(next->brother, A) != id){
		next = next->brother;
		if (next == NULL || next->brother == NULL){
			printf("\n - ERROR removeNode (1) -\n");
			return heap;
		}
	}
	next->brother = next->brother->brother;
	return heap;
}
int len(node heap){ /* NOT NEEDED */
	node next = heap;
	int len = 0;
	while(next != NULL){
		len++;
		next = next->brother;
	}
	return len;
}
node extractMin(node heap){
	int uniteHeadId, heapLen = len(heap), minId = argMin(heap); /*if minId is -1 it breaks function*/
	node min = &A[minId], newHeap = removeNode(heap, minId);
	node childRev = reverseChildren(min->child);
	resetNode(min);
	if (heapLen > 1 && childRev != NULL){
		uniteHeadId = unite(newHeap, childRev);
		return &A[uniteHeadId];
	}
	if (heapLen == 1 && childRev == NULL)
		return min;
	if (heapLen == 1)
		return childRev;
	if (childRev == NULL)
		return newHeap;
	return min; /* this ret should be useless */
}
/*node decreaseKey(node n, int newVal){
	if (newVal >= n->v)
		return n;
	if (n->father != NULL && n->father->v > newVal){
		n->v = n->father->v;
		return decreaseKey(n->father, newVal);
	}
	n->v = newVal;
	return n;
}*/
node decreaseKey (node n, int newVal){
	if (newVal >= n->v)
		return n;
	while (!(n->father == NULL || n->father->v <= newVal)){
		n->v = n->father->v;
		n = n->father;
	}
	n->v = newVal;
	return n;
}
node delete(node heap, node n){
	int minVal = min(heap);
	decreaseKey(n, minVal-1);
	return extractMin(heap);
}
void set(node n, int v){
	if (n->child == NULL && n->brother == NULL && n->father == NULL)
		n->v = v;
	else{
		printf("\n - ERROR set: node is not a heap - \n");
	}
}

int main(){ /*int argc, const char* argv[]*/
	int arg1, arg2, i;
	node retVal;
	char op;
	
	scanf("%d", &n);
	A = (node) calloc(n, sizeof(struct node));

	while(1){
		scanf(" %c", &op);

		if (op == 'S'){
			scanf(" %d", &arg1);
			showNode(&A[arg1]);
		}
		else if (op == 'P'){
			scanf(" %d", &arg1);
			showList(&A[arg1]);
		}
		else if (op == 'U'){
			scanf(" %d %d", &arg1, &arg2);
			printf("%d\n", unite(&A[arg1], &A[arg2]));
		}
		else if (op == 'M'){
			scanf(" %d", &arg1);
			printf("%d\n", min(&A[arg1]));
		}
		else if (op == 'A'){
			scanf(" %d", &arg1);
			printf("%d\n", argMin(&A[arg1]));
		}
		else if (op == 'E'){
			scanf(" %d", &arg1);
			retVal = extractMin(&A[arg1]);
			printf("%d\n", ptr2loc(retVal, A));
		}
		else if (op == 'R'){
			scanf(" %d %d", &arg1, &arg2);
			retVal = decreaseKey(&A[arg1], arg2);
			printf("%d\n", ptr2loc(retVal, A));
		}
		else if (op == 'D'){
			scanf(" %d %d", &arg1, &arg2);
			retVal = delete(&A[arg1], &A[arg2]);
			printf("%d\n", ptr2loc(retVal, A));
		}
		else if (op == 'V'){
			scanf(" %d %d", &arg1, &arg2);
			set(&A[arg1], arg2);
		}
		else if (op == 'X'){
			break;
		}
		else{
			printf("\nInvalid command\n");
		}
	}

	printf("Final configuration:\n");
	for (i = 0; i < n; i++)
		showNode(&A[i]);

	free(A);
	return 0;
}