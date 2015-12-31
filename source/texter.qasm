# calculate character position (ra0 = chX/chY)
shr r0, x_coord, 3          # r0 = x_coord >> 3     (divide by 8)
mov ra0.16a, r0             # ra0.x = r0
itof r0, y_coord            # r0 = (float)y_coord
ldi r2, 1 / 11.999999       # reciprocal, can't use 12.0 or we get off by 1
fmul r0, r0, r2             # r0 = r0 * (1/x)
ftoi r1, r0                 # r1 = (int)r0
mov ra0.16b, r1             # ra0.y = r1

# calculate linear character position (r0 = chPos)
mul24 r0, r1, unif          # r0 = r1 * TERMINAL_WIDTH
add r0, r0, ra0.16a         # r0 = r0 + ra0.x
thrsw                       # thread switch after 2 cycles
shl r0, r0, 2               # r0 = r0 << 2          (multiply by 4)

# load character data (r4 = chData)
add t0s, r0, unif           # t0s = r0 + TERMINAL_DATA (enqueue memory load)
ldtmu0                      # r4 = chData           (dequeue memory load)

# setup some values we're going to reuse
ldi ra5, 0xFF               # ra5 = 8-bit mask
mov r3, r4                  # r3 = 32 [ flags, glyph, bg, fg ] 0

# enqueue load foreground color
and r0, r3, ra5             # r0 = r3 & ra5         (foreground index)
shl r0, r0, 2               # r0 = r0 << 2          (multiply by 4)
add t1s, r0, unif           # t1s = r0 + PALETTE_DATA_FG

# enqueue load background color
shr r3, r3, 8               # r3 = r3 >> 8          (next field)
and r0, r3, ra5             # r0 = r3 & ra5         (background index)
shl r0, r0, 2               # r0 = r0 << 2          (multiply by 4)
add t1s, r0, unif           # t1s = r0 + PALETTE_DATA_BG

# calculate glyph position (r3 = fnI)
shr r3, r3, 8               # r3 = r3 >> 8          (next field)
and r0, r3, ra5             # r0 = r3 & ra5         (glyph index)
ldi r1, 8 * 12              # r1 = number of pixels per glyph
mul24 r3, r0, r1            # r3 = r0 * r1

# calculate glyph offset (r2 = fnXO, r1 = fnYO)
shl r0, ra0.16a, 3          # r0 = ra0.x << 3       (multiply by 8)
sub r2, x_coord, r0         # r2 = x_coord - r0

mul24 r0, ra0.16b, 12       # r0 = ra0.y * 12
sub r1, y_coord, r0         # r1 = y_coord - r0

# calculate linear glyph position (r0 = fnPos)
shl r1, r1, 3               # r1 = r1 << 3          (multiply by 8)
add r0, r1, r2              # r0 = r1 + r2 (fnXO)
add r0, r0, r3;mov r2, unif # r0 = r0 + r3 (fnI)    r2 = FONT_DATA
lthrsw                      # last thread switch after 2 cycles
shl r0, r0, 2               # r0 = r0 << 2          (multiply by 4)

# enqueue load font color
add t0s, r0, r2           # t0s = r0 + FONT_DATA  (enqueue memory load)           

# calculate effective foreground color (fnCol.b * fgCol)
ldtmu0                      # r4 = font color
and r0, r4, ra5             # r0 = r4.b
mov ra1.8abcd, r0           # ra1 = r0 duplicated
sub r0, ra5, r0; ldtmu1     # r0 = 255 - r0 (can't access ra1 here, this is for background color)
v8muld r3, ra1, r4          # multiply the colors

# calculate effective background color ((1.0 - fnCol.b) * bgCol)
mov ra1.8abcd, r0; ldtmu1   # ra1 = r0 duplicated
sbwait                      # can't access ra1 here, wait for scoreboard
v8muld r2, ra1, r4          # multiply the colors

# add the colors together and output
v8adds tlbc, r3, r2; thrend
nop                         # keep this
sbdone

# uniforms:
#  - int  TERMINAL_WIDTH
#  - int *TERMINAL_DATA
#  - int *PALETTE_DATA
#  - int *PALETTE_DATA
#  - int *FONT_DATA

# calculate character position
#  - chX = x_coord / 8
#  - chY = y_coord / 12
#  - chPos = ((chY * TERMINAL_WIDTH) + chX) * 4

# load character data
#  - chData = TERMINAL_DATA[chPos]

# queue load foreground color
#  - enqueue PALETTE_DATA[chData.g]

# queue load background color
#  - enqueue PALETTE_DATA[chData.b]

# calculate glyph position
#  - fnI = chData.r * (8 * 12)

# calculate glyph offset
#  - fnXO = x_coord % 8
#  - fnYO = y_coord % 12
#  - fnPos = (fnI + (fnYO * 8 + fnXO)) * 4

# queue load glyph color
#  - enqueue FONT_DATA[fnPos]

# load colors into registers
#  - fgCol = dequeue
#  - bgCol = dequeue
#  - fnCol = dequeue

# blend forground and background colors based on glyph color
#  - gl_FragColor = (fnCol.a * fgCol) + ((1.0 - fnCol.a) * bgCol);
#  - 
