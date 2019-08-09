# See packaging for various base options
FROM flatironinstitute/triqs:base

COPY requirements.txt /src/triqs/requirements.txt
# Install numpy first to prevent h5py from pulling in a newer version first
RUN pip install `grep numpy /src/triqs/requirements.txt` && \
    pip install -r /src/triqs/requirements.txt

RUN useradd -m build

ENV SRC=/src \
    BUILD=/home/build \
    INSTALL=/usr/local \
    PYTHONPATH=/usr/local/lib/python2.7/site-packages:${PYTHONPATH} \
    CMAKE_PREFIX_PATH=/usr/lib/cmake/triqs:${CMAKE_PREFIX_PATH}

# Hacky patch for matplotlib sphinx bug (https://github.com/matplotlib/matplotlib/pull/12456?)
RUN ln -s /src /home/src

COPY . ${SRC}/triqs
WORKDIR ${BUILD}/triqs
RUN chown -R build ${SRC}/triqs .
USER build
ARG BUILD_DOC=0
RUN cmake $SRC/triqs -DCMAKE_INSTALL_PREFIX=$INSTALL -DBuild_Documentation=$BUILD_DOC -DCLANG_OPT="$CXXFLAGS" -DMATHJAX_PATH="https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.2" && make -j2 && make test CTEST_OUTPUT_ON_FAILURE=1
USER root
RUN make install
