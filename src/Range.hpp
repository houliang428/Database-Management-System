//
//  Range.hpp
//  Database3


#ifndef Range_h
#define Range_h

namespace ECE141 {

  template<class T>
  struct Range {
    T* b = nullptr; T* e = nullptr;
    Range()=default;
    Range(T* s, T* f):b(s),e(f){}
    Range(T* s, std::size_t l):Range(s, s+l){}
    T* begin() const{ return b; }
    T* end() const{ return e; }
    T& operator[](std::size_t i)const{ return begin()[i]; }
    std::size_t size() const { return end()-begin(); }
    bool empty() const { return begin()==end(); }
  };

}


#endif /* Range_h */
