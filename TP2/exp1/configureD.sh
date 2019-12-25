r3=$(getent ahosts "r3" | cut -d " " -f 1 | uniq)

r3_adapter=$(ip route get $r3 | grep -Po '(?<=(dev )).*(?= src| proto)')

sudo tc qdisc del dev $r3_adapter root netem

sudo tc qdisc add dev $r3_adapter root netem loss 5% delay 3ms
