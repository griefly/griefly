FROM ubuntu:xenial

RUN apt update && apt install -y software-properties-common

RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test
RUN add-apt-repository -y ppa:beineri/opt-qt593-xenial

RUN apt-get update -qq
RUN apt-get install -y git curl make cmake lcov xvfb
RUN apt-get install -y ruby g++
RUN apt-get install -y mesa-common-dev libglu1-mesa-dev
RUN gem install coveralls-lcov

# Qt installation
RUN apt-get install -y qt59base qt59multimedia

# Go installation
RUN curl -sL -o /usr/bin/gimme https://raw.githubusercontent.com/travis-ci/gimme/master/gimme
RUN chmod +x /usr/bin/gimme
RUN gimme 1.6

# Qt environment variables
ARG QT_BASE_DIR=/opt/qt59
ENV QTDIR ${QT_BASE_DIR}
ENV PATH ${QT_BASE_DIR}/bin:${PATH}
ENV LD_LIBRARY_PATH ${QT_BASE_DIR}/lib/x86_64-linux-gnu:${QT_BASE_DIR}/lib:${LD_LIBRARY_PATH}
ENV PKG_CONFIG_PATH ${QT_BASE_DIR}/lib/pkgconfig:${PKG_CONFIG_PATH}

# Go environment variables
ENV GOROOT '/root/.gimme/versions/go1.6.linux.amd64'
ENV PATH "/root/.gimme/versions/go1.6.linux.amd64/bin:${PATH}"
