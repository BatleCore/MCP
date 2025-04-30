def bounds_test(hyst):
    top_val = 1023
    centre = 512 # centre ADC value
    # centre_TOP = int(centre * (1 + hyst/2))
    centre_BOT = int(centre * (1 - hyst/2))
    centre_TOP = top_val - centre_BOT
