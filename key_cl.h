#include <stdint.h>

typedef struct{
	uint32_t key;
	uint32_t index;
	uint32_t type;
	uint32_t base;
} cl_key_char;

//Setup variables and tables
void cl_key_init();

//Reset a key to a new random valid sjis-key
void cl_key_reset(cl_key_char *key, int n);

//Increment a key to the next valid sjis-key (not used)
int cl_key_inc_single(cl_key_char *key, int n);
void cl_key_inc(cl_key_char *key);

//Make a copy of the sjis tables for use outside key_cl.c
void copy_sjis_tables(uint32_t *target);

//The two last characters of each tripcode is randomly generated
//in the gpu. If the last byte is of type 2 (first character of
//a 2-byte sjis character), we have to add a final byte.
int cl_key_get_last(int key1, int key2);

//Some ascii character causes certain tripcode implementation to fail
//for no real reason other than them being a bad implementation.
//This deals with this by replacing those characters with sjis characters
//that the DES algorithm sees as the same.
void cl_key_make_safe(uint32_t *key);