require 'pp'

# open the file
# identify message types
# process message depending on its type

def tab(msg)
  puts "\t #{msg}"
end

def mesg_type(msg)
  #p msg[0].unpack('H*')
  msg[1].unpack('H*').first
  #p msg[2].unpack('C')
end

def to_int8(byte)
  byte.unpack('c').first
end
def to_uint8(byte)
  byte.unpack('C').first
end

def to_unsgn_chars(bytes)
  bytes.unpack('C*')
end
def to_int16(byte)
  byte.unpack('s').first
end
def to_int32(byte)
  byte.unpack('l').first
end
def to_uint16(byte)
  byte.unpack('S').first
end
def to_uint32(byte)
  byte.unpack('L').first
end
def to_fp32(byte)
  byte.unpack('F').first
end
def to_fp64(byte)
  byte.unpack('D').first
end


def decode_msg70(pay_load)
  nchan = to_uint8(pay_load[0])
  tab "We have #{nchan} channels of data"
  version = to_unsgn_chars pay_load[1..21].join
  tab "Equipment and Software Version Identifier #{version}"
  serial = to_uint32 pay_load[22..25].join
  tab "Serial Number #{serial}"
end

def decode_msg4a(pay_load)
  tab "alpha0 = #{ to_fp32 pay_load[0..3].join}"
  tab "alpha1 = #{ to_fp32 pay_load[4..7].join}"
  tab "alpha2 = #{ to_fp32 pay_load[8..11].join}"
  tab "alpha3 = #{ to_fp32 pay_load[12..15].join}"
  tab "beta0 = #{ to_fp32 pay_load[16..19].join}"
  tab "beta1 = #{ to_fp32 pay_load[20..23].join}"
  tab "beta2 = #{ to_fp32 pay_load[24..27].join}"
  tab "beta3 = #{ to_fp32 pay_load[28..31].join}"
  tab "rel_sign = #{ to_uint8 pay_load[32]}"
end

def decode_msg4b(pay_load)
  tab "A1 = #{to_fp64 pay_load[0..7].join} sec/sec"
  tab "A0 = #{to_fp64 pay_load[8..15].join} sec"
  tab "t_ot = #{to_uint32 pay_load[16..19].join} sec"
  tab "WN_t = #{to_uint16 pay_load[20..21].join} weeks"
  tab "delt_LS = #{to_int16 pay_load[22..23].join} sec"
  tab "WN_LSF = #{to_uint16 pay_load[24..25].join} weeks"
  tab "DN = #{to_uint16 pay_load[26..27].join} days"
  tab "delt_LSF = #{to_int16 pay_load[28..29].join} sec"
  tab "gps_comm_ok = #{to_uint8 pay_load[30]}"
  tab "n^a = #{to_uint16 pay_load[31..32].join}"
  tab "tau_c = #{to_fp64 pay_load[33..40].join}"
  tab "glonass_ok = #{to_uint8 pay_load[41]}"
end

def decode_msgf5(pay_load)
  tab "UTC = #{to_fp64 pay_load[0..7].join} ms"
  tab "WN_t = #{to_uint16 pay_load[8..9].join} weeks"
  tab "GPS-UTC = #{to_fp64 pay_load[10..17].join} ms"
  tab "GLONASS-UTC = #{to_fp64 pay_load[18..25].join} ms"
  tab "time_scale_correction = #{to_int8 pay_load[26]} ms"
  
  tab "signal_type = #{to_uint8 pay_load[27]}"
  tab "satellite_no = #{to_uint8 pay_load[28]}"
  tab "GLONASS_carrier_no = #{to_uint8 pay_load[29]}"
  tab "signal-to-noise = #{to_uint8 pay_load[30]} dB-Hz"
  tab "carrier_phase = #{to_fp64 pay_load[31..38].join} cycles"
  tab "pseudo_range = #{to_fp64 pay_load[39..46].join} ms"
  tab "doppler_fq = #{to_fp64 pay_load[47..54].join} Hz"
  tab "rawdata_flags = #{to_uint8 pay_load[55]}"
  tab "reserved = #{to_uint8 pay_load[56]}"
end

def decode_msgf6(pay_load)
  tab "X = #{to_fp64 pay_load[0..7].join} m"
  tab "Y = #{to_fp64 pay_load[8..15].join} m"
  tab "Z = #{to_fp64 pay_load[16..23].join} m"
  tab "X_MSE = #{to_fp64 pay_load[24..31].join} m"
  tab "Y_MSE = #{to_fp64 pay_load[32..39].join} m"
  tab "Z_MSE = #{to_fp64 pay_load[40..47].join} m"
  tab "dynamic_mode = #{to_uint8 pay_load[48]}"
end

def decode_msgf7(pay_load)
  sytem_type = to_uint8 pay_load[0]

  tab "system_type = #{sytem_type}"
  tab "satellite_no = #{to_uint8 pay_load[1]}"
  case sytem_type
  when 1 # GPS satellites
  tab "Crs = #{to_fp32 pay_load[2..5].join} m"
  tab "Dn = #{to_fp32 pay_load[6..9].join} radians/ms"
  tab "M0 = #{to_fp64 pay_load[10..17].join} radians"
  tab "Cuc = #{to_fp32 pay_load[18..21].join} radians"
  tab "E = #{to_fp64 pay_load[22..29].join} e"
  tab "Cus = #{to_fp32 pay_load[30..33].join} radians"
  tab "SqrtA = #{to_fp64 pay_load[34..41].join} m^0.5"
  tab "Toe = #{to_fp64 pay_load[42..49].join} ms"
  tab "Cic = #{to_fp32 pay_load[50..53].join} radians"
  tab "Omega0 = #{to_fp64 pay_load[54..61].join} radians"
  tab "Cis = #{to_fp32 pay_load[62..65].join} radians"
  tab "I0 = #{to_fp64 pay_load[66..73].join} radians"
  tab "Crc = #{to_fp32 pay_load[74..77].join} m"
  tab "W = #{to_fp64 pay_load[78..85].join} radians"
  tab "OmegaR = #{to_fp64 pay_load[86..93].join} radians/ms"
  tab "Ir = #{to_fp64 pay_load[94..101].join} radians/ms"
  tab "Tgd = #{to_fp32 pay_load[102..105].join} ms"
  tab "Toc = #{to_fp64 pay_load[106..113].join} ms"
  tab "Af2 = #{to_fp32 pay_load[114..117].join} ms/ms^2"
  tab "Af1 = #{to_fp32 pay_load[118..121].join} ms/ms"
  tab "Af0 = #{to_fp32 pay_load[122..125].join} ms"
  tab "URA = #{to_uint16 pay_load[126..127].join}"
  tab "IODE = #{to_uint16 pay_load[128..129].join}"
  tab "IODC = #{to_uint16 pay_load[130..131].join}"
  tab "CodeL2 = #{to_uint16 pay_load[132..133].join}"
  tab "L2_Pdata_flag = #{to_uint16 pay_load[134..135].join}"
  tab "WeekN = #{to_uint16 pay_load[136..137].join}"
  when 2 # GLONASS satellites
  tab "carrier_no = #{to_int8 pay_load[2]}"
  tab "X = #{to_fp64 pay_load[3..10].join} m"
  tab "Y = #{to_fp64 pay_load[11..18].join} m"
  tab "Z = #{to_fp64 pay_load[19..26].join} m"
  tab "Vx = #{to_fp64 pay_load[27..34].join} m/ms"
  tab "Vy = #{to_fp64 pay_load[35..42].join} m/ms"
  tab "Vz = #{to_fp64 pay_load[43..50].join} m/ms"
  tab "Ax = #{to_fp64 pay_load[51..58].join} m/ms^2"
  tab "Ay = #{to_fp64 pay_load[59..66].join} m/ms^2"
  tab "Az = #{to_fp64 pay_load[67..74].join} m/ms^2"
  tab "t_b = #{to_fp64 pay_load[75..82].join} ms"
  tab "gamma_n = #{to_fp32 pay_load[83..86].join} ms"
  tab "tau_n = #{to_fp32 pay_load[87..90].join} ms"
  tab "E_n = #{to_uint16 pay_load[91..92].join}"
  end
end

def process_mesg(msg)
  typ = mesg_type(msg)
  puts "Message is of type #{typ}"
  case typ
  when "70"
    decode_msg70(msg[2..-3])
  when "4a"
    decode_msg4a(msg[2..-3])
  when "4b"
    decode_msg4b(msg[2..-3])
  when "f5"
    decode_msgf5(msg[2..-3])
  when "f6"
    decode_msgf6(msg[2..-3])
  when "f7"
    decode_msgf7(msg[2..-3])
  end
  true
end

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
  ans = true if (len%138==0) or (len%93==0)
 end
 ans
end

for fname in ARGV

File.open(fname,"rb") do |fin|
 arr=[]
 nmsg = 1
 err_count = 0

 DLE = "10".lines.to_a.pack("H2")
 ETX = "03".lines.to_a.pack("H2")

 c=fin.read(1) until c==DLE
 arr << c

while c=fin.read(1) do
 arr << c

 if arr[-2]==DLE and arr[-1]==DLE
  arr[-1] = nil
 end

 if arr[-2]==DLE and arr[-1]==ETX
  arr.compact! # --> remove <DLE> duplication

  process_mesg(arr)

  err_count+=1 if not reliable?(arr)

  arr = []
  #nmsg+=1; break if nmsg==4
 end
end

puts "error msgs: #{err_count}"
end

end
