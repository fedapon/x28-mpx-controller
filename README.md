# Conexión a bus MPX/MPXH de Alarma X28

Este proyecto permite conectarnos una alarma X-28 con protocolo MPX o MPHX .

Actualmente tiene las siguientes funcionalidades:

- Recibir los paquetes y generar eventos (activada, desactivada, estoy, me voy).
- Simular pulsado de teclas de un panel.
- Activar/desactivar la alarma mediante el envío del código a la central.
- Hacer sonar la alarma mediante el envio de tecla pánico.

El librería cuanta con un buffer cirlar de paquetes, lo que permite recibir paquetes y no perderlos por un tiempo aproximado de 30 segundos mientras se hacen otras tareas de larga duración.

## Estado del proyecto

Esta es una versión inicial y tiene una funcionalidad minima. Algunas ideas pendientes:

- Monitoreo de estado de zonas/ alarma lista
- Monitoreo de estado de alarma disparada via MPX.

## Hardware requerido

Actualmente el hardware utilizado para el desarrollo es un ESP8266 pero estimo que debería funcionar con ESP32 o cualquier arduino que permita utilizar interrupción en el pin de entrada de datos.
Para conectar a la alarma se necesita basicamente un level shifter y un driver. Este circuito fue probado con los modelos:
 - 9004W-MPX (bus MPX de 12V).
 - N4-MPXH (bus MPXH 8V aprox).

![Alt text](circuito.png?raw=true "Circuito")

## Agradecimiento

Este proyecto no habría sido posible sin la ayuda de estos proyectos:
 - https://github.com/gbisheimer/x28_sniffer
 - https://github.com/hjf/esphome-x28
