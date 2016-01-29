#ifndef SYMTABLE_H_
#define SYMTABLE_H_

#include <iostream>
#include <map>
#include <vector>
#include <string>

template <typename Data>
class SymbolTable : public std::map<std::string, Data> {
public:
  using base = typename std::map<std::string, Data>;

  SymbolTable() :
    base() {
  }

  SymbolTable(const SymbolTable<Data> &) = delete;
  SymbolTable &operator=(const SymbolTable<Data> &) = delete;
  ~SymbolTable() = default;

private:
  using base::base;
};


namespace SYM {
static SymbolTable<unsigned long> symbol_table;
}

#endif // SYMTABLE_H_
