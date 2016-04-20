from setuptools import find_packages
from cx_Freeze import setup, Executable
import zmq.libzmq
build_exe_options = {
     'packages': ['zmq.backend.cython', ],
     'include_files': [zmq.libzmq.__file__, ]
	 }
includes = ["zmq", "zmq.utils.garbage", "zmq.backend.cython"]
setup(
	name = "zmq_test", 
	options = {"build_exe": build_exe_options}, 
	executables = [Executable("zmq_test.py")]
)
