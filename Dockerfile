# Use the latest Ubuntu image
FROM ubuntu:24.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive

# Install sudo then add the default user to the sudo group
RUN apt-get update && \
    apt-get install -y sudo && \
    usermod -aG sudo ubuntu && \
    echo "ubuntu ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

# Switch to the image's built-in user. This should match
# permissions with the host user.
USER ubuntu

# Update the package list and install necessary packages
RUN sudo apt-get update && \
    sudo apt-get install -y \
    aspnetcore-runtime-8.0 \
    autoconf \
    build-essential \
    clang \
    clang-format \
    clangd \
    cmake \
    cpputest \
    curl \
    default-jdk \
    dotnet-runtime-8.0 \
    dotnet-sdk-8.0 \
    doxygen \
    dwarves \
    g++ \
    gcc \
    gcovr \
    gdb-multiarch \
    git \
    libbsd-dev \
    libclang-dev \
    libgrpc++-dev \
    libgrpc-dev \
    libnorm-dev \
    libpgm-dev \
    libprotobuf-dev \
    libprotoc-dev \
    libsodium-dev \
    libstdc++-13-dev \
    libsystemd-dev \
    libtool \
    libzmq3-dev \
    llvm-dev \
    lsof \
    minicom \
    ninja-build \
    octave \
    octave-control \
    octave-dev \
    octave-image \
    octave-io \
    octave-optim \
    octave-signal \
    octave-statistics \
    pkg-config \
    protobuf-compiler \
    protobuf-compiler-grpc \
    psmisc \
    python3-pip \
    python3.12 \
    python3.12-dev \
    python3.12-venv \
    wget \
    && sudo apt-get clean \
    && sudo rm -rf /var/lib/apt/lists/*

# Set the default Python version to 3.12, then create a symlink to the python command
RUN sudo update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.12 1
RUN sudo ln -s /usr/bin/python3 /usr/bin/python

# Download and install arm-none-eabi-gcc for ARM Cortex-M cross-compilation
ENV ARM_GCC_URL=https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/download/v13.3.1-1.1/xpack-arm-none-eabi-gcc-13.3.1-1.1-linux-x64.tar.gz
RUN wget $ARM_GCC_URL -O /tmp/arm-none-eabi-gcc.tar.gz && \
    sudo tar -xzf /tmp/arm-none-eabi-gcc.tar.gz -C /opt/ && \
    sudo rm /tmp/arm-none-eabi-gcc.tar.gz && \
    sudo ln -s /opt/xpack-arm-none-eabi-gcc-13.3.1-1.1/bin/* /usr/local/bin/

# Install the latest version of poetry and configure it for in-project virtual environments
RUN curl -sSL https://install.python-poetry.org | python3 -
ENV PATH="${PATH}:/home/ubuntu/.local/bin"
RUN poetry config virtualenvs.in-project true

# Install gRPC and protobuf tools
WORKDIR /tmp
RUN git clone --recurse-submodules -b v1.66.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc
WORKDIR /tmp/grpc
RUN mkdir -p cmake/build && cd cmake/build && cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release ../.. && make -j$(nproc) && sudo make install

# Set the working directory
WORKDIR /workspace

# Default command
CMD ["/bin/bash"]
