#include "../include/Hash.h"
#include <openssl/bn.h>
#include <openssl/rand.h>
#include <iostream>
#include <cstring>

void gen_params(byte_ptr& key, byte_ptr& iv, const unsigned int key_size, const unsigned int block_size);
void aes_encrypt(const byte_ptr& key, const byte_ptr& iv, const BIGNUM* pnum, BIGNUM* cnum, const unsigned int block_size);
void aes_decrypt(const byte_ptr& key, const byte_ptr& iv, const BIGNUM* cnum, BIGNUM* rnum);

// clang++ -Wall -std=c++11 evp-encrypt.cxx -o evp-encrypt.out `pkg-config --libs openssl`
int main(int argc, char* argv[]) {
    const unsigned int key_size = 256/(CHAR_BIT*sizeof(byte));  // because we're using AES-256
    const unsigned int block_size = 128/(CHAR_BIT*sizeof(byte));

    BN_CTX* ctx = BN_CTX_new();
    BN_CTX_start(ctx);

    // Load the necessary cipher
    EVP_add_cipher(EVP_aes_256_ctr());

    // plaintext, ciphertext, recovered text
    BIGNUM* pnum = BN_CTX_get(ctx);
    BIGNUM* cnum = BN_CTX_get(ctx);
    BIGNUM* rnum = BN_CTX_get(ctx);

    BN_dec2bn(&pnum, "1110111011101110111011101110111011101110111011101110111011101110111011101110111011101110111011101110111011101110");

    byte_ptr key(new byte[key_size]);
    byte_ptr iv(new byte[block_size]);
    gen_params(key, iv, key_size, block_size);
  
    // does encryption/decryption work?
    aes_encrypt(key, iv, pnum, cnum, block_size);
    aes_decrypt(key, iv, cnum, rnum);

    // if yes, does Hash encrypt correctly?
    // TODO test Hash against cnum with large lambda

    char* pstr = BN_bn2dec(pnum);
    char* rstr = BN_bn2dec(rnum);
    std::cout << "Original message:\n" << pstr << std::endl;
    std::cout << "Recovered message:\n" << rstr << std::endl;

    OPENSSL_free(pstr);
    OPENSSL_free(rstr);
    BN_CTX_end(ctx);
    BN_CTX_free(ctx);

    return 0;
}

void gen_params(
    byte_ptr& key,
    byte_ptr& iv,
    const unsigned int key_size,
    const unsigned int block_size) {

    int rc = RAND_bytes(key.get(), key_size);
    if (rc != 1)
      throw std::runtime_error("RAND_bytes key failed");

    rc = RAND_bytes(iv.get(), block_size);
    if (rc != 1)
      throw std::runtime_error("RAND_bytes for iv failed");
}

void aes_encrypt(
    const byte_ptr& key,
    const byte_ptr& iv,
    const BIGNUM* pnum,
    BIGNUM* cnum,
    const unsigned int block_size) {

    secure_string ptext, ctext;
    ptext.resize(BN_num_bytes(pnum));
    BN_bn2bin(pnum, &ptext[0]);

    EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
    int rc = EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_ctr(), NULL, key.get(), iv.get());
    if (rc != 1)
      throw std::runtime_error("EVP_EncryptInit_ex failed");

    // Recovered text expands upto BLOCK_SIZE
    ctext.resize(ptext.size()+block_size);
    std::cout << ctext.size() << std::endl;
    int out_len1 = (int)ctext.size();
    std::cout << out_len1 << std::endl;

    rc = EVP_EncryptUpdate(ctx.get(), &ctext[0], &out_len1, (const byte*)&ptext[0], (int)ptext.size());
    std::cout << out_len1 << std::endl;
    if (rc != 1)
      throw std::runtime_error("EVP_EncryptUpdate failed");
  
    int out_len2 = (int)ctext.size() - out_len1;
    std::cout << out_len2 << std::endl;
    rc = EVP_EncryptFinal_ex(ctx.get(), &ctext[0]+out_len1, &out_len2);
    std::cout << out_len2 << std::endl;
    if (rc != 1)
      throw std::runtime_error("EVP_EncryptFinal_ex failed");

    // Set cipher text size now that we know it
    ctext.resize(out_len1 + out_len2);
    std::cout << ctext.size() << std::endl;

    BN_bin2bn((const byte*)&ctext[0], (int)ctext.size(), cnum);
}

void aes_decrypt (
    const byte_ptr& key,
    const byte_ptr& iv,
    const BIGNUM* cnum,
    BIGNUM* rnum) {

    secure_string ctext, rtext;
    ctext.resize(BN_num_bytes(cnum));
    BN_bn2bin(cnum, &ctext[0]);

    EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
    int rc = EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_ctr(), NULL, key.get(), iv.get());
    if (rc != 1)
      throw std::runtime_error("EVP_DecryptInit_ex failed");

    // Recovered text contracts upto BLOCK_SIZE
    rtext.resize(ctext.size());
    int out_len1 = (int)rtext.size();

    rc = EVP_DecryptUpdate(ctx.get(), (byte*)&rtext[0], &out_len1, (const byte*)&ctext[0], (int)ctext.size());
    if (rc != 1)
      throw std::runtime_error("EVP_DecryptUpdate failed");
  
    int out_len2 = (int)rtext.size() - out_len1;
    rc = EVP_DecryptFinal_ex(ctx.get(), (byte*)&rtext[0]+out_len1, &out_len2);
    if (rc != 1)
      throw std::runtime_error("EVP_DecryptFinal_ex failed");

    // Set recovered text size now that we know it
    rtext.resize(out_len1 + out_len2);

    BN_bin2bn((const byte*)&rtext[0], (int)rtext.size(), rnum);
}

