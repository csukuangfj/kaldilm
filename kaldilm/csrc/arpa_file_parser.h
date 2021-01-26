// kaldilm/csrc/arpa_file_parser.h

// This file is copied/modified from
// https://github.com/kaldi-asr/kaldi/blob/master/src/lm/arpa-file-parser.h

// Copyright 2014  Guoguo Chen
// Copyright 2016  Smart Action Company LLC (kkm)

#ifndef KALDILM_CSRC_ARPA_FILE_PARSER_H_
#define KALDILM_CSRC_ARPA_FILE_PARSER_H_

#include <cstdint>
#include <sstream>

#include "fst/symbol-table.h"

namespace kaldilm {

/**
  Options that control ArpaFileParser
*/
struct ArpaParseOptions {
  enum OovHandling {
    kRaiseError,      ///< Abort on OOV words
    kAddToSymbols,    ///< Add novel words to the symbol table.
    kReplaceWithUnk,  ///< Replace OOV words with "<unk>".
    kSkipNGram        ///< Skip n-gram with OOV word and continue.
  };

  /// Symbol for "<s>", Required non-epsilon.
  int32_t bos_symbol = -1;

  /// Symbol for "</s>", Required non-epsilon.
  int32_t eos_symbol = -1;

  /// Symbol for "<unk>", Required for kReplaceWithUnk.
  int32_t unk_symbol = -1;

  /// How to handle OOV words in the file.
  OovHandling oov_handling = kRaiseError;

  /// Maximum warnings to report, <0 unlimited.
  int32_t max_warnings = 30;

  // Maximum LM order. -1 means to use the largest order in the file.
  // If max_order is 1, it consumes ngram data up to unigram
  // If max_order is 2, it consumes ngram data up to bigram
  int32_t max_order = -1;
};

/**
   A parsed n-gram from ARPA LM file.
*/
struct NGram {
  std::vector<int32_t> words;  ///< Symbols in left to right order.
  float logprob = 0.0;         ///< Log-prob of the n-gram.
  float backoff = 0.0;         ///< log-backoff weight of the n-gram.
                               ///< Defaults to zero if not specified.
};

/**
    ArpaFileParser is an abstract base class for ARPA LM file conversion.

    See ConstArpaLmBuilder and ArpaLmCompiler for usage examples.
*/
class ArpaFileParser {
 public:
  /// Constructs the parser with the given options and optional symbol table.
  /// If symbol table is provided, then the file should contain text n-grams,
  /// and the words are mapped to symbols through it. bos_symbol and
  /// eos_symbol in the options structure must be valid symbols in the table,
  /// and so must be unk_symbol if provided. The table is not owned by the
  /// parser, but may be augmented, if oov_handling is set to kAddToSymbols.
  /// If symbol table is a null pointer, the file should contain integer
  /// symbol values, and oov_handling has no effect. bos_symbol and eos_symbol
  /// must be valid symbols still.
  ArpaFileParser(const ArpaParseOptions &options, fst::SymbolTable *symbols);
  virtual ~ArpaFileParser() = default;

  /// Read ARPA LM file from a stream.
  void Read(std::istream &is);

  /// Parser options.
  const ArpaParseOptions &Options() const { return options_; }

 protected:
  /// Override called before reading starts. This is the point to prepare
  /// any state in the derived class.
  virtual void ReadStarted() {}

  /// Override function called to signal that ARPA header with the expected
  /// number of n-grams has been read, and ngram_counts() is now valid.
  virtual void HeaderAvailable() {}

  /// Pure override that must be implemented to process current n-gram. The
  /// n-grams are sent in the file order, which guarantees that all
  /// (k-1)-grams are processed before the first k-gram is.
  virtual void ConsumeNGram(const NGram &) = 0;

  /// Override function called after the last n-gram has been consumed.
  virtual void ReadComplete() {}

  /// Read-only access to symbol table. Not owned, do not make public.
  const fst::SymbolTable *Symbols() const { return symbols_; }

  /// Inside ConsumeNGram(), provides the current line number.
  int32_t LineNumber() const { return line_number_; }

  /// Inside ConsumeNGram(), returns a formatted reference to the line being
  /// compiled, to print out as part of diagnostics.
  std::string LineReference() const;

  /// Increments warning count, and returns true if a warning should be
  /// printed or false if the count has exceeded the set maximum.
  bool ShouldWarn();

  /// N-gram counts. Valid from the point when HeaderAvailable() is called.
  const std::vector<int32_t> &NgramCounts() const { return ngram_counts_; }

 private:
  ArpaParseOptions options_;
  fst::SymbolTable *symbols_;  // the pointer is not owned here.
  int32_t line_number_;
  uint32_t warning_count_;
  std::string current_line_;
  std::vector<int32_t> ngram_counts_;
};

}  // namespace kaldilm

#endif  // KALDILM_CSRC_ARPA_FILE_PARSER_H_
