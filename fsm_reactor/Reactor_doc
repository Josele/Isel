Reactor

Dado que se trata de una máquina de cafe con dos tareas, las trataremos por separado en eventos y el manejador del reactor las coordinará.Para la tarea de comprobar si hay monedas, impondremos un periodo de 100 ms que será suficiente para chequear si hay una moneda sin que lleguen a echar una segunda en ese mismo periodo.
La segunda tarea puede llegar a tener un periodo aun mayor, puesto que para cumplir los diferentes estados podemos trabajar con tiempos mayores.

Habrá que asignar prioridades a los diferentes eventos. Lo hariamos siguiendo el criterio de tarea de menor plazo->mayor prioridad, pero en este caso hay flexibilidad encuanto a eso. Portanto asignaremos mayor prioridad a las tareas del café.

Se producirá un bloqueo cuando una tarea de menos prioridad no deje que se ejecuten otras de mayor prioridad y con reactor conseguiremos techo de prioridad inmediato, coniguiento como máximo un bloque igual a 1. 

Valores de un periodo de cada evento:
cofm_fsm:Periodo 499986000ns
cofm_fsm:Periodo 99996000ns

Mayor tiempo de respueta:
coin_fsm:time 547000ns
cofm_fsm:time 525000ns (un poco condicionado por las entradas  y salidas de datos)
Donde observamos que C<D.

