# Makefile pentru Tema 2 - Depozit portuar.
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

app: main.cpp depozit.cpp depozit.h
	$(CXX) $(CXXFLAGS) -o app main.cpp depozit.cpp

run: app
	./app tests/public/01.in out.txt
	python3 judge_public.py tests/public/01.in out.txt

clean:
	rm -f app out.txt

.PHONY: run clean
