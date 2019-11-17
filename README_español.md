# Torpedo Mk I

En esta carpeta se encuentran los archivos con el programa y los esquemáticos de la placa de control de un torpedo realizado mediane impresión 3D. Se puede encontrar para su descarga en Thingiverse:

https://www.thingiverse.com/thing:3944261

La electrónica es muy sencilla, y está basada en Arduino. Utiliza un interruptor Reed para armar y disparar el torpedo. Este interruptor se activa magnéticamente. Como debe de estar dentro del submarino, un imán de cierta potencia viene muy bien. Nosotros usamos el imán de un disco duro antiguo.

Una vez construida y programada la electrónica es muy sencilla de utilizar. Siempre hay que armar primero el torpedo excitando el interruptor Reed. Para dispararlo hay que excitar el interruptor una segunda vez. Para abortar el disparo se puede excitar una tercera vez, mientras está en marcha. En la EEPROM del Arduino se almacena el tiempo de activación del motor. Si en cualquiera de los estados del sistema pasa más de un minuto el sistema se apaga automáticamente.

El armado y disparo del torpedo se puede hacer de dos formas distintas:

A) Disparo por un tiempo concreto: Encender el sistema poniendo y manteniendo el imán durante el tiempo de inicio (1seg) y durante el tiempo de programación que queramos programar sin quitar el imán, (el sistema nos dará un feedback nada más arrancar, trascurrido un segundo nos dará el segundo feedback y empezará a programar el tiempo de propulsión y cuando liberemos la entrada nos dará el tercer feedback y memorizará el tiempo de propulsión). Pasar el imán de nuevo para proceder al disparo por el tiempo programado en el paso 1.

B) Disparo con el tiempo programado del último disparo: Encender el sistema poniendo y quitando el imán durante menos de 1 segundo (el sistema nos dará un feedback). Pasar el imán de nuevo para proceder al disparo por el tiempo almacenado en la EEPROM.
