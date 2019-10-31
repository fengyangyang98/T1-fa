cc = gcc
cflag = 
python = python3
obj = main.o nfa.o csr.o util.o dfa.o re.o

test: $(obj)
	$(cc) $(cflag) -o fa $(obj)


	
main.o: main.c
	$(cc) $(cflag) -c main.c

csr.o: csr.c
	$(cc) $(cflag) -c csr.c 

util.o: util.c
	$(cc) $(cflag) -c util.c

nfa.o: nfa.c
	$(cc) $(cflag) -c nfa.c

dfa.o: dfa.c
	$(cc) $(cflag) -c dfa.c

re.o: re.c
	$(cc) $(cflag) -c re.c

	
right: fa
	@echo "The test example one:"
	@echo "------------------------"
	./fa -p ./dfa_test/1.dfa -f ./dfa_test/1.test
	@echo "------------------------"
	@echo "\n"	

	@echo "The test example two:"
	@echo "------------------------"
	./fa -p ./dfa_test/2.dfa -f ./dfa_test/2.test
	@echo "------------------------"
	@echo "\n"	

	@echo "The test example three:"
	@echo "------------------------"	
	./fa -p ./dfa_test/3.dfa -f ./dfa_test/3.test
	@echo "------------------------"


wrong: fa
	@echo "The state is not in the state set:"
	@echo "------------------------"
	./fa -p ./dfa_test/sub.dfa
	@echo "------------------------\n"

	@echo "The undermined next state:"
	@echo "------------------------"
	./fa -p ./dfa_test/tran.dfa
	@echo "-----------------------\n"

	@echo "The wrong stopping:"
	@echo "------------------------"
	./fa -p ./dfa_test/tran2.dfa
	@echo "-----------------------\n"

	@echo "No init state:"
	@echo "------------------------"
	./fa -p ./dfa_test/init.dfa
	@echo "-----------------------\n"

	@echo "No acceptable state:"
	@echo "------------------------"
	./fa -p ./dfa_test/acc.dfa
	@echo "-----------------------\n"

string: fa
	@echo "The test example one:"
	@echo "------------------------"	
	./fa -p ./dfa_test/3.dfa -f ./dfa_test/3-1.test
	@echo "------------------------\n"

	@echo "The test example two:"
	@echo "------------------------"	
	./fa -p ./dfa_test/3.dfa -f ./dfa_test/3-2.test
	@echo "------------------------"


1: fa
	@cat ./re_test/1.re
	@echo "\n"
	@./fa -r ./re_test/1.re							
	@./fa -e ./re_test/1.nfa -i
	@./fa -e ./re_test/1.nfa -v
	@./fa -d ./re_test/1.dfa.fine -m
	@./fa -d ./re_test/1min.dfa.fine -s bbabbab
	@./fa -d ./re_test/1min.dfa.fine  -s ''
	@./fa -d ./re_test/1min.dfa.fine -s c

	@echo "\n"

	./fa -d ./re_test/1.dfa.fine -a
	@echo "\n"
	./fa -e ./re_test/1.nfa -a
	@echo "\n"
	@./fa -d "./re_test/1.dfa.fine" -a

	@$(python) fa.py ./re_test/1.nfa


2: fa
	@cat ./re_test/2.re
	@echo "\n"
	@./fa -r ./re_test/2.re							
	@./fa -e ./re_test/2.nfa -i
	@./fa -e ./re_test/2.nfa -v
	@./fa -d ./re_test/2.dfa.fine -m
	@./fa -d "./re_test/2min.dfa.fine" -s aababbabb
	@./fa -d "./re_test/2min.dfa.fine" -s aababbb
	@echo "\n"

	./fa -d ./re_test/2.dfa.fine -a
	@echo "\n"
	./fa -e ./re_test/2.nfa -a
	@echo "\n"
	@./fa -d "./re_test/2.dfa.fine" -a

	@$(python) fa.py ./re_test/2.nfa

3: fa
	@cat ./re_test/3.re
	@echo "\n"
	@./fa -r ./re_test/3.re							
	@./fa -e ./re_test/3.nfa -i
	@./fa -e ./re_test/3.nfa -v
	@./fa -d ./re_test/3.dfa.fine -m
	@./fa -d "./re_test/3min.dfa.fine" -s aababba
	@./fa -d "./re_test/3min.dfa.fine" -s aababbab

	@echo "\n"
	./fa -d ./re_test/3.dfa.fine -a
	@echo "\n"
	./fa -e ./re_test/3.nfa -a
	@echo "\n"
	@./fa -d "./re_test/3.dfa.fine" -a

	@$(python) fa.py ./re_test/3.nfa

v: fa

	

t: fa
	@cat ./re_test/t.re
	@echo "\n"
	@./fa -r ./re_test/t.re							
	@./fa -e ./re_test/t.nfa -i
	@./fa -e ./re_test/t.nfa -v
	@$(python) fa.py ./re_test/t.nfa
	@./fa -d ./re_test/t.dfa.fine -m
	@./fa -d ./re_test/t-min.dfa.fine -s ab
	


.PHONY:clean

clean:
	rm *.o fa *.jpg *.gv