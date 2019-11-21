r3=$(getent ahosts "r3" | cut -d " " -f 1 | uniq)

r3_adapter=$(ip route get $r3 | grep -Po '(?<=(dev )).*(?= src| proto)')

sudo tc qdisc add dev $d_adapter root netem
sudo tc qdisc add dev $d_adapter root netem delay 20ms 5ms distribution normal
