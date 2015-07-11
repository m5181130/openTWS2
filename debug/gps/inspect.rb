def reliable?(arr)
 ans = false
 msgID = arr[1].unpack("H2")[0]
 len = arr[2..-3].length
 case msgID
 when "70"
  ans = true if len==76
 when "4a"
  ans = true if len==33
 when "4b"
  ans = true if len==42
 when "f5"
  ans = true if (len-27)%30==0
 when "f6"
  ans = true if len==49
 when "f7"
  ans = true if (len-138==0) or (len-93==0)
 end
 puts "#{msgID} len: #{len}\t#{ans}"
 ans
end

# $stdout.reopen("data.dat", "w")

DLE_BIN = "10".lines.to_a.pack("H2")
ETX_BIN = "03".lines.to_a.pack("H2")

for fname in ARGV
File.open(fname,"rb") { |fin|

puts fname

arr = []

while c=fin.read(1) do
 arr << c
 if arr[-2]==DLE_BIN and arr[-1]==DLE_BIN
  arr[-1] = nil
 end

 if arr[-2]==DLE_BIN and arr[-1]==ETX_BIN
  arr.compact! # --> remove <DLE> duplication
  
  p arr.join.unpack("H*")[0].scan(/../).join(" ") if not reliable?(arr)

  arr = []
 end
end
}
end
