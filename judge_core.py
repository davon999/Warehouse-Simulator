#!/usr/bin/env python3
# Validator + evaluator pentru Tema 2 (Depozit portuar). Verifica fiecare actiune dupa
# regulile din enunt; o miscare ilegala inseamna scor 0. Intoarce (ok, scor, mesaj).
import sys


def _read_tokens(path):
    with open(path, "r", encoding="utf-8") as f:
        return f.read().split()


def parse_input(path):
    t = _read_tokens(path)
    i = 0
    L = int(t[i]); zile = int(t[i + 1]); bani = int(t[i + 2]); n = int(t[i + 3]); i += 4
    goods = {}
    for _ in range(n):
        nume = t[i]; timp = int(t[i + 1]); cost = int(t[i + 2]); pret = int(t[i + 3]); i += 4
        goods[nume] = {"timp": timp, "cost": cost, "pret": pret}
    assert t[i] == "ROTATIE", "sectiunea ROTATIE lipseste"
    m = int(t[i + 1]); i += 2
    isNA = {g: False for g in goods}
    allowed = {g: set() for g in goods}
    for _ in range(m):
        a = t[i]; b = t[i + 1]; i += 2
        if b == "N/A":
            isNA[a] = True
        else:
            allowed[a].add(b)
    return {"L": L, "zile": zile, "bani": bani, "goods": goods, "isNA": isNA, "allowed": allowed}


def _parse_cell(cell, L):
    if "x" not in cell:
        return None
    a, b = cell.split("x", 1)
    if len(a) != 2 or len(b) != 2 or not a.isdigit() or not b.isdigit():
        return None
    r, c = int(a), int(b)
    if r < 0 or r >= L or c < 0 or c >= L:
        return None
    return (r, c)


def evaluate(in_path, out_path):
    cfg = parse_input(in_path)
    L = cfg["L"]; zile = cfg["zile"]; goods = cfg["goods"]
    isNA = cfg["isNA"]; allowed = cfg["allowed"]
    money = cfg["bani"]

    # Mesajele de eroare NU contin text din iesire (ar putea fi copiat dintr-un test ascuns):
    # doar numarul liniei fizice, tipul erorii si valori calculate de arbitru (zi valida,
    # pozitia valida reformatata, marfa din catalogul testului, bani).
    with open(out_path, "r", encoding="utf-8") as f:
        numbered = [(k + 1, ln.strip()) for k, ln in enumerate(f) if ln.strip()]
    lines = [ln for _, ln in numbered]
    nrs = [k for k, _ in numbered]

    cells = {}

    def st(rc):
        if rc not in cells:
            cells[rc] = {"occ": False, "marfa": None, "load_day": 0, "ever": False,
                         "ship_day": -100, "ship_marfa": None}
        return cells[rc]

    prev_day = 0
    suma_declarata = None
    for pos, ln in enumerate(lines):
        nr = nrs[pos]
        parts = ln.split()
        if parts[0] == "SUMA_FINALA:" or (parts[0] == "SUMA_FINALA" and len(parts) >= 2):
            try:
                suma_declarata = int(parts[-1])
                suma_nr = nr
            except ValueError:
                return (False, money, "linia %d: SUMA_FINALA cu valoare invalida" % nr)
            break
        if len(parts) < 3:
            return (False, money, "linia %d: linie de actiune incompleta" % nr)
        try:
            zi = int(parts[0])
        except ValueError:
            return (False, money, "linia %d: ziua nu e numar intreg" % nr)
        act = parts[1]
        cell = parts[2]
        if zi < 1 or zi > zile:
            return (False, money, "linia %d: ziua in afara intervalului [1,%d]" % (nr, zile))
        if zi < prev_day:
            return (False, money, "linia %d: actiunile nu sunt in ordine crescatoare a zilelor (ziua %d dupa ziua %d)" % (nr, zi, prev_day))
        prev_day = zi
        rc = _parse_cell(cell, L)
        if rc is None:
            return (False, money, "linia %d: pozitia invalida (format LLxCC, intre 00 si %02d)" % (nr, L - 1))
        s = st(rc)
        cell = "%02dx%02d" % rc
        if act == "incarca":
            if len(parts) < 4:
                return (False, money, "linia %d: incarca fara marfa" % nr)
            marfa = parts[3]
            if marfa not in goods:
                return (False, money, "linia %d: marfa inexistenta in catalog" % nr)
            if s["occ"]:
                return (False, money, "linia %d: pozitia %s deja ocupata la ziua %d" % (nr, cell, zi))
            cost = goods[marfa]["cost"]
            if money < cost:
                return (False, money, "linia %d: bani insuficienti (%d < %d) pentru %s" % (nr, money, cost, marfa))
            if not s["ever"]:
                if not isNA[marfa]:
                    return (False, money, "linia %d: pozitia noua %s: doar marfa cu N/A poate fi incarcata (nu %s)" % (nr, cell, marfa))
            else:
                if zi < s["ship_day"] + 1:
                    return (False, money, "linia %d: reincarcare prea devreme in %s (minim 1 zi de la expediere)" % (nr, cell))
                if marfa == s["ship_marfa"] and zi < s["ship_day"] + 3:
                    return (False, money, "linia %d: aceeasi marfa in %s prea devreme (minim 3 zile)" % (nr, cell))
                if not (isNA[marfa] or s["ship_marfa"] in allowed[marfa]):
                    return (False, money, "linia %d: succesiune invalida in %s: %s nu poate urma dupa %s" % (nr, cell, marfa, s["ship_marfa"]))
            money -= cost
            s["occ"] = True; s["marfa"] = marfa; s["load_day"] = zi; s["ever"] = True
        elif act == "expediaza":
            if not s["occ"]:
                return (False, money, "linia %d: expediere de pe o pozitie goala: %s la ziua %d" % (nr, cell, zi))
            marfa = s["marfa"]
            if zi < s["load_day"] + goods[marfa]["timp"]:
                return (False, money, "linia %d: expediere inainte ca marfa sa fie gata in %s (ziua %d < %d)" % (nr, cell, zi, s["load_day"] + goods[marfa]["timp"]))
            money += goods[marfa]["pret"]
            s["occ"] = False; s["marfa"] = None
            s["ship_day"] = zi; s["ship_marfa"] = marfa
        else:
            return (False, money, "linia %d: actiune necunoscuta (doar incarca / expediaza)" % nr)

    if suma_declarata is None:
        return (False, money, "lipseste linia SUMA_FINALA")
    if suma_declarata != money:
        return (False, money, "linia %d: SUMA_FINALA declarata != suma reala (%d)" % (suma_nr, money))
    return (True, money, "ok")


if __name__ == "__main__":
    ok, score, msg = evaluate(sys.argv[1], sys.argv[2])
    print("OK" if ok else "ILEGAL", "scor=%d" % score, msg)
