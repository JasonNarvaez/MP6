# MP6
Communicates to the dataserver through a single TCP connection

Compile:
make

Run:
Running client:
./client -n [#of data requests per person] -b [size of bounded buffer] -w [# Network request channels] -h [name of host] -p [port number of host]

Run data server:
./dataserver -p [port number] -b [backlog of server socket]
