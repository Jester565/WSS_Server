CC = g++ -std=c++11
CC_FLAGS = -w -g
LIBS = -lodbc -lboost_system -lboost_serialization -lboost_thread -lprotobuf -lcrypto -lssl -L/home/ubuntu/cproj/AsyncServerParent/AsyncServerParent/ -lasync_server -L/home/ubuntu/cproj/HTMLHandle/HTMLHandle/ -lhtml_handle -L/home/ubuntu/cproj/Boost_WS_Server/Boost_WS_Server/ -lws_server
INCLUDES = -I/home/ubuntu/cproj/AsyncServerParent/AsyncServerParent/ -I/home/ubuntu/cproj/HTMLHandle/HTMLHandle/ -I/home/ubuntu/cproj/Boost_WS_Server/Boost_WS_Server/

EXEC = run
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Main target                                                                                                                                                                                               
$(EXEC): $(OBJECTS)
	$(CC) -c $(OBJECTS) -o $(EXEC) $(LIBS)
	ar rcs libwss_server.a $(OBJECTS)

# To obtain object files                                                                                                                                                                                    
%.o: %.cpp
	$(CC) -c $(INCLUDES) $(CC_FLAGS) $< -o $@

# To remove generated files                                                                                                                                                                                 
clean:
	rm -f $(EXEC) $(OBJECTS)
