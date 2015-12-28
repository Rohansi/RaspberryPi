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
add t0s, r0, unif           # t0s = r0 + PALETTE_DATA_FG

# enqueue load background color
shr r3, r3, 8               # r3 = r3 >> 8          (next field)
and r0, r3, ra5             # r0 = r3 & ra5         (background index)
shl r0, r0, 2               # r0 = r0 << 2          (multiply by 4)
add t0s, r0, unif           # t0s = r0 + PALETTE_DATA_BG

# calculate glyph position (ra1 = fnX/fnY)
shr r3, r3, 8               # r3 = r3 >> 8          (next field)
and r1, r3, ra5             # r1 = r3 & ra5         (glyph index)

and r0, r1, 15              # r0 = r1 & 15          (modulus by 16)
shl r0, r0, 3               # r0 = r0 << 3          (multiply by 8)
mov ra1.16a, r0             # ra1.x = r0

shr r0, r1, 4               # r0 = r1 >> 4          (divide by 16)
mul24 r0, r0, 12            # r0 = r0 * 12
mov ra1.16b, r0             # ra1.y = r0

# calculate glyph offset (ra2 = fnXO/fnYO)
shl r0, ra0.16a, 3          # r0 = ra0.x << 3       (multiply by 8)
sub r0, x_coord, r0         # r0 = x_coord - r0
add r0, r0, ra1.16a         # r0 = r0 + ra1.x
mov ra2.16a, r0             # ra2.x = r0 + ra1.x

mul24 r0, ra0.16b, 12       # r0 = ra0.y * 12
sub r0, y_coord, r0         # r0 = y_coord - r0
add r0, r0, ra1.16b         # r0 = r0 + ra1.y
mov ra2.16b, r0             # ra2.y = r0 + ra1.y

# calculate linear glyph position (r0 = fnPos)
ldi r1, 16 * 8              # r1 = width of font image
mul24 r0, ra2.16b, r1       # r0 = ra2.y * r1
add r0, r0, ra2.16a         # r0 = r0 + ra2.x
shl r0, r0, 2               # r0 = r0 << 2          (multiply by 4)

# enqueue load font color
add t0s, r0, unif           # t0s = r0 + FONT_DATA  (enqueue memory load)           

# dequeue color lookups into accumulators
ldtmu0
mov r3, r4                  # r3 = foreground color
ldtmu0
mov r2, r4                  # r2 = background color
ldtmu0                      # r4 = font color

# calculate effective foreground color (fnCol.b * fgCol)
and r0, r4, ra5             # r0 = r4.b
mov ra1.8abcd, r0           # ra1 = r0 duplicated
sub r0, ra5, r0             # r0 = 255 - r0 (can't access ra1 here, this is for background color)
v8muld r3, ra1, r3          # multiply the colors

# calculate effective background color ((1.0 - fnCol.b) * bgCol)
mov ra1.8abcd, r0           # ra1 = r0 duplicated
sbwait                      # can't access ra1 here, wait for scoreboard
v8muld r2, ra1, r2          # multiply the colors

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
#  - fnX = (chData.r % 16) * 8
#  - fnY = (chData.r / 16) * 12

# calculate glyph offset
#  - fnXO = fnX + (x_coord % 8)
#  - fnYO = fnY + (y_coord % 12)
#  - fnPos = ((fnYO * (16 * 8)) + fnXO) * 4

# queue load glyph color
#  - enqueue FONT_DATA[fnPos]

# load colors into registers
#  - fgCol = dequeue
#  - bgCol = dequeue
#  - fnCol = dequeue

# blend forground and background colors based on glyph color
#  - gl_FragColor = (fnCol.a * fgCol) + ((1.0 - fnCol.a) * bgCol);
#  - 
