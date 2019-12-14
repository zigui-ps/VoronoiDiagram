n = 100
puts n
n.times do |i|
	puts "%d %d" % [rand() * 1e9, rand() * 1e9]
end

=begin
n = 1000000
puts n
n.times do |i|
	d = rand() * 2 * Math::PI
	puts "%d %d" % [(1e9 * Math.cos(d)), (1e9 * Math.sin(d))]
end
=end

=begin
n = 1000000
puts n*2
n.times do |i|
	i += 1
	puts "%d %d" % [i, -i]
	puts "%d %d" % [-i, -i]
end
=end


=begin
n = 2000
m = 1000
puts n*m
n.times do |i|
	m.times do |j|
		puts "%d %d" % [i, 2*j] if i%2 == 0
		puts "%d %d" % [i, 2*j+1] if i%2 == 1
	end
end
=end
=begin
a = []
r = 2*3*5*7*11*13*17
r.times do |i|
	next if i == 0
	p = ((r*r - i*i) ** 0.5).to_i
	if p*p + i*i == r*r then
		a << [p, i]
		a << [p, -i]
		a << [-p, -i]
		a << [-p, i]
	end
end

puts a.length
a.each do |x|
	puts "%d %d" % x
end
=end
