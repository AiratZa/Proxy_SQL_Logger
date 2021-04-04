NAME = proxy_sql_logger

FLAGS = -std=c++98 -Wall -Wextra -Werror

CORE_SRCS = ProxyServer.cpp Client.cpp Config.cpp utils.cpp

SRCS =  $(CORE_SRCS) main.cpp

OBJS = $(SRCS:.cpp=.o)

MAKE_DEPS = $(SRCS:.cpp=.d)

all: $(NAME)

$(NAME): $(OBJS)
	g++ $(FLAGS) $(OBJS) -I./ -o $(NAME)

clean:
	rm -rf $(OBJS) $(MAKE_DEPS)

fclean: clean
	rm -rf $(NAME)

re:	fclean $(NAME)

%.o: %.cpp
	g++ $(FLAGS) -c $< -o $@  -MD

include $(wildcard $(MAKE_DEPS))

.PHONY: all clean fclean re
