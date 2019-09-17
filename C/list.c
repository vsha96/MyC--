#include <stdio.h>
#include <stdlib.h>

struct list {
	int elem;
	struct list *next;
   };

void listShow(const struct list *first) 
{
   struct list *temp;
   temp = first;
   if (!temp)
   {
	while (!temp->next)
	{
	   printf("%i ->",&temp->elem);
	}
   }
}

void listInput(struct list *first) 
{
   struct list *temp;
   int size,c,i;
   printf("Write size of your list\n");
   scanf("%i",&size);
   printf("Input sequence of numbers\n");
   if (!size)
   {
	scanf("%i",&c);
	first = malloc(sizeof(*first));
	first->elem = c;
	first->next = NULL;
	temp = first;
   }
   for(i=2;i<=size;i++)
	{
	   scanf("%i",&c);
	   temp->next = malloc(sizeof(*first));
	   temp = temp->next;
	   temp->elem = c;
	   temp->next = NULL;
	}
   printf("List is generated\n");
}

int main() 
{
   struct list *l1;
   listInput(l1);
   listShow(l1);
}