executable1 = travelmonitor
executable2 = monitor

all : travelmonitor monitor

travelmonitor : travelmonitor.c named_pipes.c extra_lists.c skiplist.c get_string.c bloom_travel.c
	gcc -o travelmonitor travelmonitor.c named_pipes.c extra_lists.c skiplist.c get_string.c bloom_travel.c -g

monitor: monitor.c get_string.c bloom.c extra_lists.c skiplist.c
	gcc -o monitor monitor.c get_string.c bloom.c extra_lists.c skiplist.c -g
clean:
	rm -f $(executable1) $(executable2) 