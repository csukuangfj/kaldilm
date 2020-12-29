// kaldilm/csrc/symbol_table.h

#ifndef KALDILM_CSRC_SYMBOL_TABLE_H_
#define KALDILM_CSRC_SYMBOL_TABLE_H_

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// DenseSymbolMap is copied/modified from OpenFST
// SymbolTable is copied/modified from OpenFST

namespace kaldilm {

constexpr int64_t kNoSymbol = -1;

struct SymbolTableTextOptions {
  bool allow_negative_labels = false;
  std::string fst_field_separator = "\t ";
};

namespace internal {
// List of symbols with a dense hash for looking up symbol index, rehashing at
// 75% occupancy.
class DenseSymbolMap {
 public:
  // Argument type for symbol lookups and inserts.

  using KeyType = const std::string &;

  DenseSymbolMap();

  std::pair<int64_t, bool> InsertOrFind(KeyType key);

  int64_t Find(KeyType key) const;

  size_t Size() const { return symbols_.size(); }

  const std::string &GetSymbol(size_t idx) const { return symbols_[idx]; }

  void RemoveSymbol(size_t idx);

  void ShrinkToFit();

 private:
  static constexpr int64_t kEmptyBucket = -1;

  // num_buckets must be power of 2.
  void Rehash(size_t num_buckets);

  size_t GetHash(KeyType key) const { return str_hash_(key) & hash_mask_; }

  const std::hash<typename std::remove_const<
      typename std::remove_reference<KeyType>::type>::type>
      str_hash_;
  std::vector<std::string> symbols_;
  std::vector<int64_t> buckets_;
  uint64_t hash_mask_;
};

class SymbolTableImpl {
 public:
  using SymbolType = DenseSymbolMap::KeyType;

  SymbolTableImpl() = default;
  SymbolTableImpl(const SymbolTableImpl &impl)
      : available_key_(impl.available_key_),
        dense_key_limit_(impl.dense_key_limit_),
        symbols_(impl.symbols_),
        idx_key_(impl.idx_key_),
        key_map_(impl.key_map_) {}

  std::unique_ptr<SymbolTableImpl> Copy() const {
    return std::make_unique<SymbolTableImpl>(*this);
  }

  bool Member(int64_t key) const { return !Find(key).empty(); }
  bool Member(SymbolType symbol) const { return Find(symbol) != kNoSymbol; }

  int64_t AddSymbol(SymbolType symbol, int64_t key);

  int64_t AddSymbol(SymbolType symbol) {
    return AddSymbol(symbol, available_key_);
  }

  void RemoveSymbol(int64_t key);

  // Returns the string associated with the key. If the key is out of
  // range (<0, >max), return an empty string.
  std::string Find(int64_t key) const;

  // Returns the key associated with the symbol; if the symbol
  // does not exists, returns kNoSymbol.
  int64_t Find(SymbolType symbol) const {
    int64_t idx = symbols_.Find(symbol);
    if (idx == kNoSymbol || idx < dense_key_limit_) return idx;
    return idx_key_[idx - dense_key_limit_];
  }

  static SymbolTableImpl *ReadText(
      std::istream &strm, const std::string &source,
      const SymbolTableTextOptions &opts = SymbolTableTextOptions());

  int64_t AvailableKey() const { return available_key_; }

  size_t NumSymbols() const { return symbols_.Size(); }

  void ShrinkToFit();

  int64_t GetNthKey(ssize_t pos) const {
    if (pos < 0 || static_cast<size_t>(pos) >= symbols_.Size()) {
      return kNoSymbol;
    } else if (pos < dense_key_limit_) {
      return pos;
    }
    return Find(symbols_.GetSymbol(pos));
  }

 private:
  int64_t available_key_ = 0;
  int64_t dense_key_limit_ = 0;

  DenseSymbolMap symbols_;
  // Maps index to key for index >= dense_key_limit:
  //   key = idx_key_[index - dense_key_limit]
  std::vector<int64_t> idx_key_;
  // Maps key to index for key >= dense_key_limit_.
  //  index = key_map_[key]
  std::map<int64_t, int64_t> key_map_;
};

}  // namespace internal

// Symbol (string) to integer (and reverse) mapping.
//
// The SymbolTable implements the mappings of labels to strings and reverse.
// SymbolTables are used to describe the alphabet of the input and output
// labels for arcs in a Finite State Transducer.
//
// SymbolTables are reference-counted and can therefore be shared across
// multiple machines. For example a language model grammar G, with a
// SymbolTable for the words in the language model can share this symbol
// table with the lexical representation L o G.
class SymbolTable {
 public:
  using SymbolType = internal::SymbolTableImpl::SymbolType;

  // Constructs a symbol table
  SymbolTable() : impl_(std::make_shared<internal::SymbolTableImpl>()) {}

  ~SymbolTable() = default;

  class iterator {
   public:
    using iterator_category = std::input_iterator_tag;

    class value_type {
     public:
      // Return the label of the current symbol.
      int64_t Label() const { return key_; }

      // Return the string of the current symbol.
      std::string Symbol() const { return table_->Find(key_); }

     private:
      explicit value_type(const SymbolTable &table, ssize_t pos)
          : table_(&table), key_(table.GetNthKey(pos)) {}

      // Sets this item to the pos'th element in the symbol table
      void SetPosition(ssize_t pos) { key_ = table_->GetNthKey(pos); }

      friend class SymbolTable::iterator;

      const SymbolTable *table_;  // Does not own the underlying SymbolTable.
      int64_t key_;
    };

    using difference_type = std::ptrdiff_t;
    using pointer = const value_type *const;
    using reference = const value_type &;

    iterator &operator++() {
      ++pos_;
      if (static_cast<size_t>(pos_) < nsymbols_) iter_item_.SetPosition(pos_);
      return *this;
    }

    iterator operator++(int) {
      iterator retval = *this;
      ++(*this);
      return retval;
    }

    bool operator==(const iterator &that) const { return (pos_ == that.pos_); }

    bool operator!=(const iterator &that) const { return !(*this == that); }

    reference operator*() { return iter_item_; }

    pointer operator->() const { return &iter_item_; }

   private:
    explicit iterator(const SymbolTable &table, ssize_t pos = 0)
        : pos_(pos), nsymbols_(table.NumSymbols()), iter_item_(table, pos) {}

    friend class SymbolTable;

    ssize_t pos_;
    size_t nsymbols_;
    value_type iter_item_;
  };

  using const_iterator = iterator;
  const_iterator begin() const { return const_iterator(*this, 0); }

  const_iterator end() const { return const_iterator(*this, NumSymbols()); }

  const_iterator cbegin() const { return begin(); }

  const_iterator cend() const { return end(); }

  // Reads a text representation of the symbol table from an istream. Pass a
  // name to give the resulting SymbolTable.
  static SymbolTable *ReadText(
      std::istream &strm, const std::string &source,
      const SymbolTableTextOptions &opts = SymbolTableTextOptions()) {
    auto impl = internal::SymbolTableImpl::ReadText(strm, source, opts);
    auto shared_ptr = std::shared_ptr<internal::SymbolTableImpl>(impl);
    return impl ? new SymbolTable(shared_ptr) : nullptr;
  }

  // Reads a text representation of the symbol table.
  static SymbolTable *ReadText(
      const std::string &source,
      const SymbolTableTextOptions &opts = SymbolTableTextOptions());

  // Creates a reference counted copy.
  SymbolTable *Copy() const { return new SymbolTable(*this); }

  // Adds a symbol with given key to table. A symbol table also keeps track of
  // the last available key (highest key value in the symbol table).
  int64_t AddSymbol(SymbolType symbol, int64_t key) {
    return impl_->AddSymbol(symbol, key);
  }

  // Adds a symbol to the table. The associated value key is automatically
  // assigned by the symbol table.
  int64_t AddSymbol(SymbolType symbol) { return impl_->AddSymbol(symbol); }

  // Returns the current available key (i.e., highest key + 1) in the symbol
  // table.
  int64_t AvailableKey() const { return impl_->AvailableKey(); }

  // Returns the string associated with the key; if the key is out of
  // range (<0, >max), returns an empty string.
  std::string Find(int64_t key) const { return impl_->Find(key); }

  // Returns the key associated with the symbol; if the symbol does not exist,
  // kNoSymbol is returned.
  int64_t Find(SymbolType symbol) const { return impl_->Find(symbol); }

  bool Member(int64_t key) const { return impl_->Member(key); }

  bool Member(SymbolType symbol) const { return impl_->Member(symbol); }

  // Returns the current number of symbols in table (not necessarily equal to
  // AvailableKey()).
  size_t NumSymbols() const { return impl_->NumSymbols(); }

  void RemoveSymbol(int64_t key) { return impl_->RemoveSymbol(key); }

  int64_t GetNthKey(ssize_t pos) const { return impl_->GetNthKey(pos); }

  // Dumps a text representation of the symbol table via a stream.
  bool WriteText(std::ostream &strm, const SymbolTableTextOptions &opts =
                                         SymbolTableTextOptions()) const;

  // Dumps a text representation of the symbol table.
  bool WriteText(const std::string &source) const;

 private:
  explicit SymbolTable(std::shared_ptr<internal::SymbolTableImpl> impl)
      : impl_(std::move(impl)) {}

  std::shared_ptr<internal::SymbolTableImpl> impl_;
};

}  // namespace kaldilm

#endif  // KALDILM_CSRC_SYMBOL_TABLE_H_
