#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include "freertos/xtensa_context.h"
#include "esp_private/panic_reason.h"
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/gpio_reg.h"
#include "soc/dport_reg.h"

#define L5_INTR_STACK_SIZE  20
#define LX_INTR_A15_OFFSET 0
#define LX_INTR_A14_OFFSET 4
#define LX_INTR_A13_OFFSET 8
#define LX_INTR_A12_OFFSET 12
#define LX_INTR_A11_OFFSET 16

.extern circularBuffer
.extern write Pointer 

.equ BUFFER_SIZE, 256
.equ INT_PIN, 2 //34

    .data
_l5_intr_stack:
    .space      L5_INTR_STACK_SIZE

    .section .iram1,"ax"
    .global     xt_highint5
    .type       xt_highint5,@function
    .align      4
xt_highint5:

gpio_pins_map:
    .byte 32
    .byte 33
    .byte 25
    .byte 26
    .byte 26
    .byte 14
    .byte 12
    .byte 13

    movi    a0, _l5_intr_stack // assuming a0 saved restored by EXCSAVE_5 at the end of this
    s32i    a15, a0, LX_INTR_A15_OFFSET
    s32i    a14, a0, LX_INTR_A14_OFFSET
    s32i    a13, a0, LX_INTR_A13_OFFSET
    s32i    a12, a0, LX_INTR_A12_OFFSET
    s32i    a11, a0, LX_INTR_A12_OFFSET

    // check if interrupt was triggered by pin 32
    movi    a15, 1
    slli    a15, a15, INT_PIN
    movi    a13, GPIO_STATUS1_REG
    l32i    a14, a13, 0
    and     a15, a15, a14
    beqz    a15, not_int_pin

    movi a15, 0

loop_gpio_pins:

    // load pinto a14
    movi a14, gpio_pins_map
    add a14, a14, a15
    l32i a14, a14, 0

    //  check if its 31 or less
    movi a13, 32
    sub a13, a14, a13
    bgez a13, high_gpio

    movi a13, GPIO_IN_REG
   
    j get_pin_val

high_gpio:
    l32i a14, a13, 0
    movi a13, GPIO_IN1_REG

get_pin_val:
    // see if pin is high or low
    movi a12, 1
    ssl a14
    sll a12, a12
    and a12, a12, a13
    beqz a12, next_pin  //if its low jump to next pin

    // add high value to proper bit   
    movi a12, 1
    ssl a15
    sll a12, a12
    or a11, a11, a12  

next_pin:
    // increment which pin
    addi a15, a15, 1
    bnei a15, 8, store_value

    j loop_gpio_pins

store_value:
    // write value to circular buffer
    movi a15, writePointer
    l32i a15, a15, 0
    movi a14, circularBuffer
    add a14, a14, a15
    s8i  a14, a11, 0
    memw

    // increment writePointer
    addi a15, a15, 1
    bnei a15, 256, clear_int

    // buffer needs to reset
    movi a14, 0

clear_int:
    movi a14, writePointer
    s32i a15, a14, 0
    memw

    /* Clear interupt on GPIO34 */
    movi    a14, GPIO_STATUS1_W1TC_REG
    movi    a15, (1 << 2)
    s32i    a15, a14, 0
    memw


not_int_pin:
    l32i    a15, a0, LX_INTR_A15_OFFSET
    l32i    a14, a0, LX_INTR_A14_OFFSET
    l32i    a13, a0, LX_INTR_A13_OFFSET
    l32i    a13, a0, LX_INTR_A12_OFFSET
    l32i    a13, a0, LX_INTR_A11_OFFSET

    rsr     a0, EXCSAVE_5 // restore a0
    rfi     5

    .global ld_include_xt_highint5
ld_include_xt_highint5:
