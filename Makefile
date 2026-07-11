CC := cc
NAME := ft_malloc
SRCS_DIR := srcs/
SOURCES :=	ft_malloc.c	\

SOURCES	:=	$(addprefix $(SRCS_DIR), $(SOURCES))

OBJ_DIR := .build/
OBJS := $(SOURCES:$(SRCS_DIR)%.c=$(OBJ_DIR)%.o)

DEPS := $(SOURCES:$(SRCS_DIR)%.c=$(OBJ_DIR)%.d)

CFLAGS := -MP -MMD -Wall -Werror -Wextra -std=c11 -pedantic-errors -g
INCLUDE := -I includes/ -I libft/includes/
LIB := libft/libft.a

all: $(NAME)

$(NAME): $(OBJS)
	$(MAKE) --no-print-directory -C libft/
	$(CC) $(OBJS) $(CFLAGS) $(INCLUDE) $(LIB) -o $(NAME)

$(OBJ_DIR)%.o: $(SRCS_DIR)%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE) $< -c -o $@

gdb: $(NAME)
	gdb -tui $(NAME)

clean:
	$(MAKE) clean --no-print-directory -C libft/
	rm -rf $(OBJ_DIR)

fclean: clean
	$(MAKE) fclean --no-print-directory -C libft/
	rm -rf $(NAME)

re: fclean all

.PHONY: all fclean clean re gdb

-include $(DEPS)