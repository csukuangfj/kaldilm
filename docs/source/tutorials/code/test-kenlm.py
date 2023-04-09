#!/usr/bin/env python3

import kenlm


def test():
    # Note: When we use p(x), we are actually referring to log10(p(x))
    model = kenlm.LanguageModel("./test.arpa")
    # model.score() return probability in log10()

    # p("a") in log10
    assert abs(model.score("a", bos=False, eos=False) - (-0.6989700)) < 1e-5

    # p(a|<s>) in log10
    assert abs(model.score("a", eos=False) - (-0.2041200)) < 1e-5

    # p(a b | <s>)
    # = p(a | <s>) + p(b | <s> a)
    # = (-0.204120) + (-0.0280287)
    # = -0.23214869
    #  print(model.score("a b", eos=False, bos=True))
    assert abs(model.score("a b", eos=False, bos=True) - (-0.23214869)) < 1e-5

    # p(a b </s> | <s>)
    # = p(a | <s>) + p(b | <s> a) + p(</s> | a b)
    # = (-0.204120) + (-0.0280287) + backoff(a b) + p(</s> | b)
    # = (-0.204120) + (-0.0280287) + backoff(a b) + p(</s> | b)
    # = (-0.204120) + (-0.0280287) + (-0.3010300) + backoff(b) + p(</s>)
    # = (-0.204120) + (-0.0280287) + (-0.3010300) + (-0.8573325) + (-0.6989700)
    # = -2.0894812
    #  print(model.score("a b", eos=True, bos=True))
    assert abs(model.score("a b", eos=True, bos=True) - (-2.0894812)) < 1e-5
    # Pay attention to the computation of p(</s> | a b)
    # p(</s> | a b)
    # = backoff (a b) + p (</s> | b)
    # = backoff (a b) + backoff(b) + p(</s>)
    #
    # Also note that p(</s>) is 0

    # p(b d </s> | <s>)
    # = p(b | <s>) + p (d | <s> b) + p(</s> | b d)
    # = backoff(<s>) + p(b) + p (d | <s> b) + p(</s> | b d)
    # = backoff(<s>) + p(b) + backoff(<s> b) + p(d | b) + p(</s> | b d)
    # = backoff(<s>) + p(b) + backoff(<s> b) + backoff(b) +  p(d) + p(</s> | b d)
    # = backoff(<s>) + p(b) + backoff(<s> b) + backoff(b) +  p(d) + backoff(b d) + p(</s> | d)
    # = backoff(<s>) + p(b) + backoff(<s> b) + backoff(b) +  p(d) + backoff(b d) + backoff(d) + p(</s>)
    # = (-0.8573325) + (-1.0000000) +  0     + (-0.8573325) + (-0.6989700) + 0   + (-1.1583625) + (-0.6989700)
    # = -5.2709675
    #  print(model.score("b d", eos=True, bos=True))
    assert abs(model.score("b d", eos=True, bos=True) - (-5.2709675)) < 1e-5
    print(model.score("b d", eos=True, bos=True))
    print(list(model.full_scores("b d", eos=True, bos=True)))
    # Note:
    # p(b | <s>) = backoff(<s>) + p(b)
    #
    # p(d | <s> b) = backoff(<s> b) + p (d | b)
    # since backoff (<s> b) does not exist, so it is 0
    #
    # p(d | b) = backoff(b) + p(d)
    #
    # p (</s> | b d) = backoff(b d) + p(</s> | d)
    #                = backoff(b d) + backoff(d) + p(</s>)


def test_statefull():
    model = kenlm.LanguageModel("./test.arpa")
    s1 = kenlm.State()
    s2 = kenlm.State()
    model.BeginSentenceWrite(s1)
    accum = model.BaseScore(s1, "a", s2)  # p(a | <s>)
    #  print(accum)  # -0.2041200
    assert abs(accum - model.score("a", bos=True, eos=False)) < 1e-5
    accum += model.BaseScore(s2, "b", s1)  # p(a | <s>) +  p(b | <s> a)
    #  print(accum)  # -0.23214869
    assert abs(accum - model.score("a b", bos=True, eos=False)) < 1e-5

    # reset
    s1 = kenlm.State()
    s2 = kenlm.State()
    model.BeginSentenceWrite(s1)
    accum = model.BaseScore(s1, "b", s2)  # p(b | <s>)
    #  print(accum)  # -1.857332
    assert abs(accum - model.score("b", bos=True, eos=False)) < 1e-5
    # backoff(<s>) + p(b) = -0.8573325 + (-1) = -1.8573325
    accum += model.BaseScore(s2, "c", s1)  # p(b | <s>) + p(c | b)
    #  print(accum)  # -1.91532436
    # p(b | <s>) + p(c | b) = -1.8573325 + (-0.0579919) = -1.9153244
    assert abs(accum - model.score("b c", bos=True, eos=False)) < 1e-5

    accum += model.BaseScore(s1, "d", s2)  # p(b | <s>) + p(c | b) + p(d | b c)
    #  print(accum)
    # p(b | <s>) + p(c | b) + p(d | b c) = -1.9153244 + (-0.0280287) = -1.9433531
    assert abs(accum - model.score("b c d", bos=True, eos=False)) < 1e-5

    # now for oov
    # reset
    s1 = kenlm.State()
    s2 = kenlm.State()
    model.BeginSentenceWrite(s1)
    accum = model.BaseScore(s1, "g", s2)  # p(g | <s>)
    # p(g | <s>) = backoff(<s>) + p(<unk>) = -0.8573325 + (-100) = -100.8573325
    print(accum)  # -100.8573325
    for i in ["a", "b", "c", "d", "e", "f", "</s>"]:
        assert (
            abs(model.BaseScore(s2, i, s1) - model.score(i, bos=False, eos=False))
            < 1e-5
        )
    #  print(model.BaseScore(s2, "kk", s1)) # -100
    accum += model.BaseScore(s2, "a", s1)  # p(g | <s>) + p(a)
    print(accum)
    assert abs(accum - model.score("g a", bos=True, eos=False)) < 1e-5
    accum += model.BaseScore(s1, "b", s2)  # p(g | <s>) + p(a) + p(b|a)
    print(accum)


def main():
    test()
    test_statefull()


if __name__ == "__main__":
    main()
