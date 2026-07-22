CC := cc
NAME := libft_malloc_x86_64_Linux.so
BIN := a.out
SRCS_DIR := srcs/
SOURCES :=	ft_malloc.c	\
			page.c		\
			show_mem.c	\
			ft_free.c	\
			ft_realloc.c\

SOURCES	:=	$(addprefix $(SRCS_DIR), $(SOURCES))

OBJ_DIR := .build/
OBJS := $(SOURCES:$(SRCS_DIR)%.c=$(OBJ_DIR)%.o)

DEPS := $(SOURCES:$(SRCS_DIR)%.c=$(OBJ_DIR)%.d)

CFLAGS := -MP -MMD -Wall -Werror -Wextra -std=gnu11 -pedantic-errors -g
CFLAGS_TEST := -g
INCLUDE := -I includes/ -I libft/includes/

ifeq ($(MAKECMDGOALS),debug)
CFLAGS += -D DEBUG
endif

all: $(NAME) $(BIN)
debug: $(NAME) $(BIN)

$(NAME): $(OBJS)
	ar rcs $@ $^

$(BIN): $(NAME)
	$(CC) test.c $(NAME) $(CFLAGS_TEST)

$(OBJ_DIR)%.o: $(SRCS_DIR)%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE) $< -c -o $@

gdb: $(NAME)
	gdb -tui $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)
	rm -fr $(BIN)

re: fclean all

.PHONY: all debug fclean clean re gdb

-include $(DEPS)