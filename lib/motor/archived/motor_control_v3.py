
def motor_conversion_v2(speed, turning, hyst = 0.1, turning_cap = 0.25, info = False):
  print("VERSION THREE")
  """
  INPUT: 10bit - 1023 max
  OUTPUT: 8bit - 254 MAX
  """

  if info:
    print("\tV3")

  # parameters
  hyst = hyst # 10% tollerance for centre
  turning_cap = turning_cap
  top_val = 1023

  centre = 512 # centre ADC value
  centre_BOT = int(centre * (1 - hyst/2))
  centre_TOP = top_val - centre_BOT
  
  hyst_range = centre_TOP - centre_BOT
  true_range = top_val - hyst_range

  if info:
    # print(f"centre: {centre}")
    print(f"centre TOP: {centre_TOP}") 
    print(f"centre BOT: {centre_BOT}")
    print(f"true range: {true_range}")
    # print(f"top range: {top_range}")
    # print(f"bot range: {bot_range}")
    print(f"hyst range: {hyst_range}")

  # default variables
  left_duty = 0
  left_dir = 0 # 0 reverse, 1 forward
  right_duty = 0
  right_dir = 0 # 0 reverse, 1 forward
  turn_magnitude = 0 # defined default
  turn_direction = 0 # defined default

  # results - define at end?
  fast_dir = 1
  fast_duty = 0
  slow_dir = 1
  slow_duty = 0

  # trim speed value
  if speed > centre_TOP:
    speed = speed - hyst_range
  elif speed > centre_BOT:
    speed = centre_BOT

  if info:
    print(f"trimmed speed value: {speed}")
  
  # TURNING
  if turning > centre_TOP: # HIGH LEFT
    turn_magnitude = (turning - centre_TOP) * turning_cap / centre_BOT
    turn_direction = 1 # LEFT
  elif turning < centre_BOT: # LOW RIGHT
    turn_magnitude = (centre_BOT - turning) * turning_cap / centre_BOT

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

  # mapping - stage 1

  fast_side = (fast_side - centre_BOT)/centre_BOT
  fast_duty = abs(fast_side)
  if fast_duty:
    fast_dir = fast_side/fast_duty # potential to divide by zero !!!
  else: # default?
    fast_dir = 1

  slow_side = (slow_side - centre_BOT)/centre_BOT
  slow_duty = abs(slow_side)
  if slow_duty:
    slow_dir = slow_side/slow_duty
  else:
    slow_dir = 1

  if info:
    print("fast duty: ", fast_duty)
    print("fast dir: ", fast_dir)
    print("slow duty: ", slow_duty)
    print("slow dir: ", slow_dir)

  if turn_direction == 0: # LEFT - Right must be faster
    left_duty = int(slow_duty * 254)
    left_dir = slow_dir
    right_duty = int(fast_duty * 254)
    right_dir = fast_dir
  else:
    left_duty = int(fast_duty * 254)
    left_dir = fast_dir
    right_duty = int(slow_duty * 254)
    right_dir = slow_dir

  return left_duty, left_dir, right_duty, right_dir
