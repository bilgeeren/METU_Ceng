s=$(getent ahosts "s" | cut -d " " -f 1 | uniq)
d=$(getent ahosts "d" | cut -d " " -f 1 | uniq)

s_adapter=$(ip route get $s | grep -Po '(?<=(dev )).*(?= src| proto)')
d_adapter=$(ip route get $d | grep -Po '(?<=(dev )).*(?= src| proto)')

sudo tc qdisc del dev $s_adapter root netem
sudo tc qdisc del dev $d_adapter root netem

sudo tc qdisc change dev $s_adapter root netem loss 5% delay 3ms
sudo tc qdisc change dev $d_adapter root netem loss 5% delay 3ms



