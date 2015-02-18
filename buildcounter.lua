#!/usr/bin/lua

--[[
Kuji32 Flash MCU Programmer
Copyright (C) 2014 Kari Sigurjonsson

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
--]]

--Script to increment the BUILD number in the file 'VERSION'.
local die = function(msg) print(msg or "Fatal Error!"); os.exit(1) end
local F = io.open("VERSION", "r") or die("Could not open file 'VERSION' for reading.")
local l, k, v, t = "", "", "", {}
for l in F:lines() do
	k, v = l:match("([%w%p]*)=(.*)")
	if (k and v) then t[#t + 1] = { key = k, value = v } end
	if (k == "BUILD") then
		t[#t].value = (tonumber(t[#t].value) or 0) + 1
		print("Build #"..t[#t].value.." - "..os.date("%Y-%m-%d %H:%M:%S"))
	end
end
F:close()
F = io.open("VERSION", "w+") or die("Could not open file 'VERSION' for writing.")
for k = 1, #t do F:write(t[k].key.."="..t[k].value.."\n") end
F:close()
