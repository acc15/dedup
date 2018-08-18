/*
 * Original source is here
 * https://ru.wikibooks.org/wiki/%D0%A0%D0%B5%D0%B0%D0%BB%D0%B8%D0%B7%D0%B0%D1%86%D0%B8%D0%B8_%D0%B0%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC%D0%BE%D0%B2/%D0%A0%D0%B0%D1%81%D1%81%D1%82%D0%BE%D1%8F%D0%BD%D0%B8%D0%B5_%D0%9B%D0%B5%D0%B2%D0%B5%D0%BD%D1%88%D1%82%D0%B5%D0%B9%D0%BD%D0%B0
 */

#ifndef LEVENSHTEIN_DISTANCE_HPP
#define LEVENSHTEIN_DISTANCE_HPP

#include <vector>
#include <algorithm>

template <typename T>
typename T::value_type levenshtein_distance(const T & src, const T & dst) {
  const typename T::size_type m = src.size();
  const typename T::size_type n = dst.size();
  if (m == 0) {
    return n;
  }
  if (n == 0) {
    return m;
  }

  std::vector< std::vector<typename T::value_type> > matrix(m + 1);

  for (typename T::size_type i = 0; i <= m; ++i) {
    matrix[i].resize(n + 1);
    matrix[i][0] = i;
  }
  for (typename T::size_type i = 0; i <= n; ++i) {
    matrix[0][i] = i;
  }

  typename T::value_type above_cell, left_cell, diagonal_cell, cost;

  for (typename T::size_type i = 1; i <= m; ++i) {
    for(typename T::size_type j = 1; j <= n; ++j) {
      cost = src[i - 1] == dst[j - 1] ? 0 : 1;
      above_cell = matrix[i - 1][j];
      left_cell = matrix[i][j - 1];
      diagonal_cell = matrix[i - 1][j - 1];
      matrix[i][j] = std::min(std::min(above_cell + 1, left_cell + 1), diagonal_cell + cost);
    }
  }

  return matrix[m][n];
}

#endif // LEVENSHTEIN_DISTANCE_HPP
