.section .iram1,"ax"
.global xt_highint5
.type xt_highint5,@function
.align 4

# Define circular buffer and pointers
.extern readPointer
.extern writePointer
.extern circularBuffer

BUFFER_SIZE = 128

xt_highint5:
    # Save context
    push  {r0-r3, lr}

    # Check if the interrupt was triggered by pin 34
    rsr   a0, EXCSAVE_5
    and   a0, a0, #0x7f # Mask out the INUM field (interrupt number)
    mov   a1, #34       # Pin number triggering the interrupt
    cmp   a0, a1
    bne   .exit         # Exit if not triggered by pin 34

    # Read GPIO pins 32,33,25,26,27,14,12,13 in inverted order
    # Store them as an 8-bit number
    mov   r0, 0          # Initialize result to 0
    ldr   r1, =0x600D0000 # GPIO input register base address
    ldr   r2, [r1, #0x10] # Read GPIO 32-39 input values
    lsr   r2, r2, #32     # Align the bits to the right

    # Mask and combine the bits in inverted order
    ldr   r3, =0x0000000C # Pin bitmask for pins 13,12,14,27,26,25,33,32 in inverted order
    and   r2, r2, r3      # Mask out unwanted bits
    lsl   r2, r2, #24      # Shift the bits to MSB position
    orr   r0, r0, r2      # Combine with the result

    # Add to circular buffer
    ldr   r1, =circularBuffer
    ldr   r2, =writePointer
    ldr   r2, [r2]
    strb  r0, [r1, r2]

    # Increment write pointer
    add   r2, r2, #1
    cmp   r2, #BUFFER_SIZE
    movlt r2, #0
    str   r2, [r1, #4]  # Write new write pointer value

.exit:
    # Restore context and return from interrupt
    pop   {r0-r3, lr}
    rfi   5