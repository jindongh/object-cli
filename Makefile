
all: objcli

CXXFLAGS := -g

objcli: obj_cli.o test_obj_cli.o
	${CXX} ${CXXFLAGS} -o $@ obj_cli.o test_obj_cli.o

clean:
	rm -fr *.o
	rm -fr objcli
