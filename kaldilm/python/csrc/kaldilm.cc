// kaldilm/python/csrc/kaldilm.cc
//
// Copyright (c)  2020  Xiaomi Corporation (authors: Fangjun Kuang)

#include "kaldilm/python/csrc/kaldilm.h"

#include <fstream>

#include "fst/fstlib.h"
#include "fst/script/print.h"
#include "fst/symbol-table.h"
#include "kaldilm/csrc/arpa_file_parser.h"
#include "kaldilm/csrc/arpa_lm_compiler.h"
#include "kaldilm/csrc/log.h"

namespace kaldilm {

template <class Arc>
static void PrintFstInTextFormat(std::ostream &os,
                                 const fst::VectorFst<Arc> &t) {
  bool ok;
  // Text-mode output.  Note: we expect that t.InputSymbols() and
  // t.OutputSymbols() would always return NULL.  The corresponding input
  // routine would not work if the FST actually had symbols attached.  Write a
  // newline to start the FST; in a table, the first line of the FST will
  // appear on its own line.
  os << '\n';
  bool acceptor = false, write_one = false;
  // fst::FstPrinter<Arc> printer(t, t.InputSymbols(), t.OutputSymbols(), NULL,
  //                              acceptor, write_one, "\t");

  fst::FstPrinter<Arc> printer(t, nullptr, nullptr, nullptr, acceptor,
                               write_one, "\t");
  printer.Print(&os, "<unknown>");
  if (os.fail()) KALDILM_ERR << "Stream failure detected writing FST to stream";
  // Write another newline as a terminating character.  The read routine will
  // detect this [this is a Kaldi mechanism, not something in the original
  // OpenFst code].
  os << '\n';
  ok = os.good();

  if (!ok) KALDILM_ERR << "Error writing FST to stream";
}

std::string Arpa2Fst(const std::string &input_arpa,
                     const std::string &output_fst = "",
                     const std::string bos_symbol = "<s>",
                     const std::string &disambig_symbol = "",
                     const std::string &eos_symbol = "</s>",
                     bool ilabel_sort = true, bool keep_symbols = false,
                     int32_t max_arpa_warnings = 30,
                     const std::string &read_symbol_table = "",
                     const std::string &write_symbol_table = "",
                     int32_t max_order = -1) {
  ArpaParseOptions options;
  options.max_order = max_order;
  options.max_warnings = max_arpa_warnings;

  std::string read_syms_filename = read_symbol_table;
  std::string write_syms_filename = write_symbol_table;

  std::string arpa_rxfilename = input_arpa;
  std::string fst_wxfilename = output_fst;

  int64 disambig_symbol_id = 0;

  fst::SymbolTable *symbols;
  if (!read_syms_filename.empty()) {
    // Use existing symbols. Required symbols must be in the table.
    std::ifstream kisym(read_syms_filename);
    symbols = fst::SymbolTable::ReadText(kisym, read_syms_filename);
    if (symbols == nullptr)
      KALDILM_ERR << "Could not read symbol table from file "
                  << read_syms_filename;

    options.oov_handling = ArpaParseOptions::kSkipNGram;
    if (!disambig_symbol.empty()) {
      disambig_symbol_id = symbols->Find(disambig_symbol);
      if (disambig_symbol_id == -1)  // fst::kNoSymbol
        KALDILM_ERR << "Symbol table " << read_syms_filename
                    << " has no symbol for " << disambig_symbol;
    }
  } else {
    // Create a new symbol table and populate it from ARPA file.
    symbols = new fst::SymbolTable(fst_wxfilename);
    options.oov_handling = ArpaParseOptions::kAddToSymbols;
    symbols->AddSymbol("<eps>", 0);
    if (!disambig_symbol.empty()) {
      disambig_symbol_id = symbols->AddSymbol(disambig_symbol);
    }
  }

  // Add or use existing BOS and EOS.
  options.bos_symbol = symbols->AddSymbol(bos_symbol);
  options.eos_symbol = symbols->AddSymbol(eos_symbol);

  // If producing new (not reading existing) symbols and not saving them,
  // need to keep symbols with FST, otherwise they would be lost.
  if (read_syms_filename.empty() && write_syms_filename.empty())
    keep_symbols = true;

  // Actually compile LM.
  KALDILM_ASSERT(symbols != nullptr);
  ArpaLmCompiler lm_compiler(options, disambig_symbol_id, symbols);
  {
    std::fstream ki(arpa_rxfilename);
    lm_compiler.Read(ki);
  }

  // Sort the FST in-place if requested by options.
  if (ilabel_sort) {
    fst::ArcSort(lm_compiler.MutableFst(), fst::StdILabelCompare());
  }

  // Write symbols if requested.
  if (!write_syms_filename.empty()) {
    std::ofstream kosym(write_syms_filename);
    symbols->WriteText(kosym);
  }

  // Write LM FST.
  if (fst_wxfilename.size() > 0) {
    std::ofstream kofst(fst_wxfilename, std::ios::binary);
    fst::FstWriteOptions wopts(fst_wxfilename);
    wopts.write_isymbols = wopts.write_osymbols = keep_symbols;
    lm_compiler.Fst().Write(kofst, wopts);
  }

  delete symbols;

  std::ostringstream os;
  PrintFstInTextFormat<fst::StdArc>(os, lm_compiler.Fst());
  return os.str();
}

}  // namespace kaldilm

PYBIND11_MODULE(_kaldilm, m) {
  m.doc() = "Python wrapper for kaldilm";
  m.def("arpa2fst", &kaldilm::Arpa2Fst, py::arg("input_arpa"),
        py::arg("output_fst") = "", py::arg("bos_symbol") = "<s>",
        py::arg("disambig_symbol") = "", py::arg("eos_symbol") = "</s>",
        py::arg("ilabel_sort") = true, py::arg("keep_symbols") = false,
        py::arg("max_arpa_warnings") = 30, py::arg("read_symbol_table") = "",
        py::arg("write_symbol_table") = "", py::arg("max_order") = -1);
}
