# Ordering of imports is important here. Mbedutils will publish it's proto file paths
# to the python path. But it must be done first before importing any Ichnaea proto files.
import mbedutils.rpc.proto
