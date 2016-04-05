TEX=$(wildcard *.tex)
PDF=Modern-Fuzzing-with-afl
BUILD=pdflatex
SRC=Modern-Fuzzing-with-afl

all: ${PDF}

${PDF}: ${TEX}
	${BUILD} ${SRC}
	bibtex ${SRC}.aux
	${BUILD} ${SRC}
	bibtex ${SRC}.aux
	bibtex ${SRC}.aux
	${BUILD} ${SRC}
	${BUILD} ${SRC}

clean:
	rm -rf *.pdf *.aux *.log *.out *.glg *.glo *.gls *.ist *.lof *.lot *.toc *.xdy *.glsdefs *.loa *.blg *.bbl

.PHONY: clean
