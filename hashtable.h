
typedef struct hashtable_entry hashtable_entry_t;

typedef union
{
    void *pointer;
    int integer;
    float number;
    long long_integer;
    double long_number;
} hashtable_value_t;

typedef struct hashtable_entry {
    char key[32];
    hashtable_value_t value;
    hashtable_entry_t *next_ptr;
} hashtable_entry_t;

typedef struct {
    hashtable_entry_t *entries;
    int capacity;
    int count;
    double max_load_factor;
} hashtable_t;

int HashTableHash(char *key);
hashtable_t *HashTableNew(int capacity, double max_load_factor);
hashtable_t *HashTableNewDefault();
hashtable_entry_t *HashTableGet(hashtable_t *table, char *key);
hashtable_entry_t *HashTableGetOrInsert(hashtable_t *table, char *key);
int HashTableRemove(hashtable_t *table, char *key);
void HashTableSet(hashtable_entry_t *entry, hashtable_value_t value);
hashtable_t *HashTableCopy(hashtable_t *table, int new_capacity);
void HashTableFree(hashtable_t *table);
void HashTableRehash(hashtable_t *table, int new_capacity);

/*
 * Returns the next item in the hash table. If *entry_ptr is NULL, returns
 * the first item.
 */
int HashTableNext(hashtable_t *table, hashtable_entry_t **entry_ptr);

