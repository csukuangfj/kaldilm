// kaldilm/csrc/symbol_table.cc

// DenseSymbolMap is copied/modified from OpenFST

#include "kaldilm/csrc/symbol_table.h"

#include <string.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "kaldilm/csrc/log.h"
#include "kaldilm/csrc/string_utils.h"

namespace kaldilm {

namespace internal {

constexpr int64_t DenseSymbolMap::kEmptyBucket;

// Maximum line length in textual symbols file.
constexpr int kLineLen = 8096;

DenseSymbolMap::DenseSymbolMap()
    : str_hash_(),
      buckets_(1 << 4, kEmptyBucket),
      hash_mask_(buckets_.size() - 1) {}

std::pair<int64_t, bool> DenseSymbolMap::InsertOrFind(KeyType key) {
  static constexpr float kMaxOccupancyRatio = 0.75;  // Grows when 75% full.
  if (Size() >= kMaxOccupancyRatio * buckets_.size()) {
    Rehash(buckets_.size() * 2);
  }
  size_t idx = GetHash(key);
  while (buckets_[idx] != kEmptyBucket) {
    const auto stored_value = buckets_[idx];
    if (symbols_[stored_value] == key) return {stored_value, false};
    idx = (idx + 1) & hash_mask_;
  }
  const auto next = Size();
  buckets_[idx] = next;
  symbols_.emplace_back(key);
  return {next, true};
}

int64_t DenseSymbolMap::Find(KeyType key) const {
  size_t idx = str_hash_(key) & hash_mask_;
  while (buckets_[idx] != kEmptyBucket) {
    const auto stored_value = buckets_[idx];
    if (symbols_[stored_value] == key) return stored_value;
    idx = (idx + 1) & hash_mask_;
  }
  return buckets_[idx];
}

void DenseSymbolMap::Rehash(size_t num_buckets) {
  buckets_.resize(num_buckets);
  hash_mask_ = buckets_.size() - 1;
  std::fill(buckets_.begin(), buckets_.end(), kEmptyBucket);
  for (size_t i = 0; i < Size(); ++i) {
    size_t idx = GetHash(symbols_[i]);
    while (buckets_[idx] != kEmptyBucket) {
      idx = (idx + 1) & hash_mask_;
    }
    buckets_[idx] = i;
  }
}

void DenseSymbolMap::RemoveSymbol(size_t idx) {
  symbols_.erase(symbols_.begin() + idx);
  Rehash(buckets_.size());
}

void DenseSymbolMap::ShrinkToFit() { symbols_.shrink_to_fit(); }

SymbolTableImpl *SymbolTableImpl::ReadText(
    std::istream &strm, const std::string &source,
    const SymbolTableTextOptions &opts /*= SymbolTableTextOptions()*/) {
  auto impl = std::make_unique<SymbolTableImpl>();
  int64_t nline = 0;
  char line[kLineLen];
  const auto separator = opts.fst_field_separator + "\n";
  while (!strm.getline(line, kLineLen).fail()) {
    ++nline;
    std::vector<char *> col;
    SplitString(line, separator.c_str(), true, &col);
    if (col.empty()) continue;  // Empty line.
    if (col.size() != 2) {
      KALDILM_ERR << "SymbolTable::ReadText: Bad number of columns ("
                  << col.size() << "), "
                  << "file = " << source << ", line = " << nline << ":<" << line
                  << ">";
      return nullptr;
    }
    const char *symbol = col[0];
    const char *value = col[1];
    char *p;
    const auto key = strtoll(value, &p, 10);
    if (*p != '\0' || (!opts.allow_negative_labels && key < 0) ||
        key == kNoSymbol) {
      KALDILM_ERR << "SymbolTable::ReadText: Bad non-negative integer \""
                  << value << "\", "
                  << "file = " << source << ", line = " << nline;
      return nullptr;
    }
    impl->AddSymbol(symbol, key);
  }
  impl->ShrinkToFit();
  return impl.release();
}

std::string SymbolTableImpl::Find(int64_t key) const {
  int64_t idx = key;
  if (key < 0 || key >= dense_key_limit_) {
    const auto it = key_map_.find(key);
    if (it == key_map_.end()) return "";
    idx = it->second;
  }
  if (idx < 0 || idx >= symbols_.Size()) return "";
  return symbols_.GetSymbol(idx);
}

int64_t SymbolTableImpl::AddSymbol(SymbolType symbol, int64_t key) {
  if (key == kNoSymbol) return key;
  const auto insert_key = symbols_.InsertOrFind(symbol);
  if (!insert_key.second) {
    const auto key_already = GetNthKey(insert_key.first);
    if (key_already == key) return key;
    KALDILM_LOG << "SymbolTable::AddSymbol: symbol = " << symbol
                << " already in symbol_map_ with key = " << key_already
                << " but supplied new key = " << key << " (ignoring new key)";
    return key_already;
  }
  if (key + 1 == static_cast<int64_t>(symbols_.Size()) &&
      key == dense_key_limit_) {
    ++dense_key_limit_;
  } else {
    idx_key_.push_back(key);
    key_map_[key] = symbols_.Size() - 1;
  }
  if (key >= available_key_) available_key_ = key + 1;
  return key;
}

void SymbolTableImpl::RemoveSymbol(const int64_t key) {
  auto idx = key;
  if (key < 0 || key >= dense_key_limit_) {
    auto iter = key_map_.find(key);
    if (iter == key_map_.end()) return;
    idx = iter->second;
    key_map_.erase(iter);
  }
  if (idx < 0 || idx >= static_cast<int64_t>(symbols_.Size())) return;
  symbols_.RemoveSymbol(idx);
  // Removed one symbol, all indexes > idx are shifted by -1.
  for (auto &k : key_map_) {
    if (k.second > idx) --k.second;
  }
  if (key >= 0 && key < dense_key_limit_) {
    // Removal puts a hole in the dense key range. Adjusts range to [0, key).
    const auto new_dense_key_limit = key;
    for (int64_t i = key + 1; i < dense_key_limit_; ++i) {
      key_map_[i] = i - 1;
    }
    // Moves existing values in idx_key to new place.
    idx_key_.resize(symbols_.Size() - new_dense_key_limit);
    for (int64_t i = symbols_.Size(); i >= dense_key_limit_; --i) {
      idx_key_[i - new_dense_key_limit - 1] = idx_key_[i - dense_key_limit_];
    }
    // Adds indexes for previously dense keys.
    for (int64_t i = new_dense_key_limit; i < dense_key_limit_ - 1; ++i) {
      idx_key_[i - new_dense_key_limit] = i + 1;
    }
    dense_key_limit_ = new_dense_key_limit;
  } else {
    // Remove entry for removed index in idx_key.
    for (size_t i = idx - dense_key_limit_; i + 1 < idx_key_.size(); ++i) {
      idx_key_[i] = idx_key_[i + 1];
    }
    idx_key_.pop_back();
  }
  if (key == available_key_ - 1) available_key_ = key;
}

void SymbolTableImpl::ShrinkToFit() { symbols_.ShrinkToFit(); }

}  // namespace internal

SymbolTable *SymbolTable::ReadText(const std::string &source,
                                   const SymbolTableTextOptions &opts) {
  std::ifstream strm(source, std::ios_base::in);
  if (!strm.good()) {
    KALDILM_ERR << "SymbolTable::ReadText: Can't open file: " << source;
    return nullptr;
  }
  return ReadText(strm, source, opts);
}

bool SymbolTable::WriteText(std::ostream &strm,
                            const SymbolTableTextOptions &opts) const {
  if (opts.fst_field_separator.empty()) {
    KALDILM_ERR << "Missing required field separator";
    return false;
  }
  bool once_only = false;
  for (const auto &item : *this) {
    std::ostringstream line;
    if (item.Label() < 0 && !opts.allow_negative_labels && !once_only) {
      KALDILM_LOG << "Negative symbol table entry when not allowed";
      once_only = true;
    }
    line << item.Symbol() << opts.fst_field_separator[0] << item.Label()
         << '\n';
    strm.write(line.str().data(), line.str().length());
  }
  return true;
}

bool SymbolTable::WriteText(const std::string &source) const {
  if (!source.empty()) {
    std::ofstream strm(source);
    if (!strm) {
      KALDILM_ERR << "SymbolTable::WriteText: Can't open file: " << source;
      return false;
    }
    if (!WriteText(strm, SymbolTableTextOptions())) {
      KALDILM_ERR << "SymbolTable::WriteText: Write failed: " << source;
      return false;
    }
    return true;
  } else {
    return WriteText(std::cout, SymbolTableTextOptions());
  }
}

}  // namespace kaldilm
