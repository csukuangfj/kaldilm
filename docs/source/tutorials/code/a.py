#!/usr/bin/env python3


import kenlm
from typing import List


def get_score(model: kenlm.Model, word: str, history: List[str]):
    """
    If history is [w0, w1, w2] and word is w3, the function returns
    p(w3|w0,w1,w2)

    Caution:
      The returned score is in log10.

    Args:
      model:
        The kenLM model.
      word:
        The current word.
      history:
        The history words.
    """
    order = model.order
    history = history[-(order - 1) :] if order > 1 else history

    in_state = kenlm.State()
    out_state = kenlm.State()
    model.NullContextWrite(in_state)

    for w in history:
        model.BaseScore(in_state, w, out_state)
        in_state, out_state = out_state, in_state

    print(model.BaseScore(in_state, word, out_state), f"{' '.join(history)} {word}")


def print_unigram(model):
    s1 = kenlm.State()
    s2 = kenlm.State()

    model.NullContextWrite(s1)
    for i in ["a", "b", "c", "d", "e", "</s>", "f"]:
        accum = model.BaseScore(s1, i, s2)
        print(accum, i)


def print_bigram(model):
    bigrams = ["<s> a", "<s> d", "a b", "a </s>", "b c", "c d", "d e", "e </s>"]
    bigrams += ["e f", "f a"]
    for b in bigrams:
        w0, w1 = b.split()
        s1 = kenlm.State()
        s2 = kenlm.State()
        model.NullContextWrite(s1)
        accum = model.BaseScore(s1, w0, s2)
        accum = model.BaseScore(s2, w1, s1)
        print(accum, b)


def print_trigram(model):
    trigrams = ["<s> a b", "a b c", "b c d", "c d e", "d e </s>"]
    trigrams += ["d e f", "<s> d e", "e f a", "f a </s>"]
    for t in trigrams:
        w0, w1, w2 = t.split()
        s1 = kenlm.State()
        s2 = kenlm.State()
        model.NullContextWrite(s1)
        accum = model.BaseScore(s1, w0, s2)
        accum = model.BaseScore(s2, w1, s1)
        accum = model.BaseScore(s1, w2, s2)
        print(accum, t)


def test():
    # Note: When we use p(x), we are actually referring to log10(p(x))
    model = kenlm.LanguageModel("./test.arpa")
    #  print_unigram(model)
    #  print_bigram(model)
    #  print_trigram(model)
    #  print(model.order)
    get_score(model, "a", [])
    get_score(model, "b", [])
    get_score(model, "c", [])
    get_score(model, "d", [])
    get_score(model, "e", [])
    get_score(model, "</s>", [])
    get_score(model, "f", [])

    get_score(model, "a", ["<s>"])
    get_score(model, "d", ["<s>"])
    get_score(model, "b", ["a"])
    get_score(model, "</s>", ["a"])
    get_score(model, "c", ["b"])
    get_score(model, "d", ["c"])
    get_score(model, "e", ["d"])
    get_score(model, "</s>", ["e"])
    get_score(model, "f", ["e"])
    get_score(model, "a", ["f"])

    get_score(model, "b", ["<s>", "a"])
    get_score(model, "c", ["a", "b"])
    get_score(model, "d", ["b", "c"])
    get_score(model, "e", ["c", "d"])
    get_score(model, "</s>", ["d", "e"])
    get_score(model, "f", ["d", "e"])
    get_score(model, "e", ["<s>", "d"])
    get_score(model, "a", ["e", "f"])
    get_score(model, "</s>", ["f", "a"])

    get_score(model, "b", ["<s>", "a"])
    get_score(model, "b", ["b", "c", "<s>", "a"])


def main():
    test()


if __name__ == "__main__":
    main()
