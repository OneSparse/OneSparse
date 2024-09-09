FROM ubuntu:latest

# install base dependences    
RUN apt-get update && \
    apt-get install -y make cmake git curl build-essential m4 sudo gdbserver \
    gdb libreadline-dev bison flex zlib1g-dev libicu-dev icu-devtools tmux zile zip vim gawk wget python3

# add postgres user and make data dir        
RUN groupadd -r postgres && useradd --no-log-init -r -m -s /bin/bash -g postgres -G sudo postgres
ENV PGDATA /home/postgres/data
RUN /bin/rm -Rf "$PGDATA" && mkdir "$PGDATA"
WORKDIR "/home/postgres"

# get postgres source and compile with debug and no optimization
RUN git clone --branch REL_16_STABLE https://github.com/postgres/postgres.git --depth=1 && \
    cd postgres && ./configure \
    --prefix=/usr/ \
    --without-icu \
    --enable-debug \
    --enable-depend --enable-cassert --enable-profiling \
    CFLAGS="-ggdb -Og -g3 -fno-omit-frame-pointer" \
#    CFLAGS="-O3" \
    && make -j 4 && make install

RUN curl -s -L https://github.com/theory/pgtap/archive/v1.2.0.tar.gz | tar zxvf - && cd pgtap-1.2.0 && make && make install
RUN cpan App::cpanminus && cpan TAP::Parser::SourceHandler::pgTAP && cpan App::prove
    
# get GraphBLAS, compile with debug symbols    
RUN curl -s -L -J https://github.com/DrTimothyAldenDavis/GraphBLAS/archive/refs/tags/v9.2.0.tar.gz | \
    tar zxvf - && cd GraphBLAS-9.2.0 && \
#    sed -i 's/^\/\/ #undef NDEBUG/#undef NDEBUG/g' Source/GB.h && \
#    sed -i 's/^\/\/ #define GB_PRINT_MALLOC 1/#define GB_PRINT_MALLOC 1/g' Source/GB.h && \
    make library \
    CMAKE_OPTIONS='-DCMAKE_BUILD_TYPE=Debug' \
    && make install

# chown dependencies    
RUN chown -R postgres:postgres /home/postgres
    
# put test stuff into pg home        
RUN mkdir "/home/postgres/onesparse"
WORKDIR "/home/postgres/onesparse"
COPY . .
    
# make the extension
RUN python3 generate.py onesparse/onesparse--0.1.0.sql    
RUN make && make install && make clean
RUN ldconfig

# chown just onesparse
RUN chown -R postgres:postgres /home/postgres/onesparse
    
# make postgres a sudoer        
RUN echo "postgres ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/user && \
    chmod 0440 /etc/sudoers.d/user

# start the database            
USER postgres
RUN initdb -D "$PGDATA"
EXPOSE 5432
# wait forever

CMD tail -f /dev/null
