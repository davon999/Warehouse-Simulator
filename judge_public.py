#!/usr/bin/env python3
# Validator local pentru testele publice: verifica daca iesirea ta e valida si iti arata
# scorul obtinut (suma finala). Aceeasi logica de validare ca pe server.
#   python3 judge_public.py <intrare> <iesire>
import sys
import json
import os
from judge_core import evaluate


def main():
    in_path, out_path = sys.argv[1], sys.argv[2]
    try:
        ok, score, msg = evaluate(in_path, out_path)
    except Exception as e:  # noqa: BLE001
        print("Eroare la validare:", e)
        sys.exit(1)
    if not ok:
        print("IESIRE INVALIDA:", msg)
        sys.exit(1)
    prag = None
    here = os.path.dirname(os.path.abspath(__file__))
    lp = os.path.join(here, "tests", "public", "limits.json")
    if os.path.exists(lp):
        with open(lp, "r", encoding="utf-8") as f:
            limits = json.load(f)
        base = os.path.basename(in_path)
        prag = limits.get(base)
    print("VALID. Scor (suma finala) = %d" % score)
    if prag is not None:
        print("Prag test public = %d -> %s" % (prag, "TRECE" if score >= prag else "NU TRECE"))


if __name__ == "__main__":
    main()
