# Add the proto directory to the Python path. This is necessary to import the generated Python code.
import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), "proto"))