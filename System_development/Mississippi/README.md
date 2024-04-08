# Mississippi

Files related to "spike-only" sensor package version.

## Arduino
A unique address must be assigned to each node in the `this_node` variable in file `Arduino/rf24_network.ino`

`0o00` will be the address of the base station. Each node can be labeled `0o01, 0o02, etc...`

## RaspberryPi
All known nodes must be declared in the `OTHER_NODES` list.
Ex: for 5 nodes connected to base station `OTHER_NODES = [0o1, 0o2, 0o3, 0o4, 0o5]`

*Max of 5 nodes able to connect to nRF24L01+ chip at once.
