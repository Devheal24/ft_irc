# COLOR
GREEN   	:= \033[1;38;5;46m
RESET   	:= \033[0m

NAME		:= ircserv
CPP			:= c++
CPP_FLAGS	:= -Wall -Werror -Wextra -std=c++98 -g -fPIE
INCLUDES	:= -Iincludes

# DIR
SRCS_DIR	:= srcs/
OBJS_DIR	:= objs/

SRCS		:= $(SRCS_DIR)main.cpp \
				$(SRCS_DIR)Server.cpp \
				$(SRCS_DIR)Client.cpp \
				$(SRCS_DIR)Channel.cpp \
				$(SRCS_DIR)Commands.cpp

OBJS		:= $(SRCS:$(SRCS_DIR)%.cpp=$(OBJS_DIR)%.o)

all:		$(NAME)

$(NAME):	$(OBJS)
			@$(CPP) $(CPP_FLAGS) $(INCLUDES) $(OBJS) -o $(NAME)
			@printf "${GREEN}\r[▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬] SUCCESS 100%%${RESET}\n"

TOTAL		:= $(words $(SRCS))
COUNT		:= 0

$(OBJS_DIR)%.o: $(SRCS_DIR)%.cpp
			@mkdir -p $(dir $@)
			@$(CPP) $(CPP_FLAGS) $(INCLUDES) -c $< -o $@
			@$(eval COUNT=$(shell echo $$(($(COUNT)+1))))
			@PERCENT=$$(($(COUNT)*99/$(TOTAL))) ; \
			BAR=$$(($(COUNT)*39/$(TOTAL))) ; \
			if [ $$PERCENT -lt 33 ]; then \
				COLOR_CODE=196; \
			elif [ $$PERCENT -lt 66 ]; then \
				COLOR_CODE=208; \
			else \
				COLOR_CODE=226; \
			fi ; \
			printf "\033[38;5;%sm\r[" $$COLOR_CODE ; \
			i=1; while [ $$i -le $$BAR ]; do printf "▬"; i=$$((i+1)); done ; \
			while [ $$i -le 40 ]; do printf " "; i=$$((i+1)); done ; \
			printf "] LOADING %3d%%\033[0m" $$PERCENT


clean:
			@if ls $(OBJS) >/dev/null 2>&1; then \
			echo "${GREEN}====   $(NAME)   ==== : >>>OBJS CLEANED<<<${RESET}"; \
			fi
			@rm -f $(OBJS)
			@rm -rf $(OBJS_DIR)

fclean:		clean
			@if ls $(NAME) >/dev/null 2>&1; then \
			echo "${GREEN}====   $(NAME)   ==== : >>>ALL CLEANED<<<${RESET}"; \
			fi
			@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
