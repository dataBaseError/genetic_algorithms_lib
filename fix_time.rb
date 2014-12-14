#!/bin/ruby
###############################################################################
# 
# The MIT License (MIT)
#
# Copyright (c) 2014  Joseph Heron, Jonathan Gillett
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
###############################################################################



file = File.new("analysis/output_12132014_1443.csv", "r")
new_text = Array.new

while(line = file.gets)

	elements = line.chomp!.split(", ")
	min, sec = elements[-1].scan(/([0-9]+)m([0-9]+.[0-9]{3})/)[0]

	min = min.to_i
	sec = sec.to_f
	sec+= 60*min

	elements[-1] = sec
	new_text << elements.join(", ")
end
file.close()

out = File.new("analysis/output_12132014_1443_fixed.cvs", "w")

new_text.each do |line|
	out.puts line
end
out.close()

