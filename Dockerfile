FROM ubuntu:latest
   
# install base dependences    
RUN apt-get update && \
    apt-get install -y make cmake git curl build-essential m4 sudo gdbserver python3-pip \
    gdb libreadline-dev bison flex zlib1g-dev libicu-dev icu-devtools tmux zile zip vim \
    gawk wget python3-full python3-virtualenv graphviz

RUN deluser --remove-home ubuntu
# add postgres user and make data dir        
RUN groupadd -r postgres && useradd --no-log-init -r -m -s /bin/bash -g postgres -G sudo postgres

# make postgres a sudoer
RUN echo "postgres ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/user && \
    chmod 0440 /etc/sudoers.d/user

USER postgres
    
ENV PGDATA /home/postgres/data
RUN /bin/rm -Rf "$PGDATA" && mkdir "$PGDATA"
WORKDIR "/home/postgres"

# get postgres source and compile with debug and no optimization
RUN git clone --branch REL_17_STABLE https://github.com/postgres/postgres.git --depth=1 && \
    cd postgres && ./configure \
    --prefix=/usr/ \
    --without-icu \
    --enable-debug \
    --enable-depend --enable-cassert --enable-profiling \
#    CFLAGS="-ggdb -Og -g3 -fno-omit-frame-pointer" \
#    CFLAGS="-O3" \
    && make -j 4 && sudo make install

# get GraphBLAS, compile with debug symbols    
RUN curl -s -L -J https://github.com/DrTimothyAldenDavis/GraphBLAS/archive/refs/tags/v9.4.2.tar.gz | \
    tar zxvf - && cd GraphBLAS-9.4.2 && \
    make library \
    CMAKE_OPTIONS='-DGRAPHBLAS_COMPACT=1' \
    && sudo make install

RUN sudo ldconfig

# put test stuff into pg home        
RUN mkdir "/home/postgres/onesparse"
WORKDIR "/home/postgres/onesparse"
COPY . .

RUN sudo make && sudo make install && sudo make clean

# start the database            
RUN initdb -D "$PGDATA" -c shared_preload_libraries='onesparse' -c search_path='onesparse' --encoding UTF8
EXPOSE 5432
# wait forever

CMD tail -f /dev/null
