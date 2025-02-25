# This will compile cat1.c and name the output binary ‘cat1’

echo2: echo2.c
	$(CC) -Wall $^ -o $@

install: cat1
	sudo install -m 755 echo2 /usr/local/bin/

clean:
	rm -f echo2

# TODO: figure out how to use the 'install' command to copy ‘cat1’ into
# the /usr/local/bin directory. The user should be able to type
# 'sudo make install' to install your program!
