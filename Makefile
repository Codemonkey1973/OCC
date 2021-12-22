############################################################################
# 
# Copyright 2021 Lee Mitchell <lee@indigopepper.com>
# This file is part of OCC (Orlaco Camera Configurator)
# 
# OCC is free software: you can redistribute it
# and/or modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the License,
# or (at your option) any later version.
# 
# OCC is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with OCC.  If not,
# see <http://www.gnu.org/licenses/>.
# 
############################################################################

TARGET=occ.exe

CC=gcc

all:
ifeq ($(OS),Windows_NT)
	$(CC) -o $(TARGET) main.c orlaco.c -lws2_32
else
	$(CC) -o $(TARGET) main.c orlaco.c
endif

clean:
	rm -rf $(TARGET)