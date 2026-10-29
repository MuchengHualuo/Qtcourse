# -*- coding: utf-8 -*-
"""把截图序列合成为演示动画 GIF（Pillow）。"""
import glob
import os
import sys

from PIL import Image

src_dir = sys.argv[1]
out_path = sys.argv[2]
scale = float(sys.argv[3]) if len(sys.argv) > 3 else 0.5
delay_ms = int(sys.argv[4]) if len(sys.argv) > 4 else 2600

files = sorted(glob.glob(os.path.join(src_dir, "*.png")))
if not files:
    raise SystemExit("no frames found")

frames = []
for path in files:
    img = Image.open(path).convert("RGB")
    if scale != 1.0:
        img = img.resize((int(img.width * scale), int(img.height * scale)), Image.LANCZOS)
    frames.append(img.convert("P", palette=Image.ADAPTIVE, colors=128))

frames[0].save(
    out_path,
    save_all=True,
    append_images=frames[1:],
    duration=delay_ms,
    loop=0,
    optimize=True,
    disposal=2,
)
print(f"{out_path}: {len(frames)} frames, {os.path.getsize(out_path)/1024:.0f} KiB")
