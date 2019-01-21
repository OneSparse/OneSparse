FROM ubuntu:latest
RUN apt-get update && \
    apt-get install -y make cmake git curl build-essential m4 sudo gdbserver \
    gdb libreadline-dev bison flex zlib1g-dev tmux emacs25-nox zile
    
RUN groupadd -r postgres && useradd --no-log-init -r -m -s /bin/bash -g postgres -G sudo postgres
ENV PGDATA /home/postgres/data
RUN /bin/rm -Rf "$PGDATA" && mkdir "$PGDATA" && chown -R postgres:postgres "$PGDATA"

WORKDIR "/home/postgres"
    
RUN curl -s -L http://faculty.cse.tamu.edu/davis/SuiteSparse/SuiteSparse-5.4.0.tar.gz | \
    tar zxvf - SuiteSparse/GraphBLAS && cd SuiteSparse/GraphBLAS && \
    make library CMAKE_OPTIONS='-DCMAKE_BUILD_TYPE=Debug' && make install
    
RUN /bin/rm -Rf SuitSparse

RUN git clone https://github.com/postgres/postgres.git --depth=1 && \
    cd postgres && ./configure CFLAGS="-O0 -g" --prefix=/usr/ --enable-depend --enable-cassert --enable-debug && \
    make -j 4 && make install
    
RUN mkdir "/home/postgres/pggraphblas"
WORKDIR "/home/postgres/pggraphblas"
COPY . .
COPY gdbinit ../.gdbinit

RUN make && make install && make clean
RUN ldconfig
    
RUN echo "postgres ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/user && \
    chmod 0440 /etc/sudoers.d/user
        
USER postgres
RUN initdb -D "$PGDATA"
EXPOSE 5432
CMD tail -f /dev/null
