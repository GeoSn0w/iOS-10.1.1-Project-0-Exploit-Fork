#ifndef cdhash_h
#define cdhash_h

#include <CommonCrypto/CommonDigest.h>

#define AMFID_HASH_SIZE CC_SHA1_DIGEST_LENGTH

void get_hash_for_amfid(char* path, uint8_t* hash_buf);

#endif
