#include <nda/clef.hpp>
#include <iostream>
#include <vector>
using namespace nda::clef;

int main() {
  int N     = 5;
  double pi = std::acos(-1);

  // automatic assignment of vector
  placeholder<0> k_;
  std::vector<double> V(N);
  make_expr(V)[k_] << cos((2 * pi * k_) / N);

  // chaining them ...
  placeholder<1> i_;
  std::vector<std::vector<double>> W(3, std::vector<double>(N));
  make_expr(W)[i_][k_] << i_ + cos((2 * pi * k_) / N);

  // check result...
  for (size_t u = 0; u < V.size(); ++u)
    if (std::abs(V[u] - cos((2 * pi * u) / N)) > 1.e-10) throw "error!";
  for (size_t w = 0; w < W.size(); ++w)
    for (size_t u = 0; u < W[w].size(); ++u)
      if (std::abs(W[w][u] - (w + cos((2 * pi * u) / N))) > 1.e-10) throw "error!";
}
