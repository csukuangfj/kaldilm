# Copyright (c)  2020  Xiaomi Corporation (authors: Fangjun Kuang)

import _kaldilm


def arpa2fst(input_arpa: str,
             output_fst: str = '',
             bos_symbol: str = '<s>',
             disambig_symbol: str = '',
             eos_symbol: str = '</s>',
             ilabel_sort: bool = True,
             keep_symbols: bool = False,
             max_arpa_warnings: int = 30,
             read_symbol_table: str = '',
             write_symbol_table: str = '',
             max_order: int = -1) -> str:
    '''Convert an ARPA file to an FST.

    This function is a wrapper of kaldi's arpa2fst and
    all the arguments have the same meaning with their counterparts in kaldi.

    Args:
      input_arpa:
        The input arpa file.
      output_fst:
        The output fst file. Note that it is a binary file.
        This function will return a text format of it.
        If it is an empty string, no output file is written.
      bos_symbol:
        Beginning of sentence symbol.
      disambig_symbol:
        Disambiguator. If provided (e.g., #0), used on input side of backoff
        links, and <s> and </s> are replaced with epsilons.
      eos_symbol:
        End of sentence symbol.
      ilabel_sort:
        Ilabel-sort the output FST.
      keep_symbols:
        Store symbol table with FST. Symbols always saved to FST if symbol
        tables are neither read or written (otherwise symbols would be lost
        entirely).
      max_arpa_warnings:
        Maximum warnings to report on ARPA parsing, 0 to disable, -1 to
        show all.
      read_symbol_table:
        Use existing symbol table.
      write_symbol_table:
        Write generated symbol table to a file.
      max_order:
        Maximum order (inclusive) in the arpa file is used to generate the final FST.
        If it is -1, all ngram data in the file are used.
        If it is 1, only unigram data are used.
        If it is 2, only ngram data up to bigram are used.

    Returns:
      Return a text format of the resulting FST with integer labels.
    '''
    s = _kaldilm.arpa2fst(input_arpa=input_arpa,
                          output_fst=output_fst,
                          bos_symbol=bos_symbol,
                          disambig_symbol=disambig_symbol,
                          eos_symbol=eos_symbol,
                          ilabel_sort=ilabel_sort,
                          keep_symbols=keep_symbols,
                          max_arpa_warnings=max_arpa_warnings,
                          read_symbol_table=read_symbol_table,
                          write_symbol_table=write_symbol_table,
                          max_order=max_order)
    return s
