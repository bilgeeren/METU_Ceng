r1=$(getent ahosts "r1" | cut -d " " -f 1 | uniq)
r2=$(getent ahosts "r2" | cut -d " " -f 1 | uniq)
r3=$(getent ahosts "r3" | cut -d " " -f 1 | uniq)

r1_adapter=$(ip route get $r1 | grep -Po '(?<=(dev )).*(?= src| proto)')
r2_adapter=$(ip route get $r2 | grep -Po '(?<=(dev )).*(?= src| proto)')
r3_adapter=$(ip route get $r3 | grep -Po '(?<=(dev )).*(?= src| proto)')



r1_random_delay=$(( ( RANDOM % 50 )  + 20 ))"ms"
r2_random_delay=$(( ( RANDOM % 50 )  + 20 ))"ms"
r3_random_delay=$(( ( RANDOM % 50 )  + 20 ))"ms"

sudo tc qdisc add dev $s_adapter root netem delay $r1_random_delay
sudo tc qdisc add dev $r2_adapter root netem delay $r2_random_delay
sudo tc qdisc add dev $d_adapter root netem delay $r3_random_delay
