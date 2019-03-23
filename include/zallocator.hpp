/*
 * https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
 */

#ifndef _ZALLOCATOR_HPP_
#define _ZALLOCATOR_HPP_

#include <string>
#include <limits>

template <typename T>
class zallocator {
  public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    inline pointer address (reference v) const {return &v;}
    inline const_pointer address (const_reference v) const {return &v;}

    inline pointer allocate (size_type n, const void* hint = 0) {
      if (n > std::numeric_limits<size_type>::max() / sizeof(T))
        throw std::bad_alloc();
      return static_cast<pointer> (::operator new (n * sizeof (value_type)));
    }

    inline void deallocate(pointer p, size_type n) {
      OPENSSL_cleanse(p, n*sizeof(T));
      ::operator delete(p); 
    }

    inline size_type max_size() const {
      return std::numeric_limits<size_type>::max() / sizeof (T);
    }

    template<typename U>
    class rebind {
      public:
        typedef zallocator<U> other;
    };

    inline void construct (pointer ptr, const T& val) {
      new (static_cast<T*>(ptr) ) T (val);
    }

    inline void destroy(pointer ptr) {
      static_cast<T*>(ptr)->~T();
    }

#if __cpluplus >= 201103L
    template<typename U, typename... Args>
    inline void construct (U* ptr, Args&&  ... args) {
      ::new (static_cast<void*> (ptr) ) U (std::forward<Args> (args)...);
    }

    template<typename U>
    inline void destroy(U* ptr) {
      ptr->~U();
    }
#endif
};

#endif  // _ZALLOCATOR_HPP_

