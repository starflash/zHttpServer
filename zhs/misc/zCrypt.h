//
// Created by zhangweiwen on 2016/8/30.
//

#ifndef ZHS_ZCRYPT_H
#define ZHS_ZCRYPT_H

#include <cstdio>
#include <crypto++/osrng.h>
#include <crypto++/des.h>
#include <crypto++/rc5.h>
#include "zhs/zhs.h"

using namespace CryptoPP;

/**
 * 加解密
 */
class zCrypt
{
public:

    enum CryptMethod
    {
        CRYPT_NONE,
        CRYPT_DES,
        CRYPT_RC5
    };

    // 构造函数
    zCrypt()
    {
        is_des_key = false;
        is_rc5_key = false;

        memset(_deskey, 0, sizeof(DESKey));
        memset(_rc5key, 0, sizeof(RC5Key));

        _method = CRYPT_NONE;
    }

    // 设置DES加密的密钥
    void setDesKey(const char *key)
    {
        if (strlen(key) >= sizeof(DESKey)) {
            memcpy(_deskey, key, sizeof(DESKey));
            is_des_key = true;
        }
    }

    // 产生DES加密的随机密钥
    void randomDesKey(DESKey *key)
    {
        size_t len = sizeof(*key);
        SecByteBlock block(len);

        OS_GenerateRandomBlock(false, block, block.size());
        memcpy(*key, block.BytePtr(), len);
    }

    // 设置RC5加密的密钥
    void setRc5Key(const char *key)
    {
        if (strlen(key) >= sizeof(RC5Key)) {
            memcpy(_rc5key, key, sizeof(RC5Key));
            is_rc5_key = true;
        }
    }

    int encrypt(void *, unsigned int);
    int decrypt(void *, unsigned int);

    void setCryptMethod(CryptMethod method) { _method = method; }
    CryptMethod getCryptMethod() const { return _method; }

private:

    int encrypt_des(unsigned char *, unsigned int);
    int decrypt_des(unsigned char *, unsigned int);

    int encrypt_rc5(unsigned char *, unsigned int);
    int decrypt_rc5(unsigned char *, unsigned int);

    // 是否设置了DES密钥
    bool is_des_key;

    // 是否设置了RC5密钥
    bool is_rc5_key;

    // DES密钥
    DESKey _deskey {};

    // RC5密钥
    RC5Key _rc5key {};

    // 加密方式
    CryptMethod _method;
};


#endif //ZHS_ZCRYPT_H
