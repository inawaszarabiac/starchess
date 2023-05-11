compile: main bot bot2
	

main: main.cpp header.h
	g++ main.cpp -o main
	
bot: bot.cpp header.h
	g++ bot.cpp -o bot
	
bot2: bot2.cpp header.h
	g++ bot2.cpp -o bot2
	
clean:
	rm *.o main bot bot2
