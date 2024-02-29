all: master server drone input window obstacles targets watchdog 
	
clean-logs:
	rm build/log/watchdog/* 
clean:
	rm build/master build/simple_process build/watchdog build/server build/drone build/input build/window build/obstacles build/targets
master:
	gcc src/master.c -o build/master
server:
	gcc src/server.c -o build/server
drone:
	gcc src/drone.c -o build/drone -lncurses -lm
input:
	gcc src/input.c -o build/input -lncurses
window:	
	gcc src/window.c -o build/window -lncurses
obstacles:
	gcc src/obstacles.c -o build/obstacles -lncurses
targets:
	gcc src/targets.c -o build/targets -lncurses
watchdog:
	gcc src/watchdog.c -o build/watchdog -lncurses
