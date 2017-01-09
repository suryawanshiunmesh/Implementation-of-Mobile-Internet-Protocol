all: ds mn fa ha
ds: data_source.c
	gcc -Wall -g data_source.c -o ds
mn: mobile_node.c
	gcc -Wall -g mobile_node.c -o mn
fa: foreign_agent.c
	gcc -Wall -g foreign_agent.c -o fa
ha: home_agent.c
	gcc -Wall -g home_agent.c -o ha
