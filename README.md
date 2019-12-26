# brazo-r1
Repositorio de control del brazo del robot R1.

## Controlador
Carpeta con el firmware del brazo, en especifico con el codigo arduino a utilizar en el arduino DUE. Los pines utilizados están definidos en la parte inicial del código ino. Las conexiones consideran 3 comunicaciones seriales, 6 pins digitales de lectura de encoders y 3 pines de lectura de finales de carrera.

Este código además maneja la comunicación de bajo nivel con el computador en la arquitectura _Maestro-Esclavo_ utilizada.

## Librería
Carpeta con software y librería básica de uso del brazo, esta librería es la **_ARMR1_**, para utilizarla solo es necesario tener el código: 

``` Import ARMR1 
foo = ARMR1.ARMR1() ```

teniendo la carpeta ARMR1 en el mismo directorio que el archivo a ejecutar.


