
DEBUG = 1
ifeq ($(DEBUG), 0)
	CPP = g++ -O3 -Wall
else
	CPP = g++ -O0 -Wall -ggdb3
endif

YACC = bison
LEX = flex

TGT = cs_ssa

BASE_OBJ = main.o error.o program.o procedure.o
BASE_HEADERS = error.hh program.hh procedure.hh
CFG_OBJ = cfg/cfg.scan.o cfg/cfg.tab.o cfg/cfg_edge.o cfg/cfg_node.o cfg/cfg_opd.o
CFG_HEADERS = cfg/cfg_edge.hh cfg/cfg_node.hh cfg/cfg_opd.hh
SSA_OBJ = ssa/ssa.scan.o ssa/ssa.tab.o ssa/ssa_edge.o ssa/ssa_meta.o ssa/ssa_node.o ssa/ssa_opd.o ssa/ssa_stmt.o
SSA_HEADERS = ssa/ssa_edge.hh ssa/ssa_meta.hh ssa/ssa_node.hh ssa/ssa_opd.hh ssa/ssa_stmt.hh
DDG_OBJ = ddg/ddg_edge.o ddg/ddg_node.o
DDG_HEADERS = ddg/ddg_edge.hh ddg/ddg_node.hh

HEADERS = $(BASE_HEADERS) $(CFG_HEADERS) $(SSA_HEADERS) $(DDG_HEADERS)

all: $(TGT)

$(TGT): $(BASE_OBJ) $(CFG_OBJ) $(SSA_OBJ) $(DDG_OBJ)
	$(CPP) $(BASE_OBJ) $(CFG_OBJ) $(SSA_OBJ) $(DDG_OBJ) -o $(TGT) -ly -ll
ifeq ($(DEBUG), 0)
	strip cs_ssa
endif

main.o: main.cc argparse.hh cfg/cfg.tab.hh ssa/ssa.tab.hh $(HEADERS)
	$(CPP) -c main.cc

error.o: error.cc error.hh stacktrace.h
	$(CPP) -c error.cc

program.o: program.cc $(HEADERS)
	$(CPP) -c program.cc

procedure.o: procedure.cc $(HEADERS)
	$(CPP) -c procedure.cc

cfg/cfg.tab.cc cfg/cfg.tab.hh: cfg/cfg.y
	$(YACC) -o cfg/cfg.tab.cc -dv cfg/cfg.y

cfg/cfg.scan.cc: cfg/cfg.l cfg/cfg.tab.hh
	$(LEX) -o cfg/cfg.scan.cc --yylineno cfg/cfg.l

ssa/ssa.tab.cc ssa/ssa.tab.hh: ssa/ssa.y
	$(YACC) -o ssa/ssa.tab.cc -dv ssa/ssa.y

ssa/ssa.scan.cc: ssa/ssa.l ssa/ssa.tab.hh
	$(LEX) -o ssa/ssa.scan.cc --yylineno ssa/ssa.l

cfg/%.o: cfg/%.cc $(HEADERS)
	$(CPP) -c $< -o $@

ssa/%.o: ssa/%.cc $(HEADERS)
	$(CPP) -c $< -o $@

ddg/%.o: ddg/%.cc $(HEADERS)
	$(CPP) -c $< -o $@

clean:
	rm -f $(TGT)
	rm -f *.o *.output
	rm -f cfg/*.o cfg/*.output
	rm -f cfg/cfg.scan.* cfg/cfg.tab.*
	rm -f ssa/*.o ssa/*.output
	rm -f ssa/ssa.scan.* ssa/ssa.tab.*
	rm -f ddg/*.o ddg/*.output
	rm -f ddg/ddg.scan.* ddg/ddg.tab.*
