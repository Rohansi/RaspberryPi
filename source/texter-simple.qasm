mov r2, 128             # image size
mov r3, 128 - 1         # bitmask used for modulo image size

mov r0, y_coord         # r0 = Y coordinate on screen
shr r0, r0, 2           # r0 = r0 >> 2      (divide by 4)
and r0, r0, r3          # r0 = r0 & r3      (modulo by image size)
                        # r0 = Y coordinate in image

mov r1, x_coord         # r1 = X coordinate on screen
shr r1, r1, 2           # r1 = r1 >> 2      (divide by 4)
and r1, r1, r3          # r1 = r1 & r3      (modulo by image size)
                        # r1 = X coordinate in image
      
mul24 r0, r0, r2        # r0 = r0 * r2      (multiply Y coordinate by width of image)
add r0, r0, r1          # r0 = r0 + r1      (add X coordinate to r0)
                        # r0 = linear index to image pixel

shl r0, r0, 2           # r0 = r0 << 2      (multiply by sizeof(uint32_t), gives us index in bytes)
mov r3, 0x81000000      # image base pointer, L2 cached only
add r0, r0, r3          # r0 = index + image base pointer

mov t0s, r0; sbwait     # write to t0s adds it to a queue to load
ldtmu0                  # get result of load, stores in r4
mov tlbc, r4; thrend    # tlbc is the color output
nop
sbdone
