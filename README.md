# idOS
Básicamente es un planificador de eventos, pero para simplificar consideramos que es un sistema operativo. Estamos en proceso y también lo traduciremos al inglés, si le interesa, su ayuda es bienvenida.
Embúllate!!
## Simular
Para la simulación en windows he probado con el Proteus, aunque tengo resultados raros con los timers y con las interrupciones, hay que revisar todo esto.
En Macos X se ha probado con simavr. Este será el comando completo para sinmular un Arduino Nano compilado en PlatforIO:
simavr -m atmega328p -f 16000000 ~/Documents/PlatformIO/Projects/idOS/.pio/build/nanoatmega328new/firmware.elf