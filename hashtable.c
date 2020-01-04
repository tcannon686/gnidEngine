
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hashtable.h"

int HashTableHash(char *key)
{
    if(key == NULL)
        return 0;
    
    int hash = 7;
    for (int i = 0; i < strlen(key); i++) {
        hash = hash * 31 + key[i];
    }
    return abs(hash);
}

hashtable_t *HashTableNew(int capacity, double max_load_factor)
{
    hashtable_t *table;
    table = (hashtable_t *) malloc(sizeof(hashtable_t));
    memset(table, 0, sizeof(hashtable_t));

    table->entries = (hashtable_entry_t *) malloc(sizeof(hashtable_entry_t) * capacity);
    memset(table->entries, 0, sizeof(hashtable_entry_t) * capacity);
    table->capacity = capacity;
    table->max_load_factor = max_load_factor;
    return table;
}

hashtable_t *HashTableNewDefault()
{
    return HashTableNew(32, 0.75);
}

hashtable_entry_t *HashTableGet(hashtable_t *table, char *key)
{
    hashtable_entry_t *entry_ptr = &table->entries[HashTableHash(key) % table->capacity];
    if(entry_ptr->key[0] == 0)
        return NULL;
    while(entry_ptr != NULL) {
        if(strcmp(key, entry_ptr->key) == 0)
            return entry_ptr;
        entry_ptr = entry_ptr->next_ptr;
    }
    return NULL;
}

hashtable_entry_t *HashTableGetOrInsert(hashtable_t *table, char *key)
{
    int index = HashTableHash(key) % table->capacity;
    hashtable_entry_t *entry_ptr = &table->entries[index];
    if(entry_ptr->key[0] == 0) {
        strcpy(entry_ptr->key, key);
        table->count ++;
        if((double) table->count / table->capacity > table->max_load_factor)
        {
            HashTableRehash(table, table->capacity * 2);
			return HashTableGet(table, key);
        }
        return entry_ptr;
    }
    while(entry_ptr != NULL)
    {
        if(strcmp(key, entry_ptr->key) == 0)
            return entry_ptr;
        if(entry_ptr->next_ptr == NULL) {
            entry_ptr->next_ptr = (hashtable_entry_t *) malloc(sizeof(hashtable_entry_t));
			memset(entry_ptr->next_ptr, 0, sizeof(hashtable_entry_t));
			strcpy(entry_ptr->next_ptr->key, key);
            table->count ++;
            if((double) table->count / table->capacity > table->max_load_factor)
            {
                HashTableRehash(table, table->capacity * 2);
				return HashTableGet(table, key);
            }
            return entry_ptr->next_ptr;
        }
        entry_ptr = entry_ptr->next_ptr;
    }
    return NULL;
}

void HashTableSet(hashtable_entry_t *entry, hashtable_value_t value)
{
    entry->value = value;
}

hashtable_t *HashTableCopy(hashtable_t *table, int new_capacity)
{
    hashtable_t *ret = HashTableNew(new_capacity, table->max_load_factor);
    for(int i = 0; i < table->capacity; i ++)
    {
        hashtable_entry_t *current = &table->entries[i];
        hashtable_entry_t *next_ptr;
        int is_first = 1;
        if(current->key[0] == 0)
            continue;
        
        while(current != NULL)
        {
            next_ptr = current->next_ptr;
            hashtable_entry_t *entry = HashTableGetOrInsert(ret, current->key);
            entry->value = current->value;
            if(!is_first)
                free(current);
            else
                is_first = 0;
            current = next_ptr;
        }
    }
    free(table);
    return ret;
}

void HashTableRehash(hashtable_t *table, int new_capacity)
{
    hashtable_entry_t *entries = malloc(sizeof(hashtable_entry_t) * new_capacity);
    memset(entries, 0, sizeof(hashtable_entry_t) * new_capacity);

    for(int i = 0; i < table->capacity; i ++)
    {
        hashtable_entry_t *current = &table->entries[i];
        hashtable_entry_t *next_ptr;
        int is_first = 1;
        if(current->key[0] == 0)
            continue;
        
        while(current != NULL)
        {
            int index = HashTableHash(current->key) % new_capacity;
            next_ptr = current->next_ptr;
            
            if(entries[index].key[0] == 0)
            {
                entries[index] = *current;
				entries[index].next_ptr = NULL;
                if(!is_first)
                    free(current);
            }
            else
            {
				hashtable_entry_t *next2;
                hashtable_entry_t *current2 = &entries[index];
				
                while(current2 != NULL)
                {
					next2 = current2->next_ptr;
                    if(current2->next_ptr == NULL)
                    {
                        if(is_first)
                        {
                            current2->next_ptr = (hashtable_entry_t *)malloc(sizeof(hashtable_entry_t));
                            memcpy(current2->next_ptr, current, sizeof(hashtable_entry_t));
							current2->next_ptr = NULL;
                        }
                        else
                        {
                            current2->next_ptr = current;
                            current->next_ptr = NULL;
                        }
                    }
					current2 = next2;
                }
            }

            is_first = 0;
            current = next_ptr;
        }
    }
    free(table->entries);
	table->capacity = new_capacity;
	table->entries = entries;
}

void HashTableFree(hashtable_t *table)
{
    for(int i = 0; i < table->capacity; i ++)
    {
        hashtable_entry_t *current = &table->entries[i];
        hashtable_entry_t *next_ptr;
        int is_first = 1;
        if(current->key[0] == 0)
            continue;
        
        while(current != NULL)
        {
            next_ptr = current->next_ptr;
            if(!is_first)
                free(current);
            else
                is_first = 0;
            current = next_ptr;
        }
    }
	free(table);
}

int HashTableNext(hashtable_t *table, hashtable_entry_t **entry_ptr)
{
    if(*entry_ptr == NULL)
        *entry_ptr = &table->entries[0];
    if((*entry_ptr)->next_ptr != NULL)
        *entry_ptr = (*entry_ptr)->next_ptr;

    int hash;
    for(hash = HashTableHash((*entry_ptr)->key) % table->capacity + 1;
        hash < table->capacity;
        hash ++)
    {
        if(table->entries[hash].key[0] != 0)
        {
            *entry_ptr = &table->entries[hash];
            return 1;
        }
    }
    return 0;
}

int HashTableRemove(hashtable_t *table, char *key)
{
    // Find the key.
    int found = 0;

    hashtable_entry_t *entry_ptr = &table->entries[
        HashTableHash(key) % table->capacity];
    hashtable_entry_t *prev_ptr = NULL;

    if(entry_ptr->key[0] == 0)
        return 0;

    while(entry_ptr != NULL) {
        if(strcmp(key, entry_ptr->key) == 0)
        {
            found = 1;
            break;
        }
        prev_ptr = entry_ptr;
        entry_ptr = entry_ptr->next_ptr;
    }

    if(!found)
        return 0;
    if(prev_ptr == NULL)
    {
        entry_ptr->key[0] = 0;
    }
    else
    {
        prev_ptr->next_ptr = entry_ptr->next_ptr;
        free(entry_ptr);
    }
    return 1;
}
