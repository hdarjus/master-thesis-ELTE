#include <iostream>
#include <string>
#include <cstring>
#include <memory>
#include <limits>
#include <stdexcept>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/bn.h>

template <typename T>
struct zallocator
{
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    pointer address (reference v) const {return &v;}
    const_pointer address (const_reference v) const {return &v;}

    pointer allocate (size_type n, const void* hint = 0) {
        if (n > std::numeric_limits<size_type>::max() / sizeof(T))
            throw std::bad_alloc();
        return static_cast<pointer> (::operator new (n * sizeof (value_type)));
    }

    void deallocate(pointer p, size_type n) {
        OPENSSL_cleanse(p, n*sizeof(T));
        ::operator delete(p); 
    }
    
    size_type max_size() const {
        return std::numeric_limits<size_type>::max() / sizeof (T);
    }
    
    template<typename U>
    struct rebind
    {
        typedef zallocator<U> other;
    };

    void construct (pointer ptr, const T& val) {
        new (static_cast<T*>(ptr) ) T (val);
    }

    void destroy(pointer ptr) {
        static_cast<T*>(ptr)->~T();
    }

#if __cpluplus >= 201103L
    template<typename U, typename... Args>
    void construct (U* ptr, Args&&  ... args) {
        ::new (static_cast<void*> (ptr) ) U (std::forward<Args> (args)...);
    }

    template<typename U>
    void destroy(U* ptr) {
        ptr->~U();
    }
#endif
};

typedef unsigned char byte;
typedef std::basic_string<char, std::char_traits<char>, zallocator<char> > secure_string;
using EVP_CIPHER_CTX_free_ptr = std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)>;

void gen_params(byte* key, byte* iv, const unsigned int key_size, const unsigned int block_size);
void aes_encrypt(const byte* key, const byte* iv, const secure_string& ptext, secure_string& ctext, const unsigned int block_size);
void aes_decrypt(const byte* key, const byte* iv, const secure_string& ctext, secure_string& rtext);

// clang++ -Wall -std=c++11 evp-encrypt.cxx -o evp-encrypt.out `pkg-config --libs openssl`
int main(int argc, char* argv[])
{
    const unsigned int key_size = 256/(CHAR_BIT*sizeof(byte));  // because we're using AES-256
    const unsigned int block_size = 128/(CHAR_BIT*sizeof(byte));

    // Load the necessary cipher
    EVP_add_cipher(EVP_aes_256_cbc());

    // plaintext, ciphertext, recovered text
    secure_string ptext = "Now is the time for all good men to come to the aide of their country";
    secure_string ctext, rtext;

    byte key[key_size], iv[block_size];
    gen_params(key, iv, key_size, block_size);
  
    aes_encrypt(key, iv, ptext, ctext, block_size);
    aes_decrypt(key, iv, ctext, rtext);

    BIGNUM* p = BN_new();
    if (NULL == BN_bin2bn(reinterpret_cast<byte*>(const_cast<char*>(ctext.c_str())), ctext.size(), p)) {
      throw std::runtime_error("Failed to convert the binary");
    }

    const char* const toprint = BN_bn2hex(p);
    std::cout << std::strlen(toprint) << std::endl << toprint << std::endl;
    delete toprint;
    
    OPENSSL_cleanse(key, key_size);
    OPENSSL_cleanse(iv, block_size);

    BN_free(p);

    std::cout << "Original message:\n" << ptext << std::endl;
    std::cout << "Recovered message:\n" << rtext << std::endl;

    return 0;
}

void gen_params(
    byte* key,
    byte* iv,
    const unsigned int key_size,
    const unsigned int block_size) {

    int rc = RAND_bytes(key, key_size);
    if (rc != 1)
      throw std::runtime_error("RAND_bytes key failed");

    rc = RAND_bytes(iv, block_size);
    if (rc != 1)
      throw std::runtime_error("RAND_bytes for iv failed");
}

void aes_encrypt(
    const byte* key,
    const byte* iv,
    const secure_string& ptext,
    secure_string& ctext,
    const unsigned int block_size) {

    EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
    int rc = EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_cbc(), NULL, key, iv);
    if (rc != 1)
      throw std::runtime_error("EVP_EncryptInit_ex failed");

    // Recovered text expands upto BLOCK_SIZE
    ctext.resize(ptext.size()+block_size);
    std::cout << ctext.size() << std::endl;
    int out_len1 = (int)ctext.size();
    std::cout << out_len1 << std::endl;

    rc = EVP_EncryptUpdate(ctx.get(), (byte*)&ctext[0], &out_len1, (const byte*)&ptext[0], (int)ptext.size());
    std::cout << out_len1 << std::endl;
    if (rc != 1)
      throw std::runtime_error("EVP_EncryptUpdate failed");
  
    int out_len2 = (int)ctext.size() - out_len1;
    std::cout << out_len2 << std::endl;
    rc = EVP_EncryptFinal_ex(ctx.get(), (byte*)&ctext[0]+out_len1, &out_len2);
    std::cout << out_len2 << std::endl;
    if (rc != 1)
      throw std::runtime_error("EVP_EncryptFinal_ex failed");

    // Set cipher text size now that we know it
    ctext.resize(out_len1 + out_len2);
    std::cout << ctext.size() << std::endl;
}

void aes_decrypt (
    const byte* key,
    const byte* iv,
    const secure_string& ctext,
    secure_string& rtext) {

    EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
    int rc = EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_cbc(), NULL, key, iv);
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
}

