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

RUN groupadd -g 1000 cs526 && \
  useradd -m -u 1000 -g cs526 -s /bin/bash cs526

ENV HOME=/home/cs526
USER cs526

RUN echo "umask 0002" >> ~/.bashrc && \
  echo "alias ll='ls -alF'" >> ~/.bashrc

ENTRYPOINT ["bash", "-l", "-c", "-i"]
WORKDIR /home/cs526/cos_ssa
