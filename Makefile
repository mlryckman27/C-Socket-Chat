build:
	gcc server.c -g -Wall -o server
	gcc client.c -g -Wall -o client

clean:
	rm server
	rm client 
