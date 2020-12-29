#ifdef NDEBUG
#undef NDEBUG
#include <cassert>
#define NDEBUG
#endif

#include <iostream>
#include <sstream>

#include "kaldilm/csrc/log.h"
#include "kaldilm/csrc/symbol_table.h"

namespace {
using kaldilm::SymbolTable;

void Test1() {
  KALDILM_LOG << "Test1\n";
  SymbolTable symbols;
  symbols.AddSymbol("a");
  symbols.AddSymbol("b");

  assert(symbols.Member(0));
  assert(symbols.Member("a"));

  assert(symbols.Member(1));
  assert(symbols.Member("b"));

  symbols.RemoveSymbol(1);

  assert(!symbols.Member(1));
  assert(!symbols.Member("b"));

  symbols.WriteText(std::cout);
}

void Test2() {
  KALDILM_LOG << "Test2\n";
  std::stringstream ss;
  ss << "a 0\n";
  ss << "b 1\n";
  ss << "c 2\n";

  SymbolTable *symbols = SymbolTable::ReadText(ss, "test");

  assert(symbols->Member(0));
  assert(symbols->Member("a"));

  assert(symbols->Member(1));
  assert(symbols->Member("b"));

  symbols->WriteText(std::cout);

  delete symbols;
}

}  // namespace

int main() {
  Test1();
  Test2();
  return 0;
}
