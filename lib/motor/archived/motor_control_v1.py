
def motor_conversion_v1(speed, turning, hyst = 0.1, turning_cap = 0.25, info = False):

  # parameters
  hyst = hyst # 10% tollerance for centre
  top_val = 1023
  centre = int(top_val/2) + 1 # centre ADC value
  centre_TOP = int(centre * (1 + hyst/2))
  centre_BOT = int(centre * (1 - hyst/2))
  top_range = top_val - centre_TOP
  bot_range = centre_BOT
  hyst_range = top_val - top_range - bot_range
  true_range = top_range + bot_range
  turning_cap = turning_cap

  # default variables
  left_duty = 0
  left_dir = 0 # 0 reverse, 1 forward
  right_duty = 0
  right_dir = 0 # 0 reverse, 1 forward
  turn_magnitude = 0 # defined default
  turn_direction = 0 # defined default

  # results
  fast_dir = 0
  fast_duty = 0
  slow_dir = 0
  slow_duty = 0

  # trim speed value
  if speed > centre_TOP:
    speed = speed - hyst_range
  elif speed > centre_BOT:
    speed = centre_BOT

  if info:
    print(f"speed value: {speed}")
  

  # TURNING
  if turning > centre_TOP: # HIGH LEFT
    turn_magnitude = (turning - centre_TOP) * turning_cap / centre_BOT
    turn_direction = 1 # LEFT
  elif turning < centre_BOT: # LOW RIGHT
    turn_magnitude = (centre_BOT - turning) * turning_cap / centre_BOT
    # turn_direction = 0 # RIGHT by default
  # else:
  #   turn_magnitude = 0 # by default
    # turn_direction = 0 # RIGHT by default
  # print(turn_magnitude, "LEFT" if turn_direction else "RIGHT")
  if info:
    print(f"turn magnitude: {turn_magnitude}")
    print(f"turn direction: {turn_direction}")



  fast_side = speed + turn_magnitude * centre_BOT
  slow_side = speed - turn_magnitude * centre_BOT

  if info:
    print("\twith turn modifier applied")
    print(f"fast side: {fast_side}")
    print(f"slow side: {slow_side}")


  # maximum limitors
  if fast_side > true_range:
    fast_side = true_range
    slow_side = true_range - 2 * turn_magnitude * centre_BOT
  elif slow_side < 0:
    slow_side = 0
    fast_side = 2 * turn_magnitude * centre_BOT

  if info:
    print("\twith limmit correction:")
    print(f"fast side: {fast_side}")
    print(f"slow side: {slow_side}")

  fast_side = (fast_side - centre_BOT)/centre_BOT
  fast_duty = abs(fast_side)
  fast_dir = fast_side/fast_duty

  slow_side = (slow_side - centre_BOT)/centre_BOT
  slow_duty = abs(slow_side)
  slow_dir = slow_side/slow_duty

  if info:
    print("fast duty: ", fast_duty)
    print("fast dir: ", fast_dir)
    print("slow duty: ", slow_duty)
    print("slow dir: ", slow_dir)

  if turn_direction == 0: # LEFT - Right must be faster
    left_duty = slow_duty
    left_dir = slow_dir
    right_duty = fast_duty
    right_dir = fast_dir
  else:
    left_duty = fast_duty
    left_dir = fast_dir
    right_duty = slow_duty
    right_dir = slow_dir

  if info:
    print(left_dir, right_dir)
    print(left_duty, right_duty)
  return left_duty, left_dir, right_duty, right_dir
