FROM ubuntu:jammy

SHELL ["/bin/bash", "-c"]
RUN apt-get update && \
  apt-get install -y wget git bash-completion tree time && \
  apt-get install -y lsb-release wget software-properties-common gnupg && \
  apt-get install -y build-essential && \
  apt-get install -y make cmake ninja-build

RUN wget https://apt.llvm.org/llvm.sh && \
  chmod +x llvm.sh && \
  ./llvm.sh 15 && \
  rm -f llvm.sh

RUN apt-get install -y bison flex libbison-dev && \
  apt-get install -y graphviz && \
  apt-get install -y zlib1g-dev libzstd-dev && \
  apt-get clean -y && \
  rm -rf /var/lib/apt/lists/*

RUN ln -s /usr/bin/clang-15 /usr/bin/clang && \
  ln -s /usr/bin/clang++-15 /usr/bin/clang++ && \
  ln -s /usr/bin/opt-15 /usr/bin/opt

RUN groupadd -g 1000 cs526 && \
  useradd -m -u 1000 -g cs526 -s /bin/bash cs526

ENV HOME=/home/cs526
USER cs526

RUN echo "umask 0002" >> ~/.bashrc && \
  echo "alias ll='ls -alF'" >> ~/.bashrc

ENTRYPOINT ["bash", "-l", "-c", "-i"]
WORKDIR /home/cs526/cos_ssa
