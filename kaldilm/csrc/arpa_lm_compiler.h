// kaldilm/csrc/arpa_lm_compiler.h

// This file is copied/modified from
// https://github.com/kaldi-asr/kaldi/blob/master/src/lm/arpa-lm-compiler.h

// Copyright 2009-2011 Gilles Boulianne
// Copyright 2016 Smart Action LLC (kkm)

#include "fst/fstlib.h"
#include "fst/symbol-table.h"
#include "kaldilm/csrc/arpa_file_parser.h"

namespace kaldilm {

class ArpaLmCompilerImplInterface;

class ArpaLmCompiler : public ArpaFileParser {
 public:
  ArpaLmCompiler(const ArpaParseOptions &options, int sub_eps,
                 fst::SymbolTable *symbols)
      : ArpaFileParser(options, symbols), sub_eps_(sub_eps), impl_(nullptr) {}
  ~ArpaLmCompiler();

  const fst::StdVectorFst &Fst() const { return fst_; }
  fst::StdVectorFst *MutableFst() { return &fst_; }

 protected:
  // ArpaFileParser overrides.
  void HeaderAvailable() override;
  void ConsumeNGram(const NGram &ngram) override;
  void ReadComplete() override;

 private:
  // this function removes states that only have a backoff arc coming
  // out of them.
  void RemoveRedundantStates();
  void Check() const;

  int sub_eps_;
  ArpaLmCompilerImplInterface *impl_;  // Owned.
  fst::StdVectorFst fst_;
  template <class HistKey>
  friend class ArpaLmCompilerImpl;
};

}  // namespace kaldilm
