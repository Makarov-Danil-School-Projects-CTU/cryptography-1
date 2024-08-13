#ifndef __PROGTEST__

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <openssl/evp.h>
#include <openssl/rand.h>

#endif /* __PROGTEST__ */

using namespace std;


void doNextWord(unsigned char *msg, int *actualMsgPos, const int msgLen) {
    if (*actualMsgPos == msgLen - 1)
        *actualMsgPos = 0;

    if (msg[*actualMsgPos] == 255)
        msg[(*actualMsgPos)++] = 0;
    else
        msg[(*actualMsgPos)++]++;
}

void generateMessage(unsigned char *msg, const int msgLen, int *randomIteration, int *actualMsgPos) {
    if (*randomIteration == 100) {
        *randomIteration = 0;
        if (RAND_bytes(msg, msgLen))
            return;
    }

    if (*randomIteration % 10 == 0) {
        (*randomIteration)++;
        if (msgLen > 1) {
            for (int i = 0; i < msgLen / 2; i++)
                msg[i] = rand() & 0xff;
            return;
        }
    }

    (*randomIteration)++;
    doNextWord(msg, actualMsgPos, msgLen);
}

char *toHexStr(const unsigned char *msg, unsigned int hashLen) {
    auto hexString = (char *) malloc(hashLen * 2 + 1);

    if (!hexString)
        return nullptr;

    for (size_t i = 0; i < hashLen; i++)
        sprintf(&hexString[i * 2], "%02x", msg[i]);

    return hexString;
}

bool checkLeadingZeros(const unsigned char *hashArr, size_t bits) {
    size_t bytes = bits / 8;

    for (size_t i = 0; i < bytes; i++)
        if (hashArr[i] != 0)
            return false;

    size_t extraBits = bits % 8;
    if (extraBits > 0 && (hashArr[bytes] >> (8 - extraBits)) != 0)
        return false;

    return true;
}

int findHashEx(int bits, char **message, char **hash, const char *hashFunction) {
    if (!EVP_get_digestbyname(hashFunction))
        return 0;

    const EVP_MD *type = EVP_get_digestbyname(hashFunction);

    auto hashArr = (unsigned char *) malloc(EVP_MD_size(type));
    if (!hashArr)
        return 0;

    unsigned int hashLen;

    if (bits < 0 || bits > EVP_MD_size(type) * 8)
        return 0;

    OpenSSL_add_all_digests();
    srand(time(nullptr));
    const int msgLen = 50;
    unsigned char tmpMsg[msgLen] = {0};
    bool ret = false;

    int actualPos = 0;
    int randomIteration = 0;

    while (!ret) {
        generateMessage(tmpMsg, msgLen, &randomIteration, &actualPos);
        EVP_MD_CTX *ctx = EVP_MD_CTX_new();
        if (ctx == nullptr) {
            free(hashArr);
            return 0;
        }
        if (!EVP_DigestInit_ex(ctx, type, nullptr)) {
            EVP_MD_CTX_free(ctx);
            free(hashArr);
            return 0;
        }
        if (!EVP_DigestUpdate(ctx, tmpMsg, msgLen)) {
            EVP_MD_CTX_free(ctx);
            free(hashArr);
            return 0;
        }
        if (!EVP_DigestFinal_ex(ctx, hashArr, &hashLen)) {
            EVP_MD_CTX_free(ctx);
            free(hashArr);
            return 0;
        }
        EVP_MD_CTX_free(ctx);

        ret = checkLeadingZeros(hashArr, bits);
    }

    *message = toHexStr(tmpMsg, msgLen);
    *hash = toHexStr(hashArr, hashLen);
    if (!(*message) || !(*hash)) {
        free(hashArr);
        return 0;
    }
    free(hashArr);
    return 1;
}

int findHash(int bits, char **message, char **hash) {
    return findHashEx(bits, message, hash, "sha512");
}


#ifndef __PROGTEST__

int checkHash(int bits, char *hexString) {
    // DIY
    return 1;
}

int main(void) {
    char *message, *hash;
    assert(findHash(5, &message, &hash) == 1);
    free(message);
    free(hash);
    assert(findHash(10, &message, &hash) == 1);
    free(message);
    free(hash);
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */