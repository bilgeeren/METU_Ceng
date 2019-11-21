s=$(getent ahosts "s" | cut -d " " -f 1 | uniq)
r1=$(getent ahosts "r1" | cut -d " " -f 1 | uniq)
r2=$(getent ahosts "r2" | cut -d " " -f 1 | uniq)
d=$(getent ahosts "d" | cut -d " " -f 1 | uniq)

s_adapter=$(ip route get $s | grep -Po '(?<=(dev )).*(?= src| proto)')
r1_adapter=$(ip route get $r1 | grep -Po '(?<=(dev )).*(?= src| proto)')
r2_adapter=$(ip route get $r2 | grep -Po '(?<=(dev )).*(?= src| proto)')
d_adapter=$(ip route get $d | grep -Po '(?<=(dev )).*(?= src| proto)')



s_random_delay=$(( ( RANDOM % 50 )  + 20 ))"ms"
r1_random_delay=$(( ( RANDOM % 50 )  + 20 ))"ms"
r2_random_delay=$(( ( RANDOM % 50 )  + 20 ))"ms"
d_random_delay=$(( ( RANDOM % 50 )  + 20 ))"ms"

sudo tc qdisc add dev $s_adapter root netem delay $s_random_delay
sudo tc qdisc add dev $r1_adapter root netem delay $r1_random_delay
sudo tc qdisc add dev $r3_adapter root netem delay $r2_random_delay
sudo tc qdisc add dev $d_adapter root netem delay $d_random_delay

