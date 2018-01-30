//
// Created by zhangweiwen on 2016/8/30.
//

#include "zCrypt.h"

/**
 * 加密
 *
 * @param data 待加密数据
 * @param len  数据长度
 * @return     -1  失败
 *             -2  加密方式不支持
 *             >=0 剩余字节数
 */
int zCrypt::encrypt(void *data, unsigned int len)
{
    switch (_method) {
        case CRYPT_NONE:
            return -1;
        case CRYPT_DES:
            return encrypt_des(static_cast<unsigned char *> (data), len);
        case CRYPT_RC5:
            return encrypt_rc5(static_cast<unsigned char *> (data), len);
    }

    return -2;
}

/**
 * DES加密
 *
 * @param data 待加密数据
 * @param len  数据长度
 * @return     -1  失败
 *             >=0 剩余字节数
 */
int zCrypt::encrypt_des(unsigned char *data, unsigned int len)
{
    if (nullptr == data || !is_des_key) {
        return -1;
    }

    BlockTransformation *tf = new DES_EDE2_Encryption(_deskey, sizeof(_deskey));
    unsigned int offset = 0;

    while (offset <= len - 8) {
        tf->ProcessBlock(data + offset);
        offset += 8;
    }

    return len - offset;
}

/**
 * RC5加密
 *
 * @param data 待加密数据
 * @param len  数据长度
 * @return     -1  失败
 *             >=0 剩余字节数
 */
int zCrypt::encrypt_rc5(unsigned char *data, unsigned int len)
{
    if (nullptr == data || !is_rc5_key) {
        return -1;
    }

    BlockTransformation *tf = new RC5Encryption(_rc5key, sizeof(_rc5key), 12);
    unsigned int offset = 0;

    while (offset <= len - 8) {
        tf->ProcessBlock(data + offset);
        offset += 8;
    }

    return len - offset;
}

/**
 * 解密
 *
 * @param data 待解密数据
 * @param len  数据长度
 * @return     -1  失败
 *             -2  加密方式不支持
 *             >=0 剩余字节数
 */
int zCrypt::decrypt(void *data, unsigned int len)
{
    switch (_method) {
        case CRYPT_NONE:
            return -1;
        case CRYPT_DES:
            return decrypt_des(static_cast<unsigned char*> (data), len);
        case CRYPT_RC5:
            return decrypt_rc5(static_cast<unsigned char*> (data), len);
    }

    return -2;
}

/**
 * DES解密
 *
 * @param data 待解密数据
 * @param len  数据长度
 * @return     -1  失败
 *             >=0 剩余字节数
 */
int zCrypt::decrypt_des(unsigned char *data, unsigned int len)
{
    if (nullptr == data || !is_des_key) {
        return -1;
    }

    BlockTransformation *tf = new DES_EDE2_Decryption(_deskey, sizeof(_deskey));
    unsigned int offset = 0;

    while (offset <= len - 8) {
        tf->ProcessBlock(data + offset);
        offset += 8;
    }

    return len - offset;
}

/**
 * RC5解密
 *
 * @param data 待解密数据
 * @param len  数据长度
 * @return     -1  失败
 *             >=0 剩余字节数
 */
int zCrypt::decrypt_rc5(unsigned char *data, unsigned int len)
{
    if (nullptr == data || !is_rc5_key) {
        return -1;
    }

    BlockTransformation *tf = new RC5Decryption(_rc5key, sizeof(_rc5key), 12);
    unsigned int offset = 0;

    while (offset <= len - 8) {
        tf->ProcessBlock(data + offset);
        offset += 8;
    }

    return len - offset;
}