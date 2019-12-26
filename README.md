# brazo-r1
Repositorio de control del brazo del robot R1.

## Controlador
Carpeta con el firmware del brazo, en especifico con el codigo arduino a utilizar en el arduino DUE. Los pines utilizados están definidos en la parte inicial del código ino. Las conexiones consideran 3 comunicaciones seriales, 6 pins digitales de lectura de encoders y 3 pines de lectura de finales de carrera.

Este código además maneja la comunicación de bajo nivel con el computador en la arquitectura _Maestro-Esclavo_ utilizada.

## Librería
Carpeta con software y librería básica de uso del brazo, esta librería es la **_ARMR1_**, para utilizarla solo es necesario tener el código: 

~~~python
Import ARMR1
foo = ARMR1.ARMR1()
~~~

Para esto hay que tener la carpeta ARMR1 en el mismo directorio que el archivo a ejecutar.

## Comunicación

La comunicación funciona de manera distinta según si es del esclavo al maestro o viceversa.

Por esto especificaremos el sistema de comunicación según cada caso.

### Maestro a Esclavo
La comunicación maestro a esclavo es la más compleja, esta consiste en ordenes que pueden o ejecutar ciertos códigos del esclavo, cambiar valores o pedir información.

Para esto separamos los comandos enviados en órdenes o literales, esto está indicado en el primer bit del mensaje. Si este bit es 1 significa que el mensaje es una órden, en caso de ser 0 este mensaje es un literal.

En caso de ser un literal los siguientes 7 bits son la información respectiva, esta se lee según el contexto de las instrucciones anteriores. Por ejemplo, si la instrucción anterior fue de cambio de referencia del hombro entonces estos 7 bits corresponden a los 7 bits mas significativos de la nueva referencia y los 7 bits del literal siguiente corresponderán a los bits menos significativos, con todo esto completamos una referencia de 14 bits.

En caso de ser un 1 el primer bit entonces miramos al siguiente bit, este indicará si es una orden a un motor o una orden "global". En caso de ser un 0 es una orden global, estas son ordenes hardcodeadas que no dependen de contexto. Entre estas órdenes se encuentran cosas como _UpdateAllReferences_ que ejecutan acciones que modifican el comportamiento completo del brazo y no interactúan con un motor en específico.

En caso de ser un 1 el bit de motor o global tenemos que los siguientes 2 bits definen a que motor se le está entregando la orden, estos corresponden a:

* 00 = Cadera
* 01 = Hombro
* 10 = Codo

Luego los siguientes 4 bits corresponden a la orden que se le quiere entregar a ese motor. Por ejemplo si queremos decirle al hombro que nos entregue su posición actual tenemos que construir el mensaje de la siguiente manera:

* Primer bit es 1 ya que es una órden
* Segundo bit es 1 ya que es una órden a un motor
* Bits 3 y 4 son 01 ya que la instrucción va a la articulación del Hombro
* Últimos 4 bits son 0000 ya que esta es la orden de entregar posición.

Osea el mensaje a enviar es 0b1101000 = 208