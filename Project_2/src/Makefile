JFLAGS = -g
JC = javac
JV = java
all: server client

server: server.java
	$(JC) $(JFLAGS) $<

client: client.java
	$(JC) $(JFLAGS) $<

run-server: server
	$(JV) $< 127.0.0.1 8080

run-client: client
	$(JV) $< 127.0.0.1 8080

clean:
	rm *.class
