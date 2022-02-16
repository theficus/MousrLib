#include "buttons.h"

/*
ButtonState getPressedButtons(uint32_t mask)
{
    ButtonState bs;
    bs.r = (!(mask & 1 << BUTTON_RIGHT)) ? ButtonPressState::Down : ButtonPressState::Up;
    bs.d = (!(mask & 1 << BUTTON_DOWN)) ? ButtonPressState::Down : ButtonPressState::Up;
    bs.l = (!(mask & 1 << BUTTON_LEFT)) ? ButtonPressState::Down : ButtonPressState::Up;
    bs.u = (!(mask & 1 << BUTTON_UP)) ? ButtonPressState::Down : ButtonPressState::Up;
    bs.sel = (!(mask & 1 << BUTTON_SEL)) ? ButtonPressState::Down : ButtonPressState::Up;
    bs.raw = mask;

    s_printf("PRESS: u=%d d=%d l=%d r=%d s=%d bin=" BYTE_TO_BINARY_PATTERN "\n",
             (int)bs.u,
             (int)bs.d,
             (int)bs.l,
             (int)bs.r,
             (int)bs.sel,
             BYTE_TO_BINARY(bs.raw));

    return bs;
}
*/
