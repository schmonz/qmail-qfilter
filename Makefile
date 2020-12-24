all:
	cc -c parse_sender.c
	cc -c -I/opt/pkg/include test_parse_sender.c
	cc -o test_parse_sender -L/opt/pkg/lib -lcheck test_parse_sender.o parse_sender.o
	./test_parse_sender
