FROM ubuntu:xenial

RUN apt update && apt install -y software-properties-common

RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test
RUN add-apt-repository -y ppa:beineri/opt-qt593-xenial

RUN apt-get update -qq
RUN apt-get install -y git curl make ruby python-software-properties
RUN apt-get install -y cmake g++-5 qt59base qt59multimedia lcov mesa-common-dev libglu1-mesa-dev
RUN gem install coveralls-lcov

# RUN eval "$(curl -sL https://raw.githubusercontent.com/travis-ci/gimme/master/gimme | GIMME_GO_VERSION=1.6 bash)"
# RUN /opt/qt59/bin/qt59-env.sh

ENV CXX "g++-5"
