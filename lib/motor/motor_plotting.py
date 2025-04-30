def init_joyplot(ax, label=None):
  ax.scatter(1023/2, 1023/2, s=50000, c="#888888", edgecolors='black')
  ax.set_xlim(0-256, 1023+256)
  ax.set_ylim(0-256, 1023+256)
  ax.set_aspect('equal')
  ax.set_title(label)
