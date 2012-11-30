#include "key_cl.h"
#include "cl_util.h"
#include <assert.h>
#include <stdlib.h>

int *chars;
int *existence;
int *base_chars;
int *type;
int base_count;
int *count;

#define P1 ((uint64_t)16807)
#define P2 ((uint64_t)0)
#define N ((uint64_t)2147483647)
uint32_t state;
inline uint64_t next_int(uint64_t x){
	return (P1*x+P2)%N;
}

void cl_key_init(uint32_t seed){
	state = seed;
	chars = (int *)calloc(256*256 + 256*256 + 256 + 256 + 256, sizeof(int));
	existence = chars+256*256;
	type = existence+256*256;
	count = type+256;
	base_chars = count+256;
	base_count = 0;
	
	size_t size;
	uint8_t *sjis_dat = readFile("sjis.dat", &size);
	uint8_t *temp = sjis_dat;
	
	int i;
	for(i = 0; i < 256; i++){
		type[i] = *sjis_dat++;
		if(type[i]){
			base_chars[base_count++] = i;
		}
	}
	
	for(i = 0; i < 256; i++){
		int j;
		while(j = *sjis_dat++){
			existence[256*i+j] = 1;
			chars[256*i+count[i]++] = j;
		}
	}
	
	free(temp);
}

void cl_key_reset(cl_key_char *key, int n){
	if(n==8) return;
	key[n].index = state = next_int(state);
	if(n==0){
		key[n].index %= base_count;
		key[n].key = base_chars[key[n].index];
		key[n].type = type[key[n].key];
	}
	else{

		if(key[n-1].type==1){
			key[n].index %= base_count;
			key[n].key = base_chars[key[n].index];
			key[n].type = type[key[n].key];
		}
		else{

			key[n].index %= count[key[n-1].key];
			key[n].key = chars[key[n-1].key*256 + key[n].index];
			key[n].type = 1;
		}
	}
	
	key[n].base = key[n].key;
	
	cl_key_reset(key, n+1);
}

int cl_key_inc_single(cl_key_char *key, int n){
	assert(n>0);
	assert(n<8);
	int type_ = key[n-1].type;
	key[n].index++;
	
	//This is not the second byte of a Shift-jis two byte character
	if(type_ < 2){
		
		//This is, numerically, the largest possible byte for this type
		//So we can't increment it
		if(key[n].index == base_count){
			return 0;
		}
		assert(key[n].index<base_count);
		key[n].key = base_chars[key[n].index];
		//Make sure we do not re-use a key that is identical in the eye of the DES algorithm.
		if(key[n].key >= 0x80+key[n].base){
			//While this key would be identical to a key we've already used
			//keep incrementing it
			while((type[key[n].key-0x80] == key[n].type)){
				key[n].index++;
				if(key[n].index == base_count){
					return 0;
				}
				key[n].key = base_chars[key[n].index];
			}
		}
		key[n].type = type[key[n].key];
		return 1;
	}
	//This is the second byte of a Shift-jis two-byte characters
	else{
		int prev = key[n-1].key;
		int roof = count[prev];
		
		//This is, numerically, the largest possible byte for this sequence
		//So we can't increment it
		if(key[n].index == roof){
			return 0;
		}
		
		//Update the key
		key[n].key =  chars[prev*256 + key[n].index];
		
		//Make sure we do not re-use a key that is identical in the eye of the DES algorithm.
		if(key[n].key >= 0x80+key[n].base){
			//While this key would be identical to a key we've already used
			//keep incrementing it
			while((existence[256*prev + key[n].key-0x80])){
				key[n].index++;
				if(key[n].index == roof){
					return 0;
				}
				key[n].key = chars[prev*256 + key[n].index];
			}
			//All was used, can't increment.
			if(key[n].index == roof){
				return 0;
			}
		}
		
		return 1;
		
	}
	assert(1 /*shouldn't be reached */);
	return 0; // Shouldn't happen
}

void cl_key_inc(cl_key_char *key){
	int n = 7;
	while(!cl_key_inc_single(key,n)){
		key[n].base = 0;
		key[n].index = 0;
		n--;
	}
	while(n++<7)cl_key_inc_single(key,n);
}


void copy_sjis_tables(uint32_t *target){
	memcpy(target, chars, 256*256*2*sizeof(uint32_t));
}

int cl_key_get_last(int key1, int key2){
	if(!(type[key1]==1&&type[key2]==2))return 0;
	return chars[key2*256];
}

void strrepl(uint8_t *string, uint8_t target, uint8_t replacement, uint32_t len){
	int i = 0;
	for(;i < len; i++){
		if(string[i]==target)string[i]=replacement;
	}
}

void cl_key_make_safe(uint32_t *key){
	char bad_ascii[] = "!\"#$%&()*+,-";
	int i;
	for(i = 0; i < strlen(bad_ascii); i++)strrepl(key, bad_ascii[i], bad_ascii[i]+0x80, 8);
}