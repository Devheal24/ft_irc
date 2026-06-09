NAME		:= irc
CPP			:= c++
CPP_FLAGS	:= -Wall -Werror -Wextra -std=c++98 -g
INCLUDES	:= -Iincludes
SRCS_DIR	:= srcs/
OBJS_DIR	:= objs/

SRCS		:= \

OBJS		:= $(SRCS:$(SRCS_DIR)%.cpp=$(OBJS_DIR)%.o)

all: $(NAME)
	@echo "prgrm created!"

$(NAME): $(OBJS)
	@echo "Creating program."
	@$(CPP) $(CPP_FLAGS) $(INCLUDES) $(OBJS) -o $(NAME)

$(OBJS_DIR)%.o: $(SRCS_DIR)%.cpp
	@echo "Compiling mandatory part files..."
	@mkdir -p $(dir $@)
	@$(CPP) $(CPP_FLAGS) $(INCLUDES) -c $< -o $@

clean:
	@clear
	@echo "Removing object files."
	@rm -f $(OBJS)

fclean: clean
	@echo "Removing program file."
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
