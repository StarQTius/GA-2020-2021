#ifndef GATE_OPENNN_ITERATOR_HPP
#define GATE_OPENNN_ITERATOR_HPP

#include <iterator>

/*******************************************************************************
  openNN_iterator.hpp : ensembles de classes d'itérateurs pour les conteneurs de
  la bibliothèque openNN, qui n'implémentent pas les itérateurs. Pour l'instant
  il n'y a que l'itérateur pour les openNN::Vector<>.
*******************************************************************************/

namespace gate {

template<typename T>
class VectorIterator {
 public:
   typedef

   VectorIterator& operator++(int) { value_ptr++; return *this; }
   VectorIterator operator++() { auto copy = *this; value_ptr++; return copy; }
   VectorIterator& operator--(int) { value_ptr--; return *this; }
   VectorIterator operator--() { auto copy = *this; value_ptr--; return copy; }
   VectorIterator& operator+=( n) { value_ptr += n; return *this; }
   VectorIterator& operator-=(difference_type n) { value_ptr -= n; return *this; }
   T& operator[](difference_type n) { return *(value_ptr + n); }
   T& operator*() { return *value_ptr; }
   T* operator->() { return value_ptr; }

   /*
     Fonctions membres constantes
   */

   VectorIterator operator+(difference_type n) const {
     return VectorIterator{ .value_ptr = value_ptr + n};
   }
   VectorIterator operator-(difference_type n) const {
     return VectorIterator{ .value_ptr = value_ptr - n};
   }
  const T& operator*() const { return *value_ptr; }
  const T* operator->() const { return value_ptr; }
  const T& operator[](difference_type n) const { return *(value_ptr + n); }
  auto operator<=>(const VectorIterator&) const = default;

 private:
  pointer value_ptr;

};

template<typename T>
inline auto operator+(size_t n, const VectorIterator<T>& it) { return it + n; }
template<typename T>
inline auto operator-(size_t n, const VectorIterator<T>& it) { return it - n; }

}

#endif
