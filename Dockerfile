FROM ubuntu:latest

# install base dependences    
RUN apt-get update && \
    apt-get install -y make cmake git curl build-essential m4 sudo gdbserver \
    gdb libreadline-dev bison flex zlib1g-dev tmux emacs25-nox zile zip

# add postgres user and make data dir        
RUN groupadd -r postgres && useradd --no-log-init -r -m -s /bin/bash -g postgres -G sudo postgres
ENV PGDATA /home/postgres/data
RUN /bin/rm -Rf "$PGDATA" && mkdir "$PGDATA" && chown -R postgres:postgres "$PGDATA"
WORKDIR "/home/postgres"

# get SuiteSparse, compile graphblas with debug symbols    
RUN curl -s -L http://faculty.cse.tamu.edu/davis/SuiteSparse/SuiteSparse-5.4.0.tar.gz | \
    tar zxvf - SuiteSparse/GraphBLAS && cd SuiteSparse/GraphBLAS && \
    make library CMAKE_OPTIONS='-DCMAKE_BUILD_TYPE=Debug' && make install
RUN /bin/rm -Rf SuitSparse

# get postgres source and compile with debug and no optimization
RUN git clone --branch REL_11_STABLE https://github.com/postgres/postgres.git --depth=1 && \
    cd postgres && ./configure CFLAGS="-O0 -g" --prefix=/usr/ --enable-depend --enable-cassert --enable-debug && \
    make -j 4 && make install

RUN curl -s -L https://github.com/theory/pgtap/archive/v0.99.0.tar.gz | tar zxvf - && \   
    cd pgtap-0.99.0 && make && make install

# put test stuff into pg home        
RUN mkdir "/home/postgres/pggraphblas"
WORKDIR "/home/postgres/pggraphblas"
COPY . .
COPY gdbinit ../.gdbinit

# make the extension    
RUN make && make install && make clean
RUN ldconfig

# make postgres a sudoer        
RUN echo "postgres ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/user && \
    chmod 0440 /etc/sudoers.d/user

# start the database            
USER postgres
RUN initdb -D "$PGDATA"
EXPOSE 5432
# wait forever
CMD tail -f /dev/null
