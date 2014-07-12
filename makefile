CC=gcc

config_led:
	cp configs/led_light.config Dali/include/config.h

config_ckt_breaker:
	cp configs/circuit_breaker.config Dali/include/config.h

clean:
	rm -f config.h
