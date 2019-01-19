FROM ubuntu:latest
RUN apt-get update && \
    apt-get install -y make cmake git curl build-essential m4 sudo gdbserver gdb libreadline-dev bison flex zlib1g-dev tmux
    
RUN curl -s -L http://faculty.cse.tamu.edu/davis/SuiteSparse/SuiteSparse-5.4.0.tar.gz | \
    tar zxvf - SuiteSparse/GraphBLAS && cd SuiteSparse/GraphBLAS && \
    make library CMAKE_OPTIONS='-DCMAKE_BUILD_TYPE=Debug' && make install
    
RUN /bin/rm -Rf SuitSparse

ENV PGDATA /postgres/data

RUN git clone https://github.com/postgres/postgres.git --depth=1 && \
    cd postgres && ./configure --prefix=/usr/ --enable-depend --enable-cassert --enable-debug && \
    make -j 4 && make install
    
RUN mkdir "/pggraphblas"
WORKDIR "/pggraphblas"
COPY . .
RUN make && make install && make clean
RUN ldconfig
RUN groupadd -r postgres && useradd --no-log-init -r -g postgres postgres
RUN /bin/rm -Rf "$PGDATA" && mkdir "$PGDATA" && chown -R postgres:postgres "$PGDATA" /pggraphblas
USER postgres
RUN initdb -D "$PGDATA"
EXPOSE 5432
CMD tail -f /dev/null
