Code has 2 parts in it.
One part is link cost and the other part is experiment part.

In link cost part: 
    First, You need to execute source.py and destination.py on source node and destination node respectively.
    Then, you need to execute router2.py on router2
    And finally router1.py on router1 and router3 on router3.py.

After this, the r1, r2 and r3 nodes will save the link costs in a txt file.

In experiment part:
    First you need to synchronize the times of the nodes with the following ntp command:
	"sudo ntpdate -u pool.ntp.org"
    Then, you need to add time delay to the nodes. 
    in experiment/configure*.sh scripts, we added a time delay with "sudo tc qdisc add dev $r3_adapter root netem delay 50ms 5ms distribution normal"
    this command adds 50+-5 ms time delay to the r3_adapter. 
    General time delay command: 
       	"tc qdisc add dev [INTERFACE] root netem delay 50ms 5ms distribution normal"
    You should change the time delays in different experiments.
    And run the shell scripts before doing the experiment on the related nodes.
    
    To accomplish the experiment, you should run destination.py on destination node and router3.py on r3 node.
    As a last step, you should run source.py on source node. Then the destination node will print timestamps, average 
    and error rate.
