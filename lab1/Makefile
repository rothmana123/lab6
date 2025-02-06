# This will compile about.c and name the output binary 'about'
# You should figure out **why** it does that, though...
about: about.c
	$(CC) -Wall $^ -o $@

install: about
	sudo install -m 755 about /usr/local/bin/
    #attempted to fix spacing issue in Micro
    # TODO: figure out how to use the 'install' command to copy 'about' into
    # the /usr/local/bin directory. The user should be able to type
    # 'sudo make install' to install your program!
