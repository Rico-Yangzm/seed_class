cc = gcc
OBJ = main
OBJS = main.o
$(OBJ):$(OBJS)
	$(cc) -std=c99 -Wall -Wextra -pedantic $(OBJS) -o $(OBJ)
$*.o: $%.c
	$(cc) -std=c99 -Wall -Wextra -pedantic -c $^ -o $@

.PHONY: clean
clean:
	$(RM) *.o main