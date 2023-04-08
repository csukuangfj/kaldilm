ARPA format
===========

How to generate an arpa file
----------------------------

Given the following text,

.. code-block:: bash

   a b c d e
   d e f a
   a b c d e f a


we can convert it to a n-gram LM saved in `ARPA format <https://cmusphinx.github.io/wiki/arpaformat/>`_
using the script `shared/make_kn_lm.py <https://github.com/k2-fsa/icefall/blob/master/icefall/shared/make_kn_lm.py>`_:

.. code-block:: bash

   cat > test.txt <<EOF
   a b c d e
   d e f a
   a b c d e f a
   EOF

   wget https://raw.githubusercontent.com/k2-fsa/icefall/master/icefall/shared/make_kn_lm.py

   python3 ./make_kn_lm.py -ngram-order 3 -text ./test.txt -lm test.arpa

The content of ``test.arpa`` is given below:

.. literalinclude:: ./code/test.arpa

How to interpret an arpa file
-----------------------------

.. code-block:: bash

  \data\
  ngram 1=8
  ngram 2=10
  ngram 3=9

An arpa file begins with the literal string ``\data\`` in the first line.
The lines that follow it contain the number of entries for each order:

  - ``ngram 1=8``: There are 8 entries for unigram
  - ``ngram 2=10``: There are 10 entries for bigram
  - ``ngram 3=9``: There are 9 entries for trigram. The highest order of this file
    is 3.

.. code-block:: bash

  \1-grams:
  -99.0000000	<s>	-0.8573325
  -0.6989700	a	-0.7481880
  -1.0000000	b	-0.8573325
  -1.0000000	c	-0.8061800
  -0.6989700	d	-1.1583625
  -1.0000000	e	-0.7481880
  -0.6989700	</s>
  -1.0000000	f	-0.8061800

``\1-grams:`` means the following entries belong to unigram.
Each entry of unigram has 2 or 3 columns.

  - Column 0: probability in :math:`\log_{10}`, i.e., :math:`\log_{10}(p)`
  - Column 1: the word
  - Column 2: back-off probability in :math:`\log_{10}`, If this column is absent, it is
    :math:`\log_{10}(1) = 0` by default

.. caution::

   .. math::

      \log(p) = \frac{\log_{10}(p)}{\log_{10}\mathrm{e}} = \log_{10}(p) \log(10) = \log_{10}(p) \times 2.302585092994046

   .. math::

      \log_{10}(p) = \frac{\log(p)}{\log(10)} = \frac{\log(p)}{2.302585092994046} = \log(p) \times 0.4342944819032518
