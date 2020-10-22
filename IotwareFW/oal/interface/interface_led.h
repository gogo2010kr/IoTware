/*
Copyright (C) <2020> <ETRI>

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option) any later version. This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details. You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef INTERFACE_LED_H
#define INTERFACE_LED_H


#include "iw_common.h"
#include "iw_dev_led.h"

iw_error_t led_on(led_color_t color);
iw_error_t led_off(led_color_t color);
iw_error_t led_toggle(led_color_t color);

#endif /* INTERFACE_LED_H */
