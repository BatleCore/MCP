import matplotlib.pyplot as plt
from motor_plotting import *

# hyst = 0.1 # 10% tollerance for centre
# top_val = 1023
# centre = int(top_val/2) + 1 # centre ADC value
# centre_TOP = int(centre * (1 + hyst/2))
# centre_BOT = int(centre * (1 - hyst/2))
# top_range = top_val - centre_TOP
# bot_range = centre_BOT
# hyst_range = top_val - top_range - bot_range
# true_range = top_range + bot_range
# turning_cap = 0.25 # 25% 

# print(f"centre: {centre}")
# print(f"centre TOP: {centre_TOP}")
# print(f"centre BOT: {centre_BOT}")
# print(f"true range: {true_range}")
# print(f"top range: {top_range}")
# print(f"bot range: {bot_range}")
# print(f"hyst range: {hyst_range}")



def motor_conversion_prototype(speed, turning, hyst = 0.1, turning_cap = 0.25, info = False):

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

  if info:
    print(f"centre: {centre}")
    print(f"centre TOP: {centre_TOP}") 
    print(f"centre BOT: {centre_BOT}")
    print(f"true range: {true_range}")
    print(f"top range: {top_range}")
    print(f"bot range: {bot_range}")
    print(f"hyst range: {hyst_range}")

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
  if info:
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
  if info:
    print(f"turn magnitude: {turn_magnitude}")
    print(f"turn direction: {turn_direction}")



  fast_side = speed + turn_magnitude * centre_BOT
  slow_side = speed - turn_magnitude * centre_BOT

  if info:
    print("\twith turn modifier applied")
    print(f"fast side: {fast_side}")
    print(f"slow side: {slow_side}")

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

  if fast_side > centre_BOT:
    fast_duty = (fast_side - centre_BOT) / centre_BOT
    fast_dir = 1 # forward
  elif fast_side == centre_BOT:
    fast_duty = 0
    fast_dir = -1
  else:
    fast_duty = (centre_BOT - fast_side) / centre_BOT
    fast_dir = -1 # reverse

  if slow_side > centre_BOT:
    slow_duty = (slow_side - centre_BOT) / centre_BOT
    slow_dir = 1 # forward
  elif slow_side == centre_BOT:
    slow_duty = 0
    slow_dir = -1
  else:
    slow_duty = (centre_BOT - slow_side) / centre_BOT
    slow_dir = -1 # reverse

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


speed = 1023
turning = 0

# left_duty, left_dir, right_duty, right_dir = motor_conversion_prototype(speed=speed, turning=turning)

'''
# Create figure and two subplots (1 row, 2 columns)
fig, axs = plt.subplots(2, 2, figsize=(8, 8))
ax_JoyL = axs[0][0]
ax_JoyR = axs[0][1]
init_joyplot(ax_JoyR, "Right Joystick")
init_joyplot(ax_JoyL, "Left Joystick")

# Left subplot: Scatter plot with one circle point at the center (0, 0)
ax_JoyL.scatter(1023/2, speed, s=10000, c="#444444", edgecolors='black')
ax_JoyR.scatter(turning, 1023/2, s=10000, c="#444444", edgecolors='black') # marker



# for bar formatting
bar_positions = [0, 1]
bar_heights = [1023, 1023]         # total bar heights
bottom = [0, 0]
data_values = [speed, turning]         # horizontal line marker values

# Draw bars
axs[1, 0].bar(bar_positions, bar_heights, bottom=bottom, width=0.5, color='lightgray', edgecolor='black')

# for data calculated ranges
centre_bottoms = [centre_BOT, centre_BOT]
centre_range = [hyst_range, hyst_range]
axs[1, 0].bar(bar_positions, centre_range, bottom=centre_bottoms, width=0.5, color="#666666")

# Draw horizontal lines (markers) across each bar
for x, val in zip(bar_positions, data_values):
  axs[1, 0].hlines(val, x - 0.25, x + 0.25, colors='red', linewidth=2)

axs[1, 0].set_ylim(-128, 1024+128)
axs[1, 0].set_xticks(bar_positions)
axs[1, 0].set_xticklabels(["speed", "turning"])
axs[1, 0].set_title("Scale-Type Bar Plot")

axs[1, 1].bar(bar_positions, [2,2], bottom=[-1,-1], width=0.5, color='lightgray', edgecolor='black')
axs[1, 1].set_ylim(-1.1, 1.1)
axs[1, 1].set_xticks(bar_positions)
for x, val in zip(bar_positions, [0,0]):
  axs[1, 1].hlines(val, x - 0.25, x + 0.25, colors='black', linewidth=2)

left_speed = left_duty if left_dir == 1 else left_duty * -1
right_speed = right_duty if right_dir == 1 else right_duty * -1

for x, val in zip(bar_positions, [left_speed, right_speed]):
  axs[1, 1].hlines(val, x - 0.25, x + 0.25, colors='red', linewidth=2)
axs[1, 1].set_xticklabels(["Left", "Right"])
axs[1, 1].set_title("Motor Speed and Direction")


# Display the figure
plt.tight_layout()
plt.show()

'''