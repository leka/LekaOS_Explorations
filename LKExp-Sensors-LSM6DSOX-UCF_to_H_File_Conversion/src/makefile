#compiler
CC=g++ -Wall

#exec name
EXEC=ucf-to-h

#what will be run if user calls make
all: $(EXEC) rm_temp echoEnd


#compilation process
#first the upmost dependency
#then go down until executable
main.o: main.cpp hFileTemplate.h
	$(CC) -c $< 
	@echo  #

$(EXEC): main.o
	$(CC) -o $@ $^
	@echo  #

#clean the folder from temp files
rm_temp:
	@echo cleaning directory
	@rm -f *.o
	@rm -f *.gch

#clean the folder from temp files
clean: rm_temp
	@echo removing exec
	@rm -f $(EXEC)
	

#tell user exec name
echoEnd:
	@echo  #
	@echo  #
	@echo Project has been compiled, type \"./$(EXEC) --help\" to get more info
