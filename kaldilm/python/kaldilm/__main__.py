#!/usr/bin/env python3
#
# Copyright (c)  2020  Xiaomi Corporation (authors: Fangjun Kuang)

if __name__ == '__main__':
    import argparse

    from .arpa2fst import arpa2fst

    def _str2bool(v):
        '''
        This function is modified from
        https://stackoverflow.com/questions/15008758/parsing-boolean-values-with-argparse
        '''
        if isinstance(v, bool):
            return v
        elif v.lower() in ('yes', 'true', 't', 'y', '1'):
            return True
        elif v.lower() in ('no', 'false', 'f', 'n', '0'):
            return False
        else:
            raise argparse.ArgumentTypeError('Boolean value expected.')

    parser = argparse.ArgumentParser("Python wrapper of kaldi's arpa2fst")

    parser.add_argument('--bos-symbol',
                        help='Beginning of sentence symbol (default = "<s>")',
                        default='<s>')
    parser.add_argument('--disambig-symbol',
                        help='Disambiguator. If provided (e.g., #0), '
                        'used on input side of backoff links, '
                        'and <s> and </s> are replaced '
                        'with epsilons (default = "")',
                        default='')
    parser.add_argument('--eos-symbol',
                        help='End of sentence symbol (default = "</s>")',
                        default='</s>')
    parser.add_argument('--ilabel-sort',
                        help='Ilabel-sort the output FST (default = true)',
                        type=_str2bool,
                        default=True)
    parser.add_argument('--keep-symbols',
                        help='Store symbol table with FST. '
                        'Symbols always saved to FST if symbol'
                        'tables are neither read or written '
                        '(otherwise symbols would be lost entirely) '
                        '(default = false)',
                        type=_str2bool,
                        default=False)
    parser.add_argument('--max-arpa-warnings',
                        help='Maximum warnings to report on ARPA parsing, '
                        '0 to disable, -1 to show all (default = 30)',
                        type=int,
                        default=30)
    parser.add_argument('--read-symbol-table',
                        help='Use existing symbol table (default = "")',
                        default='')
    parser.add_argument('--write-symbol-table',
                        help='(Write generated symbol table to '
                        'a file (default = "")',
                        default='')
    parser.add_argument('--max-order',
                        help='Maximum order (inclusive) in the arpa file is '
                        'used to generate the final FST. '
                        'If it is -1, all ngram data in the file are used.'
                        'If it is 1, only unigram data are used.'
                        'If it is 2, only ngram data up to bigram are used.'
                        'Default is -1.',
                        default=-1,
                        type=int)
    parser.add_argument('input_arpa', help='input arpa filename')
    parser.add_argument('output_fst',
                        default='',
                        nargs='?',
                        help='Output fst filename. '
                        'If empty, no output file is created.')
    args = parser.parse_args()

    s = arpa2fst(input_arpa=args.input_arpa,
                 output_fst=args.output_fst,
                 bos_symbol=args.bos_symbol,
                 disambig_symbol=args.disambig_symbol,
                 eos_symbol=args.eos_symbol,
                 ilabel_sort=args.ilabel_sort,
                 keep_symbols=args.keep_symbols,
                 max_arpa_warnings=args.max_arpa_warnings,
                 read_symbol_table=args.read_symbol_table,
                 write_symbol_table=args.write_symbol_table,
                 max_order=args.max_order)
    print(s)
