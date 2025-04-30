from motor_proto import *
from motor_control_v2 import *
import numpy.random as npr

speed = 100
turning = 900
tests = 5 # number of tests
info = False
rng = npr.default_rng()

for i in range(tests):
  speed = rng.integers(0, 1024)
  turning = rng.integers(0, 1024)
  results_proto = motor_conversion_prototype(speed, turning, hyst = 0.1, turning_cap = 0.25, info=info)
  results_v1 = motor_conversion_v2(speed, turning, hyst = 0.1, turning_cap = 0.25, info=info)
  print(f"test {i}")
  if results_proto != results_v1:
    print("FAILED")
    print(f"speed:   {speed}")
    print(f"turning: {turning}")
    print(f"proto: {results_proto}")
    print(f"v1   : {results_v1}")

  else:
    print("PASSED\n")

# output_prototype = motor_conversion_prototype(speed, turning, hyst = 0.1, turning_cap = 0.25)
# print(output_prototype)