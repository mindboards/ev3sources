/*
 * LEGO® MINDSTORMS EV3
 *
 * Copyright (C) 2010-2013 The LEGO Group
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#ifndef LEGO_TI_OMAPL_PRU_SUART_H
#define LEGO_TI_OMAPL_PRU_SUART_H 1

int  lego_pru_suart_init(void);
void lego_pru_suart_exit(void);

int  lego_pru_uart_init(int port);
void lego_pru_uart_exit(int port);
int lego_pru_uart_activate(int port);
void lego_pru_uart_deactivate(int port);
void lego_pru_set_baudrate(int port, unsigned int baud);
int  lego_pru_uart_get_break_state(int port);
int  lego_pru_write_bytes(int port, unsigned char *pdata, int size);
int  lego_pru_read_bytes(int port, unsigned char *pdata, int size);
int lego_pru_size_data_rx_buffer(int port);

#endif
