SRCS =	main.cpp \
		./srcs/tools.cpp \
		./srcs/Config.cpp \
		./srcs/Checker.cpp \

NAME = webserv

CPPC = c++

CPPCFLAGS = -Wall -Wextra -Werror

RM = rm -f

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CPPC) $(CPPCFLAGS) $(OBJS) -o $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re