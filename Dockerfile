FROM postgres:11
RUN apt-get update && apt-get install -y \
    make cmake git postgresql-server-dev-11 \
    postgresql-11-pgtap postgresql-11-dbgsym \
    curl build-essential m4 sudo gdb
    
RUN curl -s -L http://faculty.cse.tamu.edu/davis/SuiteSparse/SuiteSparse-5.4.0.tar.gz | \
    tar zxvf - SuiteSparse/GraphBLAS && cd SuiteSparse/GraphBLAS && \
    make library CMAKE_OPTIONS='-DCMAKE_BUILD_TYPE=Debug' && make install
    
RUN /bin/rm -Rf SuitSparse
RUN mkdir "/pggraphblas"
WORKDIR "/pggraphblas"
COPY . .
RUN make && make install && make clean
RUN ldconfig
