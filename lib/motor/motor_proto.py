hyst = 0.1 # 10% tollerance for centre
top_val = 1023
centre = int(top_val/2) + 1 # centre ADC value
centre_TOP = int(centre * (1 + hyst/2))
centre_BOT = int(centre * (1 - hyst/2))
top_range = top_val - centre_TOP
bot_range = centre_BOT
hyst_range = top_val - top_range - bot_range
true_range = top_range + bot_range
turning_cap = 0.25 # 25% 

# print(f"centre: {centre}")
# print(f"centre TOP: {centre_TOP}")
print(f"centre BOT: {centre_BOT}")
print(f"true range: {true_range}")
# print(f"top range: {top_range}")
# print(f"bot range: {bot_range}")
# print(f"hyst range: {hyst_range}")



def motor_conversion(speed, turning):
  # 0 -> 1023
  left_duty = 0
  left_dir = 0 # 0 reverse, 1 forward
  right_duty = 0
  right_dir = 0 # 0 reverse, 1 forward
  speed_magnitude = 0
  turn_magnitude = 0
  speed_direction = 0 # 0 reverse, 1 forward
  turn_direction = 0 # 0 right, 1 left

  # trim speed value
  if speed > centre_TOP: # HIGH FORWARD
    speed = speed - hyst_range
  elif speed < centre_BOT: # LOW REVERSE
    speed = speed
  else:
    speed = centre_BOT
  print(f"speed value: {speed}")
  """
  now, if speed > centre_BOT: forward
  if speed < centre_BOT: reverse
  if speed == centre_BOT: stationary
  """
  

  # TURNING
  if turning > centre_TOP: # HIGH LEFT
    turn_magnitude = (turning - centre_TOP) * turning_cap / centre_BOT
    turn_direction = 1 # LEFT
  elif turning < centre_BOT: # LOW RIGHT
    turn_magnitude = (centre_BOT - turning) * turning_cap / centre_BOT
    turn_direction = 0 # RIGHT
  else:
    turn_magnitude = 0
  # print(turn_magnitude, "LEFT" if turn_direction else "RIGHT")
  print(f"turn magnitude: {turn_magnitude}")
  print(f"turn direction: {turn_direction}")



  fast_side = speed + turn_magnitude * centre_BOT
  slow_side = speed - turn_magnitude * centre_BOT

  print("\twith turn modifier applied")
  print(f"fast side: {fast_side}")
  print(f"slow side: {slow_side}")

  if fast_side > true_range:
    fast_side = true_range
    slow_side = true_range - 2 * turn_magnitude * centre_BOT
  elif slow_side < 0:
    slow_side = 0
    fast_side = 2 * turn_magnitude * centre_BOT

  print("\twith limmit correction:")
  print(f"fast side: {fast_side}")
  print(f"slow side: {slow_side}")

  if fast_side > centre_BOT:
    fast_duty = (fast_side - centre_BOT) / centre_BOT
    fast_dir = 1 # forward
  elif fast_side == centre_BOT:
    fast_duty = 0
    fast_dir = 0
  else:
    fast_duty = (centre_BOT - fast_side) / centre_BOT
    fast_dir = 0 # reverse

  if slow_side > centre_BOT:
    slow_duty = (slow_side - centre_BOT) / centre_BOT
    slow_dir = 1 # forward
  elif slow_side == centre_BOT:
    slow_duty = 0
    slow_dir = 0
  else:
    slow_duty = (centre_BOT - slow_side) / centre_BOT
    slow_dir = 0 # reverse

  print("fast duty: ", fast_duty)
  print("fast dir: ", fast_dir)
  print("slow duty: ", slow_duty)
  print("slow dir: ", slow_dir)

  if turn_direction == 1: # LEFT - Right must be faster
    left_duty = slow_duty
    left_dir = slow_dir
    right_duty = fast_duty
    right_dir = fast_dir
  else:
    left_duty = fast_duty
    left_dir = fast_dir
    right_duty = slow_duty
    right_dir = slow_dir

  print(left_dir, right_dir)
  print(left_duty, right_duty)


speed = 500
turning = 100

motor_conversion(speed=speed, turning=turning)