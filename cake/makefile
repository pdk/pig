makeo = CC -g -c 
maker = CC -g -o 

misc.o : misc.c std.h
	$(makeo) misc.c

get_word.o : get_word.c get_word.h std.h
	$(makeo) get_word.c

cset.o : cset.c cset.h std.h
	$(makeo) cset.c

rule.o : rule.c rule.h std.h
	$(makeo) rule.c

table.o : table.c table.h std.h
	$(makeo) table.c

phonology.o : phonology.c phonology.h get_word.h table.h std.h
	$(makeo) phonology.c

lexicon.o : lexicon.c lexicon.h get_word.h table.h phonology.h std.h
	$(makeo) lexicon.c

network.o : network.c network.h table.h get_word.h lexicon.h std.h
	$(makeo) network.c

parse.o : parse.c parse.h std.h lexicon.h network.h misc.h
	$(makeo) parse.c

pig.o : pig.c std.h phonology.h lexicon.h parse.h network.h
	$(makeo) pig.c

all = rule.o cset.o get_word.o pig.o misc.o table.o phonology.o lexicon.o \
	network.o parse.o

pig : $(all)
	$(maker) pig $(all)

