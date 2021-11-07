# idOS
Básicamente es un planificador de eventos que incluye un timer y pone al micro en modo de bajo consumo mientras no esté haciendo nada, pero para simplificar consideramos que es un sistema operativo. Estamos en proceso y también lo traduciremos al inglés, si le interesa, su ayuda es bienvenida.
## Desarrollo
Se ha desarrollado en VSCode con PlatformIO. Hasta el momento solo se ha programado y testeado (tanto en simulación como en hardware) en Arduino Nano (atmega328p). Una vez creado el proyecto de Arduino Nano en PlatformIO, copiar todo el directorio en src/.
Para comenzar a programar entrar en main.cpp.
Para comenzar a partir de los ejemplos o simplemente correrlos, sustituir el contenido de main.cpp por el contenido del ejemplo en cuestión.
## Simular
Para la simulación en windows he probado con el Proteus.
En Macos X se ha probado con simavr. Este será el comando completo para sinmular un Arduino Nano compilado en PlatforIO:
simavr -m atmega328p -f 16000000 ~/Documents/PlatformIO/Projects/idOS/.pio/build/nanoatmega328new/firmware.elf
Supongo que en Linux funciones de la misma forma que en Macos X.
## Nota
En la medida que se avance en el desarrollo de idOS el contenido de este archivo irá cambiando.
