CC := cc
ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif
NAME_SYMBOLIC_LINK := libft_malloc.so
NAME_LINKED := $(addprefix ft_malloc_, $(HOSTTYPE))
NAME := $(addprefix libft_malloc_, $(HOSTTYPE))
NAME := $(addprefix $(NAME), .so)
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

CFLAGS := -MP -MMD -Wall -Werror -Wextra -std=gnu11 -pedantic-errors -fPIC -g
CFLAGS_TEST := -g
INCLUDE := -I includes/ -I libft/includes/

LIB := libft/libft.a

ifeq ($(MAKECMDGOALS),debug)
CFLAGS += -D DEBUG
endif

all: $(NAME) $(BIN)
debug: $(NAME) $(BIN)

$(NAME): $(OBJS)
	$(MAKE) --no-print-directory -C libft/
# 	ar rcs -o $(NAME) $(OBJS)
	$(CC) -shared $(CFLAGS) -o $@ $^
	ln -sf ./$(NAME) ./$(NAME_SYMBOLIC_LINK)

$(BIN): $(NAME)
	$(CC) test.c -o $@ -L. -l$(NAME_LINKED) $(LIB) $(CFLAGS_TEST)

$(OBJ_DIR)%.o: $(SRCS_DIR)%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE) $< -c -o $@

gdb: $(NAME)
	gdb -tui $(NAME)

clean:
	$(MAKE) clean -C libft/
	rm -rf $(OBJ_DIR)

fclean: clean
	$(MAKE) fclean -C libft/
	rm -rf $(NAME_SYMBOLIC_LINK)
	rm -rf $(NAME)
	rm -fr $(BIN)

re: fclean all

.PHONY: all debug fclean clean re gdb

-include $(DEPS)