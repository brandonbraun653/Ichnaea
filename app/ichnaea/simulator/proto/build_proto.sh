# Generate the C++ and Python bindings for the proto files in this directory.
# Expects to be run from this directory and within the project's python virtual environment/docker container.
# Without this, you're likely to get errors about missing packages.

THIS_DIR=$(pwd)
SIM_GEN_DIR=$(realpath "$THIS_DIR/../../../../src/sim/generated")

for proto_file in *.proto;
do
  echo "Generating C++ output for $proto_file in $SIM_GEN_DIR"
  protoc --grpc_out="$SIM_GEN_DIR" --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` "$proto_file"
  protoc --cpp_out="$SIM_GEN_DIR" "$proto_file"

  echo "Generating Python bindings for $proto_file" in $THIS_DIR
  python -m grpc_tools.protoc --python_out=. --mypy_out=. --grpc_python_out=. --proto_path=. "$proto_file"
done
