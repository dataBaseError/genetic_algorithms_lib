#!/bin/ruby


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

