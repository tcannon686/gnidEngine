

struct list_element;
typedef struct list_element list_element_t;

typedef union
{
	void *data_ptr;
	long data_long;
	int data_int;
	short data_short;
	char data_char;
	double data_double;
	float data_float;
} list_data_t;

typedef struct list_element
{
	list_data_t data;
	list_element_t *next_ptr;
	list_element_t *last_ptr;
} list_element_t;

typedef struct list_iterator
{
	list_element_t *element;
	list_element_t *next;
} list_iterator_t;

typedef struct
{
	list_element_t *base_ptr;
	list_element_t *top_ptr;
	int count;
	void (*free)(void *);
	int (*cmp)(list_data_t, list_data_t);
} list_t;

/*
 * Creates a new list. free is a function pointer that frees element data, or
 * NULL if the data should not be freed. cmp is a function for comparing data,
 * or NULL if not needed.
 */
list_t *ListNew(void (*free)(void *), int (*cmp)(list_data_t, list_data_t));

/*
 * Iterates through the list and frees each element and its data.
 */
void ListFree(list_t *list_ptr);

void ListAppend(list_t *list_ptr, list_data_t data);

/* Helper macro for appending to a list. */
#define ListAppendType(type, list_ptr, data) { list_data_t _d; _d.type = (data); ListAppend((list_ptr), _d); }
#define ListAppendPointer(list_ptr, data) ListAppendType(data_ptr, (list_ptr), (data))
#define ListAppendLong(list_ptr, data) ListAppendType(data_long, (list_ptr), data))
#define ListAppendInt(list_ptr, data) ListAppendType(data_int, (list_ptr), (data))
#define ListAppendShort(list_ptr, data) ListAppendType(data_short, (list_ptr), (data))
#define ListAppendChar(list_ptr, data) ListAppendType(data_char, (list_ptr), (data))
#define ListAppendDouble(list_ptr, data) ListAppendType(data_double, (list_ptr), (data))
#define ListAppendFloat(list_ptr, data) ListAppendType(data_float, (list_ptr), (data))
list_data_t ListGet(list_t *list_ptr, int index);
list_data_t ListGetFirst(list_t *list_ptr);
list_data_t ListGetLast(list_t *list_ptr);
/*
 * Removes an element, and frees its data if should_free is true.
 */
void ListRemove(list_t *list_ptr, int index, int should_free);
void ListRemoveFirst(list_t *list_ptr, int should_free);
void ListRemoveLast(list_t *list_ptr, int should_free);
void ListRemoveValue(list_t *list_ptr, list_data_t data, int should_free);
int ListIndexOf(list_t *list_ptr, list_data_t data);
void ListRemoveIterator(list_t *list_ptr, list_iterator_t *iterator, int should_free);

void ListNext(list_iterator_t *iterator);

#define ListSize(list_ptr)				(list_ptr)->count
#define ListIsEmpty(list_ptr)			((list_ptr)->count == 0)

list_iterator_t ListIterator(list_t *list_ptr);
#define ListIsEnd(iterator_ptr)	((iterator_ptr)->element == NULL)
#define ListIteratorGet(iterator_ptr)	((iterator_ptr)->element->data)

int ListCmpPtr(list_data_t a, list_data_t b);
int ListCmpLong(list_data_t a, list_data_t b);
int ListCmpInt(list_data_t a, list_data_t b);
int ListCmpShort(list_data_t a, list_data_t b);
int ListCmpChar(list_data_t a, list_data_t b);
int ListCmpDouble(list_data_t a, list_data_t b);
int ListCmpFloat(list_data_t a, list_data_t b);

