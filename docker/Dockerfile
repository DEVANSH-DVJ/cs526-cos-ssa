FROM ubuntu:jammy

SHELL ["/bin/bash", "-c"]
RUN apt-get update && \
  apt-get install -y clang-14 wget git && \
  apt-get install -y apt-transport-https curl && \
  apt-get install -y build-essential && \
  apt-get install -y make cmake ninja-build && \
  apt-get install -y bison flex libbison-dev && \
  apt-get clean -y && \
  rm -rf /var/lib/apt/lists/*

RUN mkdir cs526

ENTRYPOINT ["bash", "-l", "-c"]
WORKDIR "/cs526"
