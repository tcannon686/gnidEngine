#include <stdlib.h>
#include <assert.h>

#include "linkedlist.h"



list_t *ListNew(void (*free)(void *), int (*cmp)(list_data_t, list_data_t))
{
	list_t *ret = (list_t *)malloc(sizeof(list_t));
	ret->base_ptr = NULL;
	ret->top_ptr = NULL;
	ret->count = 0;
	ret->free = free;
	ret->cmp = cmp;
	return ret;
}

void ListFree(list_t *list)
{
	if(list == NULL)
		return;
	list_element_t *current = list->base_ptr;
	list_element_t *next_ptr;
	while(current != NULL)
	{
		next_ptr = current->next_ptr;
		if(list->free != NULL)
			list->free(current->data.data_ptr);
		free(current);
		current = next_ptr;
	}
	free(list);
}

void ListAppend(list_t *list_ptr, list_data_t data)
{
	list_element_t *element = (list_element_t *)malloc(sizeof(list_element_t));
	element->data = data;
	element->next_ptr = NULL;
	element->last_ptr = NULL;
	
	list_ptr->count ++;
	
	if(list_ptr->base_ptr == NULL)
	{
		list_ptr->base_ptr = element;
		list_ptr->top_ptr = element;
	}
	else
	{
		list_ptr->top_ptr->next_ptr = element;
		element->last_ptr = list_ptr->top_ptr;
		list_ptr->top_ptr = element;
	}
}

list_data_t ListGet(list_t *list_ptr, int index)
{
	assert(index < list_ptr->count);
	list_element_t *element = list_ptr->base_ptr;
	for(int i = 0; i < index; i ++)
	{
		element = element->next_ptr;
	}
	return element->data;
}

list_data_t ListGetLast(list_t *list_ptr)
{
	assert(list_ptr->count > 0);
	return list_ptr->top_ptr->data;
}

list_data_t ListGetFirst(list_t *list_ptr)
{
	assert(list_ptr->count > 0);
	return list_ptr->base_ptr->data;
}

void ListRemove(list_t *list_ptr, int index, int should_free)
{
	assert(index < list_ptr->count);
	list_ptr->count --;
	list_element_t *element = list_ptr->base_ptr;
	for(int i = 0; i < index; i ++)
	{
		element = element->next_ptr;
	}
	
	if(element->next_ptr != NULL)
		element->next_ptr->last_ptr = element->last_ptr;
	if(element->last_ptr != NULL)
		element->last_ptr->next_ptr = element->next_ptr;
	else
	{
		list_ptr->base_ptr = element->next_ptr;
	}
	
	if(should_free)
		list_ptr->free(element->data.data_ptr);
	
	free(element);
}

void ListRemoveValue(list_t *list_ptr, list_data_t value, int should_free)
{
	list_ptr->count --;
	list_element_t *element = list_ptr->base_ptr;
    assert(list_ptr->cmp != NULL);
	while(1)
	{
		if(list_ptr->cmp(element->data, value) == 0)
			break;
		element = element->next_ptr;
	}

	if(element == NULL)
		return;
	
	if(element->next_ptr != NULL)
		element->next_ptr->last_ptr = element->last_ptr;
	if(element->last_ptr != NULL)
		element->last_ptr->next_ptr = element->next_ptr;
	else
	{
		list_ptr->base_ptr = element->next_ptr;
	}
	
	if(should_free)
		list_ptr->free(element->data.data_ptr);
	
	free(element);
}

void ListRemoveIterator(list_t *list_ptr, list_iterator_t *iterator, int should_free)
{
	list_element_t *element = iterator->element;
	if(element->next_ptr != NULL)
		element->next_ptr->last_ptr = element->last_ptr;
	if(element->last_ptr != NULL)
		element->last_ptr->next_ptr = element->next_ptr;
	else
	{
		list_ptr->base_ptr = element->next_ptr;
	}

	free(element);
}

void ListRemoveFirst(list_t *list_ptr, int should_free)
{
	assert(list_ptr->count > 0);
	list_ptr->count --;
	
	list_ptr->base_ptr->next_ptr->last_ptr = NULL;
	if(should_free)
		list_ptr->free(list_ptr->base_ptr->data.data_ptr);
	
	list_element_t *old_ptr = list_ptr->base_ptr;
	list_ptr->base_ptr = list_ptr->base_ptr->next_ptr;
	free(old_ptr);
}


void ListRemoveLast(list_t *list_ptr, int should_free)
{
	assert(list_ptr->count > 0);
	list_ptr->count --;
	
	list_ptr->top_ptr->last_ptr->next_ptr = NULL;
	if(should_free)
		list_ptr->free(list_ptr->top_ptr->data.data_ptr);
	free(list_ptr->top_ptr);
}

int ListIndexOf(list_t *list_ptr, list_data_t data)
{
	assert(list_ptr->cmp != NULL);
	
	list_element_t *element = list_ptr->base_ptr;
	int i = 0;
	while(element != NULL)
	{
		if(list_ptr->cmp(element->data, data) == 0)
			return i;
		i ++;
	}
	return -1;
}

list_iterator_t ListIterator(list_t *list_ptr)
{
	list_iterator_t ret;
	ret.element = list_ptr->base_ptr;
	if(list_ptr->base_ptr != NULL)
		ret.next = ret.element->next_ptr;
	else
		ret.next = NULL;
	
	return ret;
}

void ListNext(list_iterator_t *iterator)
{
	iterator->element = iterator->next;
	if(iterator->element != NULL)
	{
		iterator->next = iterator->element->next_ptr;
	}
}

int ListCmpPtr(list_data_t a, list_data_t b)
{
    return a.data_ptr > b.data_ptr ? 1
        : (a.data_ptr == b.data_ptr ? 0 : -1);
}
int ListCmpLong(list_data_t a, list_data_t b)
{
    return a.data_long > b.data_long ? 1
        : (a.data_long == b.data_long ? 0 : -1);
}
int ListCmpInt(list_data_t a, list_data_t b)
{
    return a.data_int > b.data_int ? 1
        : (a.data_int == b.data_int ? 0 : -1);
}
int ListCmpShort(list_data_t a, list_data_t b)
{
    return a.data_short > b.data_short ? 1
        : (a.data_short == b.data_short ? 0 : -1);
}
int ListCmpChar(list_data_t a, list_data_t b)
{
    return a.data_char > b.data_char ? 1
        : (a.data_char == b.data_char ? 0 : -1);
}
int ListCmpDouble(list_data_t a, list_data_t b)
{
    return a.data_double > b.data_double ? 1
        : (a.data_double == b.data_double ? 0 : -1);
}
int ListCmpFloat(list_data_t a, list_data_t b)
{
    return a.data_float > b.data_float ? 1
        : (a.data_float == b.data_float ? 0 : -1);
}

