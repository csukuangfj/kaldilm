
# arpa2fst

Python wrapper for kaldi's [arpa2fst][1].

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)][3]

# Installation

To install `kaldilm`, please run:

```bash
pip install kaldilm
```

Please create an issue [on GitHub](https://github.com/csukuangfj/kaldilm/issues/new)
if you encounter any problems while installing `kaldilm`.

# Usage

First, let us see the usage information of kaldi's arpa2fst:

```bash
kaldi/src/lmbin$ ./arpa2fst
./arpa2fst

Convert an ARPA format language model into an FST
Usage: arpa2fst [opts] <input-arpa> <output-fst>
 e.g.: arpa2fst --disambig-symbol=#0 --read-symbol-table=data/lang/words.txt lm/input.arpa G.fst

Note: When called without switches, the output G.fst will contain
an embedded symbol table. This is compatible with the way a previous
version of arpa2fst worked.

Options:
  --bos-symbol                : Beginning of sentence symbol (string, default = "<s>")
  --disambig-symbol           : Disambiguator. If provided (e. g. #0), used on input side of backoff links, and <s> and </s> are replaced with epsilons (string, default = "")
  --eos-symbol                : End of sentence symbol (string, default = "</s>")
  --ilabel-sort               : Ilabel-sort the output FST (bool, default = true)
  --keep-symbols              : Store symbol table with FST. Symbols always saved to FST if symbol tables are neither read or written
(otherwise symbols would be lost entirely) (bool, default = false)
  --max-arpa-warnings         : Maximum warnings to report on ARPA parsing, 0 to disable, -1 to show all (int, default = 30)
  --read-symbol-table         : Use existing symbol table (string, default = "")
  --write-symbol-table        : Write generated symbol table to a file (string, default = "")
```

`kaldilm` reuses the same arguments and provides only a single method [arpa2fst][2]:

```python
def arpa2fst(input_arpa: str,
             output_fst: str,
             bos_symbol: str = '<s>',
             disambig_symbol: str = '',
             eos_symbol: str = '</s>',
             ilabel_sort: bool = True,
             keep_symbols: bool = False,
             max_arpa_warnings: int = 30,
             read_symbol_table: str = '',
             write_symbol_table: str = '') -> str:
    '''Convert an ARPA file to an FST.

    This function is a wrapper of kaldi's arpa2fst and
    all the arguments have the same meaning with their counterparts in kaldi.

    Args:
      The input arpa file.
    output_fst:
      The output fst file. Note that it is a binary file.
      This function will return a text format of it.
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
      use existing symbol table.
    write_symbol_table:
      Write generated symbol table to a file.

    Returns:
      Return a text format of the resulting FST with integer labels.
    '''
```

## Example usage

Suppose you have an arpa file `input.arpa` with the following content:

```
\data\
ngram 1=4
ngram 2=2
ngram 3=2

\1-grams:
-5.234679	a -3.3
-3.456783	b
0.0000000	<s> -2.5
-4.333333	</s>

\2-grams:
-1.45678	a b -3.23
-1.30490	<s> a -4.2

\3-grams:
-0.34958	<s> a b
-0.23940	a b </s>

\end\
```

and the word symbol table is `words.txt`:

```
<eps>	0
<s>	1
</s>	2
a	3
b	4
#0 5
```

You can use the following code to convert it into an FST:

```python
#!/usr/bin/env python3

filename = './input.arpa'

import kaldilm

s = kaldilm.arpa2fst(filename,
                     'a.fst',
                     read_symbol_table='words.txt',
                     disambig_symbol='#0')
with open('a.fst.txt', 'w') as f:
    f.write(s)
```

It generates 2 files:

- `a.fst` (a binary file in OpenFST format)
- `a.fst.txt` (a text format of `a.fst` with integer labels)`

Their contents are shown below:

### cat a.fst.txt

```
2	4	3	3	3.00464
2	0	5	0	5.75646
0	1	3	3	12.0533
0	0	4	4	7.95954
0	9.97787
1	3	4	4	3.35436
1	0	5	0	7.59853
3	0	5	0	7.43735
3	0.551239
4	3	4	4	0.804938
4	1	5	0	9.67086
```

### fstprint a.fst

```
2       4       3       3       3.00464344
2       0       5       0       5.75646257
0       1       3       3       12.0532942
0       0       4       4       7.95953703
0       9.97786808
1       3       4       4       3.35435987
1       0       5       0       7.59853077
3       0       5       0       7.4373498
3       0.551238894
4       3       4       4       0.804937661
4       1       5       0       9.67085648
```

[3]: https://colab.research.google.com/drive/1rTGQiDDlhE8ezTH4kmR4m8vlvs6lnl6Z?usp=sharing
[2]: https://github.com/csukuangfj/kaldilm/blob/master/kaldilm/python/kaldilm/arpa2fst.py#L6
[1]: https://github.com/kaldi-asr/kaldi/blob/master/src/lmbin/arpa2fst.cc
