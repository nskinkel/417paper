TEX=$(wildcard *.tex)
PDF=Modern-Fuzzing-with-afl
BUILD=pdflatex
SRC=Modern-Fuzzing-with-afl

all: ${PDF}

${PDF}: ${TEX}
	${BUILD} ${SRC}
	${BUILD} ${SRC}

clean:
	rm -rf *.pdf *.aux *.log *.out *.glg *.glo *.gls *.ist *.log *.lot *.toc *.xdy *.glsdefs *.vrb *.nav *.snm

.PHONY: clean
