NAME = hashtable
TEST = hashfunc_test
COMMAND = hashmonkey
CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2 -ggdb3
LDFLAGS = -lreadline
SRC = $(addprefix hashtable_src/, main.c  hashmonkey.c  commands.c  utils.c )
SRC_TEST = $(addprefix test_src/, main.c  hashmonkey.c )
SRC_COMMAND = $(addprefix command_src/, main.c  hashmonkey.c )
OBJ = ${SRC:.c=.o}
OBJ_TEST = ${SRC_TEST:.c=.o}
OBJ_COMMAND = ${SRC_COMMAND:.c=.o}

all : ${NAME} ${TEST} ${COMMAND}

${NAME}: ${OBJ}
		echo "Compiling main program"
		${CC} ${OBJ} ${CFLAGS} ${LDFLAGS} -o ${NAME}

${TEST}: ${OBJ_TEST}
		echo "Compiling test program"
		${CC} ${OBJ_TEST} ${CFLAGS} -o ${TEST}

${COMMAND}: ${OBJ_COMMAND}
		echo "Compiling command"
		${CC} ${OBJ_COMMAND} ${CFLAGS} -o ${COMMAND}

%.o: %.c
		${CC} -c ${<} -o ${@} ${CFLAGS}

clean:
		rm -rf ${OBJ}

fclean: clean
		rm -rf ${NAME}

re : fclean all
